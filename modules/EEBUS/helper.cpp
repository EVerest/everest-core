#include "helper.hpp"

namespace module {

bool compare_use_case(const control_service::UseCase& uc1, const control_service::UseCase& uc2) {
    if (uc1.actor() != uc2.actor()) {
        return false;
    }
    if (uc1.name() != uc2.name()) {
        return false;
    }
    return true;
}

types::energy::ScheduleReqEntry create_active_schedule_req_entry(std::chrono::time_point<date::utc_clock> timestamp,
                                                                 double total_power_W) {
    types::energy::ScheduleReqEntry schedule_req_entry;
    types::energy::LimitsReq limits_req;
    schedule_req_entry.timestamp = Everest::Date::to_rfc3339(timestamp);
    limits_req.total_power_W = total_power_W;
    schedule_req_entry.limits_to_leaves = limits_req;
    schedule_req_entry.limits_to_root = limits_req;
    return schedule_req_entry;
}

types::energy::ScheduleReqEntry create_inactive_schedule_req_entry(std::chrono::time_point<date::utc_clock> timestamp) {
    types::energy::ScheduleReqEntry schedule_req_entry;
    schedule_req_entry.timestamp = Everest::Date::to_rfc3339(timestamp);
    schedule_req_entry.limits_to_leaves = types::energy::LimitsReq();
    schedule_req_entry.limits_to_root = types::energy::LimitsReq();
    return schedule_req_entry;
}

types::energy::ExternalLimits translate_to_external_limits(const common_types::LoadLimit& load_limit) {
    types::energy::ExternalLimits limits;
    std::vector<types::energy::ScheduleReqEntry> schedule_import;
    if (load_limit.is_active()) {
        schedule_import.push_back(create_active_schedule_req_entry(
            date::utc_clock::from_sys(std::chrono::system_clock::now()), load_limit.value()));
        if (load_limit.duration_nanoseconds() > 0) {
            std::chrono::time_point<date::utc_clock> timestamp;
            timestamp = date::utc_clock::from_sys(std::chrono::system_clock::now());
            timestamp += std::chrono::nanoseconds(load_limit.duration_nanoseconds());
            schedule_import.push_back(create_inactive_schedule_req_entry(timestamp));
        }
    } else {
        schedule_import.push_back(
            create_inactive_schedule_req_entry(date::utc_clock::from_sys(std::chrono::system_clock::now())));
    }
    limits.schedule_import = schedule_import;
    return limits;
}

} // namespace module
