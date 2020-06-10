# File: libstmm-games-xml-game/stmm-games-xml-game-defs.cmake

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

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_GAMES_XML_GAME_MAJOR_VERSION 0)
set(STMM_GAMES_XML_GAME_MINOR_VERSION 24) # !-U-!
set(STMM_GAMES_XML_GAME_VERSION "${STMM_GAMES_XML_GAME_MAJOR_VERSION}.${STMM_GAMES_XML_GAME_MINOR_VERSION}.0")

# required stmm-games-gtk version
set(STMM_GAMES_XML_GAME_REQ_STMM_GAMES_XML_BASE_MAJOR_VERSION 0)
set(STMM_GAMES_XML_GAME_REQ_STMM_GAMES_XML_BASE_MINOR_VERSION 24) # !-U-!
set(STMM_GAMES_XML_GAME_REQ_STMM_GAMES_XML_BASE_VERSION "${STMM_GAMES_XML_GAME_REQ_STMM_GAMES_XML_BASE_MAJOR_VERSION}.${STMM_GAMES_XML_GAME_REQ_STMM_GAMES_XML_BASE_MINOR_VERSION}")

# required stmm-games-gtk version
set(STMM_GAMES_XML_GAME_REQ_STMM_GAMES_FILE_MAJOR_VERSION 0)
set(STMM_GAMES_XML_GAME_REQ_STMM_GAMES_FILE_MINOR_VERSION 24) # !-U-!
set(STMM_GAMES_XML_GAME_REQ_STMM_GAMES_FILE_VERSION "${STMM_GAMES_XML_GAME_REQ_STMM_GAMES_FILE_MAJOR_VERSION}.${STMM_GAMES_XML_GAME_REQ_STMM_GAMES_FILE_MINOR_VERSION}")

#if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
#    include(FindPkgConfig)
#    if (NOT PKG_CONFIG_FOUND)
#        message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
#    endif()
#    # Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
#endif()

include("${PROJECT_SOURCE_DIR}/../libstmm-games-file/stmm-games-file-defs.cmake")
include("${PROJECT_SOURCE_DIR}/../libstmm-games-xml-base/stmm-games-xml-base-defs.cmake")

# include dirs
set(        STMMGAMESXMLGAME_EXTRA_INCLUDE_DIRS  "")
list(APPEND STMMGAMESXMLGAME_EXTRA_INCLUDE_DIRS  "${STMMGAMESXMLBASE_INCLUDE_DIRS}")
list(APPEND STMMGAMESXMLGAME_EXTRA_INCLUDE_DIRS  "${STMMGAMESFILE_INCLUDE_DIRS}")
set(        STMMGAMESXMLGAME_EXTRA_INCLUDE_SDIRS "")
list(APPEND STMMGAMESXMLGAME_EXTRA_INCLUDE_SDIRS "${STMMGAMESXMLBASE_INCLUDE_SDIRS}")
list(APPEND STMMGAMESXMLGAME_EXTRA_INCLUDE_SDIRS "${STMMGAMESFILE_INCLUDE_SDIRS}")

set(STMMI_TEMP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../libstmm-games-xml-game/include")
set(STMMI_TEMP_HEADERS_DIR "${STMMI_TEMP_INCLUDE_DIR}/stmm-games-xml-game")

set(        STMMGAMESXMLGAME_INCLUDE_DIRS  "")
list(APPEND STMMGAMESXMLGAME_INCLUDE_DIRS  "${STMMI_TEMP_INCLUDE_DIR}")
list(APPEND STMMGAMESXMLGAME_INCLUDE_DIRS  "${STMMI_TEMP_HEADERS_DIR}")
list(APPEND STMMGAMESXMLGAME_INCLUDE_DIRS  "${STMMGAMESXMLGAME_EXTRA_INCLUDE_DIRS}")
set(        STMMGAMESXMLGAME_INCLUDE_SDIRS "")
list(APPEND STMMGAMESXMLGAME_INCLUDE_SDIRS "${STMMGAMESXMLGAME_EXTRA_INCLUDE_SDIRS}")

# libs
set(        STMMI_TEMP_EXTERNAL_LIBRARIES    "")

set(        STMMGAMESXMLGAME_EXTRA_LIBRARIES     "")
list(APPEND STMMGAMESXMLGAME_EXTRA_LIBRARIES     "${STMMGAMESXMLBASE_LIBRARIES}")
list(APPEND STMMGAMESXMLGAME_EXTRA_LIBRARIES     "${STMMGAMESFILE_LIBRARIES}")
list(APPEND STMMGAMESXMLGAME_EXTRA_LIBRARIES     "${STMMI_TEMP_EXTERNAL_LIBRARIES}")

if (BUILD_SHARED_LIBS)
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-games-xml-game/build/libstmm-games-xml-game.so")
else()
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-games-xml-game/build/libstmm-games-xml-game.a")
endif()

set(        STMMGAMESXMLGAME_LIBRARIES "")
list(APPEND STMMGAMESXMLGAME_LIBRARIES "${STMMI_LIB_FILE}")
list(APPEND STMMGAMESXMLGAME_LIBRARIES "${STMMGAMESXMLGAME_EXTRA_LIBRARIES}")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    DefineAsSecondaryTarget(stmm-games-xml-game  ${STMMI_LIB_FILE}  "${STMMGAMESXMLGAME_INCLUDE_DIRS}"  "stmm-games-xml-base;stmm-games-file" "${STMMI_TEMP_EXTERNAL_LIBRARIES}")
endif()

#if (("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "") AND NOT TARGET stmm-games-xml-game)
#    if (BUILD_SHARED_LIBS)
#        add_library(stmm-games-xml-game SHARED IMPORTED)
#    else()
#        add_library(stmm-games-xml-game STATIC IMPORTED)
#    endif()
#    set_target_properties(stmm-games-xml-game PROPERTIES IMPORTED_LOCATION             "${STMMI_LIB_FILE}")
#    set_target_properties(stmm-games-xml-game PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${STMMGAMESXMLGAME_INCLUDE_DIRS}")
#    set(STMMI_TEMP_PROPS "")
#    get_target_property(STMMI_TEMP_GAMESGTK_PROP stmm-games-gtk INTERFACE_LINK_LIBRARIES)
#    list(APPEND STMMI_TEMP_PROPS "${STMMI_TEMP_GAMESGTK_PROP}")
#    list(APPEND STMMI_TEMP_PROPS "stmm-games-xml-base")
#    list(APPEND STMMI_TEMP_PROPS "stmm-games-file")
#    set_target_properties(stmm-games-xml-game PROPERTIES INTERFACE_LINK_LIBRARIES      "${STMMI_TEMP_PROPS}")
#endif()
