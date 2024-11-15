# Database migrations

## Introduction

Updating the schema of a database that is in production should always be done with great care. To facilitate this we make use of migration files. These are files that can be executed in sequence to get to the desired schema version that works with that version of the software.

## How to use

Using the database migrations is fairly straightforward. Most of the details are handled by the library itself. To setup these migrations there are a few things to consider:

- The user of the library is responsible for getting the migration files on the target. They also need to make sure that even on a downgrade the migration files stay on target. This is because downgrading the database schema from the "newer" version makes use of the files that are stored.
- The user of the library is also responsible of making a backup of the database before running the migration. It is designed in a way that migration failures shall not alter the database but this is not guaranteed.
  - Since the migrations are applied on construction of the charge_point, the backup needs to be made before constructing it.
- Old databases need to be removed so a new database can be created using the migrations. This is to make sure that there is exact control over the schema of the database and no remains are present.

**Requirements:**

- Minimal SQLite version 3.35.0 for `ALTER TABLE DROP COLUMN` support

## How to contribute changes to the database schema

If changes need to be made to the database schema this can be done by adding new migration files in the relevant `config/vxx/core_migrations` folder.

### File format

The filenames must have the following format:  
`x_up[-description].sql` and `x_down[-description].sql`

The description is optional and x needs to be the next number in the sequence.

The files always exist in pairs except for the initial "up" file used to start the database. The "up" file contains the changes needed to update the database schema so that it can be used with the new firmware. The "down" file must undo all the changes of the "up" file so a perfect downgrade is possible.

CMake will validate the completeness of the migration pairs and the filenames. If this is not correct CMake will fail to initialize.

### Schema changes

The schema changes should be done in such a way that the data present in the databases will persist unless it is really necessary to remove stuff.

### Unit testing

There are already unit tests present that will automatically perform a full up and down migration with all the intermediate steps in the `core_migrations` folder. The files will be auto detected for this.

Additionally it is recommended to write specific unit tests for your migration that validate that the changes you have made have the expected outcome.

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
