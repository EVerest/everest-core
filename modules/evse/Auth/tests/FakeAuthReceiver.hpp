
#include <map>

/**
 * @brief This implementation is only used for testing purposes. It is used to check which EVSE have received
 * authorization.
 *
 */
class FakeAuthReceiver {

private:
    std::map<int32_t, bool> evse_index_to_authorization_map;

public:
    FakeAuthReceiver(){};
    explicit FakeAuthReceiver(const std::vector<int32_t>& evse_indices) {
        for (const auto evse_index : evse_indices) {
            evse_index_to_authorization_map[evse_index] = false;
        }
    };
    void add_evse_index(const int32_t evse_index) {
        evse_index_to_authorization_map[evse_index] = false;
    };
    void authorize(const int32_t evse_index) {
        evse_index_to_authorization_map[evse_index] = true;
    };
    void deauthorize(const int32_t evse_index) {
        evse_index_to_authorization_map[evse_index] = false;
    };
    bool get_authorization(const int32_t evse_index) {
        return evse_index_to_authorization_map[evse_index];
    };
    void reset() {
        for (auto& e : evse_index_to_authorization_map) {
            e.second = false;
        }
    };
};
