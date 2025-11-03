# everest-sqlite

**everest-sqlite** is a modern C++17 wrapper around SQLite, designed to provide a safe, easy-to-use, and robust interface for embedded SQL database operations. It supports transactional database access, schema migrations, and convenient statement handling with RAII patterns.

everest-sqlite is used within several modules and libraries of the EVerest project.

## Get Involved

See the [COMMUNITY.md](https://github.com/EVerest/EVerest/blob/main/COMMUNITY.md) and [CONTRIBUTING.md](https://github.com/EVerest/EVerest/blob/main/CONTRIBUTING.md) of the EVerest project to get involved.

## Features

- **RAII-based transaction management** with automatic rollback on error
- **Safe, typed access to SQLite data**
- **Schema migration support** via versioned `.sql` scripts
- **Detailed exception types** for robust error handling
- **CMake-friendly** and easily embeddable

## Components

```
include/database/
├── exceptions.hpp        # Custom exceptions for database errors
├── sqlite/
├──── connection.hpp        # Database connection and transaction logic
├──── schema_updater.hpp    # Schema migration tooling
└──── statement.hpp         # RAII wrapper for sqlite3_stmt
```

## Getting Started

The recommended way to build this project is to use EVerests dependency manager EDM.

### Dependencies

- **SQLite3**
- **nlohmann_json**
- (Optional) **[everest-cmake](https://github.com/EVerest/everest-cmake)**
- (Optional) **GTest** for unit testing

### Build

Make sure to check out this repository along with [everest-cmake](https://github.com/EVerest/everest-cmake).

```bash
git clone <this-repo>
cd everest-sqlite
mkdir build && cd build
cmake ..
make
```

To build with unit tests:

```bash
cmake -DBUILD_TESTING=ON ..
make
ctest
make everest-sqlite_gcovr_coverage # to generate a coverage report
```

To build without EDM, you can use:

```bash
cmake -DDISABLE_EDM=ON ..
make
```

## Usage

### 1. Connecting to a database

```cpp
Connection db("my_database.db");
if (!db.open_connection()) {
    throw std::runtime_error("Could not open database");
}
```

### 2. Transactions

```cpp
auto tx = db.begin_transaction();
db.execute_statement("INSERT INTO table_name (col) VALUES ('data')");
tx->commit(); // or tx->rollback();
```

### 3. Prepared Statements

```cpp
auto stmt = db.new_statement("SELECT name FROM users WHERE id = :id");
stmt->bind_int(1, 1);
if (stmt->step() == SQLITE_ROW) {
    std::string name = stmt->column_text(0);
}
```

### 4. Schema Migration

Place your migration SQL files in a folder:

```
migrations/
├── 1_up.sql
├── 2_up.sql
├── 2_down.sql
└── ...
```

Apply migrations:

```cpp
SchemaUpdater updater(&db);
if (!updater.apply_migration_files("migrations", 2)) {
    throw std::runtime_error("Migration failed");
}
```

Check out the [migration documentation](docs/migrations.md) for more detailed information about the migration support.

## Exception Types

All exceptions inherit from `Exception`:
- `ConnectionException`
- `QueryExecutionException`
- `RequiredEntryNotFoundException`
- `MigrationException`
