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
in the OCPP2.0.1 specification are located. In component_schemas/additioanl all additional
components can be created (e.g. Fan, RFID-Reader, etc.)

The schema files of both folders will be used to create the tables and entries for the specified
components and variables.
"""

import sqlite3
import argparse
from pathlib import Path
import json
from glob import glob

INIT_DEVICE_MODEL_SQL = "init_device_model.sql"
STANDARDIZED_COMPONENT_SCHEMAS_DIR = Path("component_schemas/standardized")
ADDITIONAL_COMPONENT_SCHEMAS_DIR = Path("component_schemas/additional")

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


def execute_init_sql(con: sqlite3.Connection, init_device_model_sql_path: Path) :
    """Executes the sql script to create tables and insert constant entries_summary_

    Args:INIT_DEVICE_MODEL_SQL
        con (sqlite3.Connection): sqlite3 connection
    """
    with open(init_device_model_sql_path, 'r') as sql_file:
        cur = con.cursor()
        cur.executescript(sql_file.read())


def insert_components(con: sqlite3.Connection, component_schema_dirs: list[Path]):
    """Creates tables and entries for all components that are part of the component_schema_dirs list. 

    Args:
        con (sqlite3.Connection): sqlite3 connection
        component_schema_dirs (list[Path]): list fo directories to to component schema files 
    """
    for component_schema_file in component_schema_dirs:
        with open(component_schema_file) as f:
            component_schema = json.load(f)
            insert_component(con, component_schema,
                             component_schema_file.name.replace(".json", ""))


def insert_variable_characteristics(characteristics: dict, cur: sqlite3.Cursor):
    """Inserts an entry into the VARIABLE_CHARACTERISTICS table based on the given parameter

    Args:
        characteristics (dict): contains the information about the characteristics
        cur (sqlite3.Cursor): sqlite3 cursor
    """
    data = (DATATYPE_ENCODING.get(characteristics.get("dataType")), characteristics.get("maxLimit"), characteristics.get(
            "minLimit"), characteristics.get("supportsMonitoring"), None, characteristics.get("valuesList"))
    cur.execute(("INSERT OR REPLACE INTO VARIABLE_CHARACTERISTICS (DATATYPE_ID, MAX_LIMIT, MIN_LIMIT,"
                 "SUPPORTS_MONITORING, UNIT, VALUES_LIST) VALUES(?,?,?,?,?,?)"), data)


def insert_variable(variable_name: str, instance: str, component_id: str, variable_characteristics_id: int, cur: sqlite3.Cursor):
    """Inserts an entry into the VARIABLE table based on the given parameters.

    Args:
        variable (dict): contains information about the variable
        component_id (str): id of the component
        variable_characteristics_id (int): id of the entry inside the VARIABLE_CHARACTERISTICS table for this variable
        cur (sqlite3.Cursor): sqlite3 cursor
    """
    data = (variable_name, instance, component_id, variable_characteristics_id)
    cur.execute(
        "INSERT OR REPLACE INTO VARIABLE (NAME, INSTANCE, COMPONENT_ID, VARIABLE_CHARACTERISTICS_ID) VALUES(?, ?, ?, ?)", data)


def insert_attributes(attributes: list[dict], variable_id: str, cur: sqlite3.Cursor):
    """Inserts the given attributes into the VARIABLE_ATTRIBUTE table

    Args:
        attributes (list[dict]): list of attributes
        variable_id (str): id of the entry inside the VARIABLE table for this attribute
        cur (sqlite3.Cursor): sqlite3 cursor
    """
    for attribute in attributes:
        data = (variable_id, MUTABILITY_ENCODING.get(attribute.get("mutability")), 1,
                0, VARIABLE_ATTRIBUTE_TYPE_ENCODING.get(attribute.get("type")))
        cur.execute("INSERT OR REPLACE INTO VARIABLE_ATTRIBUTE (VARIABLE_ID, MUTABILITY_ID, PERSISTENT, CONSTANT,"
                    "TYPE_ID) VALUES(?, ?, ?, ?, ?)", data)


def insert_component(con: sqlite3.Connection, component_schema: dict, component_name: str):
    """Inserts the given component into the COMPONENT table

    Args:
        con (sqlite3.Connection): sqlite3 connection
        component_schema (dict): component schema
        component_name (str): name of the component
    """
    cur = con.cursor()
    cur.execute(
        f"INSERT OR REPLACE INTO COMPONENT (NAME) VALUES('{component_name}');")
    component_id = cur.lastrowid
    for variable_meta_data in component_schema.get("properties").values():
        variable_name = variable_meta_data["variable_name"]
        instance = variable_meta_data.get("instance")
        characteristics: dict = variable_meta_data["characteristics"]
        attributes: list = variable_meta_data["attributes"]

        # insert into VARIABLE_CHARACTERISTICS
        insert_variable_characteristics(characteristics, cur)

        # insert VARIABLE
        variable_characteristics_id = cur.lastrowid
        insert_variable(variable_name, instance, component_id,
                        variable_characteristics_id, cur)

        # insert VARIABLE_ATTRIBUTES
        variable_id = cur.lastrowid
        insert_attributes(attributes, variable_id, cur)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter, description="OCPP2.0.1 Database Initialization")
    parser.add_argument("--out", metavar='OUT', nargs='?', const='/tmp/ocpp201/device_model_storage.db', default='/tmp/ocpp201/device_model_storage.db', type=str,
                        help="Path to where the directory where the database file should be located", required=False)
    parser.add_argument("--config_path", metavar='CONFIG-PATH', nargs='?', const='.', default='.', type=str,
                        help="Path to libocpp/config/v201", required=False)

    Path("/tmp/ocpp201").mkdir(parents=True, exist_ok=True)

    args = parser.parse_args()
    out_file = Path(args.out)
    config_v201_path = Path(args.config_path)
    init_device_model_sql_path = config_v201_path / INIT_DEVICE_MODEL_SQL

    component_schema_dirs = glob((config_v201_path / STANDARDIZED_COMPONENT_SCHEMAS_DIR).joinpath(
        "*").as_posix()) + glob((config_v201_path / ADDITIONAL_COMPONENT_SCHEMAS_DIR).joinpath("*").as_posix())
    component_schmea_dirs = [Path(component_schema)
                             for component_schema in component_schema_dirs]

    con = sqlite3.connect(out_file)
    execute_init_sql(con, init_device_model_sql_path)
    insert_components(con, component_schmea_dirs)

    con.commit()
    con.close()
