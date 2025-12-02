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
use generated::errors::payment_terminal::{Error as PTError, PaymentTerminalError};
use generated::types::{
    authorization::{AuthorizationType, IdToken, IdTokenType, ProvidedIdToken},
    money::MoneyAmount,
    payment_terminal::{BankSessionToken, BankTransactionSummary, CardType, RejectionReason},
    session_cost::{SessionCost, SessionStatus, TariffMessage},
};
use generated::{
    get_config, AuthTokenProviderServiceSubscriber, BankSessionTokenProviderClientSubscriber,
    Context, Module, ModulePublisher, OnReadySubscriber, PaymentTerminalServiceSubscriber,
    SessionCostClientSubscriber,
};
use std::cmp::min;
use std::collections::HashMap;
use std::sync::{mpsc::channel, mpsc::Sender, Arc, Mutex};
use std::time::Duration;
use std::{net::Ipv4Addr, str::FromStr};
use zvt::constants::ErrorMessages;
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

        pub fn begin_transaction(&self, token: &str, amount: usize) -> Result<()> {
            let mut inner = self.inner.lock().unwrap();
            self.rt.block_on(inner.begin_transaction(token, amount))
        }

        pub fn cancel_transaction(&self, token: &str) -> Result<()> {
            let mut inner = self.inner.lock().unwrap();
            self.rt.block_on(inner.cancel_transaction(token))
        }

        pub fn commit_transaction(&self, token: &str, amount: u64) -> Result<TransactionSummary> {
            let mut inner = self.inner.lock().unwrap();
            self.rt.block_on(inner.commit_transaction(token, amount))
        }

        pub fn configure(&self) -> Result<()> {
            let mut inner = self.inner.lock().unwrap();
            self.rt.block_on(inner.configure())
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
                r#type: match authorization_type {
                    AuthorizationType::BankCard => IdTokenType::NoAuthorization,
                    _ => IdTokenType::Local,
                },
                additional_info: None,
            },
            authorization_type,
            certificate: None,
            connectors,
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

    /// The configurable pre-auth.
    pre_authorization_amount: usize,
}

impl From<ErrorMessages> for RejectionReason {
    fn from(code: ErrorMessages) -> Self {
        match code {
            #[cfg(feature = "with_lavego_error_codes")]
            ErrorMessages::ContactlessTransactionCountExceeded => RejectionReason::PinRequired,
            #[cfg(feature = "with_lavego_error_codes")]
            ErrorMessages::PinEntryRequiredx33 => RejectionReason::PinRequired,
            #[cfg(feature = "with_lavego_error_codes")]
            ErrorMessages::PinEntryRequiredx3d => RejectionReason::PinRequired,
            #[cfg(feature = "with_lavego_error_codes")]
            ErrorMessages::PinEntryRequiredx41 => RejectionReason::PinRequired,
            ErrorMessages::PinProcessingNotPossible
                | ErrorMessages::NecessaryDeviceNotPresentOrDefective => RejectionReason::PinRequired,
            ErrorMessages::CreditNotSufficient => RejectionReason::InsufficientFunds,
            ErrorMessages::PaymentMethodNotSupported => RejectionReason::CardNotSupported,
            ErrorMessages::AbortViaTimeoutOrAbortKey => RejectionReason::Aborted,
            #[cfg(feature = "with_lavego_error_codes")]
            ErrorMessages::Declined => RejectionReason::Timeout,
            ErrorMessages::ReceiverNotReady
                | ErrorMessages::SystemError
                | ErrorMessages::ErrorFromDialUp   // error from dial-up/communication fault
              => RejectionReason::Timeout,
            _ => RejectionReason::Unknown,
        }
    }
}

impl PaymentTerminalModule {
    /// Waits for a card and begins a transaction (sends an auth token).
    ///
    /// In case of a bank card we start the transaction on the payment terminal.
    /// In case of membership cards we just issue the token. In both cases we
    /// don't flag the token as pre-validated to allow the consumers to add
    /// custom validation steps on top.
    fn begin_transaction(&self, publishers: &ModulePublisher) -> Result<()> {
        const NO_CONNECTORS: Option<Vec<i64>> = Some(Vec::new());
        let mut token: Option<String> = None;

        // Wait for the card.
        let mut read_card_loop = || -> Result<CardInfo> {
            let mut timeout = std::time::Instant::now();
            let mut backoff_seconds = 1;

            loop {
                if let Err(inner) = self.feig.configure() {
                    log::warn!("Failed to configure: {inner:?}");
                    let inner: PTError = inner.into();
                    publishers.payment_terminal.raise_error(inner.into())
                } else {
                    // TODO: use the clear all interface when it is implemented in the framework
                    const ALL_PAYMENT_TERMINAL_ERRORS: [PaymentTerminalError; 3] = [
                        PaymentTerminalError::TerminalIdNotSet,
                        PaymentTerminalError::IncorrectDeviceId,
                        PaymentTerminalError::GenericPaymentTerminalError,
                    ];

                    for error in ALL_PAYMENT_TERMINAL_ERRORS {
                        publishers
                            .payment_terminal
                            .clear_error(PTError::PaymentTerminal(error));
                    }
                }

                let get_connectors_for_rfid_card = self.get_connectors_for(CardType::RfidCard);
                let get_connectors_for_bank_card = self.get_connectors_for(CardType::BankCard);

                if get_connectors_for_rfid_card == NO_CONNECTORS
                    && get_connectors_for_bank_card == NO_CONNECTORS
                {
                    log::debug!("Reading is disabled, waiting...");
                    std::thread::sleep(Duration::from_secs(1));
                    continue;
                }

                // Attempting to get an invoice token
                if token.is_none() && get_connectors_for_bank_card != NO_CONNECTORS {
                    if let Some(publisher) = publishers.bank_session_token_slots.get(0) {
                        if timeout.elapsed() > Duration::from_secs(0) {
                            token = publisher.get_bank_session_token()?.token;

                            // Poor man's backoff to avoid a busy loop
                            const MAX_BACKOFF_SECONDS: u64 = 60;
                            if token.is_none() {
                                backoff_seconds = min(backoff_seconds * 2, MAX_BACKOFF_SECONDS);
                                timeout = std::time::Instant::now()
                                    + Duration::from_secs(backoff_seconds);
                                log::info!(
                                    "Failed to receive invoice token, retrying in {backoff_seconds} seconds"
                                );
                            } else {
                                log::info!("Received the invoice token {token:?}");
                            }
                        }
                    }
                }

                match self.feig.read_card() {
                    Ok(card_info) => return Ok(card_info),
                    Err(e) => {
                        if let Some(Error::NoCardPresented) = e.downcast_ref::<Error>() {
                            log::debug!("No card presented");
                            continue;
                        } else {
                            if let Some(rejection_reason) = e.downcast_ref::<ErrorMessages>() {
                                log::info!("Recieved rejection reason {}", rejection_reason);

                                publishers
                                    .payment_terminal
                                    .rejection_reason((*rejection_reason).into())?;
                            }
                            return Err(anyhow::anyhow!("Failed to read card: {e:?}"));
                        }
                    }
                };
            }
        };
        let card_info = read_card_loop()?;

        let provided_token = match card_info {
            CardInfo::Bank => {
                let mut get_connectors_for_bank_card = self.get_connectors_for(CardType::BankCard);
                if get_connectors_for_bank_card == NO_CONNECTORS {
                    log::info!("Bank cards disabled, ignoring");
                } else if token.is_none() {
                    log::info!("No invoice token provided, ignoring bank card");
                    get_connectors_for_bank_card = NO_CONNECTORS;
                }

                if get_connectors_for_bank_card != NO_CONNECTORS {
                    if let Err(err) = self
                        .feig
                        .begin_transaction(token.as_ref().unwrap(), self.pre_authorization_amount)
                    {
                        log::warn!("Failed to start a transaction: {err:?}");
                        match err.downcast_ref::<ErrorMessages>() {
                            Some(rejection_reason) => {
                                log::info!("Recieved rejection reason {}", rejection_reason);

                                publishers
                                    .payment_terminal
                                    .rejection_reason((*rejection_reason).into())?;
                            }
                            None => log::info!("No error code provided"),
                        };
                        get_connectors_for_bank_card = NO_CONNECTORS;
                    }
                }

                // Reuse the bank token as invoice token so we can use the
                // invoice token later on to commit our transactions.
                ProvidedIdToken::new(
                    token.unwrap_or("INVALID".to_string()),
                    AuthorizationType::BankCard,
                    get_connectors_for_bank_card,
                )
            }
            CardInfo::MembershipCard(id_token) => {
                let get_connectors_for_rfid_card = self.get_connectors_for(CardType::RfidCard);
                if get_connectors_for_rfid_card == NO_CONNECTORS {
                    log::info!("Rfid cards disabled, ignoring");
                }
                ProvidedIdToken::new(
                    id_token,
                    AuthorizationType::RFID,
                    get_connectors_for_rfid_card,
                )
            }
        };
        publishers.token_provider.provided_token(provided_token)?;
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

    /// Returns the connectors enabled for the given `CardType`.
    ///
    /// Following the token convention Some(Vec::new()) means disabled and
    /// `None` means enabled for all. A non-empty list enabled the card-type
    /// only for specific connectors.
    fn get_connectors_for(&self, card_type: CardType) -> Option<Vec<i64>> {
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

        if !map_guard.is_empty() {
            Some(v)
        } else {
            None
        }
    }
}

impl AuthTokenProviderServiceSubscriber for PaymentTerminalModule {}

impl BankSessionTokenProviderClientSubscriber for PaymentTerminalModule {}

impl From<anyhow::Error> for PTError {
    fn from(value: anyhow::Error) -> Self {
        match value.downcast_ref::<Error>() {
            Some(inner) => match inner {
                Error::TidMismatch => {
                    PTError::PaymentTerminal(PaymentTerminalError::TerminalIdNotSet)
                }
                Error::IncorrectDeviceId { .. } => {
                    PTError::PaymentTerminal(PaymentTerminalError::IncorrectDeviceId)
                }
                _ => PTError::PaymentTerminal(PaymentTerminalError::GenericPaymentTerminalError),
            },
            None => PTError::PaymentTerminal(PaymentTerminalError::GenericPaymentTerminalError),
        }
    }
}

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

    fn on_tariff_message(&self, _context: &Context, value: TariffMessage) {
        for message in value.messages {
            log::debug!("Received tariff message {0:}", message.content);
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
            read_card_timeout: config.read_card_timeout as u8,
            password: config.password as usize,
            end_of_day_max_interval: config.end_of_day_max_interval as u64,
        },
        transactions_max_num: config.transactions_max_num as usize,
    };

    let (tx, rx) = channel();

    let pt_module = Arc::new(PaymentTerminalModule {
        tx,
        feig: SyncFeig::new(pt_config),
        connector_to_card_type: Mutex::new(HashMap::new()),
        pre_authorization_amount: config.pre_authorization_amount as usize,
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
    fn payment_terminal_module__begin_transaction_failure_when_token_is_err() {
        // Test first the case where we receive an error from the bank session token provider.
        let mut feig_mock = SyncFeig::default();
        feig_mock.expect_configure().times(1).return_once(|| Ok(()));

        let mut everest_mock = ModulePublisher::default();
        everest_mock
            .payment_terminal
            .expect_clear_error()
            .times(3)
            .return_const(());
        everest_mock
            .bank_session_token_slots
            .push(BankSessionTokenProviderClientPublisher::default());
        let token = Err(::everestrs::Error::HandlerException("oh no".to_string()));
        everest_mock.bank_session_token_slots[0]
            .expect_get_bank_session_token()
            .times(1)
            .return_once(|| token);
        everest_mock
            .payment_terminal
            .expect_clear_error()
            .times(0)
            .return_const(());

        let (tx, _) = channel();

        let pt_module = PaymentTerminalModule {
            tx,
            feig: feig_mock,
            connector_to_card_type: Mutex::new(HashMap::new()),
            pre_authorization_amount: 11,
        };

        assert!(pt_module.begin_transaction(&everest_mock).is_err());
    }

    #[test]
    fn payment_terminal_module__begin_transaction_test() {
        // When the token is None, we still expect membership card to work
        let card_info = CardInfo::MembershipCard("my membership token".to_string());
        let expected_token = "my membership token";
        let mut everest_mock = ModulePublisher::default();
        let mut feig_mock = SyncFeig::default();

        feig_mock
            .expect_read_card()
            .times(1)
            .return_once(|| Ok(card_info));

        everest_mock
            .payment_terminal
            .expect_clear_error()
            .times(3)
            .return_const(());
        everest_mock
            .bank_session_token_slots
            .push(BankSessionTokenProviderClientPublisher::default());
        everest_mock.bank_session_token_slots[0]
            .expect_get_bank_session_token()
            .times(1)
            .return_once(|| Ok(BankSessionToken { token: None }));

        everest_mock
            .token_provider
            .expect_provided_token()
            .times(1)
            .withf(|arg| arg.id_token.value == expected_token.to_string())
            .return_once(|_| Ok(()));

        feig_mock.expect_configure().times(1).return_once(|| Ok(()));

        let (tx, _) = channel();

        let pt_module = PaymentTerminalModule {
            tx,
            feig: feig_mock,
            connector_to_card_type: Mutex::new(HashMap::new()),
            pre_authorization_amount: 20,
        };

        assert!(pt_module.begin_transaction(&everest_mock).is_ok());
    }

    #[test]
    /// Unit tests for the `PaymentTerminalModule::begin_transaction`.
    fn payment_terminal_module__begin_transaction_success() {
        // Test the successful execution with BankCard.
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
                .payment_terminal
                .expect_clear_error()
                .times(3)
                .return_const(());
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

            let pre_authorization_amount = 42;
            if expected_transaction {
                feig_mock
                    .expect_begin_transaction()
                    .times(1)
                    .with(eq("my bank token"), eq(pre_authorization_amount))
                    .return_once(|_, _| Ok(()));
            }

            feig_mock.expect_configure().times(1).return_once(|| Ok(()));

            let (tx, _) = channel();

            let pt_module = PaymentTerminalModule {
                tx,
                feig: feig_mock,
                connector_to_card_type: Mutex::new(HashMap::from([(
                    1 as i64,
                    vec![CardType::BankCard],
                )])),
                pre_authorization_amount,
            };

            assert!(pt_module.begin_transaction(&everest_mock).is_ok());
        }
    }

    #[test]
    /// Unit tests for the `PaymentTerminalModule::begin_transaction` with credit card acceptance.
    fn payment_terminal_module__begin_transaction_credit_cards_accepted() {
        struct ExpectedToken {
            token_id: String,
            connectors: Option<Vec<i64>>,
        }

        const BANK_TOKEN: &str = "my bank token";
        const RIFD_TOKEN: &str = "my rfid token";

        // Now test the successful execution.
        let parameters = [
            // The case where we don't have any constraints.
            (
                CardInfo::Bank,
                true,
                true,
                ExpectedToken {
                    token_id: BANK_TOKEN.to_string(),
                    connectors: None,
                },
                HashMap::new(),
            ),
            // The case where we allow only bank cards and receive a bank card.
            // We expect that we can issue a bank token.
            (
                CardInfo::Bank,
                true,
                true,
                ExpectedToken {
                    token_id: BANK_TOKEN.to_string(),
                    connectors: Some(vec![1, 2]),
                },
                HashMap::from([
                    (1 as i64, vec![CardType::BankCard]),
                    (2 as i64, vec![CardType::BankCard]),
                ]),
            ),
            // The case where we only allow Rfid cards but receive a bank card.
            // We issue an invalid token.
            (
                CardInfo::Bank,
                false,
                false,
                ExpectedToken {
                    token_id: "INVALID".to_string(),
                    connectors: Some(Vec::new()),
                },
                HashMap::from([
                    (1 as i64, vec![CardType::RfidCard]),
                    (2 as i64, vec![CardType::RfidCard]),
                ]),
            ),
            // The case where we contraint the bank card to only one connector.
            (
                CardInfo::Bank,
                true,
                true,
                ExpectedToken {
                    token_id: BANK_TOKEN.to_string(),
                    connectors: Some(vec![1]),
                },
                HashMap::from([
                    (1 as i64, vec![CardType::BankCard]),
                    (2 as i64, vec![CardType::RfidCard]),
                ]),
            ),
            // The case where we allow only bank cards and receive a rfid card.
            // We issue an invalid token.
            (
                CardInfo::MembershipCard(RIFD_TOKEN.to_string()),
                true,
                false,
                ExpectedToken {
                    token_id: RIFD_TOKEN.to_string(),
                    connectors: Some(Vec::new()),
                },
                HashMap::from([
                    (1 as i64, vec![CardType::BankCard]),
                    (2 as i64, vec![CardType::BankCard]),
                ]),
            ),
            // The case where we contraint the rfid card to only one connector.
            (
                CardInfo::MembershipCard(RIFD_TOKEN.to_string()),
                true,
                false,
                ExpectedToken {
                    token_id: RIFD_TOKEN.to_string(),
                    connectors: Some(vec![2]),
                },
                HashMap::from([
                    (1 as i64, vec![CardType::BankCard]),
                    (2 as i64, vec![CardType::RfidCard]),
                ]),
            ),
        ];

        for (
            card_info,
            expected_invoice_token,
            expected_transaction,
            expected_token,
            connector_to_card_type,
        ) in parameters
        {
            let mut everest_mock = ModulePublisher::default();
            let mut feig_mock = SyncFeig::default();
            feig_mock.expect_configure().times(1).return_once(|| Ok(()));
            everest_mock
                .payment_terminal
                .expect_clear_error()
                .times(3)
                .return_const(());

            everest_mock
                .bank_session_token_slots
                .push(BankSessionTokenProviderClientPublisher::default());

            // let expected_token_clone = expected_token.clone();
            everest_mock.bank_session_token_slots[0]
                .expect_get_bank_session_token()
                .times(if expected_invoice_token { 1 } else { 0 })
                .return_once(|| {
                    Ok(BankSessionToken {
                        token: Some(BANK_TOKEN.to_string()),
                    })
                });

            everest_mock
                .token_provider
                .expect_provided_token()
                .times(1)
                .withf(move |arg| {
                    arg.id_token.value == expected_token.token_id
                        && arg.connectors == expected_token.connectors
                })
                .return_once(|_| Ok(()));

            feig_mock
                .expect_read_card()
                .times(1)
                .return_once(|| Ok(card_info));

            let pre_authorization_amount = 41;
            if expected_transaction {
                feig_mock
                    .expect_begin_transaction()
                    .times(1)
                    .with(eq("my bank token"), eq(pre_authorization_amount))
                    .return_once(|_, _| Ok(()));
            }

            let (tx, _) = channel();

            let pt_module = PaymentTerminalModule {
                tx,
                feig: feig_mock,
                connector_to_card_type: Mutex::new(connector_to_card_type),
                pre_authorization_amount,
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
                pre_authorization_amount: 50,
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
                pre_authorization_amount: 20,
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
            feig_mock.expect_configure().times(3).returning(|| Ok(()));
            everest_mock
                .payment_terminal
                .expect_clear_error()
                .times(9)
                .return_const(());

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

            let pre_authorization_amount = 50;
            feig_mock
                .expect_begin_transaction()
                .times(3)
                .with(eq("my bank token"), eq(pre_authorization_amount))
                .returning(|_, _| Ok(()));

            let (tx, _) = channel();

            let pt_module = PaymentTerminalModule {
                tx,
                feig: feig_mock,
                connector_to_card_type: Mutex::new(HashMap::new()),
                pre_authorization_amount,
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
