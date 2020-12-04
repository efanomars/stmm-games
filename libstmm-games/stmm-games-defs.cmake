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

# File:   stmm-games-defs.cmake

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_GAMES_MAJOR_VERSION 0)
set(STMM_GAMES_MINOR_VERSION 30) # !-U-!
set(STMM_GAMES_VERSION "${STMM_GAMES_MAJOR_VERSION}.${STMM_GAMES_MINOR_VERSION}.0")

# required stmm-input-ev version
set(STMM_GAMES_REQ_STMM_INPUT_EV_MAJOR_VERSION 0)
set(STMM_GAMES_REQ_STMM_INPUT_EV_MINOR_VERSION 16) # !-U-!
set(STMM_GAMES_REQ_STMM_INPUT_EV_VERSION "${STMM_GAMES_REQ_STMM_INPUT_EV_MAJOR_VERSION}.${STMM_GAMES_REQ_STMM_INPUT_EV_MINOR_VERSION}")

# required stmm-input-au version
set(STMM_GAMES_REQ_STMM_INPUT_AU_MAJOR_VERSION 0)
set(STMM_GAMES_REQ_STMM_INPUT_AU_MINOR_VERSION 9) # !-U-!
set(STMM_GAMES_REQ_STMM_INPUT_AU_VERSION "${STMM_GAMES_REQ_STMM_INPUT_AU_MAJOR_VERSION}.${STMM_GAMES_REQ_STMM_INPUT_AU_MINOR_VERSION}")


if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    include(FindPkgConfig)
    if (NOT PKG_CONFIG_FOUND)
        message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
    endif()
    # Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
    pkg_check_modules(STMMINPUTEV   REQUIRED  stmm-input-ev>=${STMM_GAMES_REQ_STMM_INPUT_EV_VERSION})
    pkg_check_modules(STMMINPUTAU   REQUIRED  stmm-input-au>=${STMM_GAMES_REQ_STMM_INPUT_AU_VERSION})
endif()

# include dirs
set(        STMMGAMES_EXTRA_INCLUDE_DIRS  "")
set(        STMMGAMES_EXTRA_INCLUDE_SDIRS "")
list(APPEND STMMGAMES_EXTRA_INCLUDE_SDIRS "${STMMINPUTEV_INCLUDE_DIRS}")
list(APPEND STMMGAMES_EXTRA_INCLUDE_SDIRS "${STMMINPUTAU_INCLUDE_DIRS}")

set(STMMI_TEMP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../libstmm-games/include")
set(STMMI_TEMP_HEADERS_DIR "${STMMI_TEMP_INCLUDE_DIR}/stmm-games")

set(        STMMGAMES_INCLUDE_DIRS  "")
list(APPEND STMMGAMES_INCLUDE_DIRS  "${STMMI_TEMP_INCLUDE_DIR}")
list(APPEND STMMGAMES_INCLUDE_DIRS  "${STMMI_TEMP_HEADERS_DIR}")
list(APPEND STMMGAMES_INCLUDE_DIRS  "${STMMGAMES_EXTRA_INCLUDE_DIRS}")
set(        STMMGAMES_INCLUDE_SDIRS "")
list(APPEND STMMGAMES_INCLUDE_SDIRS "${STMMGAMES_EXTRA_INCLUDE_SDIRS}")

# libs
set(        STMMI_TEMP_EXTERNAL_LIBRARIES   "")
list(APPEND STMMI_TEMP_EXTERNAL_LIBRARIES   "${STMMINPUTEV_LIBRARIES}")
list(APPEND STMMI_TEMP_EXTERNAL_LIBRARIES   "${STMMINPUTAU_LIBRARIES}")

set(        STMMGAMES_EXTRA_LIBRARIES       "")
list(APPEND STMMGAMES_EXTRA_LIBRARIES       "${STMMI_TEMP_EXTERNAL_LIBRARIES}")

if (BUILD_SHARED_LIBS)
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-games/build/libstmm-games.so")
else()
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-games/build/libstmm-games.a")
endif()

set(        STMMGAMES_LIBRARIES "")
list(APPEND STMMGAMES_LIBRARIES "${STMMI_LIB_FILE}")
list(APPEND STMMGAMES_LIBRARIES "${STMMGAMES_EXTRA_LIBRARIES}")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    DefineAsSecondaryTarget(stmm-games  ${STMMI_LIB_FILE}  "${STMMGAMES_INCLUDE_DIRS}"  "" "${STMMI_TEMP_EXTERNAL_LIBRARIES}")
endif()
