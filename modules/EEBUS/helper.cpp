#include "helper.hpp"

namespace module {


bool compare_use_case(
    const control_service::UseCase& uc1,
    const control_service::UseCase& uc2
) {
    if (uc1.actor() != uc2.actor()) {
        return false;
    }
    if (uc1.name() != uc2.name()) {
        return false;
    }
    return true;
}

} // namespace module
