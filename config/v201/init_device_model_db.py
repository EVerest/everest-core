#!/usr/bin/env python3
#
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

"""This script initializes a sqlite database with a set of components and variables that are designed 
according to the device model specified in OCPP2.0.1. It creates tables and entries according to
the specified components and variables.

The database is initialized by using the init_device_model.sql file and the json schema files
located in the component_schemas folder of  this directory. This includes two subfolders: 
component_schemas/standardized and component_schemas/additional. 

In component_schemas/standardized all components with a role standardized 
in the OCPP2.0.1 specification are located. In component_schemas/additional all additional
components can be created (e.g. Fan, RFID-Reader, etc.)

The schema files of both folders will be used to create the tables and entries for the specified
components and variables.
"""

from __future__ import annotations

import argparse
import json
import sqlite3
from contextlib import contextmanager
from dataclasses import dataclass
from pathlib import Path

STANDARDIZED_COMPONENT_SCHEMAS_DIR = Path("standardized")
CUSTOM_COMPONENT_SCHEMAS_DIR = Path("custom")

DATATYPE_ENCODING = {
    "string": 0,
    "decimal": 1,
    "integer": 2,
    "dateTime": 3,
    "boolean": 4,
    "OptionList": 5,
    "SequenceList": 6,
    "MemberList": 7
}

MUTABILITY_ENCODING = {
    "ReadOnly": 0,
    "WriteOnly": 1,
    "ReadWrite": 2
}

VARIABLE_ATTRIBUTE_TYPE_ENCODING = {
    "Actual": 0,
    "Target": 1,
    "MinSet": 2,
    "MaxSet": 3
}


class DeviceModelDatabaseInitializer:
    INIT_DEVICE_MODEL_SQL = Path(__file__).parent / "init_device_model.sql"

    def __init__(self, database_file: Path):
        self._database_file = database_file

    def initialize_database(self, schemas_path: Path, delete_db_if_exists=True):
        """ Initializes the database (creating tables etc.) and insert the component definitions from the component schema files.

        :param component_schema_files:
        :param delete_db_if_exists:
        :return:
        """
        self._execute_init_sql(delete_db_if_exists)
        component_schema_files = self._scan_for_component_schema_files(schemas_path)
        self._insert_components(component_schema_files)
        print(
            f"Successfully initialized device model sqlite storage using schemas directory {schemas_path} at {self._database_file}")

    def _scan_for_component_schema_files(self, schemas_path: Path) -> list[Path]:
        """ Scans directory for schema json files in {schemas_path}/standardized and {schemas_path}/custom

        :param schemas_path:  Path containing the model schemas in {schemas_path}/standardized and {schemas_path}/custom
        :return:
        """
        return list((schemas_path / STANDARDIZED_COMPONENT_SCHEMAS_DIR).glob("*.json")) + \
            list((schemas_path / CUSTOM_COMPONENT_SCHEMAS_DIR).glob("*.json"))

    @dataclass(frozen=True)
    class _ComponentKey:
        name: str
        instance: str | None
        evse_id: int | None
        connector_id: int | None

        @classmethod
        def from_component_dict(cls, component: dict):
            return cls(name=component["name"],
                       instance=component.get("instance"),
                       evse_id=component.get("evse_id"),
                       connector_id=component.get("connector_id"))

    @dataclass(frozen=True)
    class _VariableAttributeKey:
        name: str
        instance: str | None
        attribute_type: str

    def insert_config_and_default_values(self, config_file: Path, schemas_path: Path):
        """Inserts the values given in config file into the VARIABLE_ATTRIBUTE table

        Args:
            config_file (Path): Path to the config file
            schemas_path (Path): Path containing the model schemas in {schemas_path}/standardized and {schemas_path}/custom
        """
        configured_values = self._read_config_values(config_file)

        default_values = self._read_component_default_values(schemas_path)

        with self._connect() as cur:
            for component_key, component_values in configured_values.items():
                for var_attr_key, value in component_values.items():
                    self._insert_variable_attribute_value(cur, component_key, var_attr_key, value)

            for component_key, component_defaults in default_values.items():
                for var_attr_key, default in component_defaults.items():
                    if not configured_values.get(component_key, {}).get(var_attr_key):
                        self._insert_variable_attribute_value(cur, component_key, var_attr_key, default)

            print(f"Successfully inserted variables from {config_file} into sqlite storage at {self._database_file}")

    @classmethod
    def _read_config_values(cls, config_file: Path) -> dict[_ComponentKey, dict[_VariableAttributeKey, str]]:
        config_values = {}
        with open(config_file, 'r') as f:
            config = json.loads(f.read())
        for component in config:
            component_key = cls._ComponentKey.from_component_dict(component)
            config_values[component_key] = {}

            for variable_data in component["variables"].values():
                for attribute_type, value in variable_data["attributes"].items():
                    variable_attribute_key = cls._VariableAttributeKey(variable_data["variable_name"],
                                                                       variable_data.get("instance"),
                                                                       attribute_type)
                    config_values[component_key][variable_attribute_key] = value
        return config_values

    @contextmanager
    def _connect(self) -> sqlite3.Cursor:
        con = sqlite3.connect(self._database_file)
        cur = con.cursor()
        try:
            yield cur
            cur.close()
            con.commit()
        finally:
            con.close()

    def _insert_variable_characteristics(self, characteristics: dict, cur: sqlite3.Cursor):
        """Inserts an entry into the VARIABLE_CHARACTERISTICS table based on the given parameter

        Args:
            characteristics (dict): contains the information about the characteristics
            cur (sqlite3.Cursor): sqlite3 cursor
        """
        data = (
            DATATYPE_ENCODING.get(characteristics.get("dataType")), characteristics.get("maxLimit"),
            characteristics.get(
                "minLimit"), characteristics.get("supportsMonitoring"), characteristics.get("unit"), characteristics.get("valuesList"))
        cur.execute(("INSERT OR REPLACE INTO VARIABLE_CHARACTERISTICS (DATATYPE_ID, MAX_LIMIT, MIN_LIMIT,"
                     "SUPPORTS_MONITORING, UNIT, VALUES_LIST) VALUES(?,?,?,?,?,?)"), data)

    def _insert_variable(self, variable_name: str, instance: str, component_id: str,
                         variable_characteristics_id: int,
                         required: bool,
                         cur: sqlite3.Cursor):
        """Inserts an entry into the VARIABLE table based on the given parameters.

        Args:
            variable_name (str): name of the variable
            instance (str): instance of the variable
            component_id (str): id of the component
            variable_characteristics_id (int): id of the entry inside the VARIABLE_CHARACTERISTICS table for this variable
            cur (sqlite3.Cursor): sqlite3 cursor
        """
        data = (variable_name, instance, component_id, variable_characteristics_id, 1 if required else 0)
        cur.execute(
            "INSERT OR REPLACE INTO VARIABLE (NAME, INSTANCE, COMPONENT_ID, VARIABLE_CHARACTERISTICS_ID, REQUIRED) VALUES(?, ?, ?, ?, ?)",
            data)

    def _insert_attributes(self, attributes: list[dict], variable_id: str, cur: sqlite3.Cursor):
        """Inserts the given attributes into the VARIABLE_ATTRIBUTE table

        Args:
            attributes (list[dict]): list of attributes
            variable_id (str): id of the entry inside the VARIABLE table for this attribute
            cur (sqlite3.Cursor): sqlite3 cursor
        """
        for attribute in attributes:
            data = (variable_id, MUTABILITY_ENCODING.get(attribute.get("mutability")), 1,
                    0, VARIABLE_ATTRIBUTE_TYPE_ENCODING.get(attribute.get("type")))
            cur.execute(
                "INSERT OR REPLACE INTO VARIABLE_ATTRIBUTE (VARIABLE_ID, MUTABILITY_ID, PERSISTENT, CONSTANT,"
                "TYPE_ID) VALUES(?, ?, ?, ?, ?)", data)

    def _read_component_schemas(self, component_schema_files: list[Path]):
        component_schemas = {}
        for component_schema_file in component_schema_files:
            with open(component_schema_file) as f:
                component_schema = json.load(f)
                key = self._ComponentKey.from_component_dict(component_schema)
                component_schemas[key] = component_schema
        return component_schemas

    def _insert_components(self, component_schema_files: list[Path]):
        """Creates tables and entries for all components that are part of the component_schema_dirs list.

        Args:
            component_schema_files (list[Path]): list of component schema files
        """
        component_schemas = self._read_component_schemas(component_schema_files)
        with self._connect() as cur:
            for key, component_schema in component_schemas.items():
                self._insert_component(cur, key, component_schema)

    def _insert_component(self, cur: sqlite3.Cursor, component_key: _ComponentKey, component_schema: dict):
        """Inserts the given component into the COMPONENT table

        Args:
            con (sqlite3.Connection): sqlite3 connection
            component_schema (dict): component schema
        """

        statement = "INSERT OR REPLACE INTO COMPONENT (NAME, INSTANCE, EVSE_ID, CONNECTOR_ID) VALUES(?,?,?,?)"

        cur.execute(statement,
                    (component_key.name, component_key.instance, component_key.evse_id, component_key.connector_id))
        component_id = cur.lastrowid
        required_properties = set(component_schema.get("required", set()))
        for property_key, variable_meta_data in component_schema.get("properties", {}).items():
            variable_name = variable_meta_data["variable_name"]
            instance = variable_meta_data.get("instance")
            characteristics: dict = variable_meta_data["characteristics"]
            attributes: list = variable_meta_data["attributes"]

            # insert into VARIABLE_CHARACTERISTICS
            self._insert_variable_characteristics(characteristics, cur)

            # insert VARIABLE
            variable_characteristics_id = cur.lastrowid
            self._insert_variable(variable_name, instance, component_id,
                                  variable_characteristics_id,
                                  property_key in required_properties,
                                  cur)

            # insert VARIABLE_ATTRIBUTES
            variable_id = cur.lastrowid
            self._insert_attributes(attributes, variable_id, cur)

    def _execute_init_sql(self, delete_db_if_exists: bool):
        """Executes the sql script to create tables and insert constant entries_summary_

        Args:INIT_DEVICE_MODEL_SQL
            con (sqlite3.Connection): sqlite3 connection
        """

        if delete_db_if_exists:
            self._database_file.unlink(missing_ok=True)

        with self._connect() as cur:
            with self.INIT_DEVICE_MODEL_SQL.open("r") as sql_file:
                cur.executescript(sql_file.read())

    def _insert_variable_attribute_value(self, cur: sqlite3.Cursor,
                                         component_key: _ComponentKey,
                                         variable_attr_key: _VariableAttributeKey,
                                         value: str
                                         ):
        """Inserts a variable attribute value into the VARIABLE_ATTRIBUTE table

        Args:
            component_name (str): name of the component
            variable_name (str): name of the variable
            variable_instance(str): name of the instance
            value (str): value that should be set
            cur (sqlite3.Cursor): sqlite3 cursor
        """
        statement = ("UPDATE VARIABLE_ATTRIBUTE "
                     "SET VALUE = ? "
                     "WHERE VARIABLE_ID = ("
                     "SELECT VARIABLE.ID "
                     "FROM VARIABLE "
                     "JOIN COMPONENT ON COMPONENT.ID = VARIABLE.COMPONENT_ID "
                     "WHERE COMPONENT.NAME = ? "
                     "AND COMPONENT.INSTANCE IS ? "
                     "AND COMPONENT.EVSE_ID IS ? "
                     "AND COMPONENT.CONNECTOR_ID IS ? "
                     "AND VARIABLE.NAME = ? "
                     "AND VARIABLE.INSTANCE IS ?) "
                     "AND TYPE_ID = ?")

        # Execute the query with parameter values
        cur.execute(statement, (str(value).lower() if isinstance(value, bool) else value, component_key.name,
                                component_key.instance, component_key.evse_id, component_key.connector_id,
                                variable_attr_key.name, variable_attr_key.instance,
                                VARIABLE_ATTRIBUTE_TYPE_ENCODING[variable_attr_key.attribute_type]))

    def _read_component_default_values(self, schemas_path: Path) -> dict[
        _ComponentKey, dict[_VariableAttributeKey, str]]:

        default_values = {}
        component_schemas = self._read_component_schemas(self._scan_for_component_schema_files(schemas_path))
        for component_key, component_schema in component_schemas.items():

            for variable_meta_data in component_schema.get("properties").values():
                variable_name = variable_meta_data["variable_name"]
                instance = variable_meta_data.get("instance")

                if "default" in variable_meta_data:
                    variable_attr_key = self._VariableAttributeKey(variable_name, instance, "Actual")
                    default_values.setdefault(component_key, {})[variable_attr_key] = variable_meta_data["default"]
        return default_values


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter, description="OCPP2.0.1 Database Initialization (Schema + Data)")
    parser.add_argument('commands', metavar='COMMAND', type=str, nargs='+',
                        help='Available commands:\n'
                             '- init: initialize database schema\n'
                             '- insert: insert data from config file')
    parser.add_argument("--db", metavar="DB",
                        help="Path to the database", required=True)
    parser.add_argument("--schemas", metavar='CONFIG-PATH', nargs='?', const='.', default='component_schemas', type=str,
                        help="Path to libocpp/config/v201", required=True)

    parser.add_argument("--config", metavar="CONFIG",
                        help="Path to config file from which to write AttributeVariable values into the database on 'insert'",
                        required=False)

    args = parser.parse_args()
    commands = set(args.commands)
    database_file = Path(args.db)
    schemas_path = Path(args.schemas)

    if "insert" in commands and not args.config:
        parser.error("'insert' requires --config")

    database_initializer = DeviceModelDatabaseInitializer(database_file)

    if "init" in commands:
        if database_file.is_relative_to(Path("/tmp/ocpp201")):  # nosec
            Path("/tmp/ocpp201").mkdir(parents=True, exist_ok=True)
        database_initializer.initialize_database(schemas_path)

    if "insert" in commands:
        database_initializer.insert_config_and_default_values(Path(args.config), Path(schemas_path))
