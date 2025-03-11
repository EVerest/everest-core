//! Module for Feig's payment terminals.
//!
//! The module is an EVerest wrapper around the [ZVT](https://github.com/qwello/zvt)
//! crate. It talks directly to the underlying hardware and instructs it to read
//! cards.
//!
//! ## NFC card handling
//!
//! In case a NFC card is presented, the module will issue a token on the
//! `auth_token_provider` interface. The token-id will be generated from the
//! NFC card metadata.
//!
//! ## Bank card handling
//!
//! In case a bank card is presented, it will pre-authorize the
//! `pre_authorization_amount` and issue a token on the `auth_token_provider`
//! interface. The token-id will be taken from the `bank_session_token`
//! interface. It commits (and releases the surplus from the pre-authorized
//! amount) once it receives the same a token-id on its `session_cost`
//! interface.
//!
//! ## Implementation details
//!
//! For more details checkout the
//! * [ZVT Rust implementation](https://github.com/qwello/zvt)
//! * [ZVT documentation](https://www.terminalhersteller.de/downloads.aspx)
//! * [Feig homepage](https://www.feig-payment.de/)
//!
#![allow(non_snake_case, non_camel_case_types, clippy::all)]
include!(concat!(env!("OUT_DIR"), "/generated.rs"));

use anyhow::Result;
use generated::types::{
    authorization::{AuthorizationType, IdToken, IdTokenType, ProvidedIdToken},
    money::MoneyAmount,
    payment_terminal::{BankSessionToken, BankTransactionSummary, CardType},
    session_cost::{SessionCost, SessionStatus},
};
use generated::{
    get_config, AuthTokenProviderServiceSubscriber, BankSessionTokenProviderClientSubscriber,
    Context, Module, ModulePublisher, OnReadySubscriber, PaymentTerminalServiceSubscriber,
    SessionCostClientSubscriber,
};
use std::collections::HashMap;
use std::sync::{mpsc::channel, mpsc::Sender, Arc, Mutex};
use std::time::Duration;
use std::{net::Ipv4Addr, str::FromStr};
use zvt_feig_terminal::config::{Config, FeigConfig};
use zvt_feig_terminal::feig::{CardInfo, Error};

mod sync_feig {
    use anyhow::Result;
    use zvt_feig_terminal::{
        config::Config,
        feig::{CardInfo, Feig, TransactionSummary},
    };

    pub struct SyncFeig {
        /// Tokio runtime to call the Feig functions.
        rt: tokio::runtime::Runtime,

        /// The async impl of the Feig.
        inner: std::sync::Mutex<Feig>,
    }

    /// Sync interface for the Feig.
    ///
    /// The `Feig` implements an async interface, which we can't use in EVerest.
    /// Here we wrap the async functions and expose the sync version of them.
    ///
    /// Below we allow `dead_code` so we just wrap all async functions even
    /// though they may be unused.
    #[allow(dead_code)]
    #[cfg_attr(test, mockall::automock)]
    impl SyncFeig {
        pub fn new(config: Config) -> Self {
            // Create a runtime for the Feig terminal.
            let rt = tokio::runtime::Builder::new_multi_thread()
                .max_blocking_threads(1)
                .enable_all()
                .build()
                .unwrap();

            // Create the Feig terminal itself.
            let feig = rt.block_on(async {
                loop {
                    let response = Feig::new(config.clone()).await;
                    match response {
                        Ok(inner) => {
                            log::info!("Payment terminal initialized.");
                            return inner;
                        }
                        Err(e) => {
                            log::warn!("Payment terminal not initialized {:?}", e);
                        }
                    }
                }
            });

            Self {
                rt,
                inner: std::sync::Mutex::new(feig),
            }
        }

        pub fn read_card(&self) -> Result<CardInfo> {
            let mut inner = self.inner.lock().unwrap();
            self.rt.block_on(inner.read_card())
        }

        pub fn begin_transaction(&self, token: &str) -> Result<()> {
            let mut inner = self.inner.lock().unwrap();
            self.rt.block_on(inner.begin_transaction(token))
        }

        pub fn cancel_transaction(&self, token: &str) -> Result<()> {
            let mut inner = self.inner.lock().unwrap();
            self.rt.block_on(inner.cancel_transaction(token))
        }

        pub fn commit_transaction(&self, token: &str, amount: u64) -> Result<TransactionSummary> {
            let mut inner = self.inner.lock().unwrap();
            self.rt.block_on(inner.commit_transaction(token, amount))
        }
    }
}

#[mockall_double::double]
use sync_feig::SyncFeig;

impl ProvidedIdToken {
    fn new(
        id_token: String,
        authorization_type: AuthorizationType,
        connectors: Option<Vec<i64>>,
    ) -> Self {
        Self {
            parent_id_token: None,
            id_token: IdToken {
                value: id_token,
                r#type: IdTokenType::Local,
                additional_info: None,
            },
            authorization_type,
            certificate: None,
            connectors: connectors,
            iso_15118_certificate_hash_data: None,
            prevalidated: None,
            request_id: None,
        }
    }
}

/// Main struct for this module.
pub struct PaymentTerminalModule {
    /// Sender for the `ModulePublisher` -> to get the publisher from `on_ready`
    /// into the main thread.
    tx: Sender<ModulePublisher>,

    /// The Feig interface.
    feig: SyncFeig,

    /// Keep track on which connector support which card types
    connector_to_card_type: Mutex<HashMap<i64, Vec<CardType>>>,
}

impl PaymentTerminalModule {
    /// Waits for a card and begins a transaction (sends an auth token).
    ///
    /// In case of a bank card we start the transaction on the payment terminal.
    /// In case of membership cards we just issue the token. In both cases we
    /// don't flag the token as pre-validated to allow the consumers to add
    /// custom validation steps on top.
    fn begin_transaction(&self, publishers: &ModulePublisher) -> Result<()> {
        // Get a valid invoice token.
        let token = {
            match publishers.bank_session_token_slots.get(0) {
                None => None,
                Some(publisher) => {
                    let invoice_token = publisher.get_bank_session_token()?;
                    if invoice_token.token.is_none() {
                        anyhow::bail!("No token received")
                    }
                    invoice_token.token
                }
            }
        };
        log::info!("Received the BankSessionToken {token:?}");

        // Wait for the card.
        let read_card_loop = || -> Result<CardInfo> {
            loop {
                if !self.has_everything_enabled() && !self.is_enabled() {
                    log::debug!("Reading is disabled, waiting...");
                    std::thread::sleep(Duration::from_secs(1));
                    continue;
                }

                match self.feig.read_card() {
                    Ok(card_info) => return Ok(card_info),
                    Err(e) => match e.downcast_ref::<Error>() {
                        Some(Error::NoCardPresented) => {
                            log::debug!("No card presented");
                            continue;
                        }
                        _ => return Err(anyhow::anyhow!("Failed to read card: {e:?}")),
                    },
                };
            }
        };
        let card_info = read_card_loop()?;

        if let Some(provided_token) = match card_info {
            CardInfo::Bank => {
                let credit_card_connectors = self.connectors_for_card_type(CardType::BankCard);
                if (!self.has_everything_enabled() && credit_card_connectors.is_empty())
                    || token.is_none()
                {
                    None
                } else {
                    self.feig.begin_transaction(token.as_ref().unwrap())?;
                    // Reuse the bank token as invoice token so we can use the
                    // invoice token later on to commit our transactions.
                    let connectors = if credit_card_connectors.is_empty() {
                        None
                    } else {
                        Some(credit_card_connectors)
                    };
                    Some(ProvidedIdToken::new(
                        token.unwrap(),
                        AuthorizationType::BankCard,
                        connectors,
                    ))
                }
            }
            CardInfo::MembershipCard(id_token) => Some(ProvidedIdToken::new(
                id_token,
                AuthorizationType::RFID,
                None,
            )),
        } {
            publishers.token_provider.provided_token(provided_token)?;
        }
        Ok(())
    }

    /// The implementation of the `SessionCostClientSubscriber::on_session_cost`,
    /// but here we can return errors.
    fn on_session_cost_impl(&self, context: &Context, value: SessionCost) -> Result<()> {
        let Some(id_tag) = value.id_tag else {
            return Ok(());
        };

        // We only care about bank cards.
        match id_tag.authorization_type {
            AuthorizationType::BankCard => (),
            _ => return Ok(()),
        }

        if let SessionStatus::Running = value.status {
            log::info!("The session is still running");
            return Ok(());
        }

        let total_cost = value
            .cost_chunks
            .unwrap_or_default()
            .into_iter()
            .fold(0, |acc, chunk| {
                acc + chunk.cost.unwrap_or(MoneyAmount { value: 0 }).value
            });

        let res = self
            .feig
            .commit_transaction(&id_tag.id_token.value, total_cost as u64)?;

        context
            .publisher
            .payment_terminal
            .bank_transaction_summary(BankTransactionSummary {
                session_token: Some(BankSessionToken {
                    token: Some(id_tag.id_token.value.clone()),
                }),
                transaction_data: Some(format!("{:06}", res.trace_number.unwrap_or_default())),
            })?;
        Ok(())
    }

    fn has_everything_enabled(&self) -> bool {
        let map_guard = self.connector_to_card_type.lock().unwrap();
        return map_guard.is_empty(); // If the map was not configured, we read all the card types for each connector
    }

    fn is_enabled(&self) -> bool {
        let map_guard = self.connector_to_card_type.lock().unwrap();
        map_guard.values().any(|v| !v.is_empty()) // If any connector has set any of the card types, it's enabled
    }

    fn connectors_for_card_type(&self, card_type: CardType) -> Vec<i64> {
        let map_guard = self.connector_to_card_type.lock().unwrap();
        let mut v: Vec<i64> = map_guard
            .iter()
            .filter_map(|(&key, vec)| {
                if vec.contains(&card_type) {
                    Some(key)
                } else {
                    None
                }
            })
            .collect();
        v.sort();
        v
    }
}

impl AuthTokenProviderServiceSubscriber for PaymentTerminalModule {}

impl BankSessionTokenProviderClientSubscriber for PaymentTerminalModule {}

impl OnReadySubscriber for PaymentTerminalModule {
    fn on_ready(&self, publishers: &ModulePublisher) {
        // Send the publishers to the main thread.
        self.tx.send(publishers.clone()).unwrap();
    }
}

impl SessionCostClientSubscriber for PaymentTerminalModule {
    fn on_session_cost(&self, context: &Context, value: SessionCost) {
        let res = self.on_session_cost_impl(context, value);
        match res {
            Ok(_) => log::debug!("Transaction successful"),
            Err(err) => log::error!("Transaction failed {err:}"),
        }
    }
}

impl PaymentTerminalServiceSubscriber for PaymentTerminalModule {
    fn enable_card_reading(
        &self,
        _context: &Context,
        connector_id: i64,
        supported_cards: Vec<CardType>,
    ) -> ::everestrs::Result<()> {
        let mut map_guard = self.connector_to_card_type.lock().unwrap();
        map_guard.insert(connector_id, supported_cards);
        Ok(())
    }
    fn allow_all_cards_for_every_connector(&self, _context: &Context) -> ::everestrs::Result<()> {
        let mut map_guard = self.connector_to_card_type.lock().unwrap();
        map_guard.clear();
        Ok(())
    }
}

impl PaymentTerminalModule {}

fn main() -> Result<()> {
    let config = get_config();
    log::info!("Received the config {config:?}");

    let pt_config = Config {
        terminal_id: config.terminal_id,
        feig_serial: config.feig_serial,
        ip_address: Ipv4Addr::from_str(&config.ip)?,
        feig_config: FeigConfig {
            currency: config.currency as usize,
            pre_authorization_amount: config.pre_authorization_amount as usize,
            read_card_timeout: config.read_card_timeout as u8,
            password: config.password as usize,
        },
        transactions_max_num: config.transactions_max_num as usize,
    };

    let (tx, rx) = channel();

    let pt_module = Arc::new(PaymentTerminalModule {
        tx,
        feig: SyncFeig::new(pt_config),
        connector_to_card_type: Mutex::new(HashMap::new()),
    });

    let _module = Module::new(
        pt_module.clone(),
        pt_module.clone(),
        pt_module.clone(),
        |_index| pt_module.clone(),
        |_index| pt_module.clone(),
    );

    let read_card_debounce = Duration::from_secs(config.read_card_debounce as u64);
    let publishers = rx.recv()?;
    loop {
        log::debug!("Waiting for transactions...");
        let res = pt_module.begin_transaction(&publishers);
        match res {
            Ok(()) => {
                log::info!("Started a transaction");
                std::thread::sleep(read_card_debounce);
            }
            Err(err) => log::error!("Failed to start a transaction {err:?}"),
        }
    }
}

#[cfg(test)]
mod tests {
    use self::generated::types::money::Currency;
    use self::generated::types::money::CurrencyCode;
    use self::generated::types::session_cost::SessionCostChunk;
    use self::generated::BankSessionTokenProviderClientPublisher;

    use super::*;
    use mockall::predicate::eq;
    use zvt_feig_terminal::feig::TransactionSummary;

    #[test]
    /// Unit tests for the `PaymentTerminalModule::begin_transaction`.
    fn payment_terminal_module__begin_transaction() {
        // Test first the case where we don't receive token.
        let parameters = [
            Err(::everestrs::Error::InvalidArgument("oh no")),
            Ok(BankSessionToken { token: None }),
        ];

        for input in parameters {
            let mut everest_mock = ModulePublisher::default();
            everest_mock
                .bank_session_token_slots
                .push(BankSessionTokenProviderClientPublisher::default());
            everest_mock.bank_session_token_slots[0]
                .expect_get_bank_session_token()
                .times(1)
                .return_once(|| input);

            let feig = SyncFeig::default();
            let (tx, _) = channel();

            let pt_module = PaymentTerminalModule {
                tx,
                feig,
                connector_to_card_type: Mutex::new(HashMap::new()),
            };

            assert!(pt_module.begin_transaction(&everest_mock).is_err());
        }

        // Now test the successful execution.
        let parameters = [
            (CardInfo::Bank, "my bank token", true),
            (
                CardInfo::MembershipCard("my membership token".to_string()),
                "my membership token",
                false,
            ),
        ];

        for (card_info, expected_token, expected_transaction) in parameters {
            let mut everest_mock = ModulePublisher::default();
            let mut feig_mock = SyncFeig::default();

            everest_mock
                .bank_session_token_slots
                .push(BankSessionTokenProviderClientPublisher::default());
            everest_mock.bank_session_token_slots[0]
                .expect_get_bank_session_token()
                .times(1)
                .return_once(|| {
                    Ok(BankSessionToken {
                        token: Some("my bank token".to_string()),
                    })
                });

            everest_mock
                .token_provider
                .expect_provided_token()
                .times(1)
                .withf(|arg| arg.id_token.value == expected_token.to_string())
                .return_once(|_| Ok(()));

            feig_mock
                .expect_read_card()
                .times(1)
                .return_once(|| Ok(card_info));

            if expected_transaction {
                feig_mock
                    .expect_begin_transaction()
                    .times(1)
                    .with(eq("my bank token"))
                    .return_once(|_| Ok(()));
            }

            let (tx, _) = channel();

            let pt_module = PaymentTerminalModule {
                tx,
                feig: feig_mock,
                connector_to_card_type: Mutex::new(HashMap::from([(
                    1 as i64,
                    vec![CardType::BankCard],
                )])),
            };

            assert!(pt_module.begin_transaction(&everest_mock).is_ok());
        }
    }
    #[test]
    /// Unit tests for the `PaymentTerminalModule::begin_transaction` with credit card acceptance.
    fn payment_terminal_module__begin_transaction_credit_cards_accepted() {
        // Now test the successful execution.
        let parameters = [
            (CardInfo::Bank, "my bank token", true, HashMap::new(), None),
            (
                CardInfo::Bank,
                "my bank token",
                true,
                HashMap::from([
                    (1 as i64, vec![CardType::BankCard]),
                    (2 as i64, vec![CardType::BankCard]),
                ]),
                Some(vec![1, 2]),
            ),
            (
                CardInfo::Bank,
                "my bank token",
                false,
                HashMap::from([
                    (1 as i64, vec![CardType::RfidCard]),
                    (2 as i64, vec![CardType::RfidCard]),
                ]),
                None,
            ),
            (
                CardInfo::Bank,
                "my bank token",
                true,
                HashMap::from([
                    (1 as i64, vec![CardType::BankCard]),
                    (2 as i64, vec![CardType::RfidCard]),
                ]),
                Some(vec![1]),
            ),
        ];

        for (
            card_info,
            expected_token,
            expected_transaction,
            connector_to_card_type,
            expected_connectors,
        ) in parameters
        {
            let mut everest_mock = ModulePublisher::default();
            let mut feig_mock = SyncFeig::default();

            everest_mock
                .bank_session_token_slots
                .push(BankSessionTokenProviderClientPublisher::default());
            everest_mock
                .bank_session_token_slots
                .push(BankSessionTokenProviderClientPublisher::default());

            everest_mock.bank_session_token_slots[0]
                .expect_get_bank_session_token()
                .times(1)
                .return_once(|| {
                    Ok(BankSessionToken {
                        token: Some("my bank token".to_string()),
                    })
                });
            if expected_transaction {
                everest_mock
                    .token_provider
                    .expect_provided_token()
                    .times(1)
                    .withf(move |arg| {
                        arg.id_token.value == expected_token.to_string()
                            && arg.connectors == expected_connectors
                    })
                    .return_once(|_| Ok(()));
            }

            feig_mock
                .expect_read_card()
                .times(1)
                .return_once(|| Ok(card_info));

            if expected_transaction {
                feig_mock
                    .expect_begin_transaction()
                    .times(1)
                    .with(eq("my bank token"))
                    .return_once(|_| Ok(()));
            }

            let (tx, _) = channel();

            let pt_module = PaymentTerminalModule {
                tx,
                feig: feig_mock,
                connector_to_card_type: Mutex::new(connector_to_card_type),
            };

            assert!(pt_module.begin_transaction(&everest_mock).is_ok());
        }
    }

    #[test]
    /// We test that we don't commit anything for inputs which should be ignored.
    fn payment_terminal__on_session_cost_impl__noop() {
        let parameters = [
            SessionCost {
                cost_chunks: None,
                currency: Currency {
                    code: Some(CurrencyCode::EUR),
                    decimals: None,
                },
                id_tag: Some(ProvidedIdToken::new(
                    String::new(),
                    AuthorizationType::OCPP,
                    None,
                )),
                status: SessionStatus::Finished,
                session_id: String::new(),
                idle_price: None,
                charging_price: None,
                next_period: None,
                message: None,
                qr_code: None,
            },
            SessionCost {
                cost_chunks: None,
                currency: Currency {
                    code: Some(CurrencyCode::EUR),
                    decimals: None,
                },
                id_tag: Some(ProvidedIdToken::new(
                    String::new(),
                    AuthorizationType::RFID,
                    None,
                )),
                status: SessionStatus::Finished,
                session_id: String::new(),
                idle_price: None,
                charging_price: None,
                next_period: None,
                message: None,
                qr_code: None,
            },
            SessionCost {
                cost_chunks: None,
                currency: Currency {
                    code: Some(CurrencyCode::EUR),
                    decimals: None,
                },
                id_tag: Some(ProvidedIdToken::new(
                    String::new(),
                    AuthorizationType::BankCard,
                    None,
                )),
                status: SessionStatus::Running,
                session_id: String::new(),
                idle_price: None,
                charging_price: None,
                next_period: None,
                message: None,
                qr_code: None,
            },
        ];

        for session_cost in parameters {
            let everest_mock = ModulePublisher::default();
            let context = Context {
                name: "foo",
                publisher: &everest_mock,
                index: 0,
            };
            let feig = SyncFeig::default();
            let (tx, _) = channel();

            let pt_module = PaymentTerminalModule {
                tx,
                feig,
                connector_to_card_type: Mutex::new(HashMap::from([(
                    1 as i64,
                    vec![CardType::BankCard],
                )])),
            };
            assert!(pt_module
                .on_session_cost_impl(&context, session_cost)
                .is_ok());
        }
    }

    #[test]
    /// We test that we commit the right amount for transactions which are for
    /// us.
    fn payment_terminal__on_session_cost_impl() {
        let parameters = [
            (
                SessionCost {
                    cost_chunks: None,
                    currency: Currency {
                        code: Some(CurrencyCode::EUR),
                        decimals: None,
                    },
                    id_tag: Some(ProvidedIdToken::new(
                        "token".to_string(),
                        AuthorizationType::BankCard,
                        None,
                    )),
                    status: SessionStatus::Finished,
                    session_id: String::new(),
                    idle_price: None,
                    charging_price: None,
                    next_period: None,
                    message: None,
                    qr_code: None,
                },
                0,
            ),
            (
                SessionCost {
                    cost_chunks: Some(Vec::new()),
                    currency: Currency {
                        code: Some(CurrencyCode::EUR),
                        decimals: None,
                    },
                    id_tag: Some(ProvidedIdToken::new(
                        "token".to_string(),
                        AuthorizationType::BankCard,
                        None,
                    )),
                    status: SessionStatus::Finished,
                    session_id: String::new(),
                    idle_price: None,
                    charging_price: None,
                    next_period: None,
                    message: None,
                    qr_code: None,
                },
                0,
            ),
            (
                SessionCost {
                    cost_chunks: Some(vec![SessionCostChunk {
                        category: None,
                        cost: None,
                        timestamp_from: None,
                        timestamp_to: None,
                        metervalue_from: None,
                        metervalue_to: None,
                    }]),
                    currency: Currency {
                        code: Some(CurrencyCode::EUR),
                        decimals: None,
                    },
                    id_tag: Some(ProvidedIdToken::new(
                        "token".to_string(),
                        AuthorizationType::BankCard,
                        None,
                    )),
                    status: SessionStatus::Finished,
                    session_id: String::new(),
                    idle_price: None,
                    charging_price: None,
                    next_period: None,
                    message: None,
                    qr_code: None,
                },
                0,
            ),
            (
                SessionCost {
                    cost_chunks: Some(vec![
                        SessionCostChunk {
                            category: None,
                            cost: Some(MoneyAmount { value: 1 }),
                            timestamp_from: None,
                            timestamp_to: None,
                            metervalue_from: None,
                            metervalue_to: None,
                        },
                        SessionCostChunk {
                            category: None,
                            cost: Some(MoneyAmount { value: 2 }),
                            timestamp_from: None,
                            timestamp_to: None,
                            metervalue_from: None,
                            metervalue_to: None,
                        },
                    ]),
                    currency: Currency {
                        code: Some(CurrencyCode::EUR),
                        decimals: None,
                    },
                    id_tag: Some(ProvidedIdToken::new(
                        "token".to_string(),
                        AuthorizationType::BankCard,
                        None,
                    )),
                    status: SessionStatus::Finished,
                    session_id: String::new(),
                    idle_price: None,
                    charging_price: None,
                    next_period: None,
                    message: None,
                    qr_code: None,
                },
                3,
            ),
        ];

        for (session_cost, amount) in parameters {
            let mut everest_mock = ModulePublisher::default();
            everest_mock
                .payment_terminal
                .expect_bank_transaction_summary()
                .times(1)
                .returning(|_| Ok(()));

            let context = Context {
                name: "foo",
                publisher: &everest_mock,
                index: 0,
            };
            let mut feig = SyncFeig::default();
            feig.expect_commit_transaction()
                .times(1)
                .with(eq("token"), eq(amount))
                .returning(|_, _| {
                    Ok(TransactionSummary {
                        terminal_id: None,
                        amount: None,
                        trace_number: None,
                        date: None,
                        time: None,
                    })
                });
            let (tx, _) = channel();

            let pt_module = PaymentTerminalModule {
                tx,
                feig,
                connector_to_card_type: Mutex::new(HashMap::from([(
                    1 as i64,
                    vec![CardType::BankCard],
                )])),
            };
            assert!(pt_module
                .on_session_cost_impl(&context, session_cost)
                .is_ok());
        }
    }

    #[test]
    /// Unit tests for the `PaymentTerminalModule::begin_transaction` with credit card acceptance.
    fn payment_terminal_module__begin_transaction_with_configuration() {
        {
            let expected_token = "my bank token";

            let mut everest_mock = ModulePublisher::default();
            let mut feig_mock = SyncFeig::default();

            everest_mock
                .bank_session_token_slots
                .push(BankSessionTokenProviderClientPublisher::default());
            everest_mock
                .bank_session_token_slots
                .push(BankSessionTokenProviderClientPublisher::default());

            everest_mock.bank_session_token_slots[0]
                .expect_get_bank_session_token()
                .times(3)
                .returning(|| {
                    Ok(BankSessionToken {
                        token: Some(expected_token.to_string()),
                    })
                });

            everest_mock
                .token_provider
                .expect_provided_token()
                .times(2)
                .withf(move |arg| {
                    arg.id_token.value == expected_token.to_string() && arg.connectors == None
                })
                .returning(|_| Ok(()));

            everest_mock
                .token_provider
                .expect_provided_token()
                .times(1)
                .withf(move |arg| {
                    arg.id_token.value == expected_token.to_string()
                        && arg.connectors == Some(vec![1])
                })
                .return_once(|_| Ok(()));

            feig_mock
                .expect_read_card()
                .times(3)
                .returning(|| Ok(CardInfo::Bank));

            feig_mock
                .expect_begin_transaction()
                .times(3)
                .with(eq("my bank token"))
                .returning(|_| Ok(()));

            let (tx, _) = channel();

            let pt_module = PaymentTerminalModule {
                tx,
                feig: feig_mock,
                connector_to_card_type: Mutex::new(HashMap::new()),
            };

            let context = Context {
                name: "foo",
                publisher: &everest_mock,
                index: 0,
            };

            assert!(pt_module.begin_transaction(&everest_mock).is_ok());

            let _ = pt_module.enable_card_reading(&context, 1, vec![CardType::BankCard]);

            assert!(pt_module.begin_transaction(&everest_mock).is_ok());

            let _ = pt_module.allow_all_cards_for_every_connector(&context);

            assert!(pt_module.begin_transaction(&everest_mock).is_ok());
        }
    }
}
