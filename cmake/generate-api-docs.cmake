function(generate_async_api_docs)
  set(oneValueArgs
    API_PATH
    API_NAME
  )
  cmake_parse_arguments(FN
    ""
    "${oneValueArgs}"
    ""
    ${ARGN}
  )
  if ("${FN_API_PATH}" STREQUAL "")
    message(FATAL_ERROR "API_PATH is required")
  endif()
  if ("${FN_API_NAME}" STREQUAL "")
    message(FATAL_ERROR "API_NAME is required")
  endif()

  set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/everest_api_docs/${FN_API_NAME})
  set(ASYNC_TARGET_NAME ${FN_API_NAME}_AsyncApi)
  set(API_DOC_INSTALL_DIR ${CMAKE_INSTALL_DOCDIR}/everest_api)

  add_custom_command(
    OUTPUT ${OUTPUT_DIR}/yaml/asyncapi.yaml
    COMMAND ${CMAKE_COMMAND} -E make_directory ./html
    COMMAND ${CMAKE_COMMAND} -E make_directory ./yaml
    COMMAND ${CMAKE_COMMAND} -E copy ${FN_API_PATH} ${OUTPUT_DIR}/yaml/asyncapi.yaml
    DEPENDS ${FN_API_PATH}
    COMMENT "${FN_API_NAME}: Prepare AsyncApi documentation generation and copy asyncapi.yaml"
  )

  add_custom_command(
    OUTPUT ${OUTPUT_DIR}/html/index.html
    COMMAND ${ASYNCAPI_CMD} generate fromTemplate ${FN_API_PATH} ${asyncapi-html-template_SOURCE_DIR} --force-write --use-new-generator --output=${OUTPUT_DIR}/html
    DEPENDS ${FN_API_PATH}
    COMMENT "${FN_API_NAME}: Generate AsyncApi HTML documentation"
  )

  add_custom_target(${FN_API_NAME}_AsyncApi
    DEPENDS
       ${OUTPUT_DIR}/yaml/asyncapi.yaml
       ${OUTPUT_DIR}/html/index.html
       asyncapi_cli_install_target
       asyncapi_html_template_install_target
  )

  install(
    DIRECTORY ${OUTPUT_DIR}/html/
    DESTINATION ${API_DOC_INSTALL_DIR}/html/${FN_API_NAME}/
    OPTIONAL
  )

  install(
    FILES ${OUTPUT_DIR}/yaml/asyncapi.yaml
    DESTINATION ${API_DOC_INSTALL_DIR}/yaml/${FN_API_NAME}/
    OPTIONAL
  )

  add_dependencies(everest_api_docs ${ASYNC_TARGET_NAME})
endfunction()
