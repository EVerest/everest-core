// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "money/wrapper.hpp"
#include "money/API.hpp"
#include <vector>

namespace everest::lib::API::V1_0::types::money {

namespace {
template <class SrcT, class ConvT>
auto srcToTarOpt(std::optional<SrcT> const& src, ConvT const& converter)
    -> std::optional<decltype(converter(src.value()))> {
    if (src) {
        return std::make_optional(converter(src.value()));
    }
    return std::nullopt;
}

template <class SrcT, class ConvT> auto srcToTarVec(std::vector<SrcT> const& src, ConvT const& converter) {
    using TarT = decltype(converter(src[0]));
    std::vector<TarT> result;
    for (SrcT const& elem : src) {
        result.push_back(converter(elem));
    }
    return result;
}

template <class SrcT>
auto optToInternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_internal_api(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return to_internal_api(val); });
}

template <class SrcT>
auto optToExternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_external_api(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return to_external_api(val); });
}

template <class SrcT> auto vecToExternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return to_external_api(val); });
}

template <class SrcT> auto vecToInternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return to_internal_api(val); });
}

} // namespace

CurrencyCode_Internal to_internal_api(CurrencyCode_External const& val) {
    using SrcT = CurrencyCode_External;
    using TarT = CurrencyCode_Internal;
    switch (val) {
    case SrcT::AED:
        return TarT::AED;
    case SrcT::AFN:
        return TarT::AFN;
    case SrcT::ALL:
        return TarT::ALL;
    case SrcT::AMD:
        return TarT::AMD;
    case SrcT::ANG:
        return TarT::ANG;
    case SrcT::AOA:
        return TarT::AOA;
    case SrcT::ARS:
        return TarT::ARS;
    case SrcT::AUD:
        return TarT::AUD;
    case SrcT::AWG:
        return TarT::AWG;
    case SrcT::AZN:
        return TarT::AZN;
    case SrcT::BAM:
        return TarT::BAM;
    case SrcT::BBD:
        return TarT::BBD;
    case SrcT::BDT:
        return TarT::BDT;
    case SrcT::BGN:
        return TarT::BGN;
    case SrcT::BHD:
        return TarT::BHD;
    case SrcT::BIF:
        return TarT::BIF;
    case SrcT::BMD:
        return TarT::BMD;
    case SrcT::BND:
        return TarT::BND;
    case SrcT::BOB:
        return TarT::BOB;
    case SrcT::BOV:
        return TarT::BOV;
    case SrcT::BRL:
        return TarT::BRL;
    case SrcT::BSD:
        return TarT::BSD;
    case SrcT::BTN:
        return TarT::BTN;
    case SrcT::BWP:
        return TarT::BWP;
    case SrcT::BYN:
        return TarT::BYN;
    case SrcT::BZD:
        return TarT::BZD;
    case SrcT::CAD:
        return TarT::CAD;
    case SrcT::CDF:
        return TarT::CDF;
    case SrcT::CHE:
        return TarT::CHE;
    case SrcT::CHF:
        return TarT::CHF;
    case SrcT::CHW:
        return TarT::CHW;
    case SrcT::CLF:
        return TarT::CLF;
    case SrcT::CLP:
        return TarT::CLP;
    case SrcT::CNY:
        return TarT::CNY;
    case SrcT::COP:
        return TarT::COP;
    case SrcT::COU:
        return TarT::COU;
    case SrcT::CRC:
        return TarT::CRC;
    case SrcT::CUC:
        return TarT::CUC;
    case SrcT::CUP:
        return TarT::CUP;
    case SrcT::CVE:
        return TarT::CVE;
    case SrcT::CZK:
        return TarT::CZK;
    case SrcT::DJF:
        return TarT::DJF;
    case SrcT::DKK:
        return TarT::DKK;
    case SrcT::DOP:
        return TarT::DOP;
    case SrcT::DZD:
        return TarT::DZD;
    case SrcT::EGP:
        return TarT::EGP;
    case SrcT::ERN:
        return TarT::ERN;
    case SrcT::ETB:
        return TarT::ETB;
    case SrcT::EUR:
        return TarT::EUR;
    case SrcT::FJD:
        return TarT::FJD;
    case SrcT::FKP:
        return TarT::FKP;
    case SrcT::GBP:
        return TarT::GBP;
    case SrcT::GEL:
        return TarT::GEL;
    case SrcT::GHS:
        return TarT::GHS;
    case SrcT::GIP:
        return TarT::GIP;
    case SrcT::GMD:
        return TarT::GMD;
    case SrcT::GNF:
        return TarT::GNF;
    case SrcT::GTQ:
        return TarT::GTQ;
    case SrcT::GYD:
        return TarT::GYD;
    case SrcT::HKD:
        return TarT::HKD;
    case SrcT::HNL:
        return TarT::HNL;
    case SrcT::HTG:
        return TarT::HTG;
    case SrcT::HUF:
        return TarT::HUF;
    case SrcT::IDR:
        return TarT::IDR;
    case SrcT::ILS:
        return TarT::ILS;
    case SrcT::INR:
        return TarT::INR;
    case SrcT::IQD:
        return TarT::IQD;
    case SrcT::IRR:
        return TarT::IRR;
    case SrcT::ISK:
        return TarT::ISK;
    case SrcT::JMD:
        return TarT::JMD;
    case SrcT::JOD:
        return TarT::JOD;
    case SrcT::JPY:
        return TarT::JPY;
    case SrcT::KES:
        return TarT::KES;
    case SrcT::KGS:
        return TarT::KGS;
    case SrcT::KHR:
        return TarT::KHR;
    case SrcT::KMF:
        return TarT::KMF;
    case SrcT::KPW:
        return TarT::KPW;
    case SrcT::KRW:
        return TarT::KRW;
    case SrcT::KWD:
        return TarT::KWD;
    case SrcT::KYD:
        return TarT::KYD;
    case SrcT::KZT:
        return TarT::KZT;
    case SrcT::LAK:
        return TarT::LAK;
    case SrcT::LBP:
        return TarT::LBP;
    case SrcT::LKR:
        return TarT::LKR;
    case SrcT::LRD:
        return TarT::LRD;
    case SrcT::LSL:
        return TarT::LSL;
    case SrcT::LYD:
        return TarT::LYD;
    case SrcT::MAD:
        return TarT::MAD;
    case SrcT::MDL:
        return TarT::MDL;
    case SrcT::MGA:
        return TarT::MGA;
    case SrcT::MKD:
        return TarT::MKD;
    case SrcT::MMK:
        return TarT::MMK;
    case SrcT::MNT:
        return TarT::MNT;
    case SrcT::MOP:
        return TarT::MOP;
    case SrcT::MRU:
        return TarT::MRU;
    case SrcT::MUR:
        return TarT::MUR;
    case SrcT::MVR:
        return TarT::MVR;
    case SrcT::MWK:
        return TarT::MWK;
    case SrcT::MXN:
        return TarT::MXN;
    case SrcT::MXV:
        return TarT::MXV;
    case SrcT::MYR:
        return TarT::MYR;
    case SrcT::MZN:
        return TarT::MZN;
    case SrcT::NAD:
        return TarT::NAD;
    case SrcT::NGN:
        return TarT::NGN;
    case SrcT::NIO:
        return TarT::NIO;
    case SrcT::NOK:
        return TarT::NOK;
    case SrcT::NPR:
        return TarT::NPR;
    case SrcT::NZD:
        return TarT::NZD;
    case SrcT::OMR:
        return TarT::OMR;
    case SrcT::PAB:
        return TarT::PAB;
    case SrcT::PEN:
        return TarT::PEN;
    case SrcT::PGK:
        return TarT::PGK;
    case SrcT::PHP:
        return TarT::PHP;
    case SrcT::PKR:
        return TarT::PKR;
    case SrcT::PLN:
        return TarT::PLN;
    case SrcT::PYG:
        return TarT::PYG;
    case SrcT::QAR:
        return TarT::QAR;
    case SrcT::RON:
        return TarT::RON;
    case SrcT::RSD:
        return TarT::RSD;
    case SrcT::RUB:
        return TarT::RUB;
    case SrcT::RWF:
        return TarT::RWF;
    case SrcT::SAR:
        return TarT::SAR;
    case SrcT::SBD:
        return TarT::SBD;
    case SrcT::SCR:
        return TarT::SCR;
    case SrcT::SDG:
        return TarT::SDG;
    case SrcT::SEK:
        return TarT::SEK;
    case SrcT::SGD:
        return TarT::SGD;
    case SrcT::SHP:
        return TarT::SHP;
    case SrcT::SLE:
        return TarT::SLE;
    case SrcT::SLL:
        return TarT::SLL;
    case SrcT::SOS:
        return TarT::SOS;
    case SrcT::SRD:
        return TarT::SRD;
    case SrcT::SSP:
        return TarT::SSP;
    case SrcT::STN:
        return TarT::STN;
    case SrcT::SVC:
        return TarT::SVC;
    case SrcT::SYP:
        return TarT::SYP;
    case SrcT::SZL:
        return TarT::SZL;
    case SrcT::THB:
        return TarT::THB;
    case SrcT::TJS:
        return TarT::TJS;
    case SrcT::TMT:
        return TarT::TMT;
    case SrcT::TND:
        return TarT::TND;
    case SrcT::TOP:
        return TarT::TOP;
    case SrcT::TRY:
        return TarT::TRY;
    case SrcT::TTD:
        return TarT::TTD;
    case SrcT::TWD:
        return TarT::TWD;
    case SrcT::TZS:
        return TarT::TZS;
    case SrcT::UAH:
        return TarT::UAH;
    case SrcT::UGX:
        return TarT::UGX;
    case SrcT::USD:
        return TarT::USD;
    case SrcT::USN:
        return TarT::USN;
    case SrcT::UYI:
        return TarT::UYI;
    case SrcT::UYU:
        return TarT::UYU;
    case SrcT::UYW:
        return TarT::UYW;
    case SrcT::UZS:
        return TarT::UZS;
    case SrcT::VED:
        return TarT::VED;
    case SrcT::VES:
        return TarT::VES;
    case SrcT::VND:
        return TarT::VND;
    case SrcT::VUV:
        return TarT::VUV;
    case SrcT::WST:
        return TarT::WST;
    case SrcT::XAF:
        return TarT::XAF;
    case SrcT::XCD:
        return TarT::XCD;
    case SrcT::XDR:
        return TarT::XDR;
    case SrcT::XOF:
        return TarT::XOF;
    case SrcT::XPF:
        return TarT::XPF;
    case SrcT::XSU:
        return TarT::XSU;
    case SrcT::XUA:
        return TarT::XUA;
    case SrcT::YER:
        return TarT::YER;
    case SrcT::ZAR:
        return TarT::ZAR;
    case SrcT::ZMW:
        return TarT::ZMW;
    case SrcT::ZWL:
        return TarT::ZWL;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::money::CurrencyCode_External");
}

CurrencyCode_External to_external_api(CurrencyCode_Internal const& val) {
    using SrcT = CurrencyCode_Internal;
    using TarT = CurrencyCode_External;
    switch (val) {
    case SrcT::AED:
        return TarT::AED;
    case SrcT::AFN:
        return TarT::AFN;
    case SrcT::ALL:
        return TarT::ALL;
    case SrcT::AMD:
        return TarT::AMD;
    case SrcT::ANG:
        return TarT::ANG;
    case SrcT::AOA:
        return TarT::AOA;
    case SrcT::ARS:
        return TarT::ARS;
    case SrcT::AUD:
        return TarT::AUD;
    case SrcT::AWG:
        return TarT::AWG;
    case SrcT::AZN:
        return TarT::AZN;
    case SrcT::BAM:
        return TarT::BAM;
    case SrcT::BBD:
        return TarT::BBD;
    case SrcT::BDT:
        return TarT::BDT;
    case SrcT::BGN:
        return TarT::BGN;
    case SrcT::BHD:
        return TarT::BHD;
    case SrcT::BIF:
        return TarT::BIF;
    case SrcT::BMD:
        return TarT::BMD;
    case SrcT::BND:
        return TarT::BND;
    case SrcT::BOB:
        return TarT::BOB;
    case SrcT::BOV:
        return TarT::BOV;
    case SrcT::BRL:
        return TarT::BRL;
    case SrcT::BSD:
        return TarT::BSD;
    case SrcT::BTN:
        return TarT::BTN;
    case SrcT::BWP:
        return TarT::BWP;
    case SrcT::BYN:
        return TarT::BYN;
    case SrcT::BZD:
        return TarT::BZD;
    case SrcT::CAD:
        return TarT::CAD;
    case SrcT::CDF:
        return TarT::CDF;
    case SrcT::CHE:
        return TarT::CHE;
    case SrcT::CHF:
        return TarT::CHF;
    case SrcT::CHW:
        return TarT::CHW;
    case SrcT::CLF:
        return TarT::CLF;
    case SrcT::CLP:
        return TarT::CLP;
    case SrcT::CNY:
        return TarT::CNY;
    case SrcT::COP:
        return TarT::COP;
    case SrcT::COU:
        return TarT::COU;
    case SrcT::CRC:
        return TarT::CRC;
    case SrcT::CUC:
        return TarT::CUC;
    case SrcT::CUP:
        return TarT::CUP;
    case SrcT::CVE:
        return TarT::CVE;
    case SrcT::CZK:
        return TarT::CZK;
    case SrcT::DJF:
        return TarT::DJF;
    case SrcT::DKK:
        return TarT::DKK;
    case SrcT::DOP:
        return TarT::DOP;
    case SrcT::DZD:
        return TarT::DZD;
    case SrcT::EGP:
        return TarT::EGP;
    case SrcT::ERN:
        return TarT::ERN;
    case SrcT::ETB:
        return TarT::ETB;
    case SrcT::EUR:
        return TarT::EUR;
    case SrcT::FJD:
        return TarT::FJD;
    case SrcT::FKP:
        return TarT::FKP;
    case SrcT::GBP:
        return TarT::GBP;
    case SrcT::GEL:
        return TarT::GEL;
    case SrcT::GHS:
        return TarT::GHS;
    case SrcT::GIP:
        return TarT::GIP;
    case SrcT::GMD:
        return TarT::GMD;
    case SrcT::GNF:
        return TarT::GNF;
    case SrcT::GTQ:
        return TarT::GTQ;
    case SrcT::GYD:
        return TarT::GYD;
    case SrcT::HKD:
        return TarT::HKD;
    case SrcT::HNL:
        return TarT::HNL;
    case SrcT::HTG:
        return TarT::HTG;
    case SrcT::HUF:
        return TarT::HUF;
    case SrcT::IDR:
        return TarT::IDR;
    case SrcT::ILS:
        return TarT::ILS;
    case SrcT::INR:
        return TarT::INR;
    case SrcT::IQD:
        return TarT::IQD;
    case SrcT::IRR:
        return TarT::IRR;
    case SrcT::ISK:
        return TarT::ISK;
    case SrcT::JMD:
        return TarT::JMD;
    case SrcT::JOD:
        return TarT::JOD;
    case SrcT::JPY:
        return TarT::JPY;
    case SrcT::KES:
        return TarT::KES;
    case SrcT::KGS:
        return TarT::KGS;
    case SrcT::KHR:
        return TarT::KHR;
    case SrcT::KMF:
        return TarT::KMF;
    case SrcT::KPW:
        return TarT::KPW;
    case SrcT::KRW:
        return TarT::KRW;
    case SrcT::KWD:
        return TarT::KWD;
    case SrcT::KYD:
        return TarT::KYD;
    case SrcT::KZT:
        return TarT::KZT;
    case SrcT::LAK:
        return TarT::LAK;
    case SrcT::LBP:
        return TarT::LBP;
    case SrcT::LKR:
        return TarT::LKR;
    case SrcT::LRD:
        return TarT::LRD;
    case SrcT::LSL:
        return TarT::LSL;
    case SrcT::LYD:
        return TarT::LYD;
    case SrcT::MAD:
        return TarT::MAD;
    case SrcT::MDL:
        return TarT::MDL;
    case SrcT::MGA:
        return TarT::MGA;
    case SrcT::MKD:
        return TarT::MKD;
    case SrcT::MMK:
        return TarT::MMK;
    case SrcT::MNT:
        return TarT::MNT;
    case SrcT::MOP:
        return TarT::MOP;
    case SrcT::MRU:
        return TarT::MRU;
    case SrcT::MUR:
        return TarT::MUR;
    case SrcT::MVR:
        return TarT::MVR;
    case SrcT::MWK:
        return TarT::MWK;
    case SrcT::MXN:
        return TarT::MXN;
    case SrcT::MXV:
        return TarT::MXV;
    case SrcT::MYR:
        return TarT::MYR;
    case SrcT::MZN:
        return TarT::MZN;
    case SrcT::NAD:
        return TarT::NAD;
    case SrcT::NGN:
        return TarT::NGN;
    case SrcT::NIO:
        return TarT::NIO;
    case SrcT::NOK:
        return TarT::NOK;
    case SrcT::NPR:
        return TarT::NPR;
    case SrcT::NZD:
        return TarT::NZD;
    case SrcT::OMR:
        return TarT::OMR;
    case SrcT::PAB:
        return TarT::PAB;
    case SrcT::PEN:
        return TarT::PEN;
    case SrcT::PGK:
        return TarT::PGK;
    case SrcT::PHP:
        return TarT::PHP;
    case SrcT::PKR:
        return TarT::PKR;
    case SrcT::PLN:
        return TarT::PLN;
    case SrcT::PYG:
        return TarT::PYG;
    case SrcT::QAR:
        return TarT::QAR;
    case SrcT::RON:
        return TarT::RON;
    case SrcT::RSD:
        return TarT::RSD;
    case SrcT::RUB:
        return TarT::RUB;
    case SrcT::RWF:
        return TarT::RWF;
    case SrcT::SAR:
        return TarT::SAR;
    case SrcT::SBD:
        return TarT::SBD;
    case SrcT::SCR:
        return TarT::SCR;
    case SrcT::SDG:
        return TarT::SDG;
    case SrcT::SEK:
        return TarT::SEK;
    case SrcT::SGD:
        return TarT::SGD;
    case SrcT::SHP:
        return TarT::SHP;
    case SrcT::SLE:
        return TarT::SLE;
    case SrcT::SLL:
        return TarT::SLL;
    case SrcT::SOS:
        return TarT::SOS;
    case SrcT::SRD:
        return TarT::SRD;
    case SrcT::SSP:
        return TarT::SSP;
    case SrcT::STN:
        return TarT::STN;
    case SrcT::SVC:
        return TarT::SVC;
    case SrcT::SYP:
        return TarT::SYP;
    case SrcT::SZL:
        return TarT::SZL;
    case SrcT::THB:
        return TarT::THB;
    case SrcT::TJS:
        return TarT::TJS;
    case SrcT::TMT:
        return TarT::TMT;
    case SrcT::TND:
        return TarT::TND;
    case SrcT::TOP:
        return TarT::TOP;
    case SrcT::TRY:
        return TarT::TRY;
    case SrcT::TTD:
        return TarT::TTD;
    case SrcT::TWD:
        return TarT::TWD;
    case SrcT::TZS:
        return TarT::TZS;
    case SrcT::UAH:
        return TarT::UAH;
    case SrcT::UGX:
        return TarT::UGX;
    case SrcT::USD:
        return TarT::USD;
    case SrcT::USN:
        return TarT::USN;
    case SrcT::UYI:
        return TarT::UYI;
    case SrcT::UYU:
        return TarT::UYU;
    case SrcT::UYW:
        return TarT::UYW;
    case SrcT::UZS:
        return TarT::UZS;
    case SrcT::VED:
        return TarT::VED;
    case SrcT::VES:
        return TarT::VES;
    case SrcT::VND:
        return TarT::VND;
    case SrcT::VUV:
        return TarT::VUV;
    case SrcT::WST:
        return TarT::WST;
    case SrcT::XAF:
        return TarT::XAF;
    case SrcT::XCD:
        return TarT::XCD;
    case SrcT::XDR:
        return TarT::XDR;
    case SrcT::XOF:
        return TarT::XOF;
    case SrcT::XPF:
        return TarT::XPF;
    case SrcT::XSU:
        return TarT::XSU;
    case SrcT::XUA:
        return TarT::XUA;
    case SrcT::YER:
        return TarT::YER;
    case SrcT::ZAR:
        return TarT::ZAR;
    case SrcT::ZMW:
        return TarT::ZMW;
    case SrcT::ZWL:
        return TarT::ZWL;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::money::CurrencyCode_Internal");
}

Currency_Internal to_internal_api(Currency_External const& val) {
    Currency_Internal result;
    result.code = optToInternal(val.code);
    result.decimals = val.decimals;
    return result;
}
Currency_External to_external_api(Currency_Internal const& val) {
    Currency_External result;
    result.code = optToExternal(val.code);
    result.decimals = val.decimals;
    return result;
}
MoneyAmount_Internal to_internal_api(MoneyAmount_External const& val) {
    MoneyAmount_Internal result;
    result.value = val.value;
    return result;
}
MoneyAmount_External to_external_api(MoneyAmount_Internal const& val) {
    MoneyAmount_External result;
    result.value = val.value;
    return result;
}
Price_Internal to_internal_api(Price_External const& val) {
    Price_Internal result;
    result.currency = to_internal_api(val.currency);
    result.value = to_internal_api(val.value);
    return result;
}
Price_External to_external_api(Price_Internal const& val) {
    Price_External result;
    result.currency = to_external_api(val.currency);
    result.value = to_external_api(val.value);
    return result;
}
} // namespace everest::lib::API::V1_0::types::money
