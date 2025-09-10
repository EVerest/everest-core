# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

import re
import warnings

from enumhelper import EnumHelper
from helper import Helper
from structhelper import StructHelper
from valuegenerator import ValueGenerator
from namespace import Namespace

class FileTestGenerator:
    enums_found = False
    structs_found = False
    files_to_include = []
    def __init__(self, api_file_path, test_folder_path, across_file_struct_generator, include_path, license_header, deny_list = []):
        self.license_header = license_header
        self.api_file_path = api_file_path
        self.test_folder_path = test_folder_path
        FileTestGenerator.files_to_include.append("#include \"" + re.sub(self.test_folder_path, "", FileTestGenerator.calculate_file_path(test_folder_path, api_file_path, "hpp")+ "\""))
        FileTestGenerator.files_to_include.append("#include \"" + re.sub(include_path, "", api_file_path) + "\"")
        api_file = open(api_file_path).read()
        sanitized = Helper.sanitize(api_file)
        self.api_file = sanitized

        namespace = Namespace.from_source_file(sanitized)
        self.namespace = namespace

        helpers = []
        enums_map = {}
        struct_matches = re.finditer(StructHelper.get_regex_find_in_file(), sanitized)
        enum_matches = re.finditer(EnumHelper.get_regex_find_in_file(), sanitized)
        for x in enum_matches:
            enum_helper = EnumHelper(x.group(), namespace)
            if not (enum_helper.get_type() in deny_list):
                enums_map[enum_helper.get_type()] = enum_helper.get_fields()
                helpers.append(enum_helper)
                FileTestGenerator.enums_found = True
        for x in struct_matches:
            struct_helper = StructHelper(x.group(), across_file_struct_generator, namespace, enums_map)
            if not (struct_helper.get_type() in deny_list):
                helpers.append(struct_helper)
                FileTestGenerator.structs_found = True
        self.helpers = helpers
        self.name = re.search(r"([^/]+)/API.hpp", api_file_path).group(1)
        self.code_cpp = ""
        self.code_hpp = ""

    def get_name(self):
        return self.name

    def get_test_helper_includes(self):
        result = ""
        for path in FileTestGenerator.files_to_include:
            result += path + "\n"
        return result

    @staticmethod
    def calculate_file_path(test_folder_path, api_file_path, file_ending):
        splitpath = re.split("/", api_file_path)
        return test_folder_path + splitpath[-2] + "." + file_ending

    def get_file_path(self, file_ending):
        return FileTestGenerator.calculate_file_path(self.test_folder_path, self.api_file_path, file_ending)

    def generate_code(self, with_warning = False):
        for i in self.helpers:
            i.generate_code()
        self.generate_code_cpp(with_warning)
        self.generate_code_hpp()

    def generate_code_cpp(self, with_warning = False):
        test_code = self.get_code_test()
        if test_code[0] != "" and with_warning:
            raise NotImplementedError ("The generation of a struct could not be done automatically, it needs to be supplied manually.\n"+
                                       "The suplying of manual helpers is not fully implemented. The imports need to be added and the manual helpers need to be registered with the accross file generator. \n"+
                                       "The problematic struct is probably related to: " + test_code[0])
        code = (
                self.license_header
                + "#include <gtest/gtest.h>\n"
                + "#include \"everest_api_types/" + self.name + "/codec.hpp\"\n"
                + "#include \"SerializationTestHelpers.hpp\"\n\n"
                + self.get_test_helper_includes()
                + "using namespace everest::lib::API::V1_0::types::" + self.name + ";\n\n"
                # needed for across file types
                + "using namespace everest::lib::API::V1_0::types;\n\n"
                + "//Automatic test helpers\n"
                + test_code[1] + "\n"
                + "//Tests\n"
                + test_code[2] + "\n"
        )
        self.code_cpp = code
        if with_warning:
            if test_code[0].__len__() > 0:
                warnings.warn("Not all required methods could be generated automatically, some need to be added manually, or the code generator neeeds to be updated.")
            if not FileTestGenerator.enums_found:
                raise ValueError("There was not a single enum found in all api files. This is probably wrong.")
            if not FileTestGenerator.structs_found:
                raise ValueError("There was not a single struct found in all api files. This is probably wrong.")


    def get_code_cpp(self):
        return self.code_cpp

    def get_code_test(self):
        code = ("", "", "")
        for i in self.helpers:
            helpers = i.get_test_helpers_code(self.name)
            code = (ValueGenerator.manual_generator.get_manual_helpers(), code[1] + helpers,
                    code[2] + i.get_tests_code(self.name))
        return code

    def get_code_header(self):
        code = ""
        for i in self.helpers:
            code += i.get_test_helpers_headers_code()
        return code

    def generate_code_hpp(self):
        code = (
            self.license_header
            + "#pragma once\n"
            + "#include \"everest_api_types/" + self.name + "/API.hpp\"\n"
            + self.get_code_header()
        )
        self.code_hpp = code

    def get_code_hpp(self):
        return self.code_hpp
