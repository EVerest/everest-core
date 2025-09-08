// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "money/json_codec.hpp"
#include "money/API.hpp"
#include "money/codec.hpp"

#include "nlohmann/json.hpp"

namespace everest::lib::API::V1_0::types::money {

using json = nlohmann::json;

void to_json(json& j, CurrencyCode const& k) noexcept {
    switch (k) {
    case CurrencyCode::AED:
        j = "AED";
        return;
    case CurrencyCode::AFN:
        j = "AFN";
        return;
    case CurrencyCode::ALL:
        j = "ALL";
        return;
    case CurrencyCode::AMD:
        j = "AMD";
        return;
    case CurrencyCode::ANG:
        j = "ANG";
        return;
    case CurrencyCode::AOA:
        j = "AOA";
        return;
    case CurrencyCode::ARS:
        j = "ARS";
        return;
    case CurrencyCode::AUD:
        j = "AUD";
        return;
    case CurrencyCode::AWG:
        j = "AWG";
        return;
    case CurrencyCode::AZN:
        j = "AZN";
        return;
    case CurrencyCode::BAM:
        j = "BAM";
        return;
    case CurrencyCode::BBD:
        j = "BBD";
        return;
    case CurrencyCode::BDT:
        j = "BDT";
        return;
    case CurrencyCode::BGN:
        j = "BGN";
        return;
    case CurrencyCode::BHD:
        j = "BHD";
        return;
    case CurrencyCode::BIF:
        j = "BIF";
        return;
    case CurrencyCode::BMD:
        j = "BMD";
        return;
    case CurrencyCode::BND:
        j = "BND";
        return;
    case CurrencyCode::BOB:
        j = "BOB";
        return;
    case CurrencyCode::BOV:
        j = "BOV";
        return;
    case CurrencyCode::BRL:
        j = "BRL";
        return;
    case CurrencyCode::BSD:
        j = "BSD";
        return;
    case CurrencyCode::BTN:
        j = "BTN";
        return;
    case CurrencyCode::BWP:
        j = "BWP";
        return;
    case CurrencyCode::BYN:
        j = "BYN";
        return;
    case CurrencyCode::BZD:
        j = "BZD";
        return;
    case CurrencyCode::CAD:
        j = "CAD";
        return;
    case CurrencyCode::CDF:
        j = "CDF";
        return;
    case CurrencyCode::CHE:
        j = "CHE";
        return;
    case CurrencyCode::CHF:
        j = "CHF";
        return;
    case CurrencyCode::CHW:
        j = "CHW";
        return;
    case CurrencyCode::CLF:
        j = "CLF";
        return;
    case CurrencyCode::CLP:
        j = "CLP";
        return;
    case CurrencyCode::CNY:
        j = "CNY";
        return;
    case CurrencyCode::COP:
        j = "COP";
        return;
    case CurrencyCode::COU:
        j = "COU";
        return;
    case CurrencyCode::CRC:
        j = "CRC";
        return;
    case CurrencyCode::CUC:
        j = "CUC";
        return;
    case CurrencyCode::CUP:
        j = "CUP";
        return;
    case CurrencyCode::CVE:
        j = "CVE";
        return;
    case CurrencyCode::CZK:
        j = "CZK";
        return;
    case CurrencyCode::DJF:
        j = "DJF";
        return;
    case CurrencyCode::DKK:
        j = "DKK";
        return;
    case CurrencyCode::DOP:
        j = "DOP";
        return;
    case CurrencyCode::DZD:
        j = "DZD";
        return;
    case CurrencyCode::EGP:
        j = "EGP";
        return;
    case CurrencyCode::ERN:
        j = "ERN";
        return;
    case CurrencyCode::ETB:
        j = "ETB";
        return;
    case CurrencyCode::EUR:
        j = "EUR";
        return;
    case CurrencyCode::FJD:
        j = "FJD";
        return;
    case CurrencyCode::FKP:
        j = "FKP";
        return;
    case CurrencyCode::GBP:
        j = "GBP";
        return;
    case CurrencyCode::GEL:
        j = "GEL";
        return;
    case CurrencyCode::GHS:
        j = "GHS";
        return;
    case CurrencyCode::GIP:
        j = "GIP";
        return;
    case CurrencyCode::GMD:
        j = "GMD";
        return;
    case CurrencyCode::GNF:
        j = "GNF";
        return;
    case CurrencyCode::GTQ:
        j = "GTQ";
        return;
    case CurrencyCode::GYD:
        j = "GYD";
        return;
    case CurrencyCode::HKD:
        j = "HKD";
        return;
    case CurrencyCode::HNL:
        j = "HNL";
        return;
    case CurrencyCode::HTG:
        j = "HTG";
        return;
    case CurrencyCode::HUF:
        j = "HUF";
        return;
    case CurrencyCode::IDR:
        j = "IDR";
        return;
    case CurrencyCode::ILS:
        j = "ILS";
        return;
    case CurrencyCode::INR:
        j = "INR";
        return;
    case CurrencyCode::IQD:
        j = "IQD";
        return;
    case CurrencyCode::IRR:
        j = "IRR";
        return;
    case CurrencyCode::ISK:
        j = "ISK";
        return;
    case CurrencyCode::JMD:
        j = "JMD";
        return;
    case CurrencyCode::JOD:
        j = "JOD";
        return;
    case CurrencyCode::JPY:
        j = "JPY";
        return;
    case CurrencyCode::KES:
        j = "KES";
        return;
    case CurrencyCode::KGS:
        j = "KGS";
        return;
    case CurrencyCode::KHR:
        j = "KHR";
        return;
    case CurrencyCode::KMF:
        j = "KMF";
        return;
    case CurrencyCode::KPW:
        j = "KPW";
        return;
    case CurrencyCode::KRW:
        j = "KRW";
        return;
    case CurrencyCode::KWD:
        j = "KWD";
        return;
    case CurrencyCode::KYD:
        j = "KYD";
        return;
    case CurrencyCode::KZT:
        j = "KZT";
        return;
    case CurrencyCode::LAK:
        j = "LAK";
        return;
    case CurrencyCode::LBP:
        j = "LBP";
        return;
    case CurrencyCode::LKR:
        j = "LKR";
        return;
    case CurrencyCode::LRD:
        j = "LRD";
        return;
    case CurrencyCode::LSL:
        j = "LSL";
        return;
    case CurrencyCode::LYD:
        j = "LYD";
        return;
    case CurrencyCode::MAD:
        j = "MAD";
        return;
    case CurrencyCode::MDL:
        j = "MDL";
        return;
    case CurrencyCode::MGA:
        j = "MGA";
        return;
    case CurrencyCode::MKD:
        j = "MKD";
        return;
    case CurrencyCode::MMK:
        j = "MMK";
        return;
    case CurrencyCode::MNT:
        j = "MNT";
        return;
    case CurrencyCode::MOP:
        j = "MOP";
        return;
    case CurrencyCode::MRU:
        j = "MRU";
        return;
    case CurrencyCode::MUR:
        j = "MUR";
        return;
    case CurrencyCode::MVR:
        j = "MVR";
        return;
    case CurrencyCode::MWK:
        j = "MWK";
        return;
    case CurrencyCode::MXN:
        j = "MXN";
        return;
    case CurrencyCode::MXV:
        j = "MXV";
        return;
    case CurrencyCode::MYR:
        j = "MYR";
        return;
    case CurrencyCode::MZN:
        j = "MZN";
        return;
    case CurrencyCode::NAD:
        j = "NAD";
        return;
    case CurrencyCode::NGN:
        j = "NGN";
        return;
    case CurrencyCode::NIO:
        j = "NIO";
        return;
    case CurrencyCode::NOK:
        j = "NOK";
        return;
    case CurrencyCode::NPR:
        j = "NPR";
        return;
    case CurrencyCode::NZD:
        j = "NZD";
        return;
    case CurrencyCode::OMR:
        j = "OMR";
        return;
    case CurrencyCode::PAB:
        j = "PAB";
        return;
    case CurrencyCode::PEN:
        j = "PEN";
        return;
    case CurrencyCode::PGK:
        j = "PGK";
        return;
    case CurrencyCode::PHP:
        j = "PHP";
        return;
    case CurrencyCode::PKR:
        j = "PKR";
        return;
    case CurrencyCode::PLN:
        j = "PLN";
        return;
    case CurrencyCode::PYG:
        j = "PYG";
        return;
    case CurrencyCode::QAR:
        j = "QAR";
        return;
    case CurrencyCode::RON:
        j = "RON";
        return;
    case CurrencyCode::RSD:
        j = "RSD";
        return;
    case CurrencyCode::RUB:
        j = "RUB";
        return;
    case CurrencyCode::RWF:
        j = "RWF";
        return;
    case CurrencyCode::SAR:
        j = "SAR";
        return;
    case CurrencyCode::SBD:
        j = "SBD";
        return;
    case CurrencyCode::SCR:
        j = "SCR";
        return;
    case CurrencyCode::SDG:
        j = "SDG";
        return;
    case CurrencyCode::SEK:
        j = "SEK";
        return;
    case CurrencyCode::SGD:
        j = "SGD";
        return;
    case CurrencyCode::SHP:
        j = "SHP";
        return;
    case CurrencyCode::SLE:
        j = "SLE";
        return;
    case CurrencyCode::SLL:
        j = "SLL";
        return;
    case CurrencyCode::SOS:
        j = "SOS";
        return;
    case CurrencyCode::SRD:
        j = "SRD";
        return;
    case CurrencyCode::SSP:
        j = "SSP";
        return;
    case CurrencyCode::STN:
        j = "STN";
        return;
    case CurrencyCode::SVC:
        j = "SVC";
        return;
    case CurrencyCode::SYP:
        j = "SYP";
        return;
    case CurrencyCode::SZL:
        j = "SZL";
        return;
    case CurrencyCode::THB:
        j = "THB";
        return;
    case CurrencyCode::TJS:
        j = "TJS";
        return;
    case CurrencyCode::TMT:
        j = "TMT";
        return;
    case CurrencyCode::TND:
        j = "TND";
        return;
    case CurrencyCode::TOP:
        j = "TOP";
        return;
    case CurrencyCode::TRY:
        j = "TRY";
        return;
    case CurrencyCode::TTD:
        j = "TTD";
        return;
    case CurrencyCode::TWD:
        j = "TWD";
        return;
    case CurrencyCode::TZS:
        j = "TZS";
        return;
    case CurrencyCode::UAH:
        j = "UAH";
        return;
    case CurrencyCode::UGX:
        j = "UGX";
        return;
    case CurrencyCode::USD:
        j = "USD";
        return;
    case CurrencyCode::USN:
        j = "USN";
        return;
    case CurrencyCode::UYI:
        j = "UYI";
        return;
    case CurrencyCode::UYU:
        j = "UYU";
        return;
    case CurrencyCode::UYW:
        j = "UYW";
        return;
    case CurrencyCode::UZS:
        j = "UZS";
        return;
    case CurrencyCode::VED:
        j = "VED";
        return;
    case CurrencyCode::VES:
        j = "VES";
        return;
    case CurrencyCode::VND:
        j = "VND";
        return;
    case CurrencyCode::VUV:
        j = "VUV";
        return;
    case CurrencyCode::WST:
        j = "WST";
        return;
    case CurrencyCode::XAF:
        j = "XAF";
        return;
    case CurrencyCode::XCD:
        j = "XCD";
        return;
    case CurrencyCode::XDR:
        j = "XDR";
        return;
    case CurrencyCode::XOF:
        j = "XOF";
        return;
    case CurrencyCode::XPF:
        j = "XPF";
        return;
    case CurrencyCode::XSU:
        j = "XSU";
        return;
    case CurrencyCode::XUA:
        j = "XUA";
        return;
    case CurrencyCode::YER:
        j = "YER";
        return;
    case CurrencyCode::ZAR:
        j = "ZAR";
        return;
    case CurrencyCode::ZMW:
        j = "ZMW";
        return;
    case CurrencyCode::ZWL:
        j = "ZWL";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::money::CurrencyCode";
}
void from_json(const json& j, CurrencyCode& k) {
    std::string s = j;
    if (s == "AED") {
        k = CurrencyCode::AED;
        return;
    }
    if (s == "AFN") {
        k = CurrencyCode::AFN;
        return;
    }
    if (s == "ALL") {
        k = CurrencyCode::ALL;
        return;
    }
    if (s == "AMD") {
        k = CurrencyCode::AMD;
        return;
    }
    if (s == "ANG") {
        k = CurrencyCode::ANG;
        return;
    }
    if (s == "AOA") {
        k = CurrencyCode::AOA;
        return;
    }
    if (s == "ARS") {
        k = CurrencyCode::ARS;
        return;
    }
    if (s == "AUD") {
        k = CurrencyCode::AUD;
        return;
    }
    if (s == "AWG") {
        k = CurrencyCode::AWG;
        return;
    }
    if (s == "AZN") {
        k = CurrencyCode::AZN;
        return;
    }
    if (s == "BAM") {
        k = CurrencyCode::BAM;
        return;
    }
    if (s == "BBD") {
        k = CurrencyCode::BBD;
        return;
    }
    if (s == "BDT") {
        k = CurrencyCode::BDT;
        return;
    }
    if (s == "BGN") {
        k = CurrencyCode::BGN;
        return;
    }
    if (s == "BHD") {
        k = CurrencyCode::BHD;
        return;
    }
    if (s == "BIF") {
        k = CurrencyCode::BIF;
        return;
    }
    if (s == "BMD") {
        k = CurrencyCode::BMD;
        return;
    }
    if (s == "BND") {
        k = CurrencyCode::BND;
        return;
    }
    if (s == "BOB") {
        k = CurrencyCode::BOB;
        return;
    }
    if (s == "BOV") {
        k = CurrencyCode::BOV;
        return;
    }
    if (s == "BRL") {
        k = CurrencyCode::BRL;
        return;
    }
    if (s == "BSD") {
        k = CurrencyCode::BSD;
        return;
    }
    if (s == "BTN") {
        k = CurrencyCode::BTN;
        return;
    }
    if (s == "BWP") {
        k = CurrencyCode::BWP;
        return;
    }
    if (s == "BYN") {
        k = CurrencyCode::BYN;
        return;
    }
    if (s == "BZD") {
        k = CurrencyCode::BZD;
        return;
    }
    if (s == "CAD") {
        k = CurrencyCode::CAD;
        return;
    }
    if (s == "CDF") {
        k = CurrencyCode::CDF;
        return;
    }
    if (s == "CHE") {
        k = CurrencyCode::CHE;
        return;
    }
    if (s == "CHF") {
        k = CurrencyCode::CHF;
        return;
    }
    if (s == "CHW") {
        k = CurrencyCode::CHW;
        return;
    }
    if (s == "CLF") {
        k = CurrencyCode::CLF;
        return;
    }
    if (s == "CLP") {
        k = CurrencyCode::CLP;
        return;
    }
    if (s == "CNY") {
        k = CurrencyCode::CNY;
        return;
    }
    if (s == "COP") {
        k = CurrencyCode::COP;
        return;
    }
    if (s == "COU") {
        k = CurrencyCode::COU;
        return;
    }
    if (s == "CRC") {
        k = CurrencyCode::CRC;
        return;
    }
    if (s == "CUC") {
        k = CurrencyCode::CUC;
        return;
    }
    if (s == "CUP") {
        k = CurrencyCode::CUP;
        return;
    }
    if (s == "CVE") {
        k = CurrencyCode::CVE;
        return;
    }
    if (s == "CZK") {
        k = CurrencyCode::CZK;
        return;
    }
    if (s == "DJF") {
        k = CurrencyCode::DJF;
        return;
    }
    if (s == "DKK") {
        k = CurrencyCode::DKK;
        return;
    }
    if (s == "DOP") {
        k = CurrencyCode::DOP;
        return;
    }
    if (s == "DZD") {
        k = CurrencyCode::DZD;
        return;
    }
    if (s == "EGP") {
        k = CurrencyCode::EGP;
        return;
    }
    if (s == "ERN") {
        k = CurrencyCode::ERN;
        return;
    }
    if (s == "ETB") {
        k = CurrencyCode::ETB;
        return;
    }
    if (s == "EUR") {
        k = CurrencyCode::EUR;
        return;
    }
    if (s == "FJD") {
        k = CurrencyCode::FJD;
        return;
    }
    if (s == "FKP") {
        k = CurrencyCode::FKP;
        return;
    }
    if (s == "GBP") {
        k = CurrencyCode::GBP;
        return;
    }
    if (s == "GEL") {
        k = CurrencyCode::GEL;
        return;
    }
    if (s == "GHS") {
        k = CurrencyCode::GHS;
        return;
    }
    if (s == "GIP") {
        k = CurrencyCode::GIP;
        return;
    }
    if (s == "GMD") {
        k = CurrencyCode::GMD;
        return;
    }
    if (s == "GNF") {
        k = CurrencyCode::GNF;
        return;
    }
    if (s == "GTQ") {
        k = CurrencyCode::GTQ;
        return;
    }
    if (s == "GYD") {
        k = CurrencyCode::GYD;
        return;
    }
    if (s == "HKD") {
        k = CurrencyCode::HKD;
        return;
    }
    if (s == "HNL") {
        k = CurrencyCode::HNL;
        return;
    }
    if (s == "HTG") {
        k = CurrencyCode::HTG;
        return;
    }
    if (s == "HUF") {
        k = CurrencyCode::HUF;
        return;
    }
    if (s == "IDR") {
        k = CurrencyCode::IDR;
        return;
    }
    if (s == "ILS") {
        k = CurrencyCode::ILS;
        return;
    }
    if (s == "INR") {
        k = CurrencyCode::INR;
        return;
    }
    if (s == "IQD") {
        k = CurrencyCode::IQD;
        return;
    }
    if (s == "IRR") {
        k = CurrencyCode::IRR;
        return;
    }
    if (s == "ISK") {
        k = CurrencyCode::ISK;
        return;
    }
    if (s == "JMD") {
        k = CurrencyCode::JMD;
        return;
    }
    if (s == "JOD") {
        k = CurrencyCode::JOD;
        return;
    }
    if (s == "JPY") {
        k = CurrencyCode::JPY;
        return;
    }
    if (s == "KES") {
        k = CurrencyCode::KES;
        return;
    }
    if (s == "KGS") {
        k = CurrencyCode::KGS;
        return;
    }
    if (s == "KHR") {
        k = CurrencyCode::KHR;
        return;
    }
    if (s == "KMF") {
        k = CurrencyCode::KMF;
        return;
    }
    if (s == "KPW") {
        k = CurrencyCode::KPW;
        return;
    }
    if (s == "KRW") {
        k = CurrencyCode::KRW;
        return;
    }
    if (s == "KWD") {
        k = CurrencyCode::KWD;
        return;
    }
    if (s == "KYD") {
        k = CurrencyCode::KYD;
        return;
    }
    if (s == "KZT") {
        k = CurrencyCode::KZT;
        return;
    }
    if (s == "LAK") {
        k = CurrencyCode::LAK;
        return;
    }
    if (s == "LBP") {
        k = CurrencyCode::LBP;
        return;
    }
    if (s == "LKR") {
        k = CurrencyCode::LKR;
        return;
    }
    if (s == "LRD") {
        k = CurrencyCode::LRD;
        return;
    }
    if (s == "LSL") {
        k = CurrencyCode::LSL;
        return;
    }
    if (s == "LYD") {
        k = CurrencyCode::LYD;
        return;
    }
    if (s == "MAD") {
        k = CurrencyCode::MAD;
        return;
    }
    if (s == "MDL") {
        k = CurrencyCode::MDL;
        return;
    }
    if (s == "MGA") {
        k = CurrencyCode::MGA;
        return;
    }
    if (s == "MKD") {
        k = CurrencyCode::MKD;
        return;
    }
    if (s == "MMK") {
        k = CurrencyCode::MMK;
        return;
    }
    if (s == "MNT") {
        k = CurrencyCode::MNT;
        return;
    }
    if (s == "MOP") {
        k = CurrencyCode::MOP;
        return;
    }
    if (s == "MRU") {
        k = CurrencyCode::MRU;
        return;
    }
    if (s == "MUR") {
        k = CurrencyCode::MUR;
        return;
    }
    if (s == "MVR") {
        k = CurrencyCode::MVR;
        return;
    }
    if (s == "MWK") {
        k = CurrencyCode::MWK;
        return;
    }
    if (s == "MXN") {
        k = CurrencyCode::MXN;
        return;
    }
    if (s == "MXV") {
        k = CurrencyCode::MXV;
        return;
    }
    if (s == "MYR") {
        k = CurrencyCode::MYR;
        return;
    }
    if (s == "MZN") {
        k = CurrencyCode::MZN;
        return;
    }
    if (s == "NAD") {
        k = CurrencyCode::NAD;
        return;
    }
    if (s == "NGN") {
        k = CurrencyCode::NGN;
        return;
    }
    if (s == "NIO") {
        k = CurrencyCode::NIO;
        return;
    }
    if (s == "NOK") {
        k = CurrencyCode::NOK;
        return;
    }
    if (s == "NPR") {
        k = CurrencyCode::NPR;
        return;
    }
    if (s == "NZD") {
        k = CurrencyCode::NZD;
        return;
    }
    if (s == "OMR") {
        k = CurrencyCode::OMR;
        return;
    }
    if (s == "PAB") {
        k = CurrencyCode::PAB;
        return;
    }
    if (s == "PEN") {
        k = CurrencyCode::PEN;
        return;
    }
    if (s == "PGK") {
        k = CurrencyCode::PGK;
        return;
    }
    if (s == "PHP") {
        k = CurrencyCode::PHP;
        return;
    }
    if (s == "PKR") {
        k = CurrencyCode::PKR;
        return;
    }
    if (s == "PLN") {
        k = CurrencyCode::PLN;
        return;
    }
    if (s == "PYG") {
        k = CurrencyCode::PYG;
        return;
    }
    if (s == "QAR") {
        k = CurrencyCode::QAR;
        return;
    }
    if (s == "RON") {
        k = CurrencyCode::RON;
        return;
    }
    if (s == "RSD") {
        k = CurrencyCode::RSD;
        return;
    }
    if (s == "RUB") {
        k = CurrencyCode::RUB;
        return;
    }
    if (s == "RWF") {
        k = CurrencyCode::RWF;
        return;
    }
    if (s == "SAR") {
        k = CurrencyCode::SAR;
        return;
    }
    if (s == "SBD") {
        k = CurrencyCode::SBD;
        return;
    }
    if (s == "SCR") {
        k = CurrencyCode::SCR;
        return;
    }
    if (s == "SDG") {
        k = CurrencyCode::SDG;
        return;
    }
    if (s == "SEK") {
        k = CurrencyCode::SEK;
        return;
    }
    if (s == "SGD") {
        k = CurrencyCode::SGD;
        return;
    }
    if (s == "SHP") {
        k = CurrencyCode::SHP;
        return;
    }
    if (s == "SLE") {
        k = CurrencyCode::SLE;
        return;
    }
    if (s == "SLL") {
        k = CurrencyCode::SLL;
        return;
    }
    if (s == "SOS") {
        k = CurrencyCode::SOS;
        return;
    }
    if (s == "SRD") {
        k = CurrencyCode::SRD;
        return;
    }
    if (s == "SSP") {
        k = CurrencyCode::SSP;
        return;
    }
    if (s == "STN") {
        k = CurrencyCode::STN;
        return;
    }
    if (s == "SVC") {
        k = CurrencyCode::SVC;
        return;
    }
    if (s == "SYP") {
        k = CurrencyCode::SYP;
        return;
    }
    if (s == "SZL") {
        k = CurrencyCode::SZL;
        return;
    }
    if (s == "THB") {
        k = CurrencyCode::THB;
        return;
    }
    if (s == "TJS") {
        k = CurrencyCode::TJS;
        return;
    }
    if (s == "TMT") {
        k = CurrencyCode::TMT;
        return;
    }
    if (s == "TND") {
        k = CurrencyCode::TND;
        return;
    }
    if (s == "TOP") {
        k = CurrencyCode::TOP;
        return;
    }
    if (s == "TRY") {
        k = CurrencyCode::TRY;
        return;
    }
    if (s == "TTD") {
        k = CurrencyCode::TTD;
        return;
    }
    if (s == "TWD") {
        k = CurrencyCode::TWD;
        return;
    }
    if (s == "TZS") {
        k = CurrencyCode::TZS;
        return;
    }
    if (s == "UAH") {
        k = CurrencyCode::UAH;
        return;
    }
    if (s == "UGX") {
        k = CurrencyCode::UGX;
        return;
    }
    if (s == "USD") {
        k = CurrencyCode::USD;
        return;
    }
    if (s == "USN") {
        k = CurrencyCode::USN;
        return;
    }
    if (s == "UYI") {
        k = CurrencyCode::UYI;
        return;
    }
    if (s == "UYU") {
        k = CurrencyCode::UYU;
        return;
    }
    if (s == "UYW") {
        k = CurrencyCode::UYW;
        return;
    }
    if (s == "UZS") {
        k = CurrencyCode::UZS;
        return;
    }
    if (s == "VED") {
        k = CurrencyCode::VED;
        return;
    }
    if (s == "VES") {
        k = CurrencyCode::VES;
        return;
    }
    if (s == "VND") {
        k = CurrencyCode::VND;
        return;
    }
    if (s == "VUV") {
        k = CurrencyCode::VUV;
        return;
    }
    if (s == "WST") {
        k = CurrencyCode::WST;
        return;
    }
    if (s == "XAF") {
        k = CurrencyCode::XAF;
        return;
    }
    if (s == "XCD") {
        k = CurrencyCode::XCD;
        return;
    }
    if (s == "XDR") {
        k = CurrencyCode::XDR;
        return;
    }
    if (s == "XOF") {
        k = CurrencyCode::XOF;
        return;
    }
    if (s == "XPF") {
        k = CurrencyCode::XPF;
        return;
    }
    if (s == "XSU") {
        k = CurrencyCode::XSU;
        return;
    }
    if (s == "XUA") {
        k = CurrencyCode::XUA;
        return;
    }
    if (s == "YER") {
        k = CurrencyCode::YER;
        return;
    }
    if (s == "ZAR") {
        k = CurrencyCode::ZAR;
        return;
    }
    if (s == "ZMW") {
        k = CurrencyCode::ZMW;
        return;
    }
    if (s == "ZWL") {
        k = CurrencyCode::ZWL;
        return;
    }
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::money::CurrencyCode");
}

void to_json(json& j, Currency const& k) noexcept {
    j = json({});
    if (k.code) {
        j["code"] = k.code.value();
    }
    if (k.decimals) {
        j["decimals"] = k.decimals.value();
    }
}
void from_json(const json& j, Currency& k) {
    if (j.contains("code")) {
        k.code.emplace(j.at("code"));
    }
    if (j.contains("decimals")) {
        k.decimals.emplace(j.at("decimals"));
    }
}

void to_json(json& j, MoneyAmount const& k) noexcept {
    j = json{
        {"value", k.value},
    };
}
void from_json(const json& j, MoneyAmount& k) {
    k.value = j.at("value");
}

void to_json(json& j, Price const& k) noexcept {
    j = json{
        {"currency", k.currency},
        {"value", k.value},
    };
}
void from_json(const json& j, Price& k) {
    k.currency = j.at("currency");
    k.value = j.at("value");
}

} // namespace everest::lib::API::V1_0::types::money
