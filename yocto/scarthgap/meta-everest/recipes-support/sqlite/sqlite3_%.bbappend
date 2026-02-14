# add support for the DBSTAT virtual table
# this is being used by the libocpp AUTH_CACHE for OCPP 2.0.1
CFLAGS:append = " -DSQLITE_ENABLE_DBSTAT_VTAB"
