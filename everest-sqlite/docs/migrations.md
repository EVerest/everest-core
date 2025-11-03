# Database migrations

## Introduction

Updating the schema of a database that is in production should always be done with great care. To facilitate this we make use of migration files. These are files that can be executed in sequence to get to the desired schema version that works with that version of the software.

### File format

The filenames must have the following format:  
`x_up[-description].sql` and `x_down[-description].sql`

The description is optional and x needs to be the next number in the sequence.

The files always exist in pairs except for the initial "up" file used to start the database. The "up" file contains the changes needed to update the database schema so that it can be used with the new firmware. The "down" file must undo all the changes of the "up" file so a perfect downgrade is possible.

CMake will validate the completeness of the migration pairs and the filenames. If this is not correct CMake will fail to initialize.

### Schema changes

The schema changes should be done in such a way that the data present in the databases will persist unless it is really necessary to remove stuff.

### Unit testing

We recommend to write specific unit tests for your migration that validate that the changes you have made have the expected outcome. Examples for this are located in
[test_database_schema_updater.cpp](../tests/test_database_schema_updater.cpp)


## Design consideration

- We start out with an SQL init file that is used to start the database with. This file should not be changed anymore once this functionality is implemented.
- Every change we want to make to the database is done through migration files. These files are simply more SQL files that can be applied to the database in the right order.
  - Every change should consist of an up and a down file.
  - The up files are used to update the database to the latest schema.
  - The down files are used to downgrade a database to a previous schema. It is very important that these files stay on the target so that all older versions can apply these.
  - If for whatever reason we need to do an operation in a down file that was not supported before we are making a breaking change which should be carefully considered.
- The up/down migration file combination shall have a "version" number assigned in sequence. The version numbers of the files will be used together with the database's user_version field to determine which migrations files to run to get to the target version.
- The target version needs to be compiled into the firmware so that older versions can know which "down" migration files to apply to get back to their version of the database. This is done by having CMake generate a compile time definition based on the content of the folder with migrations.
- Each migration needs to be done in a single SQL transaction so we don't end up with changes being applied only part of the way.
- Before applying migrations a backup shall be made of the database so that in case we fail we can rollback to that version.
- Add a CICD check that validates if all the migrations can be executed.

## How to use

Using the database migrations is fairly straightforward. Most of the details are handled by the library itself. To set up these migrations, there are a few things to consider:

- **Responsibility of the consuming project**:
  - The consuming library or application is responsible for **shipping the appropriate migration files** with the target system (e.g., embedded device or deployment target).
  - These files **must be present** during upgrades *and* downgrades, since rolling back requires access to older down-migration scripts.
  - It is **strongly recommended** to back up the database before initiating a migration. While migrations are wrapped in transactions and should roll back on failure, this is not guaranteed in all edge cases.
  - **Old database files should be removed** when reinitializing a database to ensure a clean start with a well-defined schema.
  - All migrations should be performed **before any logic that depends on the schema is executed**.

## Integration for libraries using everest-sqlite

If your library or module integrates `everest-sqlite`, here's how you can hook into the migration support to safely handle schema changes between releases:

### 1. Place your migration files

Organize your SQL migration scripts in a dedicated folder in your repository, for example:

```
my_library/
├── migrations/
│   ├── 1_up.sql
│   ├── 2_up.sql
│   ├── 2_down.sql
│   └── ...
```

Follow the filename convention:
- `X_up.sql` applies a schema change to version X.
- `X_down.sql` undoes that same schema change.

### 2. Use `SchemaUpdater` during initialization

Before you use any tables or schema-specific logic in your module, run the schema updater:

```cpp
#include <database/sqlite/schema_updater.hpp>

Connection db("path/to/database.db");
db.open_connection();

SchemaUpdater updater(&db);
const uint32_t TARGET_SCHEMA_VERSION = /* set via CMake or hardcoded */;

if (!updater.apply_migration_files("path/to/migrations", TARGET_SCHEMA_VERSION)) {
    throw std::runtime_error("Migration failed");
}
```

You can derive `TARGET_SCHEMA_VERSION` from a CMake definition if you use `CollectMigrationFiles.cmake`.

### 3. Add `CollectMigrationFiles.cmake` to your build

In your CMake project:

```cmake
include(${CMAKE_CURRENT_LIST_DIR}/cmake/CollectMigrationFiles.cmake)

collect_migration_files(
  LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/migrations"
  INSTALL_DESTINATION "share/my_library/migrations"
)

# Now TARGET_MIGRATION_FILE_VERSION is available to use:
target_compile_definitions(my_library
  PRIVATE TARGET_SCHEMA_VERSION=${TARGET_MIGRATION_FILE_VERSION}
)
```

This ensures that the latest schema version is compiled into your library, which is crucial for downgrade support.

We recommend:
- Testing that your target schema version can be reached from any older version.
- Testing rollback paths.
- Using real data snapshots where applicable.