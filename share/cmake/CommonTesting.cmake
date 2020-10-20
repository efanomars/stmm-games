# Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public
# License along with this program; if not, see <http://www.gnu.org/licenses/>

# File:   CommonTesting.cmake

# TestFiles              Create test executables for a target library.
# 
# Parameters:
# STMMI_TEST_SOURCES     list of test source files for each of which a test executable
#                        is created.
# STMMI_WITH_SOURCES     list of sources that are compiled with each of the tests in 
#                        STMMI_TEST_SOURCES
# STMMI_LINKED_INCLUDES  list of include directories needed by each test.
# STMMI_LINKED_LIBS      list of libraries that have to be linked to each test.
#                        One of them might be the to be tested library target name
#                        (cmake will recognize and link the freshly created one instead of 
#                        the possibly installed one).
#                        ex. "stmm-games-xml;stmm-games-gtk". Note: don't prepend 'lib'!
# STMMI_ADD_FAKE_STUFF   bool that tells whether the stuff defined in stmm-games-fake
#                        should be copied to the ${PROJECT_BINARY_DIR} and compiled with
#                        each test.
#
# Implicit paramters (all the project's libraries have to define them):
# STMMI_HEADERS_DIR      The directory of public headers of the to be tested library
# STMMI_INCLUDE_DIR      The directory containing STMMI_HEADERS_DIR
# STMMI_SOURCES_DIR      The directory of private headers and sources of the to be tested library
#
function(TestFiles STMMI_TEST_SOURCES  STMMI_WITH_SOURCES  STMMI_LINKED_INCLUDES  STMMI_LINKED_LIBS  STMMI_ADD_FAKE_STUFF)

    if (BUILD_TESTING)

        if ($ENV{STMM_CMAKE_COMMENTS})
        message(STATUS "STMMI_TEST_SOURCES     ${STMMI_TEST_SOURCES}")
        message(STATUS "STMMI_WITH_SOURCES     ${STMMI_WITH_SOURCES}")
        message(STATUS "STMMI_LINKED_INCLUDES  ${STMMI_LINKED_INCLUDES}")
        message(STATUS "STMMI_LINKED_LIBS      ${STMMI_LINKED_LIBS}")
        message(STATUS "STMMI_ADD_FAKE_STUFF   ${STMMI_ADD_FAKE_STUFF}")
        message(STATUS "STMMI_HEADERS_DIR      ${STMMI_HEADERS_DIR}")
        message(STATUS "STMMI_SOURCES_DIR      ${STMMI_SOURCES_DIR}")
        endif()

        if (STMMI_ADD_FAKE_STUFF)
            set(DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE "THIS FILE WAS AUTOMATICALLY GENERATED! DO NOT MODIFY!")
            #
            set(STMMI_FAKES_HEADERS_DIR "${PROJECT_SOURCE_DIR}/../libstmm-games-fake/include/stmm-games-fake")
            set(STMMI_FAKES_HEADERS
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/dumbblockevent.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fakelevelview.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixtureDevices.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixtureGame.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixtureGameOwner.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixtureLayoutAuto.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixtureStdConfig.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixtureStdPreferences.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixtureTestBase.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixturevariantHighscoresDefinition.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixturevariantKeyActions.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixturevariantLayout.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixturevariantLevelInit.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixturevariantOptions.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixturevariantPlayers.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixturevariantPrefsDevices.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixturevariantPrefsPlayers.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixturevariantTeams.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/fixturevariantVariables.h"
                    "${PROJECT_BINARY_DIR}/stmm-games-fake/mockevent.h"
                 )
            file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/stmm-games-fake")
            foreach (STMMI_TEST_CUR_FAKES_HEADER  ${STMMI_FAKES_HEADERS})
                get_filename_component(STMMI_TEST_CUR_FAKES_HEADER_NAME "${STMMI_TEST_CUR_FAKES_HEADER}" NAME)
                configure_file("${STMMI_FAKES_HEADERS_DIR}/${STMMI_TEST_CUR_FAKES_HEADER_NAME}"
                               "${STMMI_TEST_CUR_FAKES_HEADER}" @ONLY)
            endforeach()
            #
            set(STMMI_FAKES_SOURCES_DIR "${PROJECT_SOURCE_DIR}/../libstmm-games-fake/src")
            set(STMMI_FAKES_SOURCES
                    "${PROJECT_BINARY_DIR}/dumbblockevent.cc"
                    "${PROJECT_BINARY_DIR}/fakelevelview.cc"
                    "${PROJECT_BINARY_DIR}/fixtureDevices.cc"
                    "${PROJECT_BINARY_DIR}/fixtureGame.cc"
                    "${PROJECT_BINARY_DIR}/fixtureGameOwner.cc"
                    "${PROJECT_BINARY_DIR}/fixtureLayoutAuto.cc"
                    "${PROJECT_BINARY_DIR}/fixtureStdConfig.cc"
                    "${PROJECT_BINARY_DIR}/fixtureStdPreferences.cc"
                    "${PROJECT_BINARY_DIR}/fixtureTestBase.cc"
                    "${PROJECT_BINARY_DIR}/fixturevariantHighscoresDefinition.cc"
                    "${PROJECT_BINARY_DIR}/fixturevariantKeyActions.cc"
                    "${PROJECT_BINARY_DIR}/fixturevariantLayout.cc"
                    "${PROJECT_BINARY_DIR}/fixturevariantLevelInit.cc"
                    "${PROJECT_BINARY_DIR}/fixturevariantOptions.cc"
                    "${PROJECT_BINARY_DIR}/fixturevariantPlayers.cc"
                    "${PROJECT_BINARY_DIR}/fixturevariantPrefsDevices.cc"
                    "${PROJECT_BINARY_DIR}/fixturevariantPrefsPlayers.cc"
                    "${PROJECT_BINARY_DIR}/fixturevariantTeams.cc"
                    "${PROJECT_BINARY_DIR}/fixturevariantVariables.cc"
                    "${PROJECT_BINARY_DIR}/mockevent.cc"
                 )
            foreach (STMMI_TEST_CUR_FAKES_SOURCE  ${STMMI_FAKES_SOURCES})
                get_filename_component(STMMI_TEST_CUR_FAKES_SOURCE_NAME "${STMMI_TEST_CUR_FAKES_SOURCE}" NAME)
                configure_file("${STMMI_FAKES_SOURCES_DIR}/${STMMI_TEST_CUR_FAKES_SOURCE_NAME}"
                               "${STMMI_TEST_CUR_FAKES_SOURCE}" @ONLY)
            endforeach()
        endif()
        #

        #  # precompile a static lib with STMMI_WITH_SOURCES
        #  list(LENGTH STMMI_WITH_SOURCES STMMI_WITH_SOURCES_LEN)
        #  if (STMMI_WITH_SOURCES_LEN GREATER 0)
        #      add_library(stmmiwithsourcesobjlib OBJECT ${STMMI_WITH_SOURCES})
        #  endif (STMMI_WITH_SOURCES_LEN GREATER 0)
        #   ^ need to target_compile_definitions target_include_directories !!!

        # Iterate over all tests found. For each, declare an executable and add it to the tests list.
        foreach (STMMI_TEST_CUR_FILE  ${STMMI_TEST_SOURCES})

#message(STATUS "STMMI_TEST_CUR_FILE     ${STMMI_TEST_CUR_FILE}")
            file(RELATIVE_PATH  STMMI_TEST_CUR_REL_FILE  ${PROJECT_SOURCE_DIR}/test  ${STMMI_TEST_CUR_FILE})

            string(REGEX REPLACE "[./]" "_" STMMI_TEST_CUR_TGT ${STMMI_TEST_CUR_REL_FILE})

            add_executable(${STMMI_TEST_CUR_TGT} ${STMMI_TEST_CUR_FILE}
                           ${STMMI_WITH_SOURCES} ${STMMI_FAKES_SOURCES})
            #if (STMMI_WITH_SOURCES_LEN GREATER 0)
            #    target_link_libraries(${STMMI_TEST_CUR_TGT} stmmiwithsourcesobjlib) # link precompiled object files
            #endif (STMMI_WITH_SOURCES_LEN GREATER 0)

            # the library files
            target_include_directories(${STMMI_TEST_CUR_TGT} BEFORE PRIVATE ${STMMI_INCLUDE_DIR})
            if (STMMI_ADD_FAKE_STUFF)
                target_include_directories(${STMMI_TEST_CUR_TGT} BEFORE PRIVATE ${PROJECT_BINARY_DIR})
            endif (STMMI_ADD_FAKE_STUFF)
            # tests can also involve non public part of the library!
            target_include_directories(${STMMI_TEST_CUR_TGT} BEFORE PRIVATE ${STMMI_SOURCES_DIR})
            target_include_directories(${STMMI_TEST_CUR_TGT} BEFORE PRIVATE ${STMMI_HEADERS_DIR})
            target_include_directories(${STMMI_TEST_CUR_TGT}        PRIVATE ${STMMI_LINKED_INCLUDES})
            target_include_directories(${STMMI_TEST_CUR_TGT}        PRIVATE ${PROJECT_SOURCE_DIR}/../share/thirdparty)

            DefineTestTargetPublicCompileOptions(${STMMI_TEST_CUR_TGT})

            target_link_libraries(${STMMI_TEST_CUR_TGT} ${STMMI_LINKED_LIBS})

            add_test(NAME ${STMMI_TEST_CUR_TGT} COMMAND ${STMMI_TEST_CUR_TGT})

        endforeach (STMMI_TEST_CUR_FILE  ${STMMI_TEST_SOURCES})
    endif (BUILD_TESTING)

endfunction()
