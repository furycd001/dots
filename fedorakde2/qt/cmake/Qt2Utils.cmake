include(CMakeParseArguments)

function(qt2_wrap_cpp source_files)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    foreach(mocable ${arg_SOURCES})
        get_filename_component(realfile ${mocable} ABSOLUTE)
        get_filename_component(outfileName ${mocable} NAME_WE)
        add_custom_command(
            OUTPUT ${outfileName}.moc
            COMMAND moc-qt2 ${realfile} -o ${outfileName}.moc
        )
        # Check if header really generates output
        list(APPEND ${outFiles} ${outfileName}.moc)
        list(APPEND ${source_files} ${CMAKE_CURRENT_BINARY_DIR}/${outfileName}.moc)
    endforeach()

    set(${source_files} ${${source_files}} PARENT_SCOPE)
endfunction()

function(qt2_wrap_moc mocable_files)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs SOURCES)

    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    foreach(mocable ${arg_SOURCES})
        get_filename_component(realfile ${mocable} ABSOLUTE)
        get_filename_component(outfileName ${mocable} NAME_WE)
        add_custom_command(
            OUTPUT moc_${outfileName}.cpp
            COMMAND moc-qt2 ${realfile} -o moc_${outfileName}.cpp
        )
        # Check if header really generates output
        list(APPEND ${outFiles} moc_${outfileName}.cpp)
        list(APPEND ${mocable_files} ${CMAKE_CURRENT_BINARY_DIR}/moc_${outfileName}.cpp)
    endforeach()

    set(${mocable_files} ${${mocable_files}} PARENT_SCOPE)
endfunction()

function(qt2_wrap_ui ui_target)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs SOURCES)

    cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    foreach(ui_file ${arg_SOURCES})
        get_filename_component(realfile ${ui_file} ABSOLUTE)
        get_filename_component(basename ${ui_file} NAME_WE)
        add_custom_command(
            OUTPUT ${basename}.h
            COMMAND uic-qt2 ${realfile} -o ${basename}.h
            )
        add_custom_command(
            OUTPUT ${basename}.cpp
            COMMAND uic-qt2 ${realfile} -i ${basename}.h -o ${basename}.cpp
            DEPENDS ${basename}.h
            )
        list(APPEND ${ui_target} ${CMAKE_CURRENT_BINARY_DIR}/${basename}.cpp)
        add_custom_command(
            OUTPUT moc_${basename}.cpp
            COMMAND moc-qt2 ${basename}.h -o moc_${basename}.cpp
            DEPENDS ${basename}.h
            )
        list(APPEND ${ui_target} ${CMAKE_CURRENT_BINARY_DIR}/moc_${basename}.cpp)
        set(outFiles ${outFiles} ${basename}.h ${basename}.cpp)
    endforeach()

    set(${ui_target} ${${ui_target}} PARENT_SCOPE)
    add_custom_target(ui_${ui_target} ALL DEPENDS ${outFiles})
endfunction()
