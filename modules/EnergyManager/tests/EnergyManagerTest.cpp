// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "EnergyManager.hpp"
#include "EnergyManagerImplStub.hpp"
#include "Market.hpp"
#include <gtest/gtest.h>
#include <utils/date.hpp>

#include <optional>
#include <utility>

/*
Example JSON from EvseManager

{
    "children": [],
    "energy_usage_root": {
        "current_A": {
        "L1": 0.029999999329447746,
        "L2": 0,
        "L3": 0,
        "N": 0
        },
        "energy_Wh_import": {
        "L1": 1.7999999523162842,
        "L2": 0,
        "L3": 0,
        "total": 1.7999999523162842
        },
        "power_W": {
        "L1": 2,
        "L2": 0,
        "L3": 0,
        "total": 2
        },
        "timestamp": "2024-03-27T12:41:16.864Z",
        "voltage_V": {
        "L1": 248.10000610351562,
        "L2": 0,
        "L3": 0
        }
    },
    "node_type": "Evse",
    "priority_request": false,
    "schedule_export": [
        {
        "limits_to_leaves": {},
        "limits_to_root": {
            "ac_max_current_A": 0,
            "ac_max_phase_count": 0,
            "ac_min_current_A": 0,
            "ac_min_phase_count": 0,
            "ac_supports_changing_phases_during_charging": false
        },
        "timestamp": "2024-03-27T12:00:00.000Z"
        }
    ],
    "schedule_import": [
        {
        "limits_to_leaves": {
            "ac_max_current_A": 24,
            "ac_max_phase_count": 3
        },
        "limits_to_root": {
            "ac_max_current_A": 32,
            "ac_max_phase_count": 3,
            "ac_min_current_A": 6,
            "ac_min_phase_count": 3,
            "ac_supports_changing_phases_during_charging": false
        },
        "timestamp": "2024-03-27T12:40:49.988Z"
        },
        {
        "limits_to_leaves": {
            "ac_max_current_A": 10,
            "ac_max_phase_count": 3
        },
        "limits_to_root": {
            "ac_max_current_A": 32,
            "ac_max_phase_count": 3,
            "ac_min_current_A": 6,
            "ac_min_phase_count": 3,
            "ac_supports_changing_phases_during_charging": false
        },
        "timestamp": "2024-03-27T12:41:04.988Z"
        },
        {
        "limits_to_leaves": {
            "ac_max_current_A": 32,
            "ac_max_phase_count": 3
        },
        "limits_to_root": {
            "ac_max_current_A": 32,
            "ac_max_phase_count": 3,
            "ac_min_current_A": 6,
            "ac_min_phase_count": 3,
            "ac_supports_changing_phases_during_charging": false
        },
        "timestamp": "2024-03-27T12:42:04.988Z"
        },
        {
        "limits_to_leaves": {
            "ac_max_current_A": 12,
            "ac_max_phase_count": 3
        },
        "limits_to_root": {
            "ac_max_current_A": 32,
            "ac_max_phase_count": 3,
            "ac_min_current_A": 6,
            "ac_min_phase_count": 3,
            "ac_supports_changing_phases_during_charging": false
        },
        "timestamp": "2024-03-27T12:43:04.988Z"
        },
        {
        "limits_to_leaves": {
            "ac_max_current_A": 0,
            "ac_max_phase_count": 3
        },
        "limits_to_root": {
            "ac_max_current_A": 32,
            "ac_max_phase_count": 3,
            "ac_min_current_A": 6,
            "ac_min_phase_count": 3,
            "ac_supports_changing_phases_during_charging": false
        },
        "timestamp": "2024-03-27T12:44:04.988Z"
        }
    ],
    "uuid": "evse_manager"
}
*/

/*
Example MQTT from grid_connection_point
Mar 28 14:20:13 everest/grid_connection_point/energy_grid/var:
{"data":{"children":[{"children":[{"children":[],"energy_usage_root":{"current_A":{"L1":0.029999999329447746,"L2":0.0,"L3":0.0,"N":0.0},"energy_Wh_import":{"L1":0.0,"L2":0.0,"L3":0.0,"total":0.0},"power_W":{"L1":2.0,"L2":0.0,"L3":0.0,"total":2.0},"timestamp":"2024-03-28T14:20:12.570Z","voltage_V":{"L1":249.10000610351563,"L2":0.0,"L3":0.0}},"node_type":"Evse","priority_request":false,"schedule_export":[{"limits_to_leaves":{},"limits_to_root":{"ac_max_current_A":0.0,"ac_max_phase_count":0,"ac_min_current_A":0.0,"ac_min_phase_count":0,"ac_supports_changing_phases_during_charging":false},"timestamp":"2024-03-28T14:00:00.000Z"}],"schedule_import":[{"limits_to_leaves":{"ac_max_current_A":24.0,"ac_max_phase_count":3},"limits_to_root":{"ac_max_current_A":32.0,"ac_max_phase_count":3,"ac_min_current_A":6.0,"ac_min_phase_count":3,"ac_supports_changing_phases_during_charging":false},"timestamp":"2024-03-28T14:19:53.557Z"},{"limits_to_leaves":{"ac_max_current_A":10.0,"ac_max_phase_count":3},"limits_to_root":{"ac_max_current_A":32.0,"ac_max_phase_count":3,"ac_min_current_A":6.0,"ac_min_phase_count":3,"ac_supports_changing_phases_during_charging":false},"timestamp":"2024-03-28T14:20:50.557Z"},{"limits_to_leaves":{"ac_max_current_A":32.0,"ac_max_phase_count":3},"limits_to_root":{"ac_max_current_A":32.0,"ac_max_phase_count":3,"ac_min_current_A":6.0,"ac_min_phase_count":3,"ac_supports_changing_phases_during_charging":false},"timestamp":"2024-03-28T14:21:50.557Z"},{"limits_to_leaves":{"ac_max_current_A":12.0,"ac_max_phase_count":3},"limits_to_root":{"ac_max_current_A":32.0,"ac_max_phase_count":3,"ac_min_current_A":6.0,"ac_min_phase_count":3,"ac_supports_changing_phases_during_charging":false},"timestamp":"2024-03-28T14:22:50.557Z"},{"limits_to_leaves":{"ac_max_current_A":0.0,"ac_max_phase_count":3},"limits_to_root":{"ac_max_current_A":32.0,"ac_max_phase_count":3,"ac_min_current_A":6.0,"ac_min_phase_count":3,"ac_supports_changing_phases_during_charging":false},"timestamp":"2024-03-28T14:23:50.557Z"}],"uuid":"evse_manager"}],"node_type":"Generic","schedule_export":[{"limits_to_leaves":{"ac_max_current_A":32.0,"ac_max_phase_count":3},"limits_to_root":{"ac_max_current_A":32.0,"ac_max_phase_count":3},"timestamp":"2024-03-28T14:00:00.000Z"}],"schedule_import":[{"limits_to_leaves":{"ac_max_current_A":32.0},"limits_to_root":{"ac_max_current_A":32.0,"ac_max_phase_count":3},"timestamp":"2024-03-28T14:18:23.044Z"}],"uuid":"cls_energy_node"}],"node_type":"Generic","schedule_export":[{"limits_to_leaves":{"ac_max_current_A":32.0,"ac_max_phase_count":3},"limits_to_root":{"ac_max_current_A":32.0,"ac_max_phase_count":3},"timestamp":"2024-03-28T14:00:00.000Z"}],"schedule_import":[{"limits_to_leaves":{"ac_max_current_A":30.0},"limits_to_root":{"ac_max_current_A":32.0,"ac_max_phase_count":3},"timestamp":"2024-03-28T14:18:24.446Z"}],"uuid":"grid_connection_point"},"name":"energy_flow_request"}

{
  "children": [
    {
      "children": [
        {
          "children": [],
          "energy_usage_root": {
            "current_A": {
              "L1": 0.029999999329447746,
              "L2": 0,
              "L3": 0,
              "N": 0
            },
            "energy_Wh_import": {
              "L1": 0,
              "L2": 0,
              "L3": 0,
              "total": 0
            },
            "power_W": {
              "L1": 2,
              "L2": 0,
              "L3": 0,
              "total": 2
            },
            "timestamp": "2024-03-28T14:20:12.570Z",
            "voltage_V": {
              "L1": 249.10000610351562,
              "L2": 0,
              "L3": 0
            }
          },
          "node_type": "Evse",
          "priority_request": false,
          "schedule_export": [
            {
              "limits_to_leaves": {},
              "limits_to_root": {
                "ac_max_current_A": 0,
                "ac_max_phase_count": 0,
                "ac_min_current_A": 0,
                "ac_min_phase_count": 0,
                "ac_supports_changing_phases_during_charging": false
              },
              "timestamp": "2024-03-28T14:00:00.000Z"
            }
          ],
          "schedule_import": [
            {
              "limits_to_leaves": {
                "ac_max_current_A": 24,
                "ac_max_phase_count": 3
              },
              "limits_to_root": {
                "ac_max_current_A": 32,
                "ac_max_phase_count": 3,
                "ac_min_current_A": 6,
                "ac_min_phase_count": 3,
                "ac_supports_changing_phases_during_charging": false
              },
              "timestamp": "2024-03-28T14:19:53.557Z"
            },
            {
              "limits_to_leaves": {
                "ac_max_current_A": 10,
                "ac_max_phase_count": 3
              },
              "limits_to_root": {
                "ac_max_current_A": 32,
                "ac_max_phase_count": 3,
                "ac_min_current_A": 6,
                "ac_min_phase_count": 3,
                "ac_supports_changing_phases_during_charging": false
              },
              "timestamp": "2024-03-28T14:20:50.557Z"
            },
            {
              "limits_to_leaves": {
                "ac_max_current_A": 32,
                "ac_max_phase_count": 3
              },
              "limits_to_root": {
                "ac_max_current_A": 32,
                "ac_max_phase_count": 3,
                "ac_min_current_A": 6,
                "ac_min_phase_count": 3,
                "ac_supports_changing_phases_during_charging": false
              },
              "timestamp": "2024-03-28T14:21:50.557Z"
            },
            {
              "limits_to_leaves": {
                "ac_max_current_A": 12,
                "ac_max_phase_count": 3
              },
              "limits_to_root": {
                "ac_max_current_A": 32,
                "ac_max_phase_count": 3,
                "ac_min_current_A": 6,
                "ac_min_phase_count": 3,
                "ac_supports_changing_phases_during_charging": false
              },
              "timestamp": "2024-03-28T14:22:50.557Z"
            },
            {
              "limits_to_leaves": {
                "ac_max_current_A": 0,
                "ac_max_phase_count": 3
              },
              "limits_to_root": {
                "ac_max_current_A": 32,
                "ac_max_phase_count": 3,
                "ac_min_current_A": 6,
                "ac_min_phase_count": 3,
                "ac_supports_changing_phases_during_charging": false
              },
              "timestamp": "2024-03-28T14:23:50.557Z"
            }
          ],
          "uuid": "evse_manager"
        }
      ],
      "node_type": "Generic",
      "schedule_export": [
        {
          "limits_to_leaves": {
            "ac_max_current_A": 32,
            "ac_max_phase_count": 3
          },
          "limits_to_root": {
            "ac_max_current_A": 32,
            "ac_max_phase_count": 3
          },
          "timestamp": "2024-03-28T14:00:00.000Z"
        }
      ],
      "schedule_import": [
        {
          "limits_to_leaves": {
            "ac_max_current_A": 32
          },
          "limits_to_root": {
            "ac_max_current_A": 32,
            "ac_max_phase_count": 3
          },
          "timestamp": "2024-03-28T14:18:23.044Z"
        }
      ],
      "uuid": "cls_energy_node"
    }
  ],
  "node_type": "Generic",
  "schedule_export": [
    {
      "limits_to_leaves": {
        "ac_max_current_A": 32,
        "ac_max_phase_count": 3
      },
      "limits_to_root": {
        "ac_max_current_A": 32,
        "ac_max_phase_count": 3
      },
      "timestamp": "2024-03-28T14:00:00.000Z"
    }
  ],
  "schedule_import": [
    {
      "limits_to_leaves": {
        "ac_max_current_A": 30
      },
      "limits_to_root": {
        "ac_max_current_A": 32,
        "ac_max_phase_count": 3
      },
      "timestamp": "2024-03-28T14:18:24.446Z"
    }
  ],
  "uuid": "grid_connection_point"
}
*/

namespace module {
std::ostream& operator<<(std::ostream& os, const std::vector<types::energy::EnforcedLimits>& limits) {
    if (limits.size() > 0) {
        std::uint32_t count = 0;
        for (const auto& i : limits) {
            os << "[" << count++ << "] " << i;
        }
    } else {
        os << "<no enforced limits>";
    }
    return os;
}
} // namespace module

namespace {

constexpr std::size_t c_timestamp_size = 24U;

const ModuleInfo c_module_info{
    "EnergyManager",
    {},               // authors
    "MIT",            // license
    "energy_manager", // ID
    {
        // path etc
        "",
        // path libexec
        "",
        // path share
        "",
    },
    false, // telemetry_enabled
    false, // global_errors_enabled
};

const types::powermeter::Powermeter c_energy_usage_root{
    "2024-03-27T12:41:16.864Z",                         // Timestamp of measurement
    {1.7999999523162842, 1.7999999523162842, 0.0, 0.0}, // types::units::Energy - import
    std::nullopt,                                       // optional - std::string - meter ID
    std::nullopt,                                       // optional - bool - phase_seq_error
    std::nullopt,                                       // optional - types::units::Energy - export
    {{2.0, 2.0, 0.0, 0.0}},                             // optional - types::units::Power - Instantaneous power in Watt
    {{248.10000610351562, 0.0, 0.0}},                   // optional - types::units::Voltage - in Volts
    std::nullopt,                                       // optional - types::units::ReactivePower
    {{std::nullopt, 0.029999999329447746, 0.0, 0.0, 0.0}}, // optional - types::units::Current - in Amps
    std::nullopt,                                          // optional - types::units::Frequency - in Hertz
    std::nullopt, // optional - types::units_signed::Energy - energy in Wh - import
    std::nullopt, // optional - types::units_signed::Energy - energy in Wh - export
    std::nullopt, // optional - types::units_signed::Power - in Watts
    std::nullopt, // optional - types::units_signed::Voltage - in Volts
    std::nullopt, // optional - types::units_signed::ReactivePower
    std::nullopt, // optional - types::units_signed::Current - in Amps
    std::nullopt, // optional - types::units_signed::Frequency - in Hertz
    std::nullopt, // optional - types::units_signed::SignedMeterValue
};

/*
types::energy::LimitsReq
std::optional<float> total_power_W
std::optional<float> ac_max_current_A
std::optional<float> ac_min_current_A
std::optional<int32_t> ac_max_phase_count
std::optional<int32_t> ac_min_phase_count
std::optional<bool> ac_supports_changing_phases_during_charging
*/

constexpr types::energy::LimitsReq limit() {
    return {std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt};
}

constexpr types::energy::LimitsReq limit_zero() {
    return {0.0, 0.0, 0.0, 0, 0, false};
}

constexpr types::energy::LimitsReq limit(float max_current) {
    return {std::nullopt, max_current, std::nullopt, 3, std::nullopt, std::nullopt};
}

constexpr types::energy::LimitsReq limit_no_phase(float max_current) {
    return {std::nullopt, max_current, std::nullopt, std::nullopt, std::nullopt, std::nullopt};
}

constexpr types::energy::LimitsReq limit(float max_current, float min_current) {
    return {std::nullopt, max_current, min_current, 3, 3, false};
}

const std::vector<types::energy::ScheduleReqEntry> c_schedule_import{
    {
        "2024-03-27T12:40:49.988Z", // timestamp for this sample in RFC3339 UTC format
        limit(32.0, 6.0),           // types::energy::LimitsReq - root
        limit(24.0),                // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
    {
        "2024-03-27T12:41:04.988Z", // timestamp for this sample in RFC3339 UTC format
        limit(32.0, 6.0),           // types::energy::LimitsReq - root
        limit(10.0),                // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
    {
        "2024-03-27T12:42:04.988Z", // timestamp for this sample in RFC3339 UTC format
        limit(32.0, 6.0),           // types::energy::LimitsReq - root
        limit(32.0),                // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
    {
        "2024-03-27T12:43:04.988Z", // timestamp for this sample in RFC3339 UTC format
        limit(32.0, 6.0),           // types::energy::LimitsReq - root
        limit(12.0),                // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
    {
        "2024-03-27T12:44:04.988Z", // timestamp for this sample in RFC3339 UTC format
        limit(32.0, 6.0),           // types::energy::LimitsReq - root
        limit(0.0),                 // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
};

const std::vector<types::energy::ScheduleReqEntry> c_schedule_export{
    {
        "2024-03-27T12:00:00.000Z", // timestamp for this sample in RFC3339 UTC format
        limit_zero(),               // types::energy::LimitsReq - root
        {},                         // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
};

types::energy::EnergyFlowRequest energy_flow_request{
    {},                            // children, std::vector<types::energy::EnergyFlowRequest>
    "evse_manager",                // UUID for this node
    types::energy::NodeType::Evse, // node_type
    false,                         // optional - bool priority_request
    std::nullopt,                  // optional - types::energy::OptimizerTarget
    c_energy_usage_root,           // optional - types::powermeter::Powermeter - root
    std::nullopt,                  // optional - types::powermeter::Powermeter - leaf
    {c_schedule_import},           // optional - std::vector<types::energy::ScheduleReqEntry> - import
    {c_schedule_export},           // optional - std::vector<types::energy::ScheduleReqEntry> - export
};

// ----------------------------------------------------------------------------
// grid_connection_point example

namespace grid_connection_point {

const types::powermeter::Powermeter c_energy_usage_root_evse_manager{
    "2024-03-28T14:20:12.570Z",       // Timestamp of measurement
    {0.0, 0.0, 0.0, 0.0},             // types::units::Energy - import
    std::nullopt,                     // optional - std::string - meter ID
    std::nullopt,                     // optional - bool - phase_seq_error
    std::nullopt,                     // optional - types::units::Energy - export
    {{2.0, 2.0, 0.0, 0.0}},           // optional - types::units::Power - Instantaneous power in Watt
    {{249.10000610351562, 0.0, 0.0}}, // optional - types::units::Voltage - in Volts
    std::nullopt,                     // optional - types::units::ReactivePower
    {{std::nullopt, 0.029999999329447746, 0.0, 0.0, 0.0}}, // optional - types::units::Current - in Amps
    std::nullopt,                                          // optional - types::units::Frequency - in Hertz
    std::nullopt, // optional - types::units_signed::Energy - energy in Wh - import
    std::nullopt, // optional - types::units_signed::Energy - energy in Wh - export
    std::nullopt, // optional - types::units_signed::Power - in Watts
    std::nullopt, // optional - types::units_signed::Voltage - in Volts
    std::nullopt, // optional - types::units_signed::ReactivePower
    std::nullopt, // optional - types::units_signed::Current - in Amps
    std::nullopt, // optional - types::units_signed::Frequency - in Hertz
    std::nullopt, // optional - types::units_signed::SignedMeterValue
};

const std::vector<types::energy::ScheduleReqEntry> c_schedule_export_evse_manager{
    {
        "2024-03-28T14:00:00.000Z", // timestamp for this sample in RFC3339 UTC format
        limit_zero(),               // types::energy::LimitsReq - root
        {},                         // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
};

const std::vector<types::energy::ScheduleReqEntry> c_schedule_import_evse_manager{
    {
        "2024-03-28T14:19:53.557Z", // timestamp for this sample in RFC3339 UTC format
        limit(32.0, 6.0),           // types::energy::LimitsReq - root
        limit(24.0),                // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
    {
        "2024-03-28T14:20:50.557Z", // timestamp for this sample in RFC3339 UTC format
        limit(32.0, 6.0),           // types::energy::LimitsReq - root
        limit(10.0),                // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
    {
        "2024-03-28T14:21:50.557Z", // timestamp for this sample in RFC3339 UTC format
        limit(32.0, 6.0),           // types::energy::LimitsReq - root
        limit(32.0),                // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
    {
        "2024-03-28T14:22:50.557Z", // timestamp for this sample in RFC3339 UTC format
        limit(32.0, 6.0),           // types::energy::LimitsReq - root
        limit(12.0),                // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
    {
        "2024-03-28T14:23:50.557Z", // timestamp for this sample in RFC3339 UTC format
        limit(32.0, 6.0),           // types::energy::LimitsReq - root
        limit(0.0),                 // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
};

const types::energy::EnergyFlowRequest c_efr_evse_manager{
    {},                               // children, std::vector<types::energy::EnergyFlowRequest>
    "evse_manager",                   // UUID for this node
    types::energy::NodeType::Evse,    // node_type
    false,                            // optional - bool priority_request
    std::nullopt,                     // optional - types::energy::OptimizerTarget
    c_energy_usage_root_evse_manager, // optional - types::powermeter::Powermeter - root
    std::nullopt,                     // optional - types::powermeter::Powermeter - leaf
    {c_schedule_import_evse_manager}, // optional - std::vector<types::energy::ScheduleReqEntry> - import
    {c_schedule_export_evse_manager}, // optional - std::vector<types::energy::ScheduleReqEntry> - export
};

const std::vector<types::energy::ScheduleReqEntry> c_schedule_export_cls_energy_node{
    {
        "2024-03-28T14:00:00.000Z", // timestamp for this sample in RFC3339 UTC format
        limit(32.0),                // types::energy::LimitsReq - root
        limit(32.0),                // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
};

const std::vector<types::energy::ScheduleReqEntry> c_schedule_import_cls_energy_node{
    {
        "2024-03-28T14:18:23.044Z", // timestamp for this sample in RFC3339 UTC format
        limit(32.0),                // types::energy::LimitsReq - root
        limit_no_phase(32.0),       // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
};

const types::energy::EnergyFlowRequest c_efr_cls_energy_node{
    {c_efr_evse_manager},                // children, std::vector<types::energy::EnergyFlowRequest>
    "cls_energy_node",                   // UUID for this node
    types::energy::NodeType::Generic,    // node_type
    std::nullopt,                        // optional - bool priority_request
    std::nullopt,                        // optional - types::energy::OptimizerTarget
    std::nullopt,                        // optional - types::powermeter::Powermeter - root
    std::nullopt,                        // optional - types::powermeter::Powermeter - leaf
    {c_schedule_import_cls_energy_node}, // optional - std::vector<types::energy::ScheduleReqEntry> - import
    {c_schedule_export_cls_energy_node}, // optional - std::vector<types::energy::ScheduleReqEntry> - export
};

const std::vector<types::energy::ScheduleReqEntry> c_schedule_export_grid_connection_point{
    {
        "2024-03-28T14:00:00.000Z", // timestamp for this sample in RFC3339 UTC format
        limit(32.0),                // types::energy::LimitsReq - root
        limit(32.0),                // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
};

const std::vector<types::energy::ScheduleReqEntry> c_schedule_import_grid_connection_point{
    {
        "2024-03-28T14:18:24.446Z", // timestamp for this sample in RFC3339 UTC format
        limit(32.0),                // types::energy::LimitsReq - root
        limit_no_phase(32.0),       // types::energy::LimitsReq - leaf
        std::nullopt,               // optional - float Conversion efficiency from root to leaf
        std::nullopt,               // optional - types::energy_price_information::PricePerkWh - Price information
    },
};

const types::energy::EnergyFlowRequest c_efr_grid_connection_point{
    {c_efr_cls_energy_node},                   // children, std::vector<types::energy::EnergyFlowRequest>
    "grid_connection_point",                   // UUID for this node
    types::energy::NodeType::Generic,          // node_type
    std::nullopt,                              // optional - bool priority_request
    std::nullopt,                              // optional - types::energy::OptimizerTarget
    std::nullopt,                              // optional - types::powermeter::Powermeter - root
    std::nullopt,                              // optional - types::powermeter::Powermeter - leaf
    {c_schedule_import_grid_connection_point}, // optional - std::vector<types::energy::ScheduleReqEntry> - import
    {c_schedule_export_grid_connection_point}, // optional - std::vector<types::energy::ScheduleReqEntry> - export
};

} // namespace grid_connection_point
} // namespace

namespace module::test {

void schedule_test(const types::energy::EnergyFlowRequest& energy_flow_request, const std::string& start_time_str,
                   float expected_limit) {
    SCOPED_TRACE(std::string("schedule_test(") + start_time_str + ")");
    struct module::Conf config {
        230.0,     // nominal_ac_voltage
            1,     // update_interval
            60,    // schedule_interval_duration
            1,     // schedule_total_duration
            0.5,   // slice_ampere
            500,   // slice_watt
            false, // debug
    };
    std::unique_ptr<energyIntf> energy;
    auto energy_managerImpl = std::make_unique<module::stub::energy_managerImplStub>();

    module::EnergyManager manager(c_module_info, std::move(energy_managerImpl), std::move(energy), config);

    const auto start_time = Everest::Date::from_rfc3339(start_time_str);
    module::globals.init(start_time, config.schedule_interval_duration, config.schedule_total_duration,
                         config.slice_ampere, config.slice_watt, config.debug, energy_flow_request);
    auto optimized_values = manager.run_optimizer(energy_flow_request);

    // check result
    // std::cout << optimized_values << std::endl;
    ASSERT_EQ(optimized_values.size(), 1);
    EXPECT_EQ(optimized_values[0].uuid, "evse_manager");
    EXPECT_EQ(optimized_values[0].valid_until.size(), c_timestamp_size);
    // valid_until should be start_time + config.update_interval * 10
    const auto valid_until = Everest::Date::to_rfc3339(start_time + std::chrono::seconds(config.update_interval * 10));
    EXPECT_EQ(optimized_values[0].valid_until, valid_until);

    ASSERT_TRUE(optimized_values[0].limits_root_side.has_value());
    ASSERT_FALSE(optimized_values[0].limits_root_side.value().total_power_W.has_value());
    ASSERT_TRUE(optimized_values[0].limits_root_side.value().ac_max_current_A.has_value());
    ASSERT_FALSE(optimized_values[0].limits_root_side.value().ac_max_phase_count.has_value());
    EXPECT_EQ(optimized_values[0].limits_root_side.value().ac_max_current_A.value(), expected_limit);

    ASSERT_TRUE(optimized_values[0].schedule.has_value());

    if (&energy_flow_request == &grid_connection_point::c_efr_grid_connection_point) {
        const auto energy_flow_schedules = energy_flow_request.children[0].children[0].schedule_import.value();
        // size is c_schedule_import + 1 (an initial schedule is added)
        ASSERT_EQ(optimized_values[0].schedule.value().size(), energy_flow_schedules.size() + 3);

        EXPECT_EQ(optimized_values[0].schedule.value()[0].timestamp.size(), c_timestamp_size);
        // start_time to the current hour
        EXPECT_EQ(optimized_values[0].schedule.value()[0].timestamp,
                  energy_flow_request.schedule_export.value()[0].timestamp);
        EXPECT_FALSE(optimized_values[0].schedule.value()[0].limits_to_root.total_power_W.has_value());
        EXPECT_TRUE(optimized_values[0].schedule.value()[0].limits_to_root.ac_max_current_A.has_value());
        EXPECT_FALSE(optimized_values[0].schedule.value()[0].limits_to_root.ac_max_phase_count.has_value());
        EXPECT_FALSE(optimized_values[0].schedule.value()[0].price_per_kwh.has_value());
        EXPECT_EQ(optimized_values[0].schedule.value()[0].limits_to_root.ac_max_current_A.value(), 24.0);

        const auto schedules = optimized_values[0].schedule.value();

        // skip 1st 3 entries (TODO: find out why)
        std::uint8_t i = 0;
        for (std::uint8_t index = 3; index < schedules.size(); index++, i++) {
            SCOPED_TRACE(schedules[index].timestamp);
            EXPECT_EQ(schedules[index].timestamp, energy_flow_schedules[i].timestamp);
            EXPECT_FALSE(schedules[index].limits_to_root.total_power_W.has_value());
            EXPECT_TRUE(schedules[index].limits_to_root.ac_max_current_A.has_value());
            EXPECT_FALSE(schedules[index].limits_to_root.ac_max_phase_count.has_value());
            EXPECT_EQ(schedules[index].limits_to_root.ac_max_current_A.value(),
                      energy_flow_schedules[i].limits_to_leaves.ac_max_current_A.value());
        }
    } else {
        // size is c_schedule_import + 1 (an initial schedule is added)
        ASSERT_EQ(optimized_values[0].schedule.value().size(), c_schedule_import.size() + 1);

        EXPECT_EQ(optimized_values[0].schedule.value()[0].timestamp.size(), c_timestamp_size);
        // start_time to the current hour
        EXPECT_EQ(optimized_values[0].schedule.value()[0].timestamp, "2024-03-27T12:00:00.000Z");
        EXPECT_FALSE(optimized_values[0].schedule.value()[0].limits_to_root.total_power_W.has_value());
        EXPECT_TRUE(optimized_values[0].schedule.value()[0].limits_to_root.ac_max_current_A.has_value());
        EXPECT_FALSE(optimized_values[0].schedule.value()[0].limits_to_root.ac_max_phase_count.has_value());
        EXPECT_FALSE(optimized_values[0].schedule.value()[0].price_per_kwh.has_value());
        EXPECT_EQ(optimized_values[0].schedule.value()[0].limits_to_root.ac_max_current_A.value(), 24.0);

        const auto schedules = optimized_values[0].schedule.value();

        // skip 1st entry
        std::uint8_t i = 0;
        for (auto itt = std::next(schedules.begin()); itt != schedules.end(); itt++, i++) {
            SCOPED_TRACE(itt->timestamp);
            EXPECT_EQ(itt->timestamp, c_schedule_import[i].timestamp);
            EXPECT_FALSE(itt->limits_to_root.total_power_W.has_value());
            EXPECT_TRUE(itt->limits_to_root.ac_max_current_A.has_value());
            EXPECT_FALSE(itt->limits_to_root.ac_max_phase_count.has_value());
            EXPECT_EQ(itt->limits_to_root.ac_max_current_A.value(),
                      c_schedule_import[i].limits_to_leaves.ac_max_current_A.value());
        }
    }
}

} // namespace module::test

namespace module {

TEST(EnergyManagerTest, empty) {
    struct module::Conf config {
        230.0,     // nominal_ac_voltage
            1,     // update_interval
            60,    // schedule_interval_duration
            1,     // schedule_total_duration
            0.5,   // slice_ampere
            500,   // slice_watt
            false, // debug
    };
    std::unique_ptr<energyIntf> energy;
    auto energy_managerImpl = std::make_unique<module::stub::energy_managerImplStub>();

    module::EnergyManager manager(c_module_info, std::move(energy_managerImpl), std::move(energy), config);

    types::energy::EnergyFlowRequest energy_flow_request;
    module::globals.init(date::utc_clock::now(), config.schedule_interval_duration, config.schedule_total_duration,
                         config.slice_ampere, config.slice_watt, config.debug, energy_flow_request);
    auto optimized_values = manager.run_optimizer(energy_flow_request);
    std::cout << optimized_values << std::endl;
    EXPECT_EQ(optimized_values.size(), 0);
}

TEST(EnergyManagerTest, noSchedules) {
    struct module::Conf config {
        230.0,     // nominal_ac_voltage
            1,     // update_interval
            60,    // schedule_interval_duration
            1,     // schedule_total_duration
            0.5,   // slice_ampere
            500,   // slice_watt
            false, // debug
    };
    std::unique_ptr<energyIntf> energy;
    auto energy_managerImpl = std::make_unique<module::stub::energy_managerImplStub>();

    module::EnergyManager manager(c_module_info, std::move(energy_managerImpl), std::move(energy), config);

    types::energy::EnergyFlowRequest energy_flow_request{
        {},                            // children, std::vector<types::energy::EnergyFlowRequest>
        "evse_manager",                // UUID for this node
        types::energy::NodeType::Evse, // node_type
        false,                         // optional - bool priority_request
        std::nullopt,                  // optional - types::energy::OptimizerTarget
        c_energy_usage_root,           // optional - types::powermeter::Powermeter - root
        std::nullopt,                  // optional - types::powermeter::Powermeter - leaf
        std::nullopt,                  // optional - std::vector<types::energy::ScheduleReqEntry> - import
        std::nullopt,                  // optional - std::vector<types::energy::ScheduleReqEntry> - export
    };

    // use a fixed time for repeatable tests
    const auto start_time = Everest::Date::from_rfc3339("2024-01-01T12:00:00.000Z");
    module::globals.init(start_time, config.schedule_interval_duration, config.schedule_total_duration,
                         config.slice_ampere, config.slice_watt, config.debug, energy_flow_request);
    auto optimized_values = manager.run_optimizer(energy_flow_request);

    // check result
    // std::cout << optimized_values << std::endl;
    ASSERT_EQ(optimized_values.size(), 1);
    EXPECT_EQ(optimized_values[0].uuid, "evse_manager");
    EXPECT_EQ(optimized_values[0].valid_until.size(), c_timestamp_size);
    // valid_until should be start_time + config.update_interval * 10
    const auto valid_until = Everest::Date::to_rfc3339(start_time + std::chrono::seconds(config.update_interval * 10));
    EXPECT_EQ(optimized_values[0].valid_until, valid_until);

    ASSERT_TRUE(optimized_values[0].limits_root_side.has_value());
    ASSERT_TRUE(optimized_values[0].limits_root_side.value().total_power_W.has_value());
    ASSERT_TRUE(optimized_values[0].limits_root_side.value().ac_max_current_A.has_value());
    ASSERT_FALSE(optimized_values[0].limits_root_side.value().ac_max_phase_count.has_value());
    EXPECT_EQ(optimized_values[0].limits_root_side.value().total_power_W.value(), 0.0);
    EXPECT_EQ(optimized_values[0].limits_root_side.value().ac_max_current_A.value(), 0.0);

    ASSERT_TRUE(optimized_values[0].schedule.has_value());
    ASSERT_EQ(optimized_values[0].schedule.value().size(), 1);
    EXPECT_EQ(optimized_values[0].schedule.value()[0].timestamp.size(), c_timestamp_size);
    EXPECT_TRUE(optimized_values[0].schedule.value()[0].limits_to_root.total_power_W.has_value());
    EXPECT_TRUE(optimized_values[0].schedule.value()[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_FALSE(optimized_values[0].schedule.value()[0].limits_to_root.ac_max_phase_count.has_value());
    EXPECT_EQ(optimized_values[0].schedule.value()[0].limits_to_root.total_power_W.value(), 0.0);
    EXPECT_EQ(optimized_values[0].schedule.value()[0].limits_to_root.ac_max_current_A.value(), 0.0);
    EXPECT_FALSE(optimized_values[0].schedule.value()[0].price_per_kwh.has_value());
}

TEST(EnergyManagerTest, schedules) {
    struct module::Conf config {
        230.0,     // nominal_ac_voltage
            2,     // update_interval
            60,    // schedule_interval_duration
            1,     // schedule_total_duration
            0.5,   // slice_ampere
            500,   // slice_watt
            false, // debug
    };
    std::unique_ptr<energyIntf> energy;
    auto energy_managerImpl = std::make_unique<module::stub::energy_managerImplStub>();

    module::EnergyManager manager(c_module_info, std::move(energy_managerImpl), std::move(energy), config);

    types::energy::EnergyFlowRequest energy_flow_request{
        {},                            // children, std::vector<types::energy::EnergyFlowRequest>
        "evse_manager",                // UUID for this node
        types::energy::NodeType::Evse, // node_type
        false,                         // optional - bool priority_request
        std::nullopt,                  // optional - types::energy::OptimizerTarget
        c_energy_usage_root,           // optional - types::powermeter::Powermeter - root
        std::nullopt,                  // optional - types::powermeter::Powermeter - leaf
        {c_schedule_import},           // optional - std::vector<types::energy::ScheduleReqEntry> - import
        {c_schedule_export},           // optional - std::vector<types::energy::ScheduleReqEntry> - export
    };

    // check energy_flow_request initialisation
    ASSERT_EQ(energy_flow_request.children.size(), 0);
    ASSERT_TRUE(energy_flow_request.schedule_import.has_value());
    ASSERT_TRUE(energy_flow_request.schedule_export.has_value());
    ASSERT_EQ(energy_flow_request.schedule_import.value().size(), c_schedule_import.size());
    ASSERT_EQ(energy_flow_request.schedule_export.value().size(), c_schedule_export.size());
    ASSERT_EQ(c_schedule_import.size(), 5);
    ASSERT_EQ(c_schedule_export.size(), 1);

    // start a little ahead of the 1st schedule
    const auto start_time = Everest::Date::from_rfc3339("2024-03-27T12:40:40.000Z");
    module::globals.init(start_time, config.schedule_interval_duration, config.schedule_total_duration,
                         config.slice_ampere, config.slice_watt, config.debug, energy_flow_request);
    auto optimized_values = manager.run_optimizer(energy_flow_request);

    // check result
    // std::cout << optimized_values << std::endl;
    ASSERT_EQ(optimized_values.size(), 1);
    EXPECT_EQ(optimized_values[0].uuid, "evse_manager");
    EXPECT_EQ(optimized_values[0].valid_until.size(), c_timestamp_size);
    // valid_until should be start_time + config.update_interval * 10
    const auto valid_until = Everest::Date::to_rfc3339(start_time + std::chrono::seconds(config.update_interval * 10));
    EXPECT_EQ(optimized_values[0].valid_until, valid_until);

    ASSERT_TRUE(optimized_values[0].limits_root_side.has_value());
    ASSERT_FALSE(optimized_values[0].limits_root_side.value().total_power_W.has_value());
    ASSERT_TRUE(optimized_values[0].limits_root_side.value().ac_max_current_A.has_value());
    ASSERT_FALSE(optimized_values[0].limits_root_side.value().ac_max_phase_count.has_value());
    EXPECT_EQ(optimized_values[0].limits_root_side.value().ac_max_current_A.value(), 24.0);

    ASSERT_TRUE(optimized_values[0].schedule.has_value());
    // size is c_schedule_import + 1 (an initial schedule is added)
    ASSERT_EQ(optimized_values[0].schedule.value().size(), c_schedule_import.size() + 1);

    EXPECT_EQ(optimized_values[0].schedule.value()[0].timestamp.size(), c_timestamp_size);
    // start_time to the current hour
    EXPECT_EQ(optimized_values[0].schedule.value()[0].timestamp, "2024-03-27T12:00:00.000Z");
    EXPECT_FALSE(optimized_values[0].schedule.value()[0].limits_to_root.total_power_W.has_value());
    EXPECT_TRUE(optimized_values[0].schedule.value()[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_FALSE(optimized_values[0].schedule.value()[0].limits_to_root.ac_max_phase_count.has_value());
    EXPECT_FALSE(optimized_values[0].schedule.value()[0].price_per_kwh.has_value());
    EXPECT_EQ(optimized_values[0].schedule.value()[0].limits_to_root.ac_max_current_A.value(), 24.0);

    const auto schedules = optimized_values[0].schedule.value();

    // skip 1st entry
    std::uint8_t i = 0;
    for (auto itt = std::next(schedules.begin()); itt != schedules.end(); itt++, i++) {
        SCOPED_TRACE(itt->timestamp);
        EXPECT_EQ(itt->timestamp, c_schedule_import[i].timestamp);
        EXPECT_FALSE(itt->limits_to_root.total_power_W.has_value());
        EXPECT_TRUE(itt->limits_to_root.ac_max_current_A.has_value());
        EXPECT_FALSE(itt->limits_to_root.ac_max_phase_count.has_value());
        EXPECT_EQ(itt->limits_to_root.ac_max_current_A.value(),
                  c_schedule_import[i].limits_to_leaves.ac_max_current_A.value());
    }
}

TEST(EnergyManagerTest, before) {
    test::schedule_test(energy_flow_request, "2024-03-27T12:40:00.000Z", 24.0);
}

TEST(EnergyManagerTest, equal_1) {
    test::schedule_test(energy_flow_request, "2024-03-27T12:40:49.988Z", 24.0);
}

TEST(EnergyManagerTest, between_1_2) {
    test::schedule_test(energy_flow_request, "2024-03-27T12:40:55.200Z", 24.0);
}

TEST(EnergyManagerTest, equal_2) {
    test::schedule_test(energy_flow_request, "2024-03-27T12:41:04.988Z", 10.0);
}

TEST(EnergyManagerTest, between_2_3) {
    test::schedule_test(energy_flow_request, "2024-03-27T12:41:50.200Z", 10.0);
}

TEST(EnergyManagerTest, equal_3) {
    test::schedule_test(energy_flow_request, "2024-03-27T12:42:04.988Z", 32.0);
}

TEST(EnergyManagerTest, between_3_4) {
    test::schedule_test(energy_flow_request, "2024-03-27T12:42:50.200Z", 32.0);
}

TEST(EnergyManagerTest, equal_4) {
    test::schedule_test(energy_flow_request, "2024-03-27T12:43:04.988Z", 12.0);
}

TEST(EnergyManagerTest, between_4_5) {
    test::schedule_test(energy_flow_request, "2024-03-27T12:43:50.200Z", 12.0);
}

TEST(EnergyManagerTest, equal_5) {
    test::schedule_test(energy_flow_request, "2024-03-27T12:44:04.988Z", 0.0);
}

TEST(EnergyManagerTest, after) {
    test::schedule_test(energy_flow_request, "2024-03-27T12:50:04.988Z", 0.0);
}

// ----------------------------------------------------------------------------
// grid_connection_point example Mar 28 14:20:13

TEST(EnergyManagerTest, gcp_before) {
    test::schedule_test(grid_connection_point::c_efr_grid_connection_point, "2024-03-28T14:05:00.000Z", 24.0);
}

TEST(EnergyManagerTest, gcp_now) {
    test::schedule_test(grid_connection_point::c_efr_grid_connection_point, "2024-03-28T14:20:13.000Z", 24.0);
}

TEST(EnergyManagerTest, gcp_equal_1) {
    test::schedule_test(grid_connection_point::c_efr_grid_connection_point, "2024-03-28T14:19:53.557Z", 24.0);
}

TEST(EnergyManagerTest, gcp_between_1_2) {
    test::schedule_test(grid_connection_point::c_efr_grid_connection_point, "2024-03-28T14:20:30.557Z", 24.0);
}

TEST(EnergyManagerTest, gcp_equal_2) {
    test::schedule_test(grid_connection_point::c_efr_grid_connection_point, "2024-03-28T14:20:50.557Z", 10.0);
}

TEST(EnergyManagerTest, gcp_between_2_3) {
    test::schedule_test(grid_connection_point::c_efr_grid_connection_point, "2024-03-28T14:21:30.557Z", 10.0);
}

TEST(EnergyManagerTest, gcp_equal_3) {
    test::schedule_test(grid_connection_point::c_efr_grid_connection_point, "2024-03-28T14:21:50.557Z", 32.0);
}

TEST(EnergyManagerTest, gcp_between_3_4) {
    test::schedule_test(grid_connection_point::c_efr_grid_connection_point, "2024-03-28T14:22:30.557Z", 32.0);
}

TEST(EnergyManagerTest, gcp_equal_4) {
    test::schedule_test(grid_connection_point::c_efr_grid_connection_point, "2024-03-28T14:22:50.557Z", 12.0);
}

TEST(EnergyManagerTest, gcp_between_4_5) {
    test::schedule_test(grid_connection_point::c_efr_grid_connection_point, "2024-03-28T14:23:30.557Z", 12.0);
}

TEST(EnergyManagerTest, gcp_equal_5) {
    test::schedule_test(grid_connection_point::c_efr_grid_connection_point, "2024-03-28T14:23:50.557Z", 0.0);
}

TEST(EnergyManagerTest, gcp_after) {
    test::schedule_test(grid_connection_point::c_efr_grid_connection_point, "2024-03-28T14:45:00.557Z", 0.0);
}

} // namespace module
