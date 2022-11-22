// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef POWERMETER_BSM_BSMSNAPSHOTMODEL_HPP
#define POWERMETER_BSM_BSMSNAPSHOTMODEL_HPP

#include "sunspec_base.hpp"

namespace bsm {

// clang-format off
inline constexpr PointInitializerList BSMSnapshotPointInitList = {

    {"Type"        , PointType::enum16 },
    {"Status"      , PointType::enum16 },
    {"RCR"         , PointType::acc32 },
    {"TotWhImp"    , PointType::acc32 },
    {"Wh_SF"       , PointType::sunssf },
    {"W"           , PointType::int16},
    {"W_SF"        , PointType::sunssf},
    {"MA1"         , PointType::string, 8 },
    {"RCnt"        , PointType::uint32},
    {"OS"          , PointType::uint32},
    {"Epoch"       , PointType::uint32},
    {"TZO"         , PointType::int16 },
    {"EpochSetCnt" , PointType::uint32 },
    {"EpochSetOS"  , PointType::uint32 },
    {"DI"          , PointType::uint16 },
    {"DO"          , PointType::uint16 },
    {"Meta1"       , PointType::string, 70 },
    {"Meta2"       , PointType::string, 50 },
    {"Meta3"       , PointType::string, 50 },
    {"Evt"         , PointType::bitfield32 },
    {"NSig"        , PointType::uint16 },
    {"BSig"        , PointType::uint16 },
    {"Sig"         , PointType::blob },

};
// clang-format on

enum struct SnapshotType {
    CURRENT = 0,
    TURN_ON = 1,
    TURN_OFF = 2,
    START = 3,
    END = 4
};

enum SnapshotStatus {
    VALID = 0,
    INVALID = 1,
    UPDATING = 2,
    FAILED_GENERAL = 3,
    FAILED_NOT_ENABLED = 4,
    FAILED_FEEDBACK = 5
};

// BSM custom model 64901 BSM style singed snapshot
class SignedSnapshot : public SunspecModelBase<BSMSnapshotPointInitList.size(), BSMSnapshotPointInitList> {

public:
    explicit SignedSnapshot(const transport::DataVector& data) : SunspecModelBase(data) {
    }

    // {"Type"        , PointType::enum16 },
    sunspec::enum16 Type() const {
        return enum16_at(Model[0].offset);
    }

    // {"Status"      , PointType::enum16 },
    sunspec::enum16 Status() const {
        return enum16_at(Model[1].offset);
    }

    // {"RCR"         , PointType::acc32 },
    sunspec::acc32 RCR() const {
        return acc32_at(Model[2].offset);
    }

    // {"TotWhImp"    , PointType::acc32 },
    sunspec::acc32 TotWhImp() const {
        return acc32_at(Model[3].offset);
    }

    // {"Wh_SF"       , PointType::sunssf },
    sunspec::sunssf Wh_SF() const {
        return sunssf_at(Model[4].offset);
    }

    // {"W"           , PointType::int16},
    sunspec::int16 W() const {
        return int16_at(Model[5].offset);
    }

    // {"W_SF"        , PointType::sunssf},
    sunspec::sunssf W_SF() const {
        return sunssf_at(Model[6].offset);
    }

    // {"MA1"         , PointType::string, 8 },
    sunspec::string MA1() const {
        return string_at_with_length(Model[7].offset, Model[7].length_in_bytes);
    }

    // {"RCnt"        , PointType::uint32},
    sunspec::uint32 RCnt() const {
        return uint32_at(Model[8].offset);
    }

    // {"OS"          , PointType::uint32},
    sunspec::uint32 OS() const {
        return uint32_at(Model[9].offset);
    }

    // {"Epoch"       , PointType::uint32},
    sunspec::uint32 Epoch() const {
        return uint32_at(Model[10].offset);
    }

    // {"TZO"         , PointType::int16 },
    sunspec::int16 TZO() const {
        return int16_at(Model[11].offset);
    }

    // {"EpochSetCnt" , PointType::uint32 },
    sunspec::uint32 EpochSetCnt() const {
        return uint32_at(Model[12].offset);
    }

    // {"EpochSetOS"  , PointType::uint32 },
    sunspec::uint32 EpochSetOS() const {
        return uint32_at(Model[13].offset);
    }

    // {"DI"          , PointType::uint16 },
    sunspec::uint16 DI() const {
        return uint16_at(Model[14].offset);
    }

    // {"DO"          , PointType::uint16 },
    sunspec::uint16 DO() const {
        return uint16_at(Model[15].offset);
    }

    // {"Meta1"       , PointType::string, 70 },
    sunspec::string Meta1() const {
        return string_at_with_length(Model[16].offset, Model[16].length_in_bytes);
    }

    // {"Meta2"       , PointType::string, 50 },
    sunspec::string Meta2() const {
        return string_at_with_length(Model[17].offset, Model[17].length_in_bytes);
    }

    // {"Meta3"       , PointType::string, 50 },
    sunspec::string Meta3() const {
        return string_at_with_length(Model[18].offset, Model[18].length_in_bytes);
    }

    // {"Evt"         , PointType::bitfield32 }
    sunspec::bitfield32 Evt() const {
        return bitfield32_at(Model[19].offset);
    }

    // {"NSig"        , PointType::uint16 },
    sunspec::uint16 NSig() const {
        return uint16_at(Model[20].offset);
    }

    // {"BSig"        , PointType::uint16 },
    sunspec::uint16 BSig() const {
        return uint16_at(Model[21].offset);
    }

    sunspec::string Sig() const {
        return string_at_with_length_from_binary_data(Model[22].offset, BSig());
    }

    sunspec::string SigPadded() const {
        return string_at_with_length_from_binary_data(Model[22].offset, NSig() * 2);
    }
};

// BSM custom model 64903 OCMF signed snapshot
// 41792 1   ID   Model ID uint16 no 64903 Model OCMF data
// 41793 1   L    Model Payload Length uint16 no 372 Without the fields 'Model ID' and 'Length of payload.
// 41794 1   Typ  Snapshot Type enum16 no 0 Signed snapshot status, see chapter 17.5
// 41795 1   St   Snapshot Status enum16 no See chapter 17.5, write to the Status field of the corresponding snapshot to
// create it. 41796 496 O    OCMF string no OCMF representation of the snapshot “Signed Current Snapshot”, the metadata
// field 1 is used as OCMF identity

// clang-format off
inline constexpr PointInitializerList BSM_OCMFSnapshotPointInitList = {

    {"ID", PointType::uint16},
    {"L", PointType::uint16},
    {"Type", PointType::enum16},
    {"St", PointType::enum16},
    {"O", PointType::string, 496}

};
// clang-format on

class SignedOCMFSnapshot
    : public SunspecModelBase<BSM_OCMFSnapshotPointInitList.size(), BSM_OCMFSnapshotPointInitList> {

public:
    explicit SignedOCMFSnapshot(const transport::DataVector& data) : SunspecModelBase(data) {
    }

    // { "ID", PointType::uint16 },
    sunspec::uint16 ID() const {
        constexpr std::size_t index = 0;
        return uint16_at(Model[index].offset);
    }

    // { "L", PointType::uint16 },
    sunspec::uint16 L() const {
        constexpr std::size_t index = 1;
        return uint16_at(Model[index].offset);
    }

    // { "Type", PointType::enum16 },
    sunspec::enum16 Type() const {
        constexpr std::size_t index = 2;
        return enum16_at(Model[index].offset);
    }

    // { "St", PointType::enum16 },
    sunspec::enum16 St() const {
        constexpr std::size_t index = 3;
        return enum16_at(Model[index].offset);
    }

    // { "O", PointType::string , 496 }
    sunspec::string O() const {
        constexpr std::size_t index = 4;
        return string_at_with_length(Model[index].offset, Model[index].length_in_bytes);
    }
};
} // namespace bsm

#endif // POWERMETER_BSM_BSMSNAPSHOTMODEL_HPP
