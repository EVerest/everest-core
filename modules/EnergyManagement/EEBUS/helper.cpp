#include "helper.hpp"

namespace module {

types::energy::ExternalLimits translate_to_external_limits(const common_types::LoadLimit& load_limit) {
    types::energy::ExternalLimits limits;
    std::vector<types::energy::ScheduleReqEntry> schedule_import;

    auto create_active_req = [](std::chrono::time_point<date::utc_clock> timestamp, double total_power_W) {
        types::energy::ScheduleReqEntry schedule_req_entry;
        types::energy::LimitsReq limits_req;
        schedule_req_entry.timestamp = Everest::Date::to_rfc3339(timestamp);
        types::energy::NumberWithSource total_power;
        total_power.value = total_power_W;
        total_power.source = "EEBUS LPC";
        limits_req.total_power_W = total_power;
        schedule_req_entry.limits_to_leaves = limits_req;
        schedule_req_entry.limits_to_root = limits_req;
        return schedule_req_entry;
    };

    auto create_inactive_req = [](std::chrono::time_point<date::utc_clock> timestamp) {
        types::energy::ScheduleReqEntry schedule_req_entry;
        schedule_req_entry.timestamp = Everest::Date::to_rfc3339(timestamp);
        schedule_req_entry.limits_to_leaves = types::energy::LimitsReq();
        schedule_req_entry.limits_to_root = types::energy::LimitsReq();
        return schedule_req_entry;
    };

    const auto now = date::utc_clock::from_sys(std::chrono::system_clock::now());

    if (load_limit.is_active()) {
        schedule_import.push_back(create_active_req(now, load_limit.value()));
        if (load_limit.duration_nanoseconds() > 0) {
            const auto timestamp = now + std::chrono::nanoseconds(load_limit.duration_nanoseconds());
            schedule_import.push_back(create_inactive_req(timestamp));
        }
    } else {
        schedule_import.push_back(create_inactive_req(now));
    }
    limits.schedule_import = schedule_import;
    return limits;
}

} // namespace module
