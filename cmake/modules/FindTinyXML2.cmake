# TINYXML2_FOUND
# TINYXML2_INCLUDE_DIR
# TINYXML2_SOURCE_DIR

option(TINYXML2_FROM_SOURCE "Integrate TinyXML2 source code inside Fast RTPS" OFF)

find_package(PkgConfig)
pkg_check_modules(TinyXML2 REQUIRED tinyxml2)
if(TinyXML2_FOUND)
    set(TINYXML2_LIBRARY ${TinyXML2_LIBRARIES})
else()
    if(THIRDPARTY)
        set(TINYXML2_FROM_SOURCE ON)
    endif()

    if(THIRDPARTY OR ANDROID)
        find_path(TINYXML2_INCLUDE_DIR NAMES tinyxml2.h NO_CMAKE_FIND_ROOT_PATH)
    else()
        find_path(TINYXML2_INCLUDE_DIR NAMES tinyxml2.h)
    endif()


    if(TINYXML2_FROM_SOURCE)
        find_path(TINYXML2_SOURCE_DIR NAMES tinyxml2.cpp NO_CMAKE_FIND_ROOT_PATH)
    else()
        find_library(TINYXML2_LIBRARY tinyxml2)
    endif()

    include(FindPackageHandleStandardArgs)

    if(TINYXML2_FROM_SOURCE)
        find_package_handle_standard_args(tinyxml2 DEFAULT_MSG TINYXML2_SOURCE_DIR TINYXML2_INCLUDE_DIR)

        mark_as_advanced(TINYXML2_INCLUDE_DIR TINYXML2_SOURCE_DIR)
    else()
        find_package_handle_standard_args(tinyxml2 DEFAULT_MSG TINYXML2_LIBRARY TINYXML2_INCLUDE_DIR)

        mark_as_advanced(TINYXML2_INCLUDE_DIR TINYXML2_LIBRARY)
    endif()
endif()
