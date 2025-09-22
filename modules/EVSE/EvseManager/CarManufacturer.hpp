// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef CAR_MANUFACTURER_HPP
#define CAR_MANUFACTURER_HPP

#include <generated/interfaces/evse_manager/Implementation.hpp>
#include <string>

/*
 Not all manufacturers can be determined from the MAC addresses since they are using MAC addresses from the OBC
 manufacturer, so many will be Unknown.

 used for mapping:

 Tesla: 0C:29:8F, 4C:FC:AA, 54:F8:F0, 98:ED:5C,
        DC:44:27:1x:xx:xx/28
 VW: 00:7D:FA (used for all brands such as Skoda as well)

 not used here since they might be in use in multiple cars:

 Opel Ampera (GM): 04:4E:AF (LG Innotek, OBC manufacturer)
 Mercedes EQC: CC:88:26 (LG Innotek, OBC manufacturer)
 BMW: 00:01:A9 (seems not to be used by OBCs of BMW)
 BMW iX: 00:30:AB 22kW OBC (Delta Networks, Inc.)
 BMW i4: EC:65:CC 11kW OBC (Panasonic Automotive Systems Company of America)
 Polestar 2: 48:C5:8D Lear Corporation GmbH
*/

namespace module {

// Helper function to determine the car manufacturer from MAC address
types::evse_manager::CarManufacturer get_manufacturer_from_mac(const std::string& mac);

} // namespace module

#endif // CAR_MANUFACTURER_HPP
