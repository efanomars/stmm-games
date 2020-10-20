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

# File:   libstmm-games-fake/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(stmm-games-fake CXX)

set(LIBRARY_OUTPUT_DIRECTORY "build")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../share/cmake")

include(CommonUtil)

CheckBinaryNotSourceTree()
CheckBuildType()
DefineSharedLibOption()
DefineCommonOptions()
DefineCommonCompileOptions("c++14")

# Headers dir
set(STMMI_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/include")
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-games-fake")
# Header files
set(STMMI_HEADERS
        "${STMMI_HEADERS_DIR}/dumbblockevent.h"
        "${STMMI_HEADERS_DIR}/fakelevelview.h"
        "${STMMI_HEADERS_DIR}/fixtureDevices.h"
        "${STMMI_HEADERS_DIR}/fixtureGame.h"
        "${STMMI_HEADERS_DIR}/fixtureGameOwner.h"
        "${STMMI_HEADERS_DIR}/fixtureLayoutAuto.h"
        "${STMMI_HEADERS_DIR}/fixtureStdConfig.h"
        "${STMMI_HEADERS_DIR}/fixtureStdPreferences.h"
        "${STMMI_HEADERS_DIR}/fixtureTestBase.h"
        "${STMMI_HEADERS_DIR}/fixturevariantHighscoresDefinition.h"
        "${STMMI_HEADERS_DIR}/fixturevariantKeyActions.h"
        "${STMMI_HEADERS_DIR}/fixturevariantLayout.h"
        "${STMMI_HEADERS_DIR}/fixturevariantLevelInit.h"
        "${STMMI_HEADERS_DIR}/fixturevariantOptions.h"
        "${STMMI_HEADERS_DIR}/fixturevariantPlayers.h"
        "${STMMI_HEADERS_DIR}/fixturevariantPrefsDevices.h"
        "${STMMI_HEADERS_DIR}/fixturevariantPrefsPlayers.h"
        "${STMMI_HEADERS_DIR}/fixturevariantTeams.h"
        "${STMMI_HEADERS_DIR}/fixturevariantVariables.h"
        "${STMMI_HEADERS_DIR}/mockevent.h"
        "${STMMI_HEADERS_DIR}/stmm-games-fake.h"
        "${STMMI_HEADERS_DIR}/stmm-games-fake-config.h"
        )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
        "${STMMI_SOURCES_DIR}/dumbblockevent.cc"
        "${STMMI_SOURCES_DIR}/fakelevelview.cc"
        "${STMMI_SOURCES_DIR}/fixtureDevices.cc"
        "${STMMI_SOURCES_DIR}/fixtureGame.cc"
        "${STMMI_SOURCES_DIR}/fixtureGameOwner.cc"
        "${STMMI_SOURCES_DIR}/fixtureLayoutAuto.cc"
        "${STMMI_SOURCES_DIR}/fixtureStdConfig.cc"
        "${STMMI_SOURCES_DIR}/fixtureStdPreferences.cc"
        "${STMMI_SOURCES_DIR}/fixtureTestBase.cc"
        "${STMMI_SOURCES_DIR}/fixturevariantHighscoresDefinition.cc"
        "${STMMI_SOURCES_DIR}/fixturevariantKeyActions.cc"
        "${STMMI_SOURCES_DIR}/fixturevariantLayout.cc"
        "${STMMI_SOURCES_DIR}/fixturevariantLevelInit.cc"
        "${STMMI_SOURCES_DIR}/fixturevariantOptions.cc"
        "${STMMI_SOURCES_DIR}/fixturevariantPlayers.cc"
        "${STMMI_SOURCES_DIR}/fixturevariantPrefsDevices.cc"
        "${STMMI_SOURCES_DIR}/fixturevariantPrefsPlayers.cc"
        "${STMMI_SOURCES_DIR}/fixturevariantTeams.cc"
        "${STMMI_SOURCES_DIR}/fixturevariantVariables.cc"
        "${STMMI_SOURCES_DIR}/mockevent.cc"
        "${STMMI_SOURCES_DIR}/stmm-games-fake.cc"
        )

# Define library
add_library(stmm-games-fake ${STMMI_SOURCES} "${PROJECT_BINARY_DIR}/stmm-games-fake-config.cc")

include("stmm-games-fake-defs.cmake")

target_include_directories(stmm-games-fake        PUBLIC "${STMMGAMESFAKE_EXTRA_INCLUDE_DIRS}")
target_include_directories(stmm-games-fake SYSTEM PUBLIC "${STMMGAMESFAKE_EXTRA_INCLUDE_SDIRS}")
target_include_directories(stmm-games-fake BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-games-fake>  # <prefix>/include/stmm-games-fake
)
target_include_directories(stmm-games-fake BEFORE PRIVATE ${STMMI_INCLUDE_DIR})

#target_link_libraries(stmm-games-fake stmm-games)
target_link_libraries(stmm-games-fake ${STMMGAMESFAKE_EXTRA_LIBRARIES})

set_target_properties(stmm-games-fake PROPERTIES  ${CMAKE_BUILD_TYPE}_POSTFIX "")
set_target_properties(stmm-games-fake PROPERTIES
        VERSION ${STMM_GAMES_FAKE_VERSION}
        SOVERSION ${STMM_GAMES_FAKE_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-games-fake-${STMM_GAMES_FAKE_VERSION}"
        )

DefineTargetPublicCompileOptions(stmm-games-fake)

# Set version for stmm-games-fake-config.cc.in
set(STMMI_PKG_VERSION "${STMM_GAMES_FAKE_VERSION}")
# Create config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-games-fake-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-games-fake-config.cc" @ONLY)

include(FindPkgConfig)
if (PKG_CONFIG_FOUND)
    # Set install dir for pkg-config file
    set(STMMI_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
    # Create pkg-config file for the library
    configure_file("${PROJECT_SOURCE_DIR}/stmm-games-fake.pc.in"
                   "${PROJECT_BINARY_DIR}/stmm-games-fake.pc" @ONLY)

endif()

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-games-fake was configured with the following options:")
message(STATUS " STMMI_HEADERS:                     ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                     ${STMMI_SOURCES}")
message(STATUS " STMMGAMESFAKE_EXTRA_INCLUDE_DIRS:  ${STMMGAMESFAKE_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMGAMESFAKE_EXTRA_INCLUDE_SDIRS: ${STMMGAMESFAKE_EXTRA_INCLUDE_SDIRS}")
message(STATUS " STMMGAMESFAKE_EXTRA_LIBRARIES:     ${STMMGAMESFAKE_EXTRA_LIBRARIES}")
message(STATUS " CMAKE_BUILD_TYPE:                  ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:             ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:        ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                   ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                    ${CMAKE_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                        ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                     ${BUILD_TESTING}")
message(STATUS " BUILD_SHARED_LIBS:                 ${BUILD_SHARED_LIBS}")
endif()

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-games-fake doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)

install(TARGETS stmm-games-fake LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS} DESTINATION "include/stmm-games-fake")

if (PKG_CONFIG_FOUND)
    install(FILES  "${PROJECT_BINARY_DIR}/stmm-games-fake.pc"   DESTINATION "lib/pkgconfig")
endif()
