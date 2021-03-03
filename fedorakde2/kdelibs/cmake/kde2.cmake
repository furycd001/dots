include("${CMAKE_CURRENT_LIST_DIR}/kde2_library.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/kde2_kidl.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/kde2_module.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/kde2_kinit_executable.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/kde2_icon.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/kde2_utils.cmake")

# The unfortunate global definitions
add_definitions(-DQT_NO_TRANSLATION -DQT_CLEAN_NAMESPACE -DQT_NO_COMPAT -DQT_NO_ASCII_CAST)

# Not sure if this is a good idea
# include_directories(${KDE2_INCLUDEDIR})
# add_definitions(-DHAVE_CONFIG_H)

# Default to the standard closest to what was originally used
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_EXTENSIONS ON)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wno-write-strings -Wno-unused-result")
    set(CMAKE_CXX_FLAGS  "${CMAKE_C_FLAGS} -Wno-write-strings -Wno-unused-result")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fpermissive")
endif()
