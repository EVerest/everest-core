# Exception Handling of Database Operations

OCPP versions 1.6 and 2.0.1 contain several requirements for the persistent storage and retrieval of data. This library leverages SQLite to fulfill these persistent storage requirements. The DatabaseHandler classes manage all related database operations, including CREATE, SELECT, INSERT, UPDATE, and DELETE commands.

## Exception Design Considerations

The primary design decision in the development of DatabaseHandler classes is to allow exceptions to propagate to higher level logic rather than catching them internally. By propagating exceptions of database operations, it is ensured the consumer application is fully aware of any issues that occur during database operations. This transparency allows higher-level logic to make informed decisions based on the specific errors encountered. Different higher level logic may require different strategies for error handling based on their specific requirements. Propagating exceptions gives developers the flexibility to implement custom handling procedures that best fit the application’s needs.

SQLite does not natively support exceptions as it is written in C. The DatabaseHandler classes check the return codes of SQLite function calls, and if an operation fails (i.e., the return code is not SQLITE_OK, SQLITE_DONE, SQLITE_ROW, etc.), an exception is thrown. This way, all database-related errors are converted into C++ exceptions, which are then propagated up to the caller.

## Implementation Strategy

The public functions of the DatabaseHandler classes are designed to throw exceptions when SQL operations fail to execute as expected Error Handling. Consumer code should implement try-catch blocks around calls to DatabaseHandler functions. This handling should be tailored to the requirements of the OCPP specification and improve the stability of the application, whether it involves the logging errors, retrying operations or other logic.
