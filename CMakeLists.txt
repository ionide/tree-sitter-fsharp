cmake_minimum_required(VERSION 3.13)

project(tree-sitter-fsharp
        VERSION "0.3.11"
        DESCRIPTION "F# grammars (implementation and signature files) for tree-sitter"
        HOMEPAGE_URL "https://github.com/ionide/tree-sitter-fsharp"
        LANGUAGES C)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(TREE_SITTER_REUSE_ALLOCATOR "Reuse the library allocator" OFF)

set(TREE_SITTER_ABI_VERSION 15 CACHE STRING "Tree-sitter ABI version")
if(NOT ${TREE_SITTER_ABI_VERSION} MATCHES "^[0-9]+$")
    unset(TREE_SITTER_ABI_VERSION CACHE)
    message(FATAL_ERROR "TREE_SITTER_ABI_VERSION must be an integer")
endif()

find_program(TREE_SITTER_CLI tree-sitter DOC "Tree-sitter CLI")

include(GNUInstallDirs)

# This repository defines two grammars: `fsharp` for implementation files and
# `fsharp_signature` for signature files. Both are generated and compiled into
# one library — each .c is its own translation unit, so the two scanners'
# static helpers in common/scanner.h do not collide.
#
# The signature grammar is derived from the base one (it starts with
# `grammar(require("../fsharp/grammar"), ...)`), so it must regenerate whenever
# either grammar.js changes.
#
# The generated sources are checked into git, so the CLI is only needed when a
# grammar.js is newer than its parser.c. Without it, fall back to the checked-in
# files rather than failing with a bare `TREE_SITTER_CLI-NOTFOUND` at build time.
if(TREE_SITTER_CLI)
    add_custom_command(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/fsharp/src/parser.c"
                       DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/fsharp/grammar.js"
                       COMMAND "${TREE_SITTER_CLI}" generate
                                --abi=${TREE_SITTER_ABI_VERSION}
                       WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/fsharp"
                       COMMENT "Generating fsharp/src/parser.c")

    add_custom_command(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/fsharp_signature/src/parser.c"
                       DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/fsharp_signature/grammar.js"
                               "${CMAKE_CURRENT_SOURCE_DIR}/fsharp/grammar.js"
                       COMMAND "${TREE_SITTER_CLI}" generate
                                --abi=${TREE_SITTER_ABI_VERSION}
                       WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/fsharp_signature"
                       COMMENT "Generating fsharp_signature/src/parser.c")
elseif(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/fsharp/src/parser.c"
       OR NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/fsharp_signature/src/parser.c")
    message(FATAL_ERROR
            "The tree-sitter CLI was not found and the generated parsers are missing. "
            "Install the CLI and reconfigure.")
else()
    message(STATUS "tree-sitter CLI not found; using the checked-in generated parsers")
endif()

add_library(tree-sitter-fsharp
            fsharp/src/parser.c
            fsharp/src/scanner.c
            fsharp_signature/src/parser.c
            fsharp_signature/src/scanner.c)
target_include_directories(tree-sitter-fsharp
                           PRIVATE fsharp/src fsharp_signature/src
                           INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bindings/c>
                                     $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_definitions(tree-sitter-fsharp PRIVATE
                           $<$<BOOL:${TREE_SITTER_REUSE_ALLOCATOR}>:TREE_SITTER_REUSE_ALLOCATOR>
                           $<$<CONFIG:Debug>:TREE_SITTER_DEBUG>)

set_target_properties(tree-sitter-fsharp
                      PROPERTIES
                      C_STANDARD 11
                      POSITION_INDEPENDENT_CODE ON
                      SOVERSION "${TREE_SITTER_ABI_VERSION}.${PROJECT_VERSION_MAJOR}"
                      DEFINE_SYMBOL "")

# Placeholders consumed by bindings/c/tree-sitter-fsharp.pc.in. Without these
# the generated .pc file has an empty prefix, libdir, includedir and Version.
set(PREFIX "${CMAKE_INSTALL_PREFIX}")
set(LIBDIR "${CMAKE_INSTALL_FULL_LIBDIR}")
set(INCLUDEDIR "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
set(URL "${PROJECT_HOMEPAGE_URL}")
set(VERSION "${PROJECT_VERSION}")
set(REQUIRES "")
set(ADDITIONAL_LIBS "")

configure_file(bindings/c/tree-sitter-fsharp.pc.in
               "${CMAKE_CURRENT_BINARY_DIR}/tree-sitter-fsharp.pc" @ONLY)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bindings/c/tree_sitter"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING PATTERN "*.h")
# The .pc file hard-codes -L${libdir}, so it belongs next to the library rather
# than in the architecture-independent datadir. This matches the Makefile.
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tree-sitter-fsharp.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
install(TARGETS tree-sitter-fsharp
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

file(GLOB QUERIES queries/*.scm)
install(FILES ${QUERIES}
        DESTINATION "${CMAKE_INSTALL_DATADIR}/tree-sitter/queries/fsharp")

file(GLOB SIGNATURE_QUERIES fsharp_signature/queries/*.scm)
install(FILES ${SIGNATURE_QUERIES}
        DESTINATION "${CMAKE_INSTALL_DATADIR}/tree-sitter/queries/fsharp_signature")

add_custom_target(ts-test "${TREE_SITTER_CLI}" test
                  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  COMMENT "tree-sitter test")
