# Additional targets to perform clang-format/clang-tidy/cppcheck

# Get all project files - FIXME: this should also use the list of generated targets
IF(NOT CHECK_CXX_SOURCE_FILES)
    MESSAGE(FATAL_ERROR "Variable CHECK_CXX_SOURCE_FILES not defined - set it to the list of files to auto-format")
    RETURN()
ENDIF()

# Adding clang-format check and formatter if found
FIND_PROGRAM(CLANG_FORMAT NAMES "clang-format-${CLANG_FORMAT_VERSION}" "clang-format")
IF(CLANG_FORMAT)
    EXEC_PROGRAM(
        ${CLANG_FORMAT} ${CMAKE_CURRENT_SOURCE_DIR}
        ARGS --version
        OUTPUT_VARIABLE CLANG_VERSION)
    STRING(REGEX REPLACE ".* ([0-9]+)\\.[0-9]+\\.[0-9]+.*" "\\1" CLANG_MAJOR_VERSION ${CLANG_VERSION})

    # Let's treat macOS differently because they don't have up-to-date versions
    IF(${CLANG_MAJOR_VERSION} EQUAL ${CLANG_FORMAT_VERSION} OR DEFINED ${APPLE})
        # On macOS we might have the right version - or not..
        IF(NOT ${CLANG_MAJOR_VERSION} EQUAL ${CLANG_FORMAT_VERSION})
            MESSAGE(
                WARNING "Found ${CLANG_FORMAT} version ${CLANG_MAJOR_VERSION}, this might lead to incompatible formatting")
        ELSE()
            MESSAGE(STATUS "Found ${CLANG_FORMAT} version ${CLANG_FORMAT_VERSION}, adding formatting targets")
        ENDIF()

        ADD_CUSTOM_TARGET(
            format
            COMMAND ${CLANG_FORMAT} -i -style=file ${CHECK_CXX_SOURCE_FILES}
            COMMENT "Auto formatting of all source files")

        ADD_CUSTOM_TARGET(
            check-format
            COMMAND
                ${CLANG_FORMAT} -style=file -output-replacements-xml ${CHECK_CXX_SOURCE_FILES}
                # print output
                | tee ${CMAKE_BINARY_DIR}/check_format_file.txt | grep -c "replacement " | tr -d "[:cntrl:]" && echo
                " replacements necessary"
                # WARNING: fix to stop with error if there are problems
            COMMAND ! grep -c "replacement " ${CMAKE_BINARY_DIR}/check_format_file.txt > /dev/null
            COMMENT "Checking format compliance")
    ELSE()
        MESSAGE(STATUS "Could only find version ${CLANG_MAJOR_VERSION} of clang-format, "
                       "but version ${CLANG_FORMAT_VERSION} is required.")
    ENDIF()
ELSE()
    MESSAGE(STATUS "Could NOT find clang-format")
ENDIF()

# Adding clang-tidy target if executable is found
SET(CXX_STD 11)
IF(${CMAKE_CXX_STANDARD})
    SET(CXX_STD ${CMAKE_CXX_STANDARD})
ENDIF()

FIND_PROGRAM(CLANG_TIDY NAMES "clang-tidy-${CLANG_TIDY_VERSION}" "clang-tidy")
# Enable clang tidy only if using a clang compiler
IF(CLANG_TIDY AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # If debug build enabled do automatic clang tidy
    IF(CMAKE_BUILD_TYPE MATCHES Debug)
        SET(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY} "-header-filter='${CMAKE_SOURCE_DIR}'")
    ENDIF()

    # Enable checking and formatting through run-clang-tidy if available
    # FIXME Make finding this program more portable
    GET_FILENAME_COMPONENT(CLANG_TIDY ${CLANG_TIDY} REALPATH)
    GET_FILENAME_COMPONENT(CLANG_DIR ${CLANG_TIDY} DIRECTORY)
    FIND_PROGRAM(
        RUN_CLANG_TIDY
        NAMES "run-clang-tidy.py" "run-clang-tidy-${CLANG_FORMAT_VERSION}.py"
        HINTS /usr/share/clang/ ${CLANG_DIR}/../share/clang/ /usr/bin/)
    IF(RUN_CLANG_TIDY)
        MESSAGE(STATUS "Found ${CLANG_TIDY}, adding linting targets")
        # write a .clang-tidy file in the binary dir to disable checks for created files
        FILE(WRITE ${CMAKE_BINARY_DIR}/.clang-tidy "\n---\nChecks: '-*,llvm-twine-local'\n...\n")

        # Set export commands on
        SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

        # Get amount of processors to speed up linting
        INCLUDE(ProcessorCount)
        PROCESSORCOUNT(NPROC)
        IF(NPROC EQUAL 0)
            SET(NPROC 1)
        ENDIF()

        ADD_CUSTOM_TARGET(
            lint
            COMMAND ${RUN_CLANG_TIDY} -fix -format -header-filter=${CMAKE_SOURCE_DIR} -j${NPROC}
            COMMENT "Auto fixing problems in all source files")

        ADD_CUSTOM_TARGET(
            check-lint
            COMMAND
                ${RUN_CLANG_TIDY} -header-filter=${CMAKE_SOURCE_DIR} -j${NPROC} | tee ${CMAKE_BINARY_DIR}/check_lint_file.txt
                # WARNING: fix to stop with error if there are problems
            COMMAND ! grep -c ": error: " ${CMAKE_BINARY_DIR}/check_lint_file.txt > /dev/null
            COMMENT "Checking for problems in source files")
    ELSE()
        MESSAGE(STATUS "Could NOT find run-clang-tidy script")
    ENDIF()
ELSE()
    IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        MESSAGE(STATUS "Could NOT find clang-tidy version ${CLANG_FORMAT_VERSION}")
    ELSE()
        MESSAGE(STATUS "Could NOT check for clang-tidy, wrong compiler: ${CMAKE_CXX_COMPILER_ID}")
    ENDIF()
ENDIF()

FIND_PROGRAM(CPPCHECK "cppcheck")
IF(CPPCHECK)
    # Set export commands on
    SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    ADD_CUSTOM_TARGET(
        cppcheck
        COMMAND
            ${CPPCHECK} --enable=all --project=${CMAKE_BINARY_DIR}/compile_commands.json --std=c++11 --verbose --quiet
            --xml-version=2 --language=c++ --suppress=missingIncludeSystem
            --output-file=${CMAKE_BINARY_DIR}/cppcheck_results.xml ${CHECK_CXX_SOURCE_FILES}
        COMMENT "Generate cppcheck report for the project")

    FIND_PROGRAM(CPPCHECK_HTML "cppcheck-htmlreport")
    IF(CPPCHECK_HTML)
        ADD_CUSTOM_TARGET(
            cppcheck-html
            COMMAND ${CPPCHECK_HTML} --title=${CMAKE_PROJECT_NAME} --file=${CMAKE_BINARY_DIR}/cppcheck_results.xml
                    --report-dir=${CMAKE_BINARY_DIR}/cppcheck_results --source-dir=${CMAKE_SOURCE_DIR}
            COMMENT "Convert cppcheck report to HTML output")
        ADD_DEPENDENCIES(cppcheck-html cppcheck)
    ENDIF()
ENDIF()
