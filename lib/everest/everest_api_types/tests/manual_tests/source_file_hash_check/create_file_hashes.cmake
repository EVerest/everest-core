function(everest_read_expected_and_create_actual_hash_csv_files expected_hashes_file project_src_folder actual_hashes_file)
  file(STRINGS ${expected_hashes_file} EXPECTED_LINES)

  set(ACTUAL_HASHES_CSV_CONTENT "")
  foreach(LINE ${EXPECTED_LINES})
      string(STRIP "${LINE}" LINE)
      if(LINE STREQUAL "" OR LINE MATCHES "^#")
        continue()
      else()
        string(REGEX MATCH "^[^,]+" FILENAME ${LINE})

        set(FILE_PATH "${project_src_folder}/${FILENAME}")

        if(EXISTS ${FILE_PATH})
            file(SHA256 ${FILE_PATH} ACTUAL_HASH)
            string(APPEND ACTUAL_HASHES_CSV_CONTENT "${FILENAME},${ACTUAL_HASH}\n")
        else()
            # If a file listed in the CSV is missing, fail verbosely
            message(FATAL_ERROR "File '${FILENAME}' listed in '${expected_hashes_file}' was not found in '${project_src_folder}'.")
        endif()
      endif()
  endforeach()

  file(WRITE ${actual_hashes_file} "${ACTUAL_HASHES_CSV_CONTENT}")
endfunction()

everest_read_expected_and_create_actual_hash_csv_files(${EXPECTED_CSV} ${PROJECT_SRC_FOLDER} ${ACTUAL_CSV})
