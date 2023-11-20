// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include <utils/yaml_loader.hpp>

#include <fstream>

#include <fmt/core.h>
#include <ryml.hpp>
#include <ryml_std.hpp>

#include <everest/logging.hpp>

static void yaml_error_handler(const char* msg, size_t len, ryml::Location loc, void*) {
    std::stringstream error_msg;
    error_msg << "YAML parsing error: ";

    if (loc) {
        if (not loc.name.empty()) {
            error_msg.write(loc.name.str, loc.name.len);
            error_msg << ":";
        }
        error_msg << loc.line << ":";
        if (loc.col) {
            error_msg << loc.col << ":";
        }
        if (loc.offset) {
            error_msg << " (" << loc.offset << "B):";
        }
    }
    error_msg.write(msg, len);

    throw std::runtime_error(error_msg.str());
}

struct RymlCallbackInitializer {
    RymlCallbackInitializer() {
        ryml::set_callbacks({nullptr, nullptr, nullptr, yaml_error_handler});
    }
};

static nlohmann::ordered_json ryml_to_nlohmann_json(const c4::yml::NodeRef& ryml_node) {
    if (ryml_node.is_map()) {
        // handle object
        auto object = nlohmann::ordered_json::object();
        for (const auto& child : ryml_node) {
            object[std::string(child.key().data(), child.key().len)] = ryml_to_nlohmann_json(child);
        }
        return object;
    } else if (ryml_node.is_seq()) {
        // handle array
        auto array = nlohmann::ordered_json::array();
        for (const auto& child : ryml_node) {
            array.emplace_back(ryml_to_nlohmann_json(child));
        }
        return array;
    } else if (ryml_node.empty() or ryml_node.val_is_null()) {
        return nullptr;
    } else {
        // check type of data
        const auto& value = ryml_node.val();
        std::string value_string(value.data(), value.len);
        const auto value_quoted = ryml_node.is_val_quoted();
        if (!value_quoted) {
            // check for numbers and booleans
            if (ryml_node.val().is_integer()) {
                return std::stoi(value_string);
            } else if (ryml_node.val().is_number()) {
                return std::stod(value_string);
            } else if (value_string == "true") {
                return true;
            } else if (value_string == "false") {
                return false;
            }
        }
        // nothing matched so far, should be string
        return value_string;
    }
}

static std::string load_yaml_content(std::filesystem::path path) {
    namespace fs = std::filesystem;

    if (path.extension().string() == ".json") {
        EVLOG_info << fmt::format("Deprecated: called load_yaml() with .json extension ('{}')", path.string());
        // try yaml first
        path.replace_extension(".yaml");
    } else if (path.extension().string() != ".yaml") {
        throw std::runtime_error(
            fmt::format("Trying to load a yaml file without yaml extension (path was '{}')", path.string()));
    }

    // first check for yaml, if not found try fall back to json and evlog debug deprecated
    if (fs::exists(path)) {
        std::ifstream ifs(path.string());
        return std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    }

    path.replace_extension(".json");

    if (fs::exists(path)) {
        EVLOG_info << "Deprecated: loaded file in json format";
        std::ifstream ifs(path.string());
        return std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    }

    // failed to find yaml and json
    throw std::runtime_error(fmt::format("File '{}.(yaml|json)' does not exist", path.stem().string()));
}

namespace Everest {

nlohmann::ordered_json load_yaml(const std::filesystem::path& path) {
    // FIXME (aw): using the static here this isn't a perfect solution
    static RymlCallbackInitializer ryml_callback_initializer;

    const auto content = load_yaml_content(path);
    // FIXME (aw): using parse_in_place would be faster but that will need the file as a whole char buffer
    const auto tree = ryml::parse_in_arena(ryml::to_csubstr(content));
    return ryml_to_nlohmann_json(tree.rootref());
}

} // namespace Everest
