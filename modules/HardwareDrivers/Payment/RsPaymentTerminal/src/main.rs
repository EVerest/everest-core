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
    authorization::{
        AuthorizationStatus, AuthorizationType, IdToken, IdTokenType, ProvidedIdToken,
        ValidationResult,
    },
    money::MoneyAmount,
    payment_terminal::{BankSessionToken, BankTransactionSummary},
    session_cost::{SessionCost, SessionStatus, TariffMessage},
};
use generated::{
    get_config, AuthTokenProviderServiceSubscriber, AuthTokenValidatorServiceSubscriber,
    BankSessionTokenProviderClientSubscriber, Context, Module, ModulePublisher, OnReadySubscriber,
    PaymentTerminalServiceSubscriber, SessionCostClientSubscriber,
};
use std::cmp::min;
use std::collections::HashMap;
use std::hash::{Hash, Hasher};
use std::sync::{mpsc::channel, mpsc::Sender, Arc, Mutex};
use std::time::Duration;
use std::{net::Ipv4Addr, str::FromStr};
use zvt::constants::ErrorMessages;
use zvt_feig_terminal::config::{Config, FeigConfig};
use zvt_feig_terminal::feig::{CardInfo, Error};

const INVALID_BANK_TOKEN: &str = "PAYMENT_TERMINAL_INVALID";

mod sync_feig {
    use anyhow::Result;
    use std::sync::Mutex;
    use zvt_feig_terminal::{
        config::Config,
        feig::{CardInfo, Feig, TransactionSummary},
    };

    pub struct SyncFeig {
        /// Tokio runtime to call the Feig functions.
        rt: tokio::runtime::Runtime,

        /// The async impl of the Feig.
        inner: Mutex<Feig>,
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
                inner: Mutex::new(feig),
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
                r#type: IdTokenType::Local,
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

impl From<ErrorMessages> for AuthorizationStatus {
    fn from(code: ErrorMessages) -> Self {
        match code {
            #[cfg(feature = "with_lavego_error_codes")]
            ErrorMessages::ContactlessTransactionCountExceeded => AuthorizationStatus::PinRequired,
            #[cfg(feature = "with_lavego_error_codes")]
            ErrorMessages::PinEntryRequiredx33 => AuthorizationStatus::PinRequired,
            #[cfg(feature = "with_lavego_error_codes")]
            ErrorMessages::PinEntryRequiredx3d => AuthorizationStatus::PinRequired,
            #[cfg(feature = "with_lavego_error_codes")]
            ErrorMessages::PinEntryRequiredx41 => AuthorizationStatus::PinRequired,
            ErrorMessages::PinProcessingNotPossible
                | ErrorMessages::NecessaryDeviceNotPresentOrDefective => AuthorizationStatus::PinRequired,
            ErrorMessages::CreditNotSufficient => AuthorizationStatus::NoCredit,
            ErrorMessages::PaymentMethodNotSupported => AuthorizationStatus::Blocked,
            ErrorMessages::AbortViaTimeoutOrAbortKey => AuthorizationStatus::Invalid,
            #[cfg(feature = "with_lavego_error_codes")]
            ErrorMessages::Declined => AuthorizationStatus::Blocked,
            ErrorMessages::ReceiverNotReady
                | ErrorMessages::SystemError
                | ErrorMessages::ErrorFromDialUp   // error from dial-up/communication fault
              => AuthorizationStatus::Timeout,
            _ => AuthorizationStatus::Unknown,
        }
    }
}

impl Hash for AuthorizationType {
    fn hash<H: Hasher>(&self, state: &mut H) {
        std::mem::discriminant(self).hash(state);
    }
}

impl Eq for AuthorizationType {}

/// Main struct for this module.
pub struct PaymentTerminalModule {
    /// Sender for the `ModulePublisher` -> to get the publisher from `on_ready`
    /// into the main thread.
    tx: Sender<ModulePublisher>,

    /// The Feig interface.
    feig: SyncFeig,

    /// The mapping of supported connectors for every card type. None means
    /// no restrictions, Some(vec![]) means no connector.
    card_type_to_connector: Mutex<HashMap<AuthorizationType, Option<Vec<i64>>>>,

    /// The configurable pre-auth.
    pre_authorization_amount: usize,
}

impl PaymentTerminalModule {
    /// Waits for a card and generates an auth token.
    ///
    /// Regardless of the card type we don't flag the token as pre-validated to
    /// allow the consumers to add custom validation steps on top. For bank
    /// cards use the auth_token_validation to pre-authorize money.
    fn read_card(&self, publishers: &ModulePublisher) -> Result<()> {
        let mut token: Option<String> = None;

        // Wait for the card.
        let mut read_card_loop = || -> CardInfo {
            let mut timeout = std::time::Instant::now();
            let mut backoff_seconds = 1;

            loop {
                if let Err(inner) = self.feig.configure() {
                    log::warn!("Failed to configure: {inner:?}");
                    let inner: PTError = inner.into();
                    publishers.payment_terminal.raise_error(inner.into());
                    continue;
                } else {
                    publishers.payment_terminal.clear_all_errors();
                }

                // Attempting to get an invoice token
                if token.is_none() {
                    if let Some(publisher) = publishers.bank_session_token_slots.get(0) {
                        if timeout.elapsed() > Duration::from_secs(0) {
                            token = publisher.get_bank_session_token().map_or(None, |v| v.token);

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
                    Ok(card_info) => return card_info,
                    Err(e) => {
                        if let Some(Error::NoCardPresented) = e.downcast_ref::<Error>() {
                            log::debug!("No card presented");
                        } else {
                            log::warn!("Failed to read a card {e:?}");
                            // Cleared in the next loop if we can configure the
                            // feig.
                            let inner: PTError = e.into();
                            publishers.payment_terminal.raise_error(inner.into());
                        }
                    }
                };
            }
        };
        let card_info = read_card_loop();
        let mut map_guard = self.card_type_to_connector.lock().unwrap();

        let provided_token = match card_info {
            CardInfo::Bank => ProvidedIdToken::new(
                // If we don't have a valid token, still issue the token but
                // reject it in the validation.
                token.unwrap_or(INVALID_BANK_TOKEN.to_string()),
                AuthorizationType::BankCard,
                map_guard
                    .entry(AuthorizationType::BankCard)
                    .or_default()
                    .clone(),
            ),
            CardInfo::MembershipCard(id_token) => ProvidedIdToken::new(
                id_token,
                AuthorizationType::RFID,
                map_guard
                    .entry(AuthorizationType::RFID)
                    .or_default()
                    .clone(),
            ),
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

    fn on_tariff_message(&self, _context: &Context, value: TariffMessage) {
        for message in value.messages {
            log::debug!("Received tariff message {0:}", message.content);
        }
    }
}

impl From<AuthorizationStatus> for ValidationResult {
    fn from(value: AuthorizationStatus) -> Self {
        ValidationResult {
            authorization_status: value,
            tariff_messages: Vec::new(),
            allowed_energy_transfer_modes: None,
            certificate_status: None,
            evse_ids: None,
            expiry_time: None,
            parent_id_token: None,
            reservation_id: None,
        }
    }
}

impl AuthTokenValidatorServiceSubscriber for PaymentTerminalModule {
    fn validate_token(
        &self,
        _context: &Context,
        provided_token: ProvidedIdToken,
    ) -> ::everestrs::Result<ValidationResult> {
        if provided_token.authorization_type != AuthorizationType::BankCard {
            log::warn!(
                "{:?} not supported: can only validate `AuthorizationType::BankCard`",
                provided_token.authorization_type
            );
            return Ok(AuthorizationStatus::Invalid.into());
        }

        if &provided_token.id_token.value == INVALID_BANK_TOKEN {
            log::warn!("Received an {INVALID_BANK_TOKEN}");
            return Ok(AuthorizationStatus::Invalid.into());
        }

        if let Err(err) = self.feig.begin_transaction(
            &provided_token.id_token.value,
            self.pre_authorization_amount,
        ) {
            log::warn!("Failed to start a transaction: {err:?}");
            match err.downcast_ref::<ErrorMessages>() {
                Some(rejection_reason) => {
                    log::info!("Recieved rejection reason {}", rejection_reason);
                    let status: AuthorizationStatus = (*rejection_reason).into();
                    return Ok(status.into());
                }
                None => {
                    log::info!("No error code provided");
                    return Ok(AuthorizationStatus::Invalid.into());
                }
            };
        }

        Ok(AuthorizationStatus::Accepted.into())
    }
}

impl PaymentTerminalServiceSubscriber for PaymentTerminalModule {
    fn enable_card_reading(
        &self,
        _context: &Context,
        connector_id: i64,
        supported_cards: Vec<AuthorizationType>,
    ) -> ::everestrs::Result<()> {
        let mut map_guard = self.card_type_to_connector.lock().unwrap();

        for (key, val) in map_guard.iter_mut() {
            match supported_cards.iter().find(|card| *card == key) {
                Some(_) => {
                    // Card is allowed -> add it to the connector
                    let connector_ids = val.get_or_insert_default();
                    if let Err(idx) = connector_ids.binary_search(&connector_id) {
                        connector_ids.insert(idx, connector_id);
                    }
                }
                None => {
                    // Card is forbidden -> remove the connector from that
                    let connector_ids = val.get_or_insert_default();
                    if let Ok(idx) = connector_ids.binary_search(&connector_id) {
                        connector_ids.remove(idx);
                    }
                }
            }
        }
        Ok(())
    }

    fn allow_all_cards_for_every_connector(&self, _context: &Context) -> ::everestrs::Result<()> {
        let mut map_guard = self.card_type_to_connector.lock().unwrap();
        *map_guard = HashMap::from_iter([
            (AuthorizationType::BankCard, None),
            (AuthorizationType::RFID, None),
        ]);
        Ok(())
    }
}

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
        card_type_to_connector: Mutex::new(HashMap::from_iter([
            (AuthorizationType::BankCard, None),
            (AuthorizationType::RFID, None),
        ])),
        pre_authorization_amount: config.pre_authorization_amount as usize,
    });

    let _module = Module::new(
        pt_module.clone(),
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
        let res = pt_module.read_card(&publishers);
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

    impl From<SyncFeig> for PaymentTerminalModule {
        fn from(feig_mock: SyncFeig) -> Self {
            let (tx, _) = channel();

            PaymentTerminalModule {
                tx,
                feig: feig_mock,
                card_type_to_connector: Mutex::new(HashMap::from_iter([
                    (AuthorizationType::BankCard, None),
                    (AuthorizationType::RFID, None),
                ])),
                pre_authorization_amount: 11,
            }
        }
    }

    #[test]
    fn payment_terminal_module__read_card__get_bank_session_token_failed() {
        let PARAMETERS = [
            (
                CardInfo::Bank,
                AuthorizationType::BankCard,
                INVALID_BANK_TOKEN,
            ),
            (
                CardInfo::MembershipCard("some id".to_string()),
                AuthorizationType::RFID,
                "some id",
            ),
        ];

        for (card_info, authorization_type, id_token) in PARAMETERS {
            let mut feig_mock = SyncFeig::default();
            feig_mock.expect_configure().times(1).return_once(|| Ok(()));
            feig_mock
                .expect_read_card()
                .times(1)
                .return_once(|| Ok(card_info));

            let mut everest_mock = ModulePublisher::default();
            everest_mock
                .bank_session_token_slots
                .push(BankSessionTokenProviderClientPublisher::default());

            everest_mock.bank_session_token_slots[0]
                .expect_get_bank_session_token()
                .times(1)
                .return_once(|| Err(::everestrs::Error::HandlerException("oh no".to_string())));

            everest_mock
                .token_provider
                .expect_provided_token()
                .times(1)
                .withf(move |arg| {
                    &arg.id_token.value == id_token && arg.authorization_type == authorization_type
                })
                .return_once(|_| Ok(()));

            everest_mock
                .payment_terminal
                .expect_clear_all_errors()
                .times(1)
                .return_once(|| ());

            let pt_module: PaymentTerminalModule = feig_mock.into();
            assert!(pt_module.read_card(&everest_mock).is_ok());
        }
    }

    #[test]
    fn payment_terminal_module__read_card__no_bank_session_token() {
        let PARAMETERS = [
            (
                CardInfo::Bank,
                AuthorizationType::BankCard,
                INVALID_BANK_TOKEN,
            ),
            (
                CardInfo::MembershipCard("some id".to_string()),
                AuthorizationType::RFID,
                "some id",
            ),
        ];
        // Here we don't have a bank session provider defined.
        for (card_info, authorization_type, id_token) in PARAMETERS {
            let mut feig_mock = SyncFeig::default();
            feig_mock.expect_configure().times(1).return_once(|| Ok(()));
            feig_mock
                .expect_read_card()
                .times(1)
                .return_once(|| Ok(card_info));

            let mut everest_mock = ModulePublisher::default();

            everest_mock
                .token_provider
                .expect_provided_token()
                .times(1)
                .withf(move |arg| {
                    &arg.id_token.value == id_token && arg.authorization_type == authorization_type
                })
                .return_once(|_| Ok(()));

            everest_mock
                .payment_terminal
                .expect_clear_all_errors()
                .times(1)
                .return_once(|| ());

            let pt_module: PaymentTerminalModule = feig_mock.into();
            assert!(pt_module.read_card(&everest_mock).is_ok());
        }
    }

    #[test]
    fn payment_terminal_module__read_card__success() {
        let PARAMETERS = [
            (
                CardInfo::Bank,
                AuthorizationType::BankCard,
                "some bank token",
                "some bank token",
            ),
            (
                CardInfo::MembershipCard("some id".to_string()),
                AuthorizationType::RFID,
                "some bank token",
                "some id",
            ),
        ];

        for (card_info, authorization_type, bank_token, id_token) in PARAMETERS {
            // When the token is None, we still expect membership card to work
            let mut feig_mock = SyncFeig::default();

            feig_mock
                .expect_read_card()
                .times(1)
                .return_once(|| Ok(card_info));

            let mut everest_mock = ModulePublisher::default();
            everest_mock
                .bank_session_token_slots
                .push(BankSessionTokenProviderClientPublisher::default());

            everest_mock.bank_session_token_slots[0]
                .expect_get_bank_session_token()
                .times(1)
                .return_once(|| {
                    Ok(BankSessionToken {
                        token: Some(bank_token.to_owned()),
                    })
                });

            everest_mock
                .token_provider
                .expect_provided_token()
                .times(1)
                .withf(move |arg| {
                    &arg.id_token.value == id_token && arg.authorization_type == authorization_type
                })
                .return_once(|_| Ok(()));

            everest_mock
                .payment_terminal
                .expect_clear_all_errors()
                .times(1)
                .return_once(|| ());

            feig_mock.expect_configure().times(1).return_once(|| Ok(()));

            let pt_module: PaymentTerminalModule = feig_mock.into();

            assert!(pt_module.read_card(&everest_mock).is_ok());
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
            let pt_module: PaymentTerminalModule = feig.into();

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
            let pt_module: PaymentTerminalModule = feig.into();

            assert!(pt_module
                .on_session_cost_impl(&context, session_cost)
                .is_ok());
        }
    }

    #[test]
    /// Test validate_token with non-BankCard authorization type
    fn payment_terminal__validate_token__invalid_authorization_type() {
        let parameters = [AuthorizationType::OCPP, AuthorizationType::RFID];

        for auth_type in parameters {
            let feig_mock = SyncFeig::default();
            let pt_module: PaymentTerminalModule = feig_mock.into();

            let provided_token = ProvidedIdToken::new("some_token".to_string(), auth_type, None);
            let everest_mock = ModulePublisher::default();
            let context = Context {
                name: "test",
                publisher: &everest_mock,
                index: 0,
            };

            let result = pt_module.validate_token(&context, provided_token);
            assert!(result.is_ok());
            let validation_result = result.unwrap();
            assert_eq!(
                validation_result.authorization_status,
                AuthorizationStatus::Invalid
            );
        }
    }

    #[test]
    /// Test validate_token with INVALID_BANK_TOKEN
    fn payment_terminal__validate_token__invalid_bank_token() {
        let feig_mock = SyncFeig::default();
        let pt_module: PaymentTerminalModule = feig_mock.into();

        let provided_token = ProvidedIdToken::new(
            INVALID_BANK_TOKEN.to_string(),
            AuthorizationType::BankCard,
            None,
        );
        let everest_mock = ModulePublisher::default();
        let context = Context {
            name: "test",
            publisher: &everest_mock,
            index: 0,
        };

        let result = pt_module.validate_token(&context, provided_token);
        assert!(result.is_ok());
        let validation_result = result.unwrap();
        assert_eq!(
            validation_result.authorization_status,
            AuthorizationStatus::Invalid
        );
    }

    #[test]
    /// Test validate_token with successful transaction
    fn payment_terminal__validate_token__success() {
        let mut feig_mock = SyncFeig::default();
        feig_mock
            .expect_begin_transaction()
            .times(1)
            .with(eq("valid_token"), eq(11))
            .returning(|_, _| Ok(()));

        let pt_module: PaymentTerminalModule = feig_mock.into();

        let provided_token =
            ProvidedIdToken::new("valid_token".to_string(), AuthorizationType::BankCard, None);
        let everest_mock = ModulePublisher::default();
        let context = Context {
            name: "test",
            publisher: &everest_mock,
            index: 0,
        };

        let result = pt_module.validate_token(&context, provided_token);
        assert!(result.is_ok());
        let validation_result = result.unwrap();
        assert_eq!(
            validation_result.authorization_status,
            AuthorizationStatus::Accepted
        );
    }

    #[test]
    /// Test validate_token with transaction failures
    fn payment_terminal__validate_token__transaction_failures() {
        let parameters = [
            (
                ErrorMessages::CreditNotSufficient,
                AuthorizationStatus::NoCredit,
            ),
            (
                ErrorMessages::PaymentMethodNotSupported,
                AuthorizationStatus::Blocked,
            ),
            (
                ErrorMessages::AbortViaTimeoutOrAbortKey,
                AuthorizationStatus::Invalid,
            ),
            (
                ErrorMessages::ReceiverNotReady,
                AuthorizationStatus::Timeout,
            ),
            (ErrorMessages::SystemError, AuthorizationStatus::Timeout),
            (ErrorMessages::ErrorFromDialUp, AuthorizationStatus::Timeout),
            (
                ErrorMessages::PinProcessingNotPossible,
                AuthorizationStatus::PinRequired,
            ),
            (
                ErrorMessages::NecessaryDeviceNotPresentOrDefective,
                AuthorizationStatus::PinRequired,
            ),
        ];

        for (error_code, expected_status) in parameters {
            let mut feig_mock = SyncFeig::default();
            feig_mock
                .expect_begin_transaction()
                .times(1)
                .with(eq("valid_token"), eq(11))
                .returning(move |_, _| Err(anyhow::Error::new(error_code)));

            let pt_module: PaymentTerminalModule = feig_mock.into();

            let provided_token =
                ProvidedIdToken::new("valid_token".to_string(), AuthorizationType::BankCard, None);
            let everest_mock = ModulePublisher::default();
            let context = Context {
                name: "test",
                publisher: &everest_mock,
                index: 0,
            };

            let result = pt_module.validate_token(&context, provided_token);
            assert!(result.is_ok());
            let validation_result = result.unwrap();
            assert_eq!(
                validation_result.authorization_status, expected_status,
                "Failed for error code {:?}",
                error_code
            );
        }
    }

    #[test]
    /// Test validate_token with unknown error (no error code provided)
    fn payment_terminal__validate_token__unknown_error() {
        let mut feig_mock = SyncFeig::default();
        feig_mock
            .expect_begin_transaction()
            .times(1)
            .with(eq("valid_token"), eq(11))
            .returning(|_, _| Err(anyhow::anyhow!("Some unknown error")));

        let pt_module: PaymentTerminalModule = feig_mock.into();

        let provided_token =
            ProvidedIdToken::new("valid_token".to_string(), AuthorizationType::BankCard, None);
        let everest_mock = ModulePublisher::default();
        let context = Context {
            name: "test",
            publisher: &everest_mock,
            index: 0,
        };

        let result = pt_module.validate_token(&context, provided_token);
        assert!(result.is_ok());
        let validation_result = result.unwrap();
        assert_eq!(
            validation_result.authorization_status,
            AuthorizationStatus::Invalid
        );
    }

    #[test]
    /// Test enable_card_reading with various scenarios
    fn payment_terminal__enable_card_reading() {
        type CardTypeMap = HashMap<AuthorizationType, Option<Vec<i64>>>;

        let parameters: Vec<(CardTypeMap, i64, Vec<AuthorizationType>, CardTypeMap)> = vec![
            // Init empty map - all card types supported
            (
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![])),
                    (AuthorizationType::RFID, Some(vec![])),
                ]),
                1,
                vec![AuthorizationType::BankCard, AuthorizationType::RFID],
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![1])),
                    (AuthorizationType::RFID, Some(vec![1])),
                ]),
            ),
            // Init empty map - with restrictions.
            (
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![])),
                    (AuthorizationType::RFID, Some(vec![])),
                ]),
                1,
                vec![AuthorizationType::RFID],
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![])),
                    (AuthorizationType::RFID, Some(vec![1])),
                ]),
            ),
            // Extend an existing map
            (
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![3])),
                    (AuthorizationType::RFID, Some(vec![1])),
                ]),
                2,
                vec![AuthorizationType::BankCard, AuthorizationType::RFID],
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![2, 3])),
                    (AuthorizationType::RFID, Some(vec![1, 2])),
                ]),
            ),
            // Extend an existing map
            (
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![])),
                    (AuthorizationType::RFID, Some(vec![1])),
                ]),
                2,
                vec![AuthorizationType::BankCard, AuthorizationType::RFID],
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![2])),
                    (AuthorizationType::RFID, Some(vec![1, 2])),
                ]),
            ),
            // Extend an existing map - idempotent (adding same connector twice)
            (
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![1])),
                    (AuthorizationType::RFID, Some(vec![])),
                ]),
                1,
                vec![AuthorizationType::BankCard],
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![1])),
                    (AuthorizationType::RFID, Some(vec![])),
                ]),
            ),
            // Extend an existing map - maintain sorted order when adding new
            // connector
            (
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![1, 3, 5])),
                    (AuthorizationType::RFID, Some(vec![])),
                ]),
                2,
                vec![AuthorizationType::BankCard],
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![1, 2, 3, 5])),
                    (AuthorizationType::RFID, Some(vec![])),
                ]),
            ),
            // Removing
            (
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![1, 2])),
                    (AuthorizationType::RFID, Some(vec![1, 2])),
                ]),
                1,
                vec![AuthorizationType::BankCard],
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![1, 2])),
                    (AuthorizationType::RFID, Some(vec![2])),
                ]),
            ),
            // Removing - maintain sorted order when removing a connector
            (
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![1, 2, 3, 5])),
                    (AuthorizationType::RFID, Some(vec![1, 2, 4])),
                ]),
                2,
                vec![],
                HashMap::from_iter([
                    (AuthorizationType::BankCard, Some(vec![1, 3, 5])),
                    (AuthorizationType::RFID, Some(vec![1, 4])),
                ]),
            ),
        ];

        for (initial_map, connector_id, supported_cards, expected_map) in parameters {
            let feig_mock = SyncFeig::default();
            let (tx, _) = channel();
            let pt_module = PaymentTerminalModule {
                tx,
                feig: feig_mock,
                card_type_to_connector: Mutex::new(initial_map),
                pre_authorization_amount: 50,
            };

            let everest_mock = ModulePublisher::default();
            let context = Context {
                name: "test",
                publisher: &everest_mock,
                index: 0,
            };

            let result = pt_module.enable_card_reading(&context, connector_id, supported_cards);
            assert!(result.is_ok());

            let actual_map = pt_module.card_type_to_connector.lock().unwrap();
            assert_eq!(
                *actual_map, expected_map,
                "Failed for connector_id {}: expected {:?}, got {:?}",
                connector_id, expected_map, *actual_map
            );
        }
    }
}
