#include <gmock/gmock.h>

#include "ocpp/v201/device_model_storage.hpp"

namespace ocpp::v201 {
class DeviceModelStorageMock : public DeviceModelStorage {
public:
    MOCK_METHOD(DeviceModelMap, get_device_model, ());
    MOCK_METHOD(std::optional<VariableAttribute>, get_variable_attribute,
                (const Component&, const Variable&, const AttributeEnum&));
    MOCK_METHOD(std::vector<VariableAttribute>, get_variable_attributes,
                (const Component&, const Variable&, const std::optional<AttributeEnum>&));
    MOCK_METHOD(bool, set_variable_attribute_value,
                (const Component&, const Variable&, const AttributeEnum&, const std::string&));
    MOCK_METHOD(void, check_integrity, ());
};
} // namespace ocpp::v201
