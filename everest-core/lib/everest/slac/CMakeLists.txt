add_subdirectory(io)
add_subdirectory(fsm/ev)
add_subdirectory(fsm/evse)

if(BUILD_DEV_TESTS)
  add_subdirectory(test)
endif()
