# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

import os
import sys
import csv

from pathlib import Path
from filetestgenerator import FileTestGenerator
from valuegenerator import AcrossFileGenerator

print("Running API Serialisation Test Autogenerator")

code_path = sys.argv[1]
test_path = sys.argv[2]
skip_path = sys.argv[3]
write_path = sys.argv[4]

name = "API.hpp"

across_file_generator = AcrossFileGenerator()
generators = []


def write_tests(generator):
    Path(write_path).mkdir(parents=True, exist_ok=True)
    with open(generator.get_file_path("hpp"), 'w') as test_file:
        test_file.write(generator.get_code_hpp())
    with open(generator.get_file_path("cpp"), 'w') as test_file:
        test_file.write(generator.get_code_cpp())

    os.system("clang-format -i " + generator.get_file_path("hpp") +
              " " + generator.get_file_path("cpp"))


def get_make_contents():
    result = "target_sources(${TEST_TARGET_NAME} PRIVATE\n"
    for generator in generators:
        result += "    " + generator.get_name() + ".cpp\n"
    result += ")"
    return result


def get_deny_list():
    deny_lists = {}
    with open(skip_path, newline='') as csvfile:
        deny_lists_reader = csv.reader(csvfile, delimiter=',', quotechar='"')
        for row in deny_lists_reader:
            deny_lists[row[0]] = row[1:]
    return deny_lists


def get_single_deny_list(deny_lists, file_to_test):
    single_deny_list = []
    for key in deny_lists.keys():
        if key in file_to_test:
            single_deny_list = deny_lists[key]
    return single_deny_list


deny_lists = get_deny_list()
license_header = ""
api_files_found = False
for root, dirs, files in os.walk(code_path):
    if name in files:
        file_to_test = os.path.join(root, name)
        print("Generating test for: ", file_to_test)
        api_files_found = True
        generator = FileTestGenerator(
            file_to_test, write_path, across_file_generator, code_path, license_header,
            get_single_deny_list(deny_lists, file_to_test))
        generator.generate_code()
        generators.append(generator)
if not api_files_found:
    raise FileNotFoundError(
        "API files were not found in include directory\nExpected for example: {include}/everest_api_types/auth/API.hpp\nInclude path that was given is: " + code_path)

for generator in generators:
    generator.generate_code(with_warning=True)

for generator in generators:
    write_tests(generator)

with open(write_path + "CMakeLists.txt", 'w') as make_file:
    make_file.write(get_make_contents())
