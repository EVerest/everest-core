#include "sunspec_readerImpl.hpp"

using namespace everest;

namespace module {
namespace main {

void sunspec_readerImpl::init() {

    EVLOG(debug) << "Initializing SunspecReader module...";
    try {
          EVLOG(debug) << "Trying to make TCP connection...";
          this->tcp_connection = std::make_unique<connection::TCPConnection>(config.ip_address, config.port);
          EVLOG(debug) << "Trying to initialize MODBUS TCP client...";
          this->mb_client = std::make_unique<modbus::ModbusTCPClient>(*this->tcp_connection);
          EVLOG(debug) << "Trying to initialize Sunspec device mapping...";
          this->sdm = std::make_unique<sunspec::SunspecDeviceMapping>(*this->mb_client, config.unit);
          EVLOG(debug) << "Trying to scan SunspecDeviceMapping...";
          this->sdm->scan();
          EVLOG(debug) << "Initializing reader after scan...";
          this->reader = std::make_unique<sunspec::SunspecReader<double>>(config.query, *this->sdm);
    }
    catch (std::exception& e) {
        EVLOG(error) << "Error during SunspecReader initialization: " << e.what() << "\n";
        this->tcp_connection = nullptr;
        this->mb_client = nullptr;
        this->sdm = nullptr;
        this->reader = nullptr;
    }

}

void sunspec_readerImpl::ready() {
    if (this->reader != nullptr) {
        this->read_loop_thread = std::thread( [this] { run_read_loop(); } );
    }
    else {
        EVLOG(error) << "SunspecReader initialization failed. Skipping read step.";
    }
}

void sunspec_readerImpl::run_read_loop() {
    std::chrono::time_point<date::utc_clock> tp_loop;
    tp_loop = date::utc_clock::now();
    while(true) {

        if (this->read_loop_thread.shouldExit()) break;

        // Reading
        std::this_thread::sleep_until(tp_loop);
        tp_loop += std::chrono::milliseconds(config.read_interval);
        uint16_t read_value = this->reader->read();
        double converted_read_value = sunspec::utils::apply_scale_factor(read_value, reader->get_scale_factor());
        uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(date::utc_clock::now().time_since_epoch()).count();

        // Publishing
        Everest::json j;
        j["timestamp"] = timestamp;
        j["value"] = converted_read_value;
        EVLOG(debug) << "SunspecReader::run_read_loop() - Publishing: " << j;
        publish_measurement(j);

    }
}

} // namespace main
} // namespace module
