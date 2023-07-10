// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

#ifndef POWERMETER_BSM_SUNSPEC_MODELS_HPP
#define POWERMETER_BSM_SUNSPEC_MODELS_HPP

#include "sunspec_base.hpp"

//////////////////////////////////////////////////////////////////////
//
// sunspec models

namespace sunspec_model {

// clang-format off
inline constexpr PointInitializerList CommonModelInitList = {

    {"ID"  , PointType::uint16},
    {"L"   , PointType::uint16},
    {"Mn"  , PointType::string, 16},
    {"Md"  , PointType::string, 16},
    {"Opt" , PointType::string, 8},
    {"Vr"  , PointType::string, 8},
    {"SN"  , PointType::string, 16},
    {"DA"  , PointType::uint16},
    {"Pad" , PointType::pad, 1}

};
// clang-format on

class Common : public SunspecModelBase<CommonModelInitList.size(), CommonModelInitList> {

public:
    explicit Common(const transport::DataVector& data) : SunspecModelBase(data) {
    }

    sunspec::string Mn() const {
        return string_at_with_length(Model[2].offset, Model[2].length_in_bytes);
    }
    sunspec::string Md() const {
        return string_at_with_length(Model[3].offset, Model[3].length_in_bytes);
    }
    sunspec::string Vr() const {
        return string_at_with_length(Model[5].offset, Model[5].length_in_bytes);
    }
    sunspec::uint16 Da() const {
        return uint16_at(Model[8].offset);
    }
};

//////////////////////////////////////////////////////////////////////
//
// sunspec AC meter

// clang-format off
inline constexpr PointInitializerList ACMeterInitList = {

    { "ID"       , PointType::uint16 }     , // 40091 1 ID Model ID uint16 no 203 SunSpec Model AC Meter
    { "L"        , PointType::uint16 }     , // 40092 1 L Model Payload Length uint16 no 105 Without the fields 'Model ID' and 'Length of payload.
    { "A"        , PointType::int16 }      , // 40093 1 A Amps int16 A no
    { "AphA"     , PointType::int16 }      , // 40094 1 AphA Amps PhaseA int16 A no
    { "AphB"     , PointType::int16 }      , // 40095 1 AphB Amps PhaseB int16 A no
    { "AphC"     , PointType::int16 }      , // 40096 1 AphC Amps PhaseC int16 A no
    { "A_SF"     , PointType::int16 }      , // 40097 1 A_SF sunssf no
    { "no"       , PointType::pad , 1 }    , // 40098 1 no Reserved
    { "PhVphA"   , PointType::int16 }      , // 40099 1 PhVphA Phase Voltage AN int16 V no
    { "PhVphB"   , PointType::int16 }      , // 40100 1 PhVphB Phase Voltage BN int16 V no
    { "PhVphC"   , PointType::int16 }      , // 40101 1 PhVphC Phase Voltage CN int16 V no
    { "no"       , PointType::pad , 4 }    , // 40102 4 no Reserved
    { "V_SF"     , PointType::sunssf }     , // 40106 1 V_SF sunssf no
    { "Hz"       , PointType::int16 }      , // 40107 1 Hz Hz int16 Hz no
    { "Hz_SF"    , PointType::sunssf }     , // 40108 1 Hz_SF sunssf no
    { "W"        , PointType::int16 }      , // 40109 1 W Watts int16 W no
    { "WphA"     , PointType::int16 }      , // 40110 1 WphA Watts phase A int16 W no
    { "WphB"     , PointType::int16 }      , // 40111 1 WphB Watts phase B int16 W no
    { "WphC"     , PointType::int16 }      , // 40112 1 WphC Watts phase C int16 W no
    { "W_SF"     , PointType::sunssf }     , // 40113 1 W_SF sunssf no
    { "VA"       , PointType::int16 }      , // 40114 1 VA VA int16 VA no
    { "VAphA"    , PointType::int16 }      , // 40115 1 VAphA VA phase A int16 VA no
    { "VAphB"    , PointType::int16}       , // 40116 1 VAphB VA phase B int16 VA no
    { "VAphC"    , PointType::int16 }      , // 40117 1 VAphC VA phase C int16 VA no
    { "VA_SF"    , PointType::int16 }      , // 40118 1 VA_SF sunssf no
    { "VAR"      , PointType::int16 }      , // 40119 1 VAR VAR int16 var no
    { "VARphA"   , PointType::int16 }      , // 40120 1 VARphA VAR phase A int16 var no
    { "VARphB"   , PointType::int16 }      , // 40121 1 VARphB VAR phase B int16 var no
    { "VARphC"   , PointType::int16 }      , // 40122 1 VARphC VAR phase C int16 var no
    { "VAR_SF"   , PointType::sunssf }     , // 40123 1 VAR_SF sunssf no
    { "no"       , PointType::pad , 1 }    , // 40124 1 no Reserved
    { "PFphA"    , PointType::int16 }      , // 40125 1 PFphA PF phase A int16 Pct no
    { "PFphB"    , PointType::int16 }      , // 40126 1 PFphB PF phase B int16 Pct no
    { "PFphC"    , PointType::int16 }      , // 40127 1 PFphC PF phase C int16 Pct no
    { "PF_SF"    , PointType::sunssf }     , // 40128 1 PF_SF sunssf no
    { "no"       , PointType::pad , 8 }    , // 40129 8 no Reserved
    { "TotWhIm"  , PointType::acc32 }      , // 40137 2 TotWhIm p Total Watt-hours Imported acc32 Wh no
    { "no"       , PointType::pad , 6 }    , // 40139 6 no Reserved
    { "TotWh_SF" , PointType::sunssf }     , // 40145 1 TotWh_SF sunssf no
    { "no"       , PointType::string, 50 } , // 40146 50 no Reserved
    { "Evt"      , PointType::bitfield32 } , // 40196 2 Evt Events bitfield32 no See chapter 17.5 Event flags of critical events of counter and communication module. A problem exists if this value is different from zero.
};
// clang-format on

class ACMeter : public SunspecModelBase<ACMeterInitList.size(), ACMeterInitList> {

public:
    explicit ACMeter(const transport::DataVector& data) : SunspecModelBase(data) {
    }

    // { "ID"       , PointType::uint16 }     , // 40091 1 ID Model ID uint16 no 203 SunSpec Model AC Meter
    sunspec::uint16 ID() const {
        std::size_t index = 0;
        return uint16_at(Model.at(index).offset);
    }

    // { "L"        , PointType::uint16 }     , // 40092 1 L Model Payload Length uint16 no 105 Without the fields
    // 'Model ID' and 'Length of payload.
    sunspec::uint16 L() const {
        constexpr std::size_t index = 1;
        return uint16_at(Model.at(index).offset);
    }

    // { "A"        , PointType::int16 }      , // 40093 1 A Amps int16 A no
    sunspec::int16 A() const {
        constexpr std::size_t index = 2;
        return int16_at(Model.at(index).offset);
    }

    // { "AphA"     , PointType::int16 }      , // 40094 1 AphA Amps PhaseA int16 A no
    sunspec::int16 AphA() const {
        constexpr std::size_t index = 3;
        return (Model.at(index).offset);
    }

    // { "AphB"     , PointType::int16 }      , // 40096 1 AphB Amps PhaseB int16 B no
    sunspec::int16 AphB() const {
        constexpr std::size_t index = 4;
        return int16_at(Model.at(index).offset);
    }

    // { "AphC"     , PointType::int16 }      , // 40096 1 AphC Amps PhaseC int16 A no
    sunspec::int16 AphC() const {
        constexpr std::size_t index = 5;
        return int16_at(Model.at(index).offset);
    }

    // { "A_SF"     , PointType::int16 }      , // 40097 1 A_SF sunssf no
    sunspec::int16 A_SF() const {
        constexpr std::size_t index = 6;
        return int16_at(Model.at(index).offset);
    }

    // { "no"       , PointType::pad }        , // 40098 1 no Reserved
    // { "PhVphA"   , PointType::int16 }      , // 40099 1 PhVphA Phase Voltage AN int16 V no
    sunspec::int16 PhVphA() const {
        const std::size_t index = 8;
        return int16_at(Model.at(index).offset);
    }

    // { "PhVphB"   , PointType::int16 }      , // 40100 1 PhVphB Phase Voltage BN int16 V no
    sunspec::int16 PhVphB() const {
        constexpr std::size_t index = 9;
        return int16_at(Model.at(index).offset);
    }

    // { "PhVphC"   , PointType::int16 }      , // 40101 1 PhVphC Phase Voltage CN int16 V no
    sunspec::int16 PhVphC() const {
        constexpr std::size_t index = 10;
        return int16_at(Model.at(index).offset);
    }

    // { "no"       , PointType::pad }        , // 40102 4 no Reserved
    // { "V_SF"     , PointType::sunssf }     , // 40106 1 V_SF sunssf no
    sunspec::sunssf V_SF() const {
        constexpr std::size_t index = 12;
        return sunssf_at(Model.at(index).offset);
    }

    // { "Hz"       , PointType::int16 }      , // 40107 1 Hz Hz int16 Hz no
    sunspec::int16 Hz() const {
        constexpr std::size_t index = 13;
        return int16_at(Model.at(index).offset);
    }

    // { "Hz_SF"    , PointType::sunssf }     , // 40108 1 Hz_SF sunssf no
    sunspec::sunssf Hz_SF() const {
        constexpr std::size_t index = 14;
        return sunssf_at(Model.at(index).offset);
    }

    // { "W"        , PointType::int16 }      , // 40109 1 W Watts int16 W no
    sunspec::int16 W() const {
        constexpr std::size_t index = 15;
        return int16_at(Model.at(index).offset);
    }

    // { "WphA"     , PointType::int16 }      , // 40110 1 WphA Watts phase A int16 W no
    sunspec::int16 WphA() const {
        constexpr std::size_t index = 16;
        return int16_at(Model.at(index).offset);
    }

    // { "WphB"     , PointType::int16 }      , // 40111 1 WphB Watts phase B int16 W no
    sunspec::int16 WphB() const {
        constexpr std::size_t index = 17;
        return int16_at(Model.at(index).offset);
    }

    // { "WphC"     , PointType::int16 }      , // 40112 1 WphC Watts phase C int16 W no
    sunspec::int16 WphC() const {
        constexpr std::size_t index = 18;
        return int16_at(Model.at(index).offset);
    }

    // { "W_SF"     , PointType::sunssf }     , // 40113 1 W_SF sunssf no
    sunspec::sunssf W_SF() const {
        constexpr std::size_t index = 19;
        return sunssf_at(Model.at(index).offset);
    }

    // { "VA"       , PointType::int16 }      , // 40114 1 VA VA int16 VA no
    sunspec::int16 VA() const {
        constexpr std::size_t index = 20;
        return int16_at(Model.at(index).offset);
    }

    // { "VAphA"    , PointType::int16 }      , // 40115 1 VAphA VA phase A int16 VA no
    sunspec::int16 VAphA() const {
        constexpr std::size_t index = 21;
        return int16_at(Model.at(index).offset);
    }

    // { "VAphB"    , PointType::int16}       , // 40116 1 VAphB VA phase B int16 VA no
    sunspec::int16 VAphB() const {
        constexpr std::size_t index = 22;
        return int16_at(Model.at(index).offset);
    }

    // { "VAphC"    , PointType::int16 }      , // 40117 1 VAphC VA phase C int16 VA no
    sunspec::int16 VAphC() const {
        constexpr std::size_t index = 23;
        return int16_at(Model.at(index).offset);
    }

    // { "VA_SF"    , PointType::int16 }      , // 40118 1 VA_SF sunssf no
    sunspec::int16 VA_SF() const {
        constexpr std::size_t index = 24;
        return int16_at(Model.at(index).offset);
    }

    // { "VAR"      , PointType::int16 }      , // 40119 1 VAR VAR int16 var no
    sunspec::int16 VAR() const {
        constexpr std::size_t index = 25;
        return int16_at(Model.at(index).offset);
    }

    // { "VARphA"   , PointType::int16 }      , // 40120 1 VARphA VAR phase A int16 var no
    sunspec::int16 VARphA() const {
        constexpr std::size_t index = 26;
        return int16_at(Model.at(index).offset);
    }

    // { "VARphB"   , PointType::int16 }      , // 40121 1 VARphB VAR phase B int16 var no
    sunspec::int16 VARphB() const {
        constexpr std::size_t index = 27;
        return int16_at(Model.at(index).offset);
    }

    // { "VARphC"   , PointType::int16 }      , // 40122 1 VARphC VAR phase C int16 var no
    sunspec::int16 VARphC() const {
        constexpr std::size_t index = 28;
        return int16_at(Model.at(index).offset);
    }

    // { "VAR_SF"   , PointType::sunssf }     , // 40123 1 VAR_SF sunssf no
    sunspec::sunssf VAR_SF() const {
        constexpr std::size_t index = 29;
        return sunssf_at(Model.at(index).offset);
    }

    // { "no"       , PointType::pad}         , // 40124 1 no Reserved
    // { "PFphA"    , PointType::int16 }      , // 40125 1 PFphA PF phase A int16 Pct no
    sunspec::int16 PFphA() const {
        constexpr std::size_t index = 31;
        return int16_at(Model.at(index).offset);
    }

    // { "PFphB"    , PointType::int16 }      , // 40126 1 PFphB PF phase B int16 Pct no
    sunspec::int16 PFphB() const {
        constexpr std::size_t index = 32;
        return int16_at(Model.at(index).offset);
    }

    // { "PFphC"    , PointType::int16 }      , // 40127 1 PFphC PF phase C int16 Pct no
    sunspec::int16 PFphC() const {
        constexpr std::size_t index = 33;
        return int16_at(Model.at(index).offset);
    }

    // { "PF_SF"    , PointType::sunssf }     , // 40128 1 PF_SF sunssf no
    sunspec::sunssf PF_SF() const {
        constexpr std::size_t index = 34;
        return sunssf_at(Model.at(index).offset);
    }

    // { "no"       , PointType::pad }        , // 40129 8 no Reserved
    // { "TotWhIm"  , PointType::acc32 }      , // 40137 2 TotWhIm p Total Watt-hours Imported acc32 Wh no
    sunspec::acc32 TotWhIm() const {
        constexpr std::size_t index = 36;
        return acc32_at(Model.at(index).offset);
    }

    // { "no"       , PointType::pad }        , // 40139 6 no Reserved
    // { "TotWh_SF" , PointType::sunssf }     , // 40145 1 TotWh_SF sunssf no
    sunspec::sunssf TotWh_SF() const {
        constexpr std::size_t index = 38;
        return sunssf_at(Model.at(index).offset);
    }

    // { "no"       , PointType::pad, 50 } , // 40146 50 no Reserved
    // { "Evt"      , PointType::bitfield32 } , // 40196 2 Evt Events bitfield32 no See chapter 17.5 Event flags of
    // critical events of counter and communication module. A problem exists if this value is different from zero.
    sunspec::bitfield32 Evt() const {
        constexpr std::size_t index = 40;
        return bitfield32_at(Model.at(index).offset);
    }
};

} // namespace sunspec_model

#endif // POWERMETER_BSM_SUNSPEC_MODELS_HPP
