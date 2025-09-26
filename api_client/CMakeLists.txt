if(EVEREST_ENABLE_PY_SUPPORT AND EVEREST_BUILD_API_CLIENTS)
  ev_create_python_wheel_targets(
    PACKAGE_NAME async_python_client_spyclient_api
    PACKAGE_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/spyclient_api
  )

  ev_create_pip_install_targets(
    PACKAGE_NAME async_python_client_spyclient_api
    PACKAGE_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/spyclient_api
    DEPENDS async_python_client_spyclient_api
  )

  add_dependencies(everest_api_clients
    async_python_client_spyclient_api_install_wheel
  )

  include(GNUInstallDirs)
  install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/api_client.py
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
  )
endif()
