# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

import re
import random

from helper import Helper


class EnumHelper(Helper):
    w = Helper.regex_whitespaces
    regex_enum_field = "(" + w + r"([A-z_0-9]+)(" + w + r")*)"
    regex_structure_type = "enum" + w + r"class"
    regex_find_in_file = regex_structure_type + w + \
        r"[A-z_][A-z0-9_]*" + w + r"\{((" + regex_enum_field + r",?" + w + r")*)\};"

    @staticmethod
    def get_regex_find_in_file():
        return EnumHelper.regex_find_in_file

    def get_regex_structure_type(self):
        return self.regex_structure_type

    def get_fields(self):
        fields_string = re.search(
            self.get_regex_find_in_file(), self.representation).group(1)
        a = []
        for i in re.finditer(self.regex_enum_field, fields_string):
            a.append(i.group(2))
        return a

    def generate_code(self):
        enum_fields_test = ""
        enum_class_name = self.get_type()
        for i in self.get_fields():
            enum_fields_test += "        " + enum_class_name + r"::" + i + ",\n"
        test = "\nTEST(" + "%s" + r", " + enum_class_name + \
            ") {\n    codec_test_all({\n" + enum_fields_test + "    });\n}\n"
        self.tests_code = [test]

    def get_value_generation(self, a, seed=""):
        random.seed(seed)
        options = self.get_fields()
        return a + "::" + options[random.randint(0, options.__len__() - 1)] + ";\n        "

    def get_comparison(self, a, b):
        return "EXPECT_EQ(" + a + ", " + b + ");\n        "
