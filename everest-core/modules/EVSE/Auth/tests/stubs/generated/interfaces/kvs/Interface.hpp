#ifndef KVS_INTERFACE_HPP
#define KVS_INTERFACE_HPP

#include <iostream>
#include <string>
#include <variant>

#include <nlohmann/json.hpp>
// #include <utils/types.hpp>

using nlohmann::json;

using Array = nlohmann::json::array_t;
using Object = nlohmann::json::object_t;

class kvsIntf {
private:
    std::variant<std::nullptr_t, Array, Object, bool, double, int, std::string> value;

public:
    kvsIntf() {
    }
    void call_store(std::string key,
                    std::variant<std::nullptr_t, Array, Object, bool, double, int, std::string> value) {
        std::cout << "Store called!" << std::endl;
        this->value = value;
    }
    std::variant<std::nullptr_t, Array, Object, bool, double, int, std::string> call_load(std::string key) {
        std::cout << "Load called!" << std::endl;

        return this->value;
    }
};

#endif // KVS_INTERFACE_HPP
