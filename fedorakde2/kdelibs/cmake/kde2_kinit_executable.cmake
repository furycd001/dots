set(kde2_kinit__internal_dir ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")

function(kde2_kinit_executable library_name)
    include(GNUInstallDirs)
    include(CMakeParseArguments)
    include(GenerateExportHeader)
    include(CMakePackageConfigHelpers)

    set(oneValueArgs
        OUTPUT_NAME
        DESKTOP_FILE
        )
    set(multiValueArgs
        LIBS
        PRIVATE_LIBS
        COMPILE_OPTIONS
        COMPILE_DEFINITIONS
        SOURCES
        INCLUDE_DIRECTORIES
        )
    set(options
        ALLOW_UNDEFINED
        )
    cmake_parse_arguments(_lib "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT _lib_SOURCES)
        message(FATAL_ERROR "Error adding library ${library_name}: No SOURCES specified.")
    endif()

    set(output_name ${library_name})
    if(_lib_OUTPUT_NAME)
        set(output_name ${_lib_OUTPUT_NAME})
    endif()

    add_library(kinit_${library_name} SHARED ${_lib_SOURCES})
    target_link_libraries(kinit_${library_name}
        PUBLIC
            ${_lib_LIBS}
        PRIVATE
            ${_lib_PRIVATE_LIBS}
            Qt::Qt2
        )

    # We need iteract over the multiple directory list
    foreach(include ${_lib_INCLUDE_DIRECTORIES})
        list(APPEND build_interface_includes "$<BUILD_INTERFACE:${include}>")
    endforeach()

    target_include_directories(kinit_${library_name}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
            ${build_interface_includes}
        )
    target_compile_options(kinit_${library_name} PRIVATE
        ${_lib_COMPILE_OPTIONS}
        ${default_options}
        )
    if(_lib_COMPILE_DEFINITIONS)
        target_compile_definitions(kinit_${library_name} PRIVATE
            ${_lib_COMPILE_DEFINITIONS}
            )
    endif()

    if(NOT ${_lib_ALLOW_UNDEFINED})
        set(link_flags "-Wl,--no-undefined")
    endif()

    set_target_properties(kinit_${library_name} PROPERTIES
        LINK_FLAGS "-Wl,--as-needed ${link_flags}"
        OUTPUT_NAME ${output_name}
        PREFIX ""
        )

    install(TARGETS kinit_${library_name}
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/kde2"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/kde2"
        )

    get_filename_component(LAPROP_SONAME ${output_name} NAME)
    get_target_property(LAPROP_DEPENDENCY_LIBS kinit_${library_name} INTERFACE_LINK_LIBRARIES)

    configure_file(${kde2_kinit__internal_dir}/kde2_libtool_template.la.in
        ${CMAKE_CURRENT_BINARY_DIR}/${output_name}.la
        @ONLY
        )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${output_name}.la
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/kde2"
        )

    if(_lib_DESKTOP_FILE)
        install(FILES ${_lib_DESKTOP_FILE}
            DESTINATION ${CMAKE_INSTALL_DATADIR}/services/
            )
    endif()

    # Generate empty dummy wrapper executable
    file(TOUCH
        ${CMAKE_CURRENT_BINARY_DIR}/${output_name}_dummy.cpp
        )
    add_executable(${output_name}
        ${CMAKE_CURRENT_BINARY_DIR}/${output_name}_dummy.cpp
        )
    target_link_libraries(${output_name} kinit_${library_name})
    install(TARGETS ${output_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    # Make sure it can find the module .so
    set_property(TARGET ${output_name}
        PROPERTY INSTALL_RPATH
        "${CMAKE_INSTALL_FULL_LIBDIR}/kde2"
    )
endfunction()
