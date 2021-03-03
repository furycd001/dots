include(CMakeParseArguments)

function(add_qt2_object_library target)
    set(multiValueArgs SOURCES COMPILE_OPTIONS COMPILE_DEFINITIONS INCLUDE_DIRS)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_library(${target} OBJECT ${arg_SOURCES})
    if(arg_COMPILE_DEFINITIONS)
        target_compile_definitions(${target}
            PRIVATE
            ${arg_COMPILE_DEFINITIONS}
            )
    endif()
    if(arg_COMPILE_OPTIONS)
        target_compile_options(${target}
            PRIVATE
                ${arg_COMPILE_OPTIONS}
            )
    endif()
    target_include_directories(${target} BEFORE
        PRIVATE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${arg_INCLUDE_DIRS}>
        )
    set_target_properties(${target}
        PROPERTIES
            POSITION_INDEPENDENT_CODE ON
        )
endfunction()
