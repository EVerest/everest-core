# California Pricing Requirements

OCPP has several whitepapers, which can be found here: https://openchargealliance.org/whitepapers/

One of them is OCPP & California Pricing Requirements. This can be optionally enabled in libocpp, for OCPP 1.6 as well as OCPP 2.0.1.

## Callbacks in libocpp

To be kind of compatible with eachother, the callbacks for OCPP 1.6 and 2.0.1 use the same structs with the pricing
information.

### User-specific price / SetUserPrice

The User-specific price is used for display purposes only and can be sent as soon as the user identifies itself with an
id token. It should not be used to calculate prices.
Internally, the messages in the DataTransfer json (for 1.6) is converted to a `DisplayMessage`, defined in
`common/types.hpp`. In case of multi language messages, they are all added to the DisplayMessage vector.  
If the message is sent when a transaction has already started, the session id will be included in the display message and the `IdentifierType` will be set to `SessionId`. If it has not started yet, the id token is sent with
`IdentifierType` set to `IdToken`.

### Running cost and Final / total cost

The running cost and final cost messages are converted to a `RunningCost` struct, also defined in `common/types.hpp`.
The triggers in the message (running cost) are handled in libocpp itself.
The prices are converted to integers, because floating point numbers are not precise enough for pricing calculations.
To set the number of decimals to calculate with, you should set NumberOfDecimalsForCostValues (1.6, in CostAndPrice /
2.0.1, TariffCostCtrlr). Default is 3. There might be messages in multiple languages, they are all added to the messages
vector.

## OCPP 1.6

OCPP 1.6 mostly uses DataTransfer to send the pricing messages, and also has some extra configuration items. In libocpp,
the DataTransfer message is converted to internally used structs as described above.

### Configuration Items

| Name | Description |
| ---- | ----------- |
| `CustomDisplayCostAndPrice` | Set to `true` to enable California Pricing (readonly) |
| `DefaultPrice` | Holds the default price and default price text in a json object. Can be updated by the CSMS. Not used by libocpp. See the specification for the specific fields. |
| `NumberOfDecimalsForCostValues` | Holds the number of decimals the cost / price values are converted with. |
| `CustomIdleFeeAfterStop` | Set to `true` to extend the transaction until `ConnectorUnplugged` is sent (readonly). The chargepoint implementation should send this DataTransfer message, this is not part of libocpp (yet, 2024-08) |
| `CustomMultiLanguageMessages` | Set to `true` to enable multi language support (readonly). |
| `Language` | Default language code for the stations UI (IETF RFC5646) (readwrite). |
| `SupportedLanguages` | Comma separated list of supported languages, specified as IETF RFC5646 (readonly). |
| `DefaultPriceText` | Holds an array (`priceTexts`) of default price texts in several languages. Each item has the `priceText` (string) in the given `language` (string) and a `priceTextOffline` (string) in the given language. The CSMS sends the DefaultPriceText per language: `"DefaultPriceText,\<language code\>"`, but libocpp will convert it to the above described json object. (readwrite) |
| `TimeOffset` | As OCPP 1.6 does not have built-in support for timezones, you can set a timezone when displaying time related pricing information. This timezone is also used for the `atTime` trigger. (readwrite) |
| `NextTimeOffsetTransitionDateTime` | When to change to summer or winter time, to the offset `TimeOffsetNextTransition` |
| `TimeOffsetNextTransition` | What the new offset should be at the given `NextTimeOffsetTransationDateTime` (readwrite) |

### Callbacks

For California Pricing to work, the following callbacks must be enabled:

- `session_cost_callback`, used for running cost and final cost
- `set_display_message_callback`, used to show a user specific price

## OCPP 2.0.1

OCPP 2.0.1 uses different mechanisms to send pricing information. The messages are converted to internally used structs as descripbed above. For California Pricing Requirements to work, DisplayMessage and TariffAndCost must be implemented as well.

### Device Model Variables

| Variable name | Instance | Component | Description |
| ------------- | -------- | --------- | ----------- |
| `CustomImplementationEnabled` | `org.openchargealliance.costmsg` | `CustomizationCtrlr` | Set to 'true' to support California Pricing (actually to indicate `customData` fields for California Pricing are supported). |
| `Enabled` | `Tariff` | `TariffCostCtrlr` | Enable showing tariffs. |
| `Enabled` | `Cost` | `TariffCostCtrlr` | Enable showing of cost. |
| `TariffFallbackMessage` | | `TariffCostCtrlr` | Fallback message to show to EV Driver when there is no driver specific tariff information. Not used by libocpp. |
| `TotalCostFallbackMessage` | | `TariffCostCtrlr` | Fallback message to sho to EV Driver when CS can not retrieve the cost for a transaction at the end of the transaction. Not used by libocpp. |
| `Currency` | | `TariffCostCtrlr` | Currency used for tariff and cost information. |
| `NumberOfDecimalsForCostValues` |  | `TariffCostCtrlr` | Holds the number of decimals the cost / price values are converted with. |
| `TariffFallbackMessage` | `Offline` | `TariffCostCtrlr` | Fallback message to be shown to an EV Driver when CS is offline. Not used by libocpp. |
| `OfflineChargingPrice` | `kWhPrice` | `TariffCostCtrlr` | The energy (kWh) price for transactions started while offline. Not used by libocpp. |
| `OfflineChargingPrice` | `hourPrice` | `TariffCostCtrlr` | The time (hour) price for transactions started while offline. Not used by libocpp. |
| `QRCodeDisplayCapable` |  | `DisplayMessageCtrlr` | Set to 'true' if station can display QR codes |
| `CustomImplementationEnabled` | `org.openchargealliance.multilanguage` | `CustomizationCtrlr` | Enable multilanguage |
| `TariffFallbackMessage` | `<language code>` | `TariffCostCtrlr` | TariffFallbackMessage in a specific language. There must be a variable with the language as instance for every supported language. |
| `OfflineTariffFallbackMessage` | `<language code>` | `TariffCostCtrlr` | TariffFallbackMessage when charging station is offline, in a specific language. There must be a variable with the language as instance for every supported language. |
| `TotalCostFallbackMessage` | `<language code>` | `TariffCostCtrlr` | Multi language TotalCostFallbackMessage. There must be a variable with the language as instance for every supported language. |
| `Language` |  | `DisplayMessageCtrlr` | Default language code (RFC 5646). The `valuesList` holds the supported languages of the charging station. The value must be one of `valuesList`. |

> **_NOTE:_**  Tariff and cost can be enabled separately. To be able to use all functionality, it is recommended to
enable both. If cost is enabled and tariff is not enabled, the total cost message will not contain the personal message (`set_running_cost_callback`).
If tariff is enabled and cost is not enabled, the total cost message will only be a DisplayMessage
(`set_display_message_callback`) containing the personal message(s).

### Callbacks

For California Pricing to work, the following callbacks must be enabled:

- `set_running_cost_callback`
- `set_display_message_callback`

For the tariff information (the personal messages), the `set_display_message_callback` is used. The same callback is also used for the SetDisplayMessageRequest in OCPP. The latter does require an id, the former will not have an id. So when `GetDisplayMessageRequest` is called from the CSMS, the Tariff display messages (that do not have an id) should not be returned. They should also be removed as soon as the transaction has ended.

Driver specific tariffs / pricing information can be returned by the CSMS in the `AuthorizeResponse` message. In
libocpp, the whole message is just forwared (pricing information is not extracted from it), because the pricing
information is coupled to the authorize response. So when Tariff and Cost are enabled, the `idTokenInfo` field must be read for pricing information.

Cost information is also sent by the CSMS in the TransactionEventResponse. In that case, the pricing / cost information
is extracted from the message and a RunningCost message is sent containing the current cost and extra messages
(optional). If only Tariff is enabled and there is a personal message in the TransationEventResponse, a DisplayMessage is sent.
