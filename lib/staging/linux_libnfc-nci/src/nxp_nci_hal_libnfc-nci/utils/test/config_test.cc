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
#include <gtest/gtest.h>

#include <config.h>

namespace {
const char SIMPLE_CONFIG_FILE[] = "/data/local/tmp/test_config.conf";
const char SIMPLE_CONFIG[] =
    "# Simple config file test\n\
STRING_VALUE=\"Hello World!\"\n\
#COMMENTED_OUT_VALUE=1\n\
NUM_VALUE=42\n\
BYTES_VALUE={0A:0b:0C:fF:00}\n";

const char INVALID_CONFIG1[] =
    "# This is an invalid config\n\
# Config values must contain an = sign\n\
TEST:1";

const char INVALID_CONFIG2[] =
    "# This is an invalid config\n\
# Byte arrays must contain at least one value\n\
TEST={}";

const char INVALID_CONFIG3[] =
    "# This is an invalid config\n\
# String values cannot be empty\n\
TEST=\"\"";

const char INVALID_CONFIG4[] =
    "# This is an invalid config\n\
# Multiple config entries with the same key\n\
TEST=1\n\
TEST=2";

const char INVALID_CONFIG5[] =
    "# This is an invalid config\n\
# Byte value width incorrect\n\
BYTES_VALUE={0A:0b:0C:1:00}\n";
}  // namespace

class ConfigTestFromFile : public ::testing::Test {
 protected:
  void SetUp() override {
    FILE* fp = fopen(SIMPLE_CONFIG_FILE, "wt");
    fwrite(SIMPLE_CONFIG, 1, sizeof(SIMPLE_CONFIG), fp);
    fclose(fp);
  }
};

TEST(ConfigTestFromString, test_simple_config) {
  ConfigFile config;
  config.parseFromString(SIMPLE_CONFIG);
  EXPECT_FALSE(config.hasKey("UNKNOWN_VALUE"));
  EXPECT_FALSE(config.hasKey("COMMENTED_OUT_VALUE"));
  EXPECT_TRUE(config.hasKey("NUM_VALUE"));
  EXPECT_TRUE(config.hasKey("STRING_VALUE"));
  EXPECT_TRUE(config.hasKey("BYTES_VALUE"));
}

TEST(ConfigTestFromString, test_simple_values) {
  ConfigFile config;
  config.parseFromString(SIMPLE_CONFIG);
  EXPECT_EQ(config.getUnsigned("NUM_VALUE"), 42u);
  EXPECT_EQ(config.getString("STRING_VALUE"), "Hello World!");
  auto bytes = config.getBytes("BYTES_VALUE");
  EXPECT_EQ(bytes.size(), 5u);
  EXPECT_EQ(bytes[0], 10);
  EXPECT_EQ(bytes[1], 11);
  EXPECT_EQ(bytes[2], 12);
  EXPECT_EQ(bytes[3], 255);
  EXPECT_EQ(bytes[4], 0);
}

TEST(ConfigTestFromString, test_invalid_configs) {
  ConfigFile config1;
  EXPECT_DEATH(config1.parseFromString(INVALID_CONFIG1), "");
  ConfigFile config2;
  EXPECT_DEATH(config2.parseFromString(INVALID_CONFIG2), "");
  ConfigFile config3;
  EXPECT_DEATH(config3.parseFromString(INVALID_CONFIG3), "");
  ConfigFile config4;
  EXPECT_DEATH(config4.parseFromString(INVALID_CONFIG4), "");
  ConfigFile config5;
  EXPECT_DEATH(config5.parseFromString(INVALID_CONFIG5), "");
}

TEST(ConfigTestFromString, test_clear) {
  ConfigFile config;
  EXPECT_FALSE(config.hasKey("NUM_VALUE"));
  config.parseFromString(SIMPLE_CONFIG);
  EXPECT_TRUE(config.hasKey("NUM_VALUE"));
  EXPECT_EQ(config.getUnsigned("NUM_VALUE"), 42u);
  config.clear();
  EXPECT_FALSE(config.hasKey("NUM_VALUE"));
  EXPECT_DEATH(config.getUnsigned("NUM_VALUE"), "");
}

TEST(ConfigTestFromString, test_isEmpty) {
  ConfigFile config;
  EXPECT_TRUE(config.isEmpty());
  config.parseFromString(SIMPLE_CONFIG);
  EXPECT_FALSE(config.isEmpty());
  config.clear();
  EXPECT_TRUE(config.isEmpty());
}

TEST_F(ConfigTestFromFile, test_file_based_config) {
  ConfigFile config;
  config.parseFromFile(SIMPLE_CONFIG_FILE);
  EXPECT_FALSE(config.hasKey("UNKNOWN_VALUE"));
  EXPECT_EQ(config.getUnsigned("NUM_VALUE"), 42u);
  EXPECT_EQ(config.getString("STRING_VALUE"), "Hello World!");
  auto bytes = config.getBytes("BYTES_VALUE");
  EXPECT_EQ(bytes.size(), 5u);
  EXPECT_EQ(bytes[0], 10);
  EXPECT_EQ(bytes[1], 11);
  EXPECT_EQ(bytes[2], 12);
  EXPECT_EQ(bytes[3], 255);
  EXPECT_EQ(bytes[4], 0);
}
