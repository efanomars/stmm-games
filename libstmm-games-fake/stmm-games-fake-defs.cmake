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

# File:   stmm-games-fake-defs.cmake

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_GAMES_FAKE_MAJOR_VERSION 0)
set(STMM_GAMES_FAKE_MINOR_VERSION 27) # !-U-!
set(STMM_GAMES_FAKE_VERSION "${STMM_GAMES_FAKE_MAJOR_VERSION}.${STMM_GAMES_FAKE_MINOR_VERSION}.0")

# required stmm-games version
set(STMM_GAMES_FAKE_REQ_STMM_GAMES_MAJOR_VERSION 0)
set(STMM_GAMES_FAKE_REQ_STMM_GAMES_MINOR_VERSION 27) # !-U-!
set(STMM_GAMES_FAKE_REQ_STMM_GAMES_VERSION "${STMM_GAMES_FAKE_REQ_STMM_GAMES_MAJOR_VERSION}.${STMM_GAMES_FAKE_REQ_STMM_GAMES_MINOR_VERSION}")

include("${PROJECT_SOURCE_DIR}/../libstmm-games/stmm-games-defs.cmake")

# include dirs
set(        STMMGAMESFAKE_EXTRA_INCLUDE_DIRS  "")
list(APPEND STMMGAMESFAKE_EXTRA_INCLUDE_DIRS  "${STMMGAMES_INCLUDE_DIRS}")
set(        STMMGAMESFAKE_EXTRA_INCLUDE_SDIRS "")
list(APPEND STMMGAMESFAKE_EXTRA_INCLUDE_SDIRS "${STMMGAMES_INCLUDE_SDIRS}")

set(STMMI_TEMP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../libstmm-games-fake/include")
set(STMMI_TEMP_HEADERS_DIR "${STMMI_TEMP_INCLUDE_DIR}/stmm-games-fake")

set(        STMMGAMESFAKE_INCLUDE_DIRS  "")
list(APPEND STMMGAMESFAKE_INCLUDE_DIRS  "${STMMI_TEMP_INCLUDE_DIR}")
list(APPEND STMMGAMESFAKE_INCLUDE_DIRS  "${STMMI_TEMP_HEADERS_DIR}")
list(APPEND STMMGAMESFAKE_INCLUDE_DIRS  "${STMMGAMESFAKE_EXTRA_INCLUDE_DIRS}")
set(        STMMGAMESFAKE_INCLUDE_SDIRS "")
list(APPEND STMMGAMESFAKE_INCLUDE_SDIRS "${STMMGAMESFAKE_EXTRA_INCLUDE_SDIRS}")

# libs
set(        STMMI_TEMP_EXTERNAL_LIBRARIES     "")

set(        STMMGAMESFAKE_EXTRA_LIBRARIES     "")
list(APPEND STMMGAMESFAKE_EXTRA_LIBRARIES     "${STMMGAMES_LIBRARIES}")
list(APPEND STMMGAMESFAKE_EXTRA_LIBRARIES     "${STMMI_TEMP_EXTERNAL_LIBRARIES}")

if (BUILD_SHARED_LIBS)
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-games-fake/build/libstmm-games-fake.so")
else()
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-games-fake/build/libstmm-games-fake.a")
endif()

set(        STMMGAMESFAKE_LIBRARIES "")
list(APPEND STMMGAMESFAKE_LIBRARIES "${STMMI_LIB_FILE}")
list(APPEND STMMGAMESFAKE_LIBRARIES "${STMMGAMESFAKE_EXTRA_LIBRARIES}")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    DefineAsSecondaryTarget(stmm-games-fake  ${STMMI_LIB_FILE}  "${STMMGAMESFAKE_INCLUDE_DIRS}"  "stmm-games" "${STMMI_TEMP_EXTERNAL_LIBRARIES}")
endif()
