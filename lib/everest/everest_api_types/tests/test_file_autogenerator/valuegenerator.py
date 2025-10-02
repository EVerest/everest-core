# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

import math
import random
import re
import string
from namespace import Namespace


class ManualGenerator:
    def __init__(self):
        self.is_used = False
        self.variable_count = 0
        self.tester_count = 0
        self.variables = {}
        self.testers = {}

    def increase_tester_name(self):
        self.tester_count += 1

    def increase_variable_name(self):
        self.variable_count += 1

    def get_variable_name(self, type_string):
        self.is_used = True
        if type_string in self.variables.keys():
            return self.variables[type_string]
        self.increase_variable_name()
        name = "manual_variable_" + self.variable_count.__str__()
        self.variables[type_string] = name
        return name

    def get_tester(self, type_string, a, b):
        self.is_used = True
        if type_string in self.testers.keys():
            return self.testers[type_string] % (a, b)
        self.increase_tester_name()
        name = "manual_tester" + self.tester_count.__str__() + "(%s, %s)"
        self.testers[type_string] = name
        return name % (a, b)

    def get_manual_helpers(self):
        code = ""
        if not self.is_used:
            return code
        for i in self.variables.keys():
            code += i + " " + self.get_variable_name(
                i) + "= null; //Todo define the generation of this variable\n        "

        for i in self.testers.keys():
            code += "void " + self.get_tester(i, i + " a",
                                              i + " b") + " {\n//Todo define the comparison of those two objects\n}"
        return code


class AcrossFileGenerator:
    def __init__(self):
        self.helpers = {"": {}}
        self.dependencies_between_helpers = []

    def add_helper(self, helper):
        namespace = helper.get_namespace()
        type_string = helper.get_type()
        self.generate_if_needed(namespace)
        self.helpers[namespace.__str__()][type_string] = helper

    def generate_if_needed(self, namespace):
        if namespace.__str__() not in self.helpers.keys():
            self.helpers[namespace.__str__()] = {}

    def get_value_generation(self, type_string, namespace, seed=""):
        helper = self.helpers[namespace.__str__()][type_string]
        return helper.get_value_generation(type_string, seed)

    def get_helpers(self):
        return self.helpers

    def get_helper(self, type_string, namespace):
        return self.helpers[namespace.__str__()][type_string]

    def has_helper(self, type_string, namespace):
        helpers = self.get_helpers()
        if namespace.__str__() not in helpers.keys():
            return False
        return type_string in helpers[namespace.__str__()].keys()


def get_vector_type(type_string):
    return ValueGenerator.generics_extractor("std::vector", type_string)


def get_vector_length():
    return int(7)


def get_vector_declaration(vector_type, variable_name_suffix="", assign_value=""):
    return "std::vector<" + vector_type + "> " + get_vector_variable_name(
        variable_name_suffix) + assign_value + ";\n        "


def get_vector_variable_name(variable_name_suffix=""):
    return "vector%s" % variable_name_suffix


class ValueGenerator:
    manual_generator = ManualGenerator()
    base_types = ["int32_t", "float", "std::string", "bool"]

    def __init__(self, struct_name, struct_namespace, enum_map, across_file_struct_generator=None):
        if (across_file_struct_generator is None):
            across_file_struct_generator = AcrossFileGenerator()
        self.across_file_struct_generator = across_file_struct_generator
        self.struct_name = struct_name
        self.struct_namespace = struct_namespace
        self.enum_map = enum_map

    def clear_manual_generation(self):
        self.manual_generator.is_used = False

    def generate_corresponding_value(self, target_variable_name, type_string, name_string, preparations="",
                                     highest_vector_field_index_used=0, struct_helper=None):
        prefix = target_variable_name + "= "
        if highest_vector_field_index_used > 0:
            prefix = type_string + " " + prefix
        suffix = ";\n        "
        seed = self.struct_name + name_string
        random.seed(seed)
        no_prior_preparations = preparations.__len__() <= 0
        if "std::vector<" in type_string:
            vector_type = self.namespace_cleanup(get_vector_type(type_string))
            vector_repr = get_vector_declaration(vector_type)
            vector_length = get_vector_length()
            for i in range(0, vector_length):
                field_name = "vector_field_" + i.__str__()
                preparations += self.generate_corresponding_value(field_name, vector_type, (name_string + i.__str__()),
                                                                  preparations="",
                                                                  highest_vector_field_index_used=(highest_vector_field_index_used + vector_length), struct_helper=struct_helper)
                vector_repr += get_vector_variable_name() + ".push_back(" + \
                    field_name + ");\n        "
            if no_prior_preparations:
                vector_repr = preparations + vector_repr
            return "{" + vector_repr + target_variable_name + "= " + "vector;}\n        "

        # if the enum of the field is in the same namespace as the struct but the namespace was specified regardless
        # it needs to be properly resolved
        namespace_prefix = ""
        type_string_cleaned = self.namespace_cleanup(type_string)

        # after this there are only enums and structs which need to be called with their type as a namespace
        # or basic types which need no namespace prefix
        if namespace_prefix.__len__() <= 0:
            namespace_prefix += type_string_cleaned + "::"

        if self.across_file_struct_generator.has_helper(type_string_cleaned, self.struct_namespace):
            generation = self.across_file_struct_generator.get_helper(
                type_string_cleaned, self.struct_namespace).get_value_generation(type_string_cleaned, seed)
            return self.struct_printout(prefix, generation)

        if self.is_struct_from_different_namespace(type_string_cleaned):
            different_namespace, type_in_different_namespace = self.extract_relative_namespace(
                type_string_cleaned)
            if self.across_file_struct_generator.has_helper(type_in_different_namespace, different_namespace):
                generation = self.across_file_struct_generator.get_helper(
                    type_in_different_namespace, different_namespace).get_value_generation(type_string_cleaned, seed)
                return self.struct_printout(prefix, generation)

        match type_string_cleaned:
            case "int32_t":
                s = ((2 ** 32) - 1) / 2
                value_string = random.randint(
                    math.floor(-s), math.floor(s)).__str__()
            case "float":
                value_string = random.random().__str__()
            case "std::string":
                value_string = "\"" + \
                    "".join(random.sample(string.ascii_letters, 30)) + "\""
            case "bool":
                value_string = bool(random.getrandbits(1)).__str__().lower()
            case _:
                # catchall for values that cannot be assigned with the info in this file alone
                value_string = ValueGenerator.manual_generator.get_variable_name(
                    type_string_cleaned)
        return prefix + value_string + suffix

    def struct_printout(self, prefix, generation):
        return prefix + generation

    def is_struct_from_different_namespace(self, type_string):
        return "::" in type_string and not "std::" in type_string

    def extract_relative_namespace(self, type_string):
        res = type_string.split("::")
        n = res[:-1]
        sub_namespace = Namespace(n)
        different_namespace = self.struct_namespace.get_absolute_hierarchy_of_sub_namespace(
            sub_namespace)
        type_in_different_namespace = res[-1]
        return different_namespace, type_in_different_namespace

    def namespace_cleanup(self, type_string):
        cleaned = type_string
        for i in self.struct_namespace.get_hierarchy():
            pattern = i + "::"
            match = re.match(pattern, cleaned)
            if match:
                cleaned = re.sub(pattern, "", cleaned)
        return cleaned

    def generate_corresponding_field_test(self, field_name, field_type, namespace, is_optional):
        return self.generate_corresponding_test("original." + field_name, "result." + field_name,
                                                field_type, namespace, is_optional)

    def generate_corresponding_test(self, original_object, result_object, field_type, namespace, is_optional):
        is_simple = field_type in self.base_types or field_type in self.enum_map.keys(
        ) or self.namespace_cleanup(field_type) in self.enum_map.keys()
        if is_simple:
            method_signature = "EXPECT_EQ"
            if is_optional:
                method_signature = "expect_opt_eq"
            return method_signature + "(" + original_object + ", " + result_object + ");\n        "
        optional_wrapper_front = ""
        optional_wrapper_rear = ""
        if is_optional:
            optional_wrapper_front = "EXPECT_EQ(" + original_object + ".has_value(), " + \
                result_object + ".has_value());\nif (" + result_object + \
                ".has_value()) {"
            optional_wrapper_rear = "}\n"
        wraped = self.generate_corresponding_test_unsafe(
            original_object, result_object, field_type, namespace, is_optional)
        return optional_wrapper_front + wraped + optional_wrapper_rear

    def generate_corresponding_test_unsafe(self, original_object, result_object, field_type, namespace, is_optional):
        if "std::vector<" in field_type:
            vector_type = self.namespace_cleanup(get_vector_type(field_type))
            assign_a = "= " + original_object
            assign_b = "= " + result_object
            if is_optional:
                assign_a += ".value()"
                assign_b += ".value()"
            return (
                "{"
                + get_vector_declaration(vector_type, "_a", assign_a)
                + get_vector_declaration(vector_type, "_b", assign_b)
                + "ASSERT_EQ(" + get_vector_variable_name("_a") + ".size(), " + get_vector_variable_name(
                    "_b") + ".size()) << \"Vectors are of unequal length\";\n"
                + "for (long unsigned int i = 0; i < " +
                get_vector_variable_name("_a") + ".size(); ++i) {\n"
                + self.generate_corresponding_test(get_vector_variable_name("_a") + "[i]",
                                                   get_vector_variable_name("_b") + "[i]", vector_type, namespace, False)
                + "}\n}"
            )

        field_type = self.namespace_cleanup(field_type)

        if is_optional:
            original_object += ".value()"
            result_object += ".value()"

        if self.across_file_struct_generator.has_helper(field_type, namespace):
            tester = self.across_file_struct_generator.get_helper(
                field_type, namespace)
            return tester.get_comparison(original_object, result_object)

        if self.is_struct_from_different_namespace(field_type):
            different_namespace, type_in_different_namespace = self.extract_relative_namespace(
                field_type)
            return self.generate_corresponding_test(original_object, result_object, type_in_different_namespace, different_namespace, False)

        return self.manual_generator.get_tester(field_type, original_object, result_object) + ";\n"

    @staticmethod
    def generics_extractor(generic_call, whole_call):
        san = re.sub(generic_call + "<", "", whole_call)
        san = san[::-1]
        san = re.sub(">", "", san, 1)[::-1]
        return san

    def get_manual_helpers(self):
        return ValueGenerator.manual_generator.get_manual_helpers()
