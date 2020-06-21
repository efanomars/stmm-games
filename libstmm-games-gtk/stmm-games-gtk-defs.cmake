# File: libstmm-games-gtk/stmm-games-gtk-defs.cmake

#  Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
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

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_GAMES_GTK_MAJOR_VERSION 0)
set(STMM_GAMES_GTK_MINOR_VERSION 25) # !-U-!
set(STMM_GAMES_GTK_VERSION "${STMM_GAMES_GTK_MAJOR_VERSION}.${STMM_GAMES_GTK_MINOR_VERSION}.0")

set(STMM_GAMES_GTK_REQ_STMM_GAMES_FILE_MAJOR_VERSION 0)
set(STMM_GAMES_GTK_REQ_STMM_GAMES_FILE_MINOR_VERSION 25) # !-U-!
set(STMM_GAMES_GTK_REQ_STMM_GAMES_FILE_VERSION "${STMM_GAMES_GTK_REQ_STMM_GAMES_FILE_MAJOR_VERSION}.${STMM_GAMES_GTK_REQ_STMM_GAMES_FILE_MINOR_VERSION}")

set(STMM_GAMES_GTK_REQ_STMM_INPUT_GTK_MAJOR_VERSION 0)
set(STMM_GAMES_GTK_REQ_STMM_INPUT_GTK_MINOR_VERSION 14) # !-U-!
set(STMM_GAMES_GTK_REQ_STMM_INPUT_GTK_VERSION "${STMM_GAMES_GTK_REQ_STMM_INPUT_GTK_MAJOR_VERSION}.${STMM_GAMES_GTK_REQ_STMM_INPUT_GTK_MINOR_VERSION}")

set(STMM_GAMES_GTK_REQ_GTKMM_VERSION "3.22")

set(STMM_GAMES_GTK_REQ_LIBRSVG_VERSION "2.40.5")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    include(FindPkgConfig)
    if (NOT PKG_CONFIG_FOUND)
        message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
    endif()
    # Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
    pkg_check_modules(STMMINPUTGTK  REQUIRED  stmm-input-gtk>=${STMM_GAMES_GTK_REQ_STMM_INPUT_GTK_VERSION})
    pkg_check_modules(GTKMM         REQUIRED  gtkmm-3.0>=${STMM_GAMES_GTK_REQ_GTKMM_VERSION})
    pkg_check_modules(LIBRSVG       REQUIRED  librsvg-2.0>=${STMM_GAMES_GTK_REQ_LIBRSVG_VERSION})
endif()

include("${PROJECT_SOURCE_DIR}/../libstmm-games-file/stmm-games-file-defs.cmake")

# include dirs
set(        STMMGAMESGTK_EXTRA_INCLUDE_DIRS  "")
list(APPEND STMMGAMESGTK_EXTRA_INCLUDE_DIRS  "${STMMGAMESFILE_INCLUDE_DIRS}")
set(        STMMGAMESGTK_EXTRA_INCLUDE_SDIRS "")
list(APPEND STMMGAMESGTK_EXTRA_INCLUDE_SDIRS "${STMMGAMESFILE_INCLUDE_SDIRS}")
list(APPEND STMMGAMESGTK_EXTRA_INCLUDE_SDIRS "${STMMINPUTGTK_INCLUDE_DIRS}")
list(APPEND STMMGAMESGTK_EXTRA_INCLUDE_SDIRS "${GTKMM_INCLUDE_DIRS}")
list(APPEND STMMGAMESGTK_EXTRA_INCLUDE_SDIRS "${LIBRSVG_INCLUDE_DIRS}")

set(STMMI_TEMP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../libstmm-games-gtk/include")
set(STMMI_TEMP_HEADERS_DIR "${STMMI_TEMP_INCLUDE_DIR}/stmm-games-gtk")

set(        STMMGAMESGTK_INCLUDE_DIRS  "")
list(APPEND STMMGAMESGTK_INCLUDE_DIRS  "${STMMI_TEMP_INCLUDE_DIR}")
list(APPEND STMMGAMESGTK_INCLUDE_DIRS  "${STMMI_TEMP_HEADERS_DIR}")
list(APPEND STMMGAMESGTK_INCLUDE_DIRS  "${STMMGAMESGTK_EXTRA_INCLUDE_DIRS}")
set(        STMMGAMESGTK_INCLUDE_SDIRS "")
list(APPEND STMMGAMESGTK_INCLUDE_SDIRS "${STMMGAMESGTK_EXTRA_INCLUDE_SDIRS}")

# libs
set(        STMMI_TEMP_EXTERNAL_LIBRARIES    "")
list(APPEND STMMI_TEMP_EXTERNAL_LIBRARIES    "${STMMINPUTGTK_LIBRARIES}")
list(APPEND STMMI_TEMP_EXTERNAL_LIBRARIES    "${GTKMM_LIBRARIES}")
list(APPEND STMMI_TEMP_EXTERNAL_LIBRARIES    "${LIBRSVG_LIBRARIES}")

set(        STMMGAMESGTK_EXTRA_LIBRARIES     "")
list(APPEND STMMGAMESGTK_EXTRA_LIBRARIES     "${STMMGAMESFILE_LIBRARIES}")
list(APPEND STMMGAMESGTK_EXTRA_LIBRARIES     "${STMMI_TEMP_EXTERNAL_LIBRARIES}")

if (BUILD_SHARED_LIBS)
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-games-gtk/build/libstmm-games-gtk.so")
else()
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-games-gtk/build/libstmm-games-gtk.a")
endif()

set(        STMMGAMESGTK_LIBRARIES "")
list(APPEND STMMGAMESGTK_LIBRARIES "${STMMI_LIB_FILE}")
list(APPEND STMMGAMESGTK_LIBRARIES "${STMMGAMESGTK_EXTRA_LIBRARIES}")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    DefineAsSecondaryTarget(stmm-games-gtk  ${STMMI_LIB_FILE}  "${STMMGAMESGTK_INCLUDE_DIRS}"  "stmm-games-file" "${STMMI_TEMP_EXTERNAL_LIBRARIES}")
endif()
