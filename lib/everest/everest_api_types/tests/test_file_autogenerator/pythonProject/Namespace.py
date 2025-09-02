import re
from Helper import Helper

class Namespace:
    @classmethod
    def from_source_file(self, file_contents):
        namespace = []
        namespace_matches = re.finditer(Helper.get_regex_namespace(), file_contents)
        for x in namespace_matches:
            match = x.group(1)
            if "::" in match:
                subspaces = re.split("::", match)
                for space in subspaces:
                    namespace.append(space)   
            else:    
                namespace.append(match)
        return Namespace(namespace)

    def __init__(self, namespace_hirearchy):
        namespace = []
        namespace= namespace + namespace_hirearchy
        self.namespace_hirearchy = namespace

    def __str__(self):
        x = ""
        for i in self.namespace_hirearchy:
            x = x + i + "::"
        return x
    
    def __eq__(self, value: object):
        return self.__str__() == value.__str__()
    
    def __hash__(self):
        return self.__str__().__hash__()
    
    def get_hirearchy(self):
        return self.namespace_hirearchy
    
    '''
     fuction is used to find the namespace hirearchy of a struct that is adressed relatively in a file
     e.g. if within the c++ namespace everest::libs::API::V1_0::types::evse_manager a struct is called via
     powermeter::PowermeterValues this function returns the namespace hirearchy everest::libs::API::V1_0::types::powermeter
    ''' 
    def get_absolute_hirearchy_of_sub_namespace(self, sub_namespace):
        hirearchy = self.get_hirearchy()
        sub_hirearchy = sub_namespace.get_hirearchy()
        sub_hirearchy_size = sub_hirearchy.__len__()
        absolute_hirearchy = hirearchy[:-sub_hirearchy_size] + sub_hirearchy
        return Namespace(absolute_hirearchy)
