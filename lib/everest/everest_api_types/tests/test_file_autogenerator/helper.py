# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

import re
from abc import ABCMeta, abstractmethod


class Helper(metaclass=ABCMeta):
    regex_whitespaces = "[\n \t]+"
    regex_field_or_class_name = "[A-z_0-9]+"
    regex_namespace = r"namespace" + regex_whitespaces + "([A-z0-9:]+)" + regex_whitespaces + r"\{"

    def __init__(self, representation, namespace=None):
        if namespace is None:
            namespace = [""]
        self.namespace = namespace
        self.representation = representation
        self.test_helpers_code = ""
        self.test_helpers_headers_code = ""
        self.tests_code = ["%s"]

    def get_representation(self):
        return self.sanitize(self.representation)

    @abstractmethod
    def generate_code(self):
        pass

    def get_test_helpers_code(self, name):
        return self.test_helpers_code

    def get_test_helpers_headers_code(self):
        return self.test_helpers_headers_code

    def get_tests_code(self, name):
        tests = ""
        for case in self.tests_code:
            tests += case % name
        return tests

    @staticmethod
    def get_regex_find_in_file():
        pass

    @abstractmethod
    def get_regex_structure_type(self):
        pass

    def get_type(self):
        return re.search(
            r"%s%s([A-z]*)%s\{" % (self.get_regex_structure_type(), Helper.regex_whitespaces, Helper.regex_whitespaces),
            self.get_representation()).group(1)

    @staticmethod
    def sanitize(file):
        # removes multi line comments
        multi_line_comment = re.compile(
            r"((/\*)(\*(?!/)|[^\*])*(\*/))")  # matches all starting with /* and ending with first */
        multi_removed = re.sub(multi_line_comment, "", file)
        # removes single line comments
        comments_removed = re.sub(r"//[^\n]*\n", "", multi_removed)
        return comments_removed

    @staticmethod
    def get_regex_namespace():
        return Helper.regex_namespace

    def get_namespace(self):
        return self.namespace
    
    def get_type_with_namespace(self):
        return self.get_namespace().__str__() + self.get_type()
