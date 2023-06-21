# Libocpp Code Generators
In this directory a collection of code generators for various purposes are located.

## C++ Code generator for v16 and v201
The script [generate_cpp.py](common/generate_cpp.py) can be used to generate datatypes, enums, and messages for v16 and v201.

```bash
python3 generate_cpp.py --schemas <json-schema-dir> --out <path-to-libocpp> --version <ocpp-version> 
```

e.g.

```bash
python3 generate_cpp.py --schemas ~/ocpp-schemas/v16/ --out ~/checkout/everest-workspace/libocpp --version v16 
```

```bash
python3 generate_cpp.py --schemas ~/ocpp-schemas/v201/ --out ~/checkout/everest-workspace/libocpp --version v201 
```

## JSON schema generator for v201 component schemas
The script [generate_component_schemas.py](v201/generate_component_schemas.py) generates component schemas for v201 based on
a modified version of the dm_component_vars.csv appendix of the OCPP2.0.1 specification. These component schema files can then
be used to initialize the device model storage database.

```bash
python3 generate_component_schemas.py --csv <path-to-dm_component_vars.csv> --out <path-to-v201-components-standardized>
```

e.g.

```bash
python3 generate_component_schemas.py --csv ~/ocpp-v201-specification/appendix/dm_component_vars.csv --out ~/checkout/everest-workspace/libocpp/config/v201/component_schemas/standardized
```

## C++ Code generator for ControllerComponentVariables
The script [generate_ctrlr_component_vars.py](v201/generate_ctrlr_component_vars.py) generates the ctrlr_component_variables cpp and hpp file of this lib for v201. 
These files provide access to ComponentVariables with a role standardized in the OCPP2.0.1 specification.

```bash
python3 generate_ctrlr_component_vars.py --csv <path-to-dm_component_vars.csv> --out <path-to-v201-components-standardized>
```
component_schemas/standardized
e.g.

```bash
python3 generate_ctrlr_component_vars.py --csv ~/ocpp-v201-specification/appendix/dm_component_vars.csv --out ~/checkout/everest-workspace/libocpp
```

## JSON generator for v201 config file
The script [generate_ocpp_config.py](v201/generate_ocpp_config.py.py) generates a skeleton of a configuration file for OCPP2.0.1 that can be modified and 
used to insert the variables into the device model storage database using the script config/insert_device_model_config.py .

```bash
python3 generate_ctrlr_component_vars.py --schemas <path-to-v201-components-standardized> --out <output-directory>
```

e.g.

```bash
python3 generate_ctrlr_component_vars.py --schemas ~/checkout/everest-workspace/libocpp/config/v201/component_schemas/standardized --out ~/checkout/everest-workspace/libocpp/config/v201
```