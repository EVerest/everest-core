// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef OFFER_HPP
#define OFFER_HPP

#include "Market.hpp"
#include <generated/interfaces/energy/Interface.hpp>
#include <vector>

namespace module {

class Offer {
public:
    Offer(Market& market);

    boost::optional<types::energy::OptimizerTarget> optimizer_target;
    ScheduleReq import_offer, export_offer;

private:
    void create_offer_for_local_market(Market& market);
};

std::ostream& operator<<(std::ostream& out, const Offer& self);

} // namespace module

#endif // OFFER_HPP
