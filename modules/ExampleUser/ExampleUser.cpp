#include "ExampleUser.hpp"

namespace module {

void ExampleUser::init() {
    invoke_init(*p_example_user);
}

void ExampleUser::ready() {
    invoke_ready(*p_example_user);
}

} // namespace module
