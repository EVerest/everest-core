include("${CMAKE_CURRENT_LIST_DIR}/packaging-helper.cmake")

# set global variable to tell depending project if it is the main
evc_get_main_project_flag(EVC_MAIN_PROJECT)


foreach (component ${everest-cmake_FIND_COMPONENTS})
	if ("bundling" STREQUAL component)
		include("${CMAKE_CURRENT_LIST_DIR}/bundling.cmake")
	else ()
		message(FATAL_ERROR "everest-cmake: unknown component '${component}'")
	endif ()
endforeach()

if (EVC_ENABLE_CCACHE)
	include("${CMAKE_CURRENT_LIST_DIR}/ccache.cmake")
endif()

# include guard - might not be needed
if (everest_cmake_FOUND)
	return()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/edm.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/python-package-helpers.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/python-virtualenv.cmake")

macro (evc_include FILE)
	include("${everest-cmake_DIR}/3rd_party/${FILE}.cmake")
endmacro()

include("${CMAKE_CURRENT_LIST_DIR}/version-information.cmake")
