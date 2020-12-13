#include "kvsImpl.hpp"

namespace module {
namespace main {

void kvsImpl::init() {
}

void kvsImpl::ready() {
}

void kvsImpl::handle_store(std::string& key,
                           boost::variant<Array, Object, bool, double, int, std::nullptr_t, std::string>& value) {
    kvs[key] = value;
};

boost::variant<Array, Object, bool, double, int, std::nullptr_t, std::string> kvsImpl::handle_load(std::string& key) {
    return kvs[key];
};

void kvsImpl::handle_delete(std::string& key) {
    kvs.erase(key);
};

bool kvsImpl::handle_exists(std::string& key) {
    return kvs.count(key) != 0;
};

} // namespace main
} // namespace module
