# File: libstmm-games-xml-base/CMakeLists.txt

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

project(stmm-games-xml-base CXX)

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
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-games-xml-base")
# Header files
set(STMMI_HEADERS_XMLUTIL
        "${STMMI_HEADERS_DIR}/xmlutil/xmlbasicparser.h"
        "${STMMI_HEADERS_DIR}/xmlutil/xmlimageparser.h"
        "${STMMI_HEADERS_DIR}/xmlutil/xmlstrconv.h"
        "${STMMI_HEADERS_DIR}/xmlutil/xmlvariantsetparser.h"
        )
#
set(STMMI_HEADERS
        "${STMMI_HEADERS_DIR}/conditionalctx.h"
        "${STMMI_HEADERS_DIR}/parserctx.h"
        "${STMMI_HEADERS_DIR}/stmm-games-xml-base-config.h"
        "${STMMI_HEADERS_DIR}/xmlcommonerrors.h"
        "${STMMI_HEADERS_DIR}/xmlcommonparser.h"
        "${STMMI_HEADERS_DIR}/xmlconditionalparser.h"
        "${STMMI_HEADERS_DIR}/xmltraitsparser.h"
        )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
            #"${STMMI_SOURCES_DIR}/xmlvariantsetparser.cc"
        #
        "${STMMI_SOURCES_DIR}/xmlutil/xmlbasicparser.cc"
        "${STMMI_SOURCES_DIR}/xmlutil/xmlimageparser.cc"
        "${STMMI_SOURCES_DIR}/xmlutil/xmlstrconv.cc"
        "${STMMI_SOURCES_DIR}/xmlutil/xmlvariantsetparser.cc"
        #
        "${STMMI_SOURCES_DIR}/conditionalctx.cc"
        "${STMMI_SOURCES_DIR}/parserctx.cc"
        "${STMMI_SOURCES_DIR}/xmlcommonerrors.cc"
        "${STMMI_SOURCES_DIR}/xmlcommonparser.cc"
        "${STMMI_SOURCES_DIR}/xmlconditionalparser.cc"
        "${STMMI_SOURCES_DIR}/xmltraitsparser.cc"
        )


# Define library
add_library(stmm-games-xml-base ${STMMI_SOURCES} "${PROJECT_BINARY_DIR}/stmm-games-xml-base-config.cc")

include("stmm-games-xml-base-defs.cmake")

target_include_directories(stmm-games-xml-base        PUBLIC ${STMMGAMESXMLBASE_EXTRA_INCLUDE_DIRS})
target_include_directories(stmm-games-xml-base SYSTEM PUBLIC ${STMMGAMESXMLBASE_EXTRA_INCLUDE_SDIRS})
target_include_directories(stmm-games-xml-base BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-games-xml-base>  # <prefix>/include/stmm-games-xml-base
)

target_link_libraries(stmm-games-xml-base ${STMMGAMESXMLBASE_EXTRA_LIBRARIES})

set_target_properties(stmm-games-xml-base PROPERTIES ${CMAKE_BUILD_TYPE}_POSTFIX "")
set_target_properties(stmm-games-xml-base PROPERTIES
        VERSION ${STMM_GAMES_XML_BASE_VERSION}
        SOVERSION ${STMM_GAMES_XML_BASE_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-games-xml-base-${STMM_GAMES_XML_BASE_VERSION}"
        )

DefineTargetPublicCompileOptions(stmm-games-xml-base)

# Set version and dirs for stmm-games-xml-base-config.cc.in
set(STMMI_PKG_VERSION "${STMM_GAMES_XML_BASE_VERSION}")
## Create config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-games-xml-base-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-games-xml-base-config.cc" @ONLY)

# Set install dir for pkg-config file
set(STMMI_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
# Create pkg-config file for the library
configure_file("${PROJECT_SOURCE_DIR}/stmm-games-xml-base.pc.in"
               "${PROJECT_BINARY_DIR}/stmm-games-xml-base.pc" @ONLY)

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-games-xml-base was configured with the following options:")
message(STATUS " STMMI_HEADERS:                        ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                        ${STMMI_SOURCES}")
message(STATUS " STMMGAMESXMLBASE_EXTRA_INCLUDE_DIRS:  ${STMMGAMESXMLBASE_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMGAMESXMLBASE_EXTRA_INCLUDE_SDIRS: ${STMMGAMESXMLBASE_EXTRA_INCLUDE_SDIRS}")
message(STATUS " STMMGAMESXMLBASE_EXTRA_LIBRARIES:     ${STMMGAMESXMLBASE_EXTRA_LIBRARIES}")
message(STATUS " CMAKE_BUILD_TYPE:                     ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:                ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:           ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                      ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                       ${CMAKE_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                           ${BUILD_DOCS}")
message(STATUS " ENV{STMM_DEBIAN_PACKAGING}:           $ENV{STMM_DEBIAN_PACKAGING}")
message(STATUS " BUILD_TESTING:                        ${BUILD_TESTING}")
message(STATUS " STMMI_PKG_DATA_DIR:                   ${STMMI_PKG_DATA_DIR}")
message(STATUS " STMMI_PKG_USER_DATA_DIR:              ${STMMI_PKG_USER_DATA_DIR}")
endif()

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-games-xml-base doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)

install(TARGETS stmm-games-xml-base LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS}                  DESTINATION "include/stmm-games-xml-base")
install(FILES ${STMMI_HEADERS_XMLUTIL}          DESTINATION "include/stmm-games-xml-base/xmlutil")
install(FILES ${STMMI_HEADERS_XMLUTILE}         DESTINATION "include/stmm-games-xml-base/xmlutile")

install(FILES  "${PROJECT_BINARY_DIR}/stmm-games-xml-base.pc"   DESTINATION "lib/pkgconfig")
