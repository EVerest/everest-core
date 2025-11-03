# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

import re
from helper import Helper


class Namespace:
    @classmethod
    def from_source_file(self, file_contents):
        namespace = []
        namespace_matches = re.finditer(
            Helper.get_regex_namespace(), file_contents)
        for x in namespace_matches:
            match = x.group(1)
            if "::" in match:
                subspaces = re.split("::", match)
                for space in subspaces:
                    namespace.append(space)
            else:
                namespace.append(match)
        return Namespace(namespace)

    def __init__(self, namespace_hierarchy):
        namespace = []
        namespace = namespace + namespace_hierarchy
        self.namespace_hierarchy = namespace

    def __str__(self):
        x = ""
        for i in self.namespace_hierarchy:
            x = x + i + "::"
        return x

    def __eq__(self, value: object):
        return self.__str__() == value.__str__()

    def __hash__(self):
        return self.__str__().__hash__()

    def get_hierarchy(self):
        return self.namespace_hierarchy

    '''
     function is used to find the namespace hierarchy of a struct that is addressed relatively in a file
     e.g. if within the c++ namespace everest::lib::API::V1_0::types::evse_manager a struct is called via
     powermeter::PowermeterValues this function returns the namespace hierarchy everest::lib::API::V1_0::types::powermeter
    '''

    def get_absolute_hierarchy_of_sub_namespace(self, sub_namespace):
        hierarchy = self.get_hierarchy()
        sub_hierarchy = sub_namespace.get_hierarchy()
        sub_hierarchy_size = sub_hierarchy.__len__()
        absolute_hierarchy = hierarchy[:-sub_hierarchy_size] + sub_hierarchy
        return Namespace(absolute_hierarchy)
