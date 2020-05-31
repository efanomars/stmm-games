# File: libstmm-games-file/CMakeLists.txt

#  Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public
#  License along with this program; if not, see <http://www.gnu.org/licenses/>

cmake_minimum_required(VERSION 3.0)

project(stmm-games-file CXX)

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
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-games-file")
# Header files
set(STMMI_HEADERS
        "${STMMI_HEADERS_DIR}/allpreferences.h"
        "${STMMI_HEADERS_DIR}/allpreferencesloader.h"
        "${STMMI_HEADERS_DIR}/file.h"
        "${STMMI_HEADERS_DIR}/gameconstraints.h"
        "${STMMI_HEADERS_DIR}/gameloader.h"
        "${STMMI_HEADERS_DIR}/highscoresloader.h"
        "${STMMI_HEADERS_DIR}/stmm-games-file-config.h"
        )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
            #"${STMMI_SOURCES_DIR}/stdthemedrawingcontext.cc"

        "${STMMI_SOURCES_DIR}/allpreferences.cc"
        "${STMMI_SOURCES_DIR}/allpreferencesloader.cc"
        "${STMMI_SOURCES_DIR}/file.cc"
        "${STMMI_SOURCES_DIR}/gameconstraints.cc"
        "${STMMI_SOURCES_DIR}/gameloader.cc"
        "${STMMI_SOURCES_DIR}/highscoresloader.cc"
        #
        )

# Define library
add_library(stmm-games-file ${STMMI_SOURCES} "${PROJECT_BINARY_DIR}/stmm-games-file-config.cc")

include("stmm-games-file-defs.cmake")

target_include_directories(stmm-games-file        PUBLIC "${STMMGAMESFILE_EXTRA_INCLUDE_DIRS}")
target_include_directories(stmm-games-file SYSTEM PUBLIC "${STMMGAMESFILE_EXTRA_INCLUDE_SDIRS}")
target_include_directories(stmm-games-file BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-games-file>  # <prefix>/include/stmm-games-file
)

#target_link_libraries(stmm-games-file stmm-games)
target_link_libraries(stmm-games-file ${STMMGAMESFILE_EXTRA_LIBRARIES})

set_target_properties(stmm-games-file PROPERTIES  ${CMAKE_BUILD_TYPE}_POSTFIX "")
set_target_properties(stmm-games-file PROPERTIES
        VERSION ${STMM_GAMES_FILE_VERSION}
        SOVERSION ${STMM_GAMES_FILE_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-games-file-${STMM_GAMES_FILE_VERSION}"
        )

DefineTargetPublicCompileOptions(stmm-games-file)

# Set version for stmm-games-file-config.cc.in
set(STMMI_PKG_VERSION "${STMM_GAMES_FILE_VERSION}")
# Create stmm-games-file-config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-games-file-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-games-file-config.cc" @ONLY)

# Set install dir for pkg-config file
set(STMMI_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
# Create pkg-config file for the library
configure_file("${PROJECT_SOURCE_DIR}/stmm-games-file.pc.in"
               "${PROJECT_BINARY_DIR}/stmm-games-file.pc" @ONLY)

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-games-file was configured with the following options:")
message(STATUS " STMMI_HEADERS:                     ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                     ${STMMI_SOURCES}")
message(STATUS " STMMGAMESFILE_EXTRA_INCLUDE_DIRS:  ${STMMGAMESFILE_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMGAMESFILE_EXTRA_INCLUDE_SDIRS: ${STMMGAMESFILE_EXTRA_INCLUDE_SDIRS}")
message(STATUS " STMMGAMESFILE_EXTRA_LIBRARIES:     ${STMMGAMESFILE_EXTRA_LIBRARIES}")
message(STATUS " CMAKE_BUILD_TYPE:                  ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:             ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:        ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                   ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                    ${CMAKE_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                        ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                     ${BUILD_TESTING}")
endif()

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-games-file doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)

install(TARGETS stmm-games-file LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS}             DESTINATION "include/stmm-games-file")

install(FILES  "${PROJECT_BINARY_DIR}/stmm-games-file.pc"   DESTINATION "lib/pkgconfig")
