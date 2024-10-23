/*
 * Copyright 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <map>
#include <string>
#include <vector>

class ConfigValue {
 public:
  enum Type { UNSIGNED, STRING, BYTES };

  ConfigValue();
  explicit ConfigValue(std::string);
  explicit ConfigValue(unsigned);
  explicit ConfigValue(std::vector<uint8_t>);
  Type getType() const;
  std::string getString() const;
  unsigned getUnsigned() const;
  std::vector<uint8_t> getBytes() const;

  bool parseFromString(std::string in);

 private:
  Type type_;
  std::string value_string_;
  unsigned value_unsigned_;
  std::vector<uint8_t> value_bytes_;
};

class ConfigFile {
 public:
  void parseFromFile(const std::string& file_name);
  void parseFromString(const std::string& config);
  void addConfig(const std::string& config, ConfigValue& value);

  bool hasKey(const std::string& key);
  std::string getString(const std::string& key);
  unsigned getUnsigned(const std::string& key);
  std::vector<uint8_t> getBytes(const std::string& key);

  bool isEmpty();
  void clear();

 private:
  ConfigValue& getValue(const std::string& key);

  std::map<std::string, ConfigValue> values_;
};
