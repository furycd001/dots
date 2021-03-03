function(kde2_library library_name)
    include(GNUInstallDirs)
    include(CMakeParseArguments)
    include(GenerateExportHeader)
    include(CMakePackageConfigHelpers)

    set(BUILD_SHARED_LIBS ON)

    set(oneValueArgs
        VERSION
        )
    set(multiValueArgs
        LIBS
        PRIVATE_LIBS
        COMPILE_OPTIONS
        COMPILE_DEFINITIONS
        SOURCES
        HEADERS
        INCLUDE_DIRECTORIES
        )
    set(options
        ALLOW_UNDEFINED
        )
    cmake_parse_arguments(_lib "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT _lib_SOURCES)
        message(FATAL_ERROR "Error adding library ${library_name}: No SOURCES specified.")
    endif()

    string(REPLACE "lib" "" real_library_name ${library_name})
    set(output_name ${real_library_name})

    add_library(${library_name} ${_lib_SOURCES})
    target_link_libraries(${library_name}
        PUBLIC
            ${_lib_LIBS}
        PRIVATE
            ${_lib_PRIVATE_LIBS}
        )

    # We need iteract over the multiple directory list
    foreach(include ${_lib_INCLUDE_DIRECTORIES})
        list(APPEND build_interface_includes "$<BUILD_INTERFACE:${include}>")
    endforeach()

    # We use the namespace equaly as well on installed exported directories
    # so we need to replace the colons with proper slashes
    string(REPLACE "::" "/" include_namespace_directory kde2)

    target_include_directories(${library_name}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
            ${build_interface_includes}
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${include_namespace_directory}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${include_namespace_directory}/${real_library_name}>
        )
    target_compile_options(${library_name} PRIVATE
        ${_lib_COMPILE_OPTIONS}
        ${default_options}
        )
    if(_lib_COMPILE_DEFINITIONS)
        target_compile_definitions(${library_name} PRIVATE
            ${_lib_COMPILE_DEFINITIONS}
            )
    endif()

    if(NOT ${_lib_ALLOW_UNDEFINED})
        set(link_flags "-Wl,--no-undefined")
    endif()

    set(library_version ${PROJECT_VERSION})
    set(library_soversion ${PROJECT_VERSION_MAJOR})
    if(_lib_VERSION)
        set(library_version ${_lib_VERSION})
        string(REPLACE "." ";" list_version ${_lib_VERSION})
        list(GET list_version 0 library_soversion)
    endif()

    set_target_properties(${library_name} PROPERTIES
        VERSION ${library_version}
        SOVERSION ${library_soversion}
        LINK_FLAGS "-Wl,--as-needed ${link_flags}"
        OUTPUT_NAME ${output_name}-2
        EXPORT_NAME ${output_name}
        )
    add_library(kde2::${real_library_name} ALIAS ${library_name})

    generate_export_header(${library_name} EXPORT_FILE_NAME ${real_library_name}_export.h)

    install(TARGETS ${library_name} EXPORT ${real_library_name}Targets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        )

    install(FILES
        ${_lib_HEADERS}
        ${export_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${include_namespace_directory}/${real_library_name}
        COMPONENT dev
        )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${library_name}/${real_library_name}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
        )

    export(TARGETS ${library_name}
        FILE ${CMAKE_CURRENT_BINARY_DIR}/${library_name}/${real_library_name}Targets.cmake
        NAMESPACE kde2::
        EXPORT_LINK_INTERFACE_LIBRARIES
        )

    # Export the aliases
    string(TOUPPER ${real_library_name} old_school_library_var)
    string(REPLACE "-" "_" old_school_library_var ${old_school_library_var})
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${library_name}/${real_library_name}Config.cmake
    "include(\"\${CMAKE_CURRENT_LIST_DIR}/${real_library_name}Targets.cmake\")"
    "\n\nset(${old_school_library_var}_LIBRARIES kde2::${library_name})"
    )

    install(EXPORT ${real_library_name}Targets
        FILE ${real_library_name}Targets.cmake
        NAMESPACE kde2::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${real_library_name}
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${library_name}/${real_library_name}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${library_name}/${real_library_name}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${real_library_name}
        COMPONENT dev
    )

endfunction()
