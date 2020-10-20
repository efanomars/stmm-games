# Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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

# File:   stmm-games-xml-base-defs.cmake

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_GAMES_XML_BASE_MAJOR_VERSION 0)
set(STMM_GAMES_XML_BASE_MINOR_VERSION 26) # !-U-!
set(STMM_GAMES_XML_BASE_VERSION "${STMM_GAMES_XML_BASE_MAJOR_VERSION}.${STMM_GAMES_XML_BASE_MINOR_VERSION}.0")

# required stmm-games version
set(STMM_GAMES_XML_BASE_REQ_STMM_GAMES_MAJOR_VERSION 0)
set(STMM_GAMES_XML_BASE_REQ_STMM_GAMES_MINOR_VERSION 26) # !-U-!
set(STMM_GAMES_XML_BASE_REQ_STMM_GAMES_VERSION "${STMM_GAMES_XML_BASE_REQ_STMM_GAMES_MAJOR_VERSION}.${STMM_GAMES_XML_BASE_REQ_STMM_GAMES_MINOR_VERSION}")

# required libxml++-2.6 version
set(STMM_GAMES_XML_BASE_REQ_LIBXMLPP_VERSION "2.36")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    include(FindPkgConfig)
    if (NOT PKG_CONFIG_FOUND)
        message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
    endif()
    # Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
    pkg_check_modules(LIBXMLPP   REQUIRED  libxml++-2.6>=${STMM_GAMES_XML_BASE_REQ_LIBXMLPP_VERSION})
endif()

include("${PROJECT_SOURCE_DIR}/../libstmm-games/stmm-games-defs.cmake")

# include dirs
set(        STMMGAMESXMLBASE_EXTRA_INCLUDE_DIRS  "")
list(APPEND STMMGAMESXMLBASE_EXTRA_INCLUDE_DIRS  "${STMMGAMES_INCLUDE_DIRS}")
set(        STMMGAMESXMLBASE_EXTRA_INCLUDE_SDIRS "")
list(APPEND STMMGAMESXMLBASE_EXTRA_INCLUDE_SDIRS "${STMMGAMES_INCLUDE_SDIRS}")
list(APPEND STMMGAMESXMLBASE_EXTRA_INCLUDE_SDIRS "${LIBXMLPP_INCLUDE_DIRS}")

set(STMMI_TEMP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../libstmm-games-xml-base/include")
set(STMMI_TEMP_HEADERS_DIR "${STMMI_TEMP_INCLUDE_DIR}/stmm-games-xml-base")

set(        STMMGAMESXMLBASE_INCLUDE_DIRS  "")
list(APPEND STMMGAMESXMLBASE_INCLUDE_DIRS  "${STMMI_TEMP_INCLUDE_DIR}")
list(APPEND STMMGAMESXMLBASE_INCLUDE_DIRS  "${STMMI_TEMP_HEADERS_DIR}")
list(APPEND STMMGAMESXMLBASE_INCLUDE_DIRS  "${STMMGAMESXMLBASE_EXTRA_INCLUDE_DIRS}")
set(        STMMGAMESXMLBASE_INCLUDE_SDIRS "")
list(APPEND STMMGAMESXMLBASE_INCLUDE_SDIRS "${STMMGAMESXMLBASE_EXTRA_INCLUDE_SDIRS}")

# libs
set(        STMMI_TEMP_EXTERNAL_LIBRARIES         "")
list(APPEND STMMI_TEMP_EXTERNAL_LIBRARIES         "${LIBXMLPP_LIBRARIES}")

set(        STMMGAMESXMLBASE_EXTRA_LIBRARIES      "")
list(APPEND STMMGAMESXMLBASE_EXTRA_LIBRARIES      "${STMMGAMES_LIBRARIES}")
list(APPEND STMMGAMESXMLBASE_EXTRA_LIBRARIES      "${STMMI_TEMP_EXTERNAL_LIBRARIES}")

if (BUILD_SHARED_LIBS)
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-games-xml-base/build/libstmm-games-xml-base.so")
else()
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-games-xml-base/build/libstmm-games-xml-base.a")
endif()

set(        STMMGAMESXMLBASE_LIBRARIES "")
list(APPEND STMMGAMESXMLBASE_LIBRARIES "${STMMI_LIB_FILE}")
list(APPEND STMMGAMESXMLBASE_LIBRARIES "${STMMGAMESXMLBASE_EXTRA_LIBRARIES}")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    DefineAsSecondaryTarget(stmm-games-xml-base  ${STMMI_LIB_FILE}  "${STMMGAMESXMLBASE_INCLUDE_DIRS}"  "stmm-games" "${STMMI_TEMP_EXTERNAL_LIBRARIES}")
endif()
