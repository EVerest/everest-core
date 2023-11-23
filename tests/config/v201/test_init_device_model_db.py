import sqlite3
from contextlib import contextmanager
from pathlib import Path

import pytest
from init_device_model_db import DeviceModelDatabaseInitializer


@pytest.fixture
def db_file(tmp_path):
    return tmp_path / "unittest_device_model.db"


class TestDeviceModelDatabaseInitializer:

    @contextmanager
    def _connect(self, db_file) -> sqlite3.Cursor:
        con = sqlite3.connect(db_file)
        cur = con.cursor()
        try:
            yield cur
            cur.close()
        finally:
            con.close()

    def _read_table(self, db_file: Path, table: str):
        with self._connect(db_file) as cur:
            query = f"SELECT * FROM {table}"  # nosec
            cur.execute(query)
            rows = cur.fetchall()

            # Get column names
            col_names = [description[0] for description in cur.description]
            row_dicts = []
            for row in rows:
                row_dict = {col_name: value for col_name, value in zip(col_names, row)}
                row_dicts.append(row_dict)
            return row_dicts

    def _assert_database_tables_as_expected(self, db_file):
        assert self._read_table(db_file, "VARIABLE") == [{'COMPONENT_ID': 1,
                                                          'ID': 1,
                                                          'INSTANCE': None,
                                                          'NAME': 'UnitTestPropertyAName',
                                                          'REQUIRED': 1,
                                                          'VARIABLE_CHARACTERISTICS_ID': 1},
                                                         {'COMPONENT_ID': 1,
                                                          'ID': 2,
                                                          'INSTANCE': None,
                                                          'NAME': 'UnitTestPropertyBName',
                                                          'REQUIRED': 0,
                                                          'VARIABLE_CHARACTERISTICS_ID': 2},
                                                         {'COMPONENT_ID': 1,
                                                          'ID': 3,
                                                          'INSTANCE': None,
                                                          'NAME': 'UnitTestPropertyCName',
                                                          'REQUIRED': 0,
                                                          'VARIABLE_CHARACTERISTICS_ID': 3}]
        assert self._read_table(db_file, "VARIABLE_MONITORING") == []
        assert self._read_table(db_file, "VARIABLE_CHARACTERISTICS") == [{'DATATYPE_ID': 4,
                                                                          'ID': 1,
                                                                          'MAX_LIMIT': None,
                                                                          'MIN_LIMIT': None,
                                                                          'SUPPORTS_MONITORING': 1,
                                                                          'UNIT': None,
                                                                          'VALUES_LIST': None},
                                                                         {'DATATYPE_ID': 0,
                                                                          'ID': 2,
                                                                          'MAX_LIMIT': None,
                                                                          'MIN_LIMIT': None,
                                                                          'SUPPORTS_MONITORING': 0,
                                                                          'UNIT': None,
                                                                          'VALUES_LIST': None},
                                                                         {'DATATYPE_ID': None,
                                                                          'ID': 3,
                                                                          'MAX_LIMIT': None,
                                                                          'MIN_LIMIT': None,
                                                                          'SUPPORTS_MONITORING': 0,
                                                                          'UNIT': None,
                                                                          'VALUES_LIST': None}]

        assert self._read_table(db_file, "VARIABLE_ATTRIBUTE_TYPE") == [{'ID': 0, 'TYPE': 'Actual'},
                                                                        {'ID': 1, 'TYPE': 'Target'},
                                                                        {'ID': 2, 'TYPE': 'MinSet'},
                                                                        {'ID': 3, 'TYPE': 'MaxSet'}]
        assert self._read_table(db_file, "COMPONENT") == [{'CONNECTOR_ID': 3,
                                                           'EVSE_ID': 2,
                                                           'ID': 1,
                                                           'INSTANCE': None,
                                                           'NAME': 'UnitTestCtrlr'}]
        assert self._read_table(db_file, "SEVERITY") == [{'ID': 0, 'SEVERITY': 'Danger'},
                                                         {'ID': 1, 'SEVERITY': 'HardwareFailure'},
                                                         {'ID': 2, 'SEVERITY': 'SystemFailure'},
                                                         {'ID': 3, 'SEVERITY': 'Critical'},
                                                         {'ID': 4, 'SEVERITY': 'Error'},
                                                         {'ID': 5, 'SEVERITY': 'Alert'},
                                                         {'ID': 6, 'SEVERITY': 'Warning'},
                                                         {'ID': 7, 'SEVERITY': 'Notice'},
                                                         {'ID': 8, 'SEVERITY': 'Informational'},
                                                         {'ID': 9, 'SEVERITY': 'Debug'}]
        assert self._read_table(db_file, "MONITOR") == [{'ID': 0, 'TYPE': 'UpperThreshold'},
                                                        {'ID': 1, 'TYPE': 'LowerThreshold'},
                                                        {'ID': 2, 'TYPE': 'Delta'},
                                                        {'ID': 3, 'TYPE': 'Periodic'},
                                                        {'ID': 4, 'TYPE': 'PeriodicClockAligned'}]
        assert self._read_table(db_file, "DATATYPE") == [{'DATATYPE': 'string', 'ID': 0},
                                                         {'DATATYPE': 'decimal', 'ID': 1},
                                                         {'DATATYPE': 'integer', 'ID': 2},
                                                         {'DATATYPE': 'dateTime', 'ID': 3},
                                                         {'DATATYPE': 'boolean', 'ID': 4},
                                                         {'DATATYPE': 'OptionList', 'ID': 5},
                                                         {'DATATYPE': 'SequenceList', 'ID': 6},
                                                         {'DATATYPE': 'MemberList', 'ID': 7}]
        assert self._read_table(db_file, "MUTABILITY") == [{'ID': 0, 'MUTABILITY': 'ReadOnly'},
                                                           {'ID': 1, 'MUTABILITY': 'WriteOnly'},
                                                           {'ID': 2, 'MUTABILITY': 'ReadWrite'}]

    def test_init(self, db_file):
        database_initializer = DeviceModelDatabaseInitializer(db_file)

        schema_base = Path(__file__).parent / "resources" / "component_schemas"
        database_initializer.initialize_database(schema_base)

        self._assert_database_tables_as_expected(db_file)

        assert self._read_table(db_file, "VARIABLE_ATTRIBUTE") == [{'CONSTANT': 0,
                                                                    'ID': 1,
                                                                    'MUTABILITY_ID': 2,
                                                                    'PERSISTENT': 1,
                                                                    'TYPE_ID': 0,
                                                                    'VALUE': None,
                                                                    'VARIABLE_ID': 1},
                                                                   {'CONSTANT': 0,
                                                                    'ID': 2,
                                                                    'MUTABILITY_ID': 0,
                                                                    'PERSISTENT': 1,
                                                                    'TYPE_ID': 0,
                                                                    'VALUE': None,
                                                                    'VARIABLE_ID': 2},
                                                                   {'CONSTANT': 0,
                                                                    'ID': 3,
                                                                    'MUTABILITY_ID': 0,
                                                                    'PERSISTENT': 1,
                                                                    'TYPE_ID': 0,
                                                                    'VALUE': None,
                                                                    'VARIABLE_ID': 3}]

    def test_insert(self, db_file):

        database_initializer = DeviceModelDatabaseInitializer(db_file)

        schema_base = Path(__file__).parent / "resources" / "component_schemas"
        database_initializer.initialize_database(schema_base)

        database_initializer.insert_config_and_default_values(Path(__file__).parent / "resources/config.json",
                                                              schema_base)

        self._assert_database_tables_as_expected(db_file)

        assert self._read_table(db_file, "VARIABLE_ATTRIBUTE") == [{'CONSTANT': 0,
                                                                    'ID': 1,
                                                                    'MUTABILITY_ID': 2,
                                                                    'PERSISTENT': 1,
                                                                    'TYPE_ID': 0,
                                                                    'VALUE': "1",  # default value
                                                                    'VARIABLE_ID': 1},
                                                                   {'CONSTANT': 0,
                                                                    'ID': 2,
                                                                    'MUTABILITY_ID': 0,
                                                                    'PERSISTENT': 1,
                                                                    'TYPE_ID': 0,
                                                                    'VALUE': "test_value",
                                                                    'VARIABLE_ID': 2},
                                                                   {'CONSTANT': 0,
                                                                    'ID': 3,
                                                                    'MUTABILITY_ID': 0,
                                                                    'PERSISTENT': 1,
                                                                    'TYPE_ID': 0,
                                                                    'VALUE': None,
                                                                    'VARIABLE_ID': 3}]
