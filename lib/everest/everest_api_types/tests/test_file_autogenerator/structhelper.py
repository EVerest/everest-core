# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

import re

from helper import Helper
from valuegenerator import ValueGenerator


class StructHelper(Helper):
    w = Helper.regex_whitespaces
    default_value =  r"\{[A-z:_<>0-9\.]+\}?"
    regex_single_field = r"([A-z:_<>0-9]+" + w + Helper.regex_field_or_class_name + r"(" + r"\{[A-z:_<>0-9\.]+\}?" + r")?" + r";)" + w
    regex_fields = r"(" + w + regex_single_field + r")*"

    def __init__(self, representation, across_file_struct_generator, namespace=None, enum_map=None):
        super().__init__(representation, namespace)
        self.has_mandatory_fields = self.get_fields_mandatory().__len__() > 0
        self.has_optional_fields = self.get_fields_optional().__len__() > 0
        self.name_no_opt_helper = self.get_type() + "TestHelperNoOptionalFieldsSet"
        if enum_map is None:
            enum_map = {"test": []}
        self.value_generator = ValueGenerator(self.get_type(), self.get_namespace(), enum_map, across_file_struct_generator)

    def get_regex_structure_type(self):
        return "struct"

    def get_fields(self):
        fields = []
        representation = self.get_representation()
        field = self.regex_single_field
        matches = re.findall(field, representation)
        for x in matches:
            sanatized = re.sub(";", "", x[0])
            # structs with default values are tested as if they had no default values
            sanatized = re.sub(self.default_value, "", sanatized)
            fields.append(sanatized)
        return fields

    def get_fields_mandatory(self):
        return self.get_fields_helper(True)

    def get_fields_helper(self, mandatory):
        a = []
        for x in self.get_fields():
            if ("std::optional" not in x) == mandatory:
                split = re.split(Helper.regex_whitespaces, x)
                assert split.__len__() == 2
                a.append((split[0], split[1]))
        return a

    def get_fields_optional(self):
        a = self.get_fields_helper(False)
        for i in range(a.__len__()):
            san = ValueGenerator.generics_extractor("std::optional", a[i][0])
            a[i] = (san, a[i][1])
        return a

    @staticmethod
    def get_regex_find_in_file():
        w = Helper.regex_whitespaces
        return r"(struct" + w + r"[A-z]*" + w + r"\{" + StructHelper.regex_fields + r"\};)"

    def get_helper_namespace(self):
        return "helper_" + self.get_type()

    def generate_test_helpers(self):
        code = self.get_code_helpers()
        self.test_helpers_code = code
        self.add_struct_to_helper()

    def generate_test_helper_headers(self):
        self.test_helpers_headers_code = self.get_code_helpers(True)


    def get_code_helpers(self, signature_only= False):
        name = self.get_helper_namespace()
        return self.get_code_generate_function(signature_only) + self.get_code_verify_function(signature_only)

    def get_code_generate_function(self, signature_only = False):
        code = "\ntemplate <> " + self.get_type_with_namespace() + " generate<" + self.get_type_with_namespace() + ">(bool set_optional_fields"
        if signature_only: return code + ");\n"
        token = "generated_object"
        code += ") { \n" + self.get_type() + " " + token + ";\n" + self.generate_set_fields(self.get_fields_mandatory(), token) + "if (set_optional_fields) {" + self.generate_set_fields(self.get_fields_optional(), token) + "}\n" + "return " + token + ";\n" + "}\n"
        return code
    
    def get_code_verify_function(self, signature_only = False):
        code = "\ntemplate <> void verify<" + self.get_type_with_namespace() + ">(" + self.get_type_with_namespace()+ " original, " + self.get_type_with_namespace() + " result)"
        if signature_only: return code + ";\n"
        code += "{\n" + self.generate_test_fields(self.get_fields_mandatory(), False) + self.generate_test_fields(self.get_fields_optional(), True) + "}\n"
        return code


    def generate_set_fields(self, fields, token):
        code = ""
        for i in fields:
            code += self.value_generator.generate_corresponding_value(token + "." + i[1],
                                                                      i[0], i[1], struct_helper=self)
        return code
    
    def generate_test_fields(self, fields, is_optional):
        code = ""
        for i in fields:
            code += self.value_generator.generate_corresponding_field_test(i[1], i[0], self.get_namespace(), is_optional)
        return code

    def add_struct_to_helper(self):
        self.value_generator.add_struct_helper(self)

    def generate_test(self):
        code = []
        type = self.get_type()
        type_namespace = self.get_type_with_namespace()
        if self.has_mandatory_fields and self.has_optional_fields:
            code.append("\nTEST(" + "%s" + ", " + type + "_no_optional_fields_set){\n    gen_test<" + type_namespace + ">(" + "false" + ");\n}\n")    
        code.append("\nTEST(" + "%s" + ", " + type + "_all_optional_fields_set){\n    gen_test<" + type_namespace + ">();\n}\n")
        self.tests_code = code

    def generate_code(self):
        self.value_generator.clear_manual_generation()
        self.generate_test_helpers()
        self.generate_test()
        self.generate_test_helper_headers()

    @staticmethod
    def get_value_generation(a, variable_name_prefix = "", variable_name_suffix=""):
        return "generate<" + a + ">();\n        "
    @staticmethod
    def get_comparison(a, b, variable_name_prefix = "", variable_name_suffix=""):
        return "verify(" + a + ", " + b + ");\n        "

