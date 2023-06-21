#!/usr/bin/env python3
#
# SPDX-License-Identifier: Apache-2.0
# Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

"""This script will use the v201 JSON configuration file passed as the script argument to 
set VariableAttributes inside the tables that have been created in the device model storage database.

This script requires an SQLite database that has been initialized using the 
init_device_model_db.py script located in this folder.
"""

from pathlib import Path
import sqlite3
import json
import argparse

VARIABLE_ATTRIBUTE_TYPE_ENCODING = {
    "Actual": 0,
    "Target": 1,
    "MinSet": 2,
    "MaxSet": 3
}

def insert_variable_attribute_value(component_name: str, variable_name: str, value: str, attribute_type: str, cur: sqlite3.Cursor):
    """Inserts a variable attribute value into the VARIABLE_ATTRIBUTE table

    Args:
        component_name (str): name of the component
        variable_name (str): name of the variable
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
                 "AND VARIABLE.NAME = ?) "
                 "AND TYPE_ID = ?")

    # Execute the query with parameter values
    cur.execute(statement, (str(value).lower() if isinstance(value, bool) else value, component_name, variable_name, VARIABLE_ATTRIBUTE_TYPE_ENCODING[attribute_type]))


def insert_config(config_file: Path, cur: sqlite3.Cursor):
    """Inserts the values given in config file into the VARIABLE_ATTRIBUTE table 

    Args:
        config_file (Path): Path to the config file
        cur (sqlite3.Cursor): sqlite3 cursor
    """
    with open(config_file, 'r') as f:
        config: dict = json.loads(f.read())
        for component in config:
            for variable_data in config[component].values():
                for attribute_type, value in variable_data["attributes"].items():
                    insert_variable_attribute_value(
                        component, variable_data["variable_name"], value, attribute_type, cur)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawTextHelpFormatter, description="OCPP2.0.1 Database Confing Loading")
    parser.add_argument("--config", metavar="CONFIG",
                        help="Path to config file to write AttributeVariable values into the database", required=True)
    parser.add_argument("--db", metavar="CONFIG",
                        help="Path to the database", required=True)

    args = parser.parse_args()
    config = Path(args.config)
    db_path = Path(args.db)
    con = sqlite3.connect(db_path)
    insert_config(config, con.cursor())
    con.commit()
    con.close()

