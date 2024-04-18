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
    bank_transaction::{BankSessionToken, BankTransactionSummary},
    money::MoneyAmount,
    session_cost::{SessionCost, SessionStatus},
};
use generated::{
    get_config, AuthTokenProviderServiceSubscriber, BankSessionTokenProviderClientSubscriber,
    BankTransactionSummaryProviderServiceSubscriber, Context, Module, ModulePublisher,
    OnReadySubscriber, SessionCostClientSubscriber,
};
use std::sync::{mpsc::channel, mpsc::Sender, Arc};
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
    fn new(id_token: String, authorization_type: AuthorizationType) -> Self {
        Self {
            parent_id_token: None,
            id_token: IdToken {
                value: id_token,
                r#type: IdTokenType::Local,
                additional_info: None
            },
            authorization_type,
            certificate: None,
            connectors: None,
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
        let invoice_token = publishers.bank_session_token.get_bank_session_token()?;
        let token = invoice_token
            .token
            .ok_or(anyhow::anyhow!("No token received"))?;
        log::info!("Received the BankSessionToken {token}");

        // Wait for the card.
        let read_card_loop = || -> Result<CardInfo> {
            loop {
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

        let provided_token = match card_info {
            CardInfo::Bank => {
                self.feig.begin_transaction(&token)?;

                // Reuse the bank token as invoice token so we can use the
                // invoice token later on to commit our transactions.
                ProvidedIdToken::new(token, AuthorizationType::BankCard)
            }
            CardInfo::MembershipCard(id_token) => {
                ProvidedIdToken::new(id_token, AuthorizationType::RFID)
            }
        };
        publishers.token_provider.provided_token(provided_token)?;
        Ok(())
    }

    /// The implementation of the `SessionCostClientSubscriber::on_session_cost`,
    /// but here we can return errors.
    fn on_session_cost_impl(&self, context: &Context, value: SessionCost) -> Result<()> {
        // We only care about bank cards.
        match value.id_tag.authorization_type {
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
            .commit_transaction(&value.id_tag.id_token.value, total_cost as u64)?;

        context
            .publisher
            .bank_transaction_summary
            .bank_transaction_summary(BankTransactionSummary {
                session_token: Some(BankSessionToken {
                    token: Some(value.id_tag.id_token.value.clone()),
                }),
                transaction_data: Some(format!("{:06}", res.trace_number.unwrap_or_default())),
            })?;
        Ok(())
    }
}

impl AuthTokenProviderServiceSubscriber for PaymentTerminalModule {}

impl BankTransactionSummaryProviderServiceSubscriber for PaymentTerminalModule {}

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
    });

    let _module = Module::new(
        pt_module.clone(),
        pt_module.clone(),
        pt_module.clone(),
        pt_module.clone(),
        pt_module.clone(),
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
                .bank_session_token
                .expect_get_bank_session_token()
                .times(1)
                .return_once(|| input);

            let feig = SyncFeig::default();
            let (tx, _) = channel();

            let pt_module = PaymentTerminalModule { tx, feig };

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
                .bank_session_token
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
                    code: CurrencyCode::EUR,
                    decimals: None,
                },
                id_tag: ProvidedIdToken::new(String::new(), AuthorizationType::OCPP),
                status: SessionStatus::Finished,
            },
            SessionCost {
                cost_chunks: None,
                currency: Currency {
                    code: CurrencyCode::EUR,
                    decimals: None,
                },
                id_tag: ProvidedIdToken::new(String::new(), AuthorizationType::RFID),
                status: SessionStatus::Finished,
            },
            SessionCost {
                cost_chunks: None,
                currency: Currency {
                    code: CurrencyCode::EUR,
                    decimals: None,
                },
                id_tag: ProvidedIdToken::new(String::new(), AuthorizationType::BankCard),
                status: SessionStatus::Running,
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

            let pt_module = PaymentTerminalModule { tx, feig };
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
                        code: CurrencyCode::EUR,
                        decimals: None,
                    },
                    id_tag: ProvidedIdToken::new("token".to_string(), AuthorizationType::BankCard),
                    status: SessionStatus::Finished,
                },
                0,
            ),
            (
                SessionCost {
                    cost_chunks: Some(Vec::new()),
                    currency: Currency {
                        code: CurrencyCode::EUR,
                        decimals: None,
                    },
                    id_tag: ProvidedIdToken::new("token".to_string(), AuthorizationType::BankCard),
                    status: SessionStatus::Finished,
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
                    }]),
                    currency: Currency {
                        code: CurrencyCode::EUR,
                        decimals: None,
                    },
                    id_tag: ProvidedIdToken::new("token".to_string(), AuthorizationType::BankCard),
                    status: SessionStatus::Finished,
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
                        },
                        SessionCostChunk {
                            category: None,
                            cost: Some(MoneyAmount { value: 2 }),
                            timestamp_from: None,
                            timestamp_to: None,
                        },
                    ]),
                    currency: Currency {
                        code: CurrencyCode::EUR,
                        decimals: None,
                    },
                    id_tag: ProvidedIdToken::new("token".to_string(), AuthorizationType::BankCard),
                    status: SessionStatus::Finished,
                },
                3,
            ),
        ];

        for (session_cost, amount) in parameters {
            let mut everest_mock = ModulePublisher::default();
            everest_mock
                .bank_transaction_summary
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

            let pt_module = PaymentTerminalModule { tx, feig };
            assert!(pt_module
                .on_session_cost_impl(&context, session_cost)
                .is_ok());
        }
    }
}
