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

# File:   stmm-games-xml-gtk-defs.cmake

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_GAMES_XML_GTK_MAJOR_VERSION 0)
set(STMM_GAMES_XML_GTK_MINOR_VERSION 26) # !-U-!
set(STMM_GAMES_XML_GTK_VERSION "${STMM_GAMES_XML_GTK_MAJOR_VERSION}.${STMM_GAMES_XML_GTK_MINOR_VERSION}.0")

# required stmm-games-gtk version
set(STMM_GAMES_XML_GTK_REQ_STMM_GAMES_GTK_MAJOR_VERSION 0)
set(STMM_GAMES_XML_GTK_REQ_STMM_GAMES_GTK_MINOR_VERSION 26) # !-U-!
set(STMM_GAMES_XML_GTK_REQ_STMM_GAMES_GTK_VERSION "${STMM_GAMES_XML_GTK_REQ_STMM_GAMES_GTK_MAJOR_VERSION}.${STMM_GAMES_XML_GTK_REQ_STMM_GAMES_GTK_MINOR_VERSION}")

# required stmm-games-gtk version
set(STMM_GAMES_XML_GTK_REQ_STMM_GAMES_XML_GAME_MAJOR_VERSION 0)
set(STMM_GAMES_XML_GTK_REQ_STMM_GAMES_XML_GAME_MINOR_VERSION 26) # !-U-!
set(STMM_GAMES_XML_GTK_REQ_STMM_GAMES_XML_GAME_VERSION "${STMM_GAMES_XML_GTK_REQ_STMM_GAMES_XML_GAME_MAJOR_VERSION}.${STMM_GAMES_XML_GTK_REQ_STMM_GAMES_XML_GAME_MINOR_VERSION}")

# if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
#     include(FindPkgConfig)
#     if (NOT PKG_CONFIG_FOUND)
#         message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
#     endif()
#     # Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
# endif()

include("${PROJECT_SOURCE_DIR}/../libstmm-games-gtk/stmm-games-gtk-defs.cmake")
include("${PROJECT_SOURCE_DIR}/../libstmm-games-xml-game/stmm-games-xml-game-defs.cmake")

# include dirs
set(        STMMGAMESXMLGTK_EXTRA_INCLUDE_DIRS  "")
list(APPEND STMMGAMESXMLGTK_EXTRA_INCLUDE_DIRS  "${STMMGAMESGTK_INCLUDE_DIRS}")
list(APPEND STMMGAMESXMLGTK_EXTRA_INCLUDE_DIRS  "${STMMGAMESXMLGAME_INCLUDE_DIRS}")
set(        STMMGAMESXMLGTK_EXTRA_INCLUDE_SDIRS "")
list(APPEND STMMGAMESXMLGTK_EXTRA_INCLUDE_SDIRS "${STMMGAMESGTK_INCLUDE_SDIRS}")
list(APPEND STMMGAMESXMLGTK_EXTRA_INCLUDE_SDIRS "${STMMGAMESXMLGAME_INCLUDE_SDIRS}")

set(STMMI_TEMP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../libstmm-games-xml-gtk/include")
set(STMMI_TEMP_HEADERS_DIR "${STMMI_TEMP_INCLUDE_DIR}/stmm-games-xml-gtk")

set(        STMMGAMESXMLGTK_INCLUDE_DIRS  "")
list(APPEND STMMGAMESXMLGTK_INCLUDE_DIRS  "${STMMI_TEMP_INCLUDE_DIR}")
list(APPEND STMMGAMESXMLGTK_INCLUDE_DIRS  "${STMMI_TEMP_HEADERS_DIR}")
list(APPEND STMMGAMESXMLGTK_INCLUDE_DIRS  "${STMMGAMESXMLGTK_EXTRA_INCLUDE_DIRS}")
set(        STMMGAMESXMLGTK_INCLUDE_SDIRS "")
list(APPEND STMMGAMESXMLGTK_INCLUDE_SDIRS "${STMMGAMESXMLGTK_EXTRA_INCLUDE_SDIRS}")

# libs
set(        STMMI_TEMP_EXTERNAL_LIBRARIES    "")

set(        STMMGAMESXMLGTK_EXTRA_LIBRARIES      "")
list(APPEND STMMGAMESXMLGTK_EXTRA_LIBRARIES     "${STMMGAMESGTK_LIBRARIES}")
list(APPEND STMMGAMESXMLGTK_EXTRA_LIBRARIES     "${STMMGAMESXMLGAME_LIBRARIES}")
list(APPEND STMMGAMESXMLGTK_EXTRA_LIBRARIES     "${STMMI_TEMP_EXTERNAL_LIBRARIES}")

if (BUILD_SHARED_LIBS)
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-games-xml-gtk/build/libstmm-games-xml-gtk.so")
else()
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-games-xml-gtk/build/libstmm-games-xml-gtk.a")
endif()

set(        STMMGAMESXMLGTK_LIBRARIES "")
list(APPEND STMMGAMESXMLGTK_LIBRARIES "${STMMI_LIB_FILE}")
list(APPEND STMMGAMESXMLGTK_LIBRARIES "${STMMGAMESXMLGTK_EXTRA_LIBRARIES}")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    DefineAsSecondaryTarget(stmm-games-xml-gtk  ${STMMI_LIB_FILE}  "${STMMGAMESXMLGTK_INCLUDE_DIRS}"  "stmm-games-xml-game;stmm-games-gtk" "${STMMI_TEMP_EXTERNAL_LIBRARIES}")
endif()
