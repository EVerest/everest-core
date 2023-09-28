function(emit_cxxrs_header)
  set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/cxxbridge/rust/")
  file(MAKE_DIRECTORY ${output_dir})
  execute_process(COMMAND ${CXXBRIDGE} --header -o ${output_dir}cxx.h)
endfunction()

function(emit_cxxrs_for_module module_name)
  set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/cxxbridge/${module_name}/")
  file(MAKE_DIRECTORY ${output_dir})

  add_custom_command(
      OUTPUT ${output_dir}lib.rs.h ${output_dir}lib.rs.cc
      COMMAND ${CXXBRIDGE} ${CMAKE_CURRENT_SOURCE_DIR}/${module_name}/src/lib.rs --header -o ${output_dir}lib.rs.h
      COMMAND ${CXXBRIDGE} ${CMAKE_CURRENT_SOURCE_DIR}/${module_name}/src/lib.rs -o ${output_dir}lib.rs.cc
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${module_name}/src/lib.rs
      COMMENT "Generating cxx for ${module_name}"
      VERBATIM
    )
endfunction()
