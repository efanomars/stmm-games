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

# File:   stmm-games-file-defs.cmake

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_GAMES_FILE_MAJOR_VERSION 0)
set(STMM_GAMES_FILE_MINOR_VERSION 30) # !-U-!
set(STMM_GAMES_FILE_VERSION "${STMM_GAMES_FILE_MAJOR_VERSION}.${STMM_GAMES_FILE_MINOR_VERSION}.0")

set(STMM_GAMES_FILE_REQ_STMM_GAMES_MAJOR_VERSION 0)
set(STMM_GAMES_FILE_REQ_STMM_GAMES_MINOR_VERSION 30) # !-U-!
set(STMM_GAMES_FILE_REQ_STMM_GAMES_VERSION "${STMM_GAMES_FILE_REQ_STMM_GAMES_MAJOR_VERSION}.${STMM_GAMES_FILE_REQ_STMM_GAMES_MINOR_VERSION}")

# if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
#     include(FindPkgConfig)
#     if (NOT PKG_CONFIG_FOUND)
#         message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
#     endif()
#     # Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
#     pkg_check_modules(STMMINPUTGTK  REQUIRED  stmm-input-gtk>=${STMM_GAMES_FILE_REQ_STMM_INPUT_GTK_VERSION})
# endif()

include("${PROJECT_SOURCE_DIR}/../libstmm-games/stmm-games-defs.cmake")

# include dirs
set(        STMMGAMESFILE_EXTRA_INCLUDE_DIRS  "")
list(APPEND STMMGAMESFILE_EXTRA_INCLUDE_DIRS  "${STMMGAMES_INCLUDE_DIRS}")
set(        STMMGAMESFILE_EXTRA_INCLUDE_SDIRS "")
list(APPEND STMMGAMESFILE_EXTRA_INCLUDE_SDIRS "${STMMGAMES_INCLUDE_SDIRS}")

set(STMMI_TEMP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../libstmm-games-file/include")
set(STMMI_TEMP_HEADERS_DIR "${STMMI_TEMP_INCLUDE_DIR}/stmm-games-file")

set(        STMMGAMESFILE_INCLUDE_DIRS  "")
list(APPEND STMMGAMESFILE_INCLUDE_DIRS  "${STMMI_TEMP_INCLUDE_DIR}")
list(APPEND STMMGAMESFILE_INCLUDE_DIRS  "${STMMI_TEMP_HEADERS_DIR}")
list(APPEND STMMGAMESFILE_INCLUDE_DIRS  "${STMMGAMESFILE_EXTRA_INCLUDE_DIRS}")
set(        STMMGAMESFILE_INCLUDE_SDIRS "")
list(APPEND STMMGAMESFILE_INCLUDE_SDIRS "${STMMGAMESFILE_EXTRA_INCLUDE_SDIRS}")

# libs
set(        STMMI_TEMP_EXTERNAL_LIBRARIES     "")

set(        STMMGAMESFILE_EXTRA_LIBRARIES     "")
list(APPEND STMMGAMESFILE_EXTRA_LIBRARIES     "${STMMGAMES_LIBRARIES}")
list(APPEND STMMGAMESFILE_EXTRA_LIBRARIES     "${STMMI_TEMP_EXTERNAL_LIBRARIES}")

if (BUILD_SHARED_LIBS)
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-games-file/build/libstmm-games-file.so")
else()
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-games-file/build/libstmm-games-file.a")
endif()

set(        STMMGAMESFILE_LIBRARIES "")
list(APPEND STMMGAMESFILE_LIBRARIES "${STMMI_LIB_FILE}")
list(APPEND STMMGAMESFILE_LIBRARIES "${STMMGAMESFILE_EXTRA_LIBRARIES}")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    DefineAsSecondaryTarget(stmm-games-file  ${STMMI_LIB_FILE}  "${STMMGAMESFILE_INCLUDE_DIRS}"  "stmm-games" "${STMMI_TEMP_EXTERNAL_LIBRARIES}")
endif()
