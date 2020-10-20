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

# File:   libstmm-games-xml-game/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(stmm-games-xml-game CXX)

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
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-games-xml-game")
# Header files
set(STMMI_HEADERS_EVENTS
        "${STMMI_HEADERS_DIR}/events/xmlalarmsevent.h"
        "${STMMI_HEADERS_DIR}/events/xmlarrayevent.h"
        "${STMMI_HEADERS_DIR}/events/xmlbackgroundevent.h"
        "${STMMI_HEADERS_DIR}/events/xmlcumulcmpevent.h"
        "${STMMI_HEADERS_DIR}/events/xmldelayedqueueevent.h"
        "${STMMI_HEADERS_DIR}/events/xmllogevent.h"
        "${STMMI_HEADERS_DIR}/events/xmlothersevent.h"
        "${STMMI_HEADERS_DIR}/events/xmlpositionerevent.h"
        "${STMMI_HEADERS_DIR}/events/xmlrandomevent.h"
        "${STMMI_HEADERS_DIR}/events/xmlscrollerevent.h"
        "${STMMI_HEADERS_DIR}/events/xmlselectevent.h"
        "${STMMI_HEADERS_DIR}/events/xmlshowtextevent.h"
        "${STMMI_HEADERS_DIR}/events/xmlsoundevent.h"
        "${STMMI_HEADERS_DIR}/events/xmlspeedevent.h"
        "${STMMI_HEADERS_DIR}/events/xmlstaticgridevent.h"
        "${STMMI_HEADERS_DIR}/events/xmlsysevent.h"
        "${STMMI_HEADERS_DIR}/events/xmltileanimatorevent.h"
        "${STMMI_HEADERS_DIR}/events/xmlvariableevent.h"
        )
#
set(STMMI_HEADERS_GAME_WIDGETS
        "${STMMI_HEADERS_DIR}/gamewidgets/xmlactionbasewidget.h"
        "${STMMI_HEADERS_DIR}/gamewidgets/xmlactionsboxwidget.h"
        "${STMMI_HEADERS_DIR}/gamewidgets/xmlactionwidget.h"
        "${STMMI_HEADERS_DIR}/gamewidgets/xmlbackgroundwidget.h"
        "${STMMI_HEADERS_DIR}/gamewidgets/xmlboxwidget.h"
        "${STMMI_HEADERS_DIR}/gamewidgets/xmlinputboxwidget.h"
        "${STMMI_HEADERS_DIR}/gamewidgets/xmllevelshowwidget.h"
        "${STMMI_HEADERS_DIR}/gamewidgets/xmlpreviewwidget.h"
        "${STMMI_HEADERS_DIR}/gamewidgets/xmlprogresswidget.h"
        "${STMMI_HEADERS_DIR}/gamewidgets/xmltransparentwidget.h"
        "${STMMI_HEADERS_DIR}/gamewidgets/xmlvarwidget.h"
        "${STMMI_HEADERS_DIR}/gamewidgets/xmlvolatilewidget.h"
        )
#
set(STMMI_HEADERS_XMLUTILE
        "${STMMI_HEADERS_DIR}/xmlutile/xmlidtilesparser.h"
        "${STMMI_HEADERS_DIR}/xmlutile/xmlnewrowsparser.h"
        "${STMMI_HEADERS_DIR}/xmlutile/xmlprobtilegenparser.h"
        )
#
set(STMMI_HEADERS
        "${STMMI_HEADERS_DIR}/gamectx.h"
        "${STMMI_HEADERS_DIR}/gameinfoctx.h"
        "${STMMI_HEADERS_DIR}/layoutctx.h"
        "${STMMI_HEADERS_DIR}/stmm-games-xml-game-config.h"
        "${STMMI_HEADERS_DIR}/xmleventparser.h"
        "${STMMI_HEADERS_DIR}/xmlgamefiles.h"
        "${STMMI_HEADERS_DIR}/xmlgameloader.h"
        "${STMMI_HEADERS_DIR}/xmlgamewidgetparser.h"
        "${STMMI_HEADERS_DIR}/xmlhighscoresloader.h"
        "${STMMI_HEADERS_DIR}/xmlpreferencesloader.h"
        "${STMMI_HEADERS_DIR}/xmlstdeventparsers.h"
        "${STMMI_HEADERS_DIR}/xmlstdgamewidgetparsers.h"
        )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
            #"${STMMI_SOURCES_DIR}/xmlvariantsetparser.cc"
        #
        "${STMMI_SOURCES_DIR}/events/xmlalarmsevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmlarrayevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmlbackgroundevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmlcumulcmpevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmldelayedqueueevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmllogevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmlothersevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmlpositionerevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmlrandomevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmlscrollerevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmlselectevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmlshowtextevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmlsoundevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmlspeedevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmlstaticgridevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmlsysevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmltileanimatorevent.cc"
        "${STMMI_SOURCES_DIR}/events/xmlvariableevent.cc"
        #
        "${STMMI_SOURCES_DIR}/gamewidgets/xmlactionbasewidget.cc"
        "${STMMI_SOURCES_DIR}/gamewidgets/xmlactionsboxwidget.cc"
        "${STMMI_SOURCES_DIR}/gamewidgets/xmlactionwidget.cc"
        "${STMMI_SOURCES_DIR}/gamewidgets/xmlbackgroundwidget.cc"
        "${STMMI_SOURCES_DIR}/gamewidgets/xmlboxwidget.cc"
        "${STMMI_SOURCES_DIR}/gamewidgets/xmlinputboxwidget.cc"
        "${STMMI_SOURCES_DIR}/gamewidgets/xmllevelshowwidget.cc"
        "${STMMI_SOURCES_DIR}/gamewidgets/xmlpreviewwidget.cc"
        "${STMMI_SOURCES_DIR}/gamewidgets/xmlprogresswidget.cc"
        "${STMMI_SOURCES_DIR}/gamewidgets/xmltransparentwidget.cc"
        "${STMMI_SOURCES_DIR}/gamewidgets/xmlvarwidget.cc"
        "${STMMI_SOURCES_DIR}/gamewidgets/xmlvolatilewidget.cc"
        #
        "${STMMI_SOURCES_DIR}/xmlutile/xmlidtilesparser.cc"
        "${STMMI_SOURCES_DIR}/xmlutile/xmlnewrowsparser.cc"
        "${STMMI_SOURCES_DIR}/xmlutile/xmlprobtilegenparser.cc"
        #
        "${STMMI_SOURCES_DIR}/gamectx.cc"
        "${STMMI_SOURCES_DIR}/gameinfoctx.cc"
        "${STMMI_SOURCES_DIR}/gameinitctx.h"
        "${STMMI_SOURCES_DIR}/gameinitctx.cc"
        "${STMMI_SOURCES_DIR}/layoutctx.cc"
        "${STMMI_SOURCES_DIR}/xmlblockparser.h"
        "${STMMI_SOURCES_DIR}/xmlblockparser.cc"
        "${STMMI_SOURCES_DIR}/xmleventparser.cc"
        "${STMMI_SOURCES_DIR}/xmlgamefiles.cc"
        "${STMMI_SOURCES_DIR}/xmlgameinfoparser.h"
        "${STMMI_SOURCES_DIR}/xmlgameinfoparser.cc"
        "${STMMI_SOURCES_DIR}/xmlgameinitparser.h"
        "${STMMI_SOURCES_DIR}/xmlgameinitparser.cc"
        "${STMMI_SOURCES_DIR}/xmlgameloader.cc"
        "${STMMI_SOURCES_DIR}/xmlgameparser.h"
        "${STMMI_SOURCES_DIR}/xmlgameparser.cc"
        "${STMMI_SOURCES_DIR}/xmlgamewidgetparser.cc"
        "${STMMI_SOURCES_DIR}/xmlhighscoresloader.cc"
        "${STMMI_SOURCES_DIR}/xmllayoutparser.h"
        "${STMMI_SOURCES_DIR}/xmllayoutparser.cc"
        "${STMMI_SOURCES_DIR}/xmlpreferencesloader.cc"
        "${STMMI_SOURCES_DIR}/xmlstdeventparsers.cc"
        "${STMMI_SOURCES_DIR}/xmlstdgamewidgetparsers.cc"
        "${STMMI_SOURCES_DIR}/xmlutilfile.h"
        "${STMMI_SOURCES_DIR}/xmlutilfile.cc"
        )


# Define library
add_library(stmm-games-xml-game ${STMMI_SOURCES} "${PROJECT_BINARY_DIR}/stmm-games-xml-game-config.cc")

include("stmm-games-xml-game-defs.cmake")

target_include_directories(stmm-games-xml-game        PUBLIC ${STMMGAMESXMLGAME_EXTRA_INCLUDE_DIRS})
target_include_directories(stmm-games-xml-game SYSTEM PUBLIC ${STMMGAMESXMLGAME_EXTRA_INCLUDE_SDIRS})
target_include_directories(stmm-games-xml-game BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-games-xml-game>  # <prefix>/include/stmm-games-xml-game
)

target_link_libraries(stmm-games-xml-game ${STMMGAMESXMLGAME_EXTRA_LIBRARIES})

set_target_properties(stmm-games-xml-game PROPERTIES ${CMAKE_BUILD_TYPE}_POSTFIX "")
set_target_properties(stmm-games-xml-game PROPERTIES
        VERSION ${STMM_GAMES_XML_GAME_VERSION}
        SOVERSION ${STMM_GAMES_XML_GAME_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-games-xml-game-${STMM_GAMES_XML_GAME_VERSION}"
        )

DefineTargetPublicCompileOptions(stmm-games-xml-game)

# Set version and dirs for stmm-games-xml-game-config.cc.in
set(STMMI_PKG_VERSION "${STMM_GAMES_XML_GAME_VERSION}")
## Create config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-games-xml-game-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-games-xml-game-config.cc" @ONLY)

# Set install dir for pkg-config file
set(STMMI_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
# Create pkg-config file for the library
configure_file("${PROJECT_SOURCE_DIR}/stmm-games-xml-game.pc.in"
               "${PROJECT_BINARY_DIR}/stmm-games-xml-game.pc" @ONLY)

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-games-xml-game was configured with the following options:")
message(STATUS " STMMI_HEADERS:                        ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                        ${STMMI_SOURCES}")
message(STATUS " STMMGAMESXMLGAME_EXTRA_INCLUDE_DIRS:  ${STMMGAMESXMLGAME_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMGAMESXMLGAME_EXTRA_INCLUDE_SDIRS: ${STMMGAMESXMLGAME_EXTRA_INCLUDE_SDIRS}")
message(STATUS " STMMGAMESXMLGAME_EXTRA_LIBRARIES:     ${STMMGAMESXMLGAME_EXTRA_LIBRARIES}")
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
    add_dependencies(stmm-games-xml-game doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)

install(TARGETS stmm-games-xml-game LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS}                  DESTINATION "include/stmm-games-xml-game")
install(FILES ${STMMI_HEADERS_EVENTS}           DESTINATION "include/stmm-games-xml-game/events")
install(FILES ${STMMI_HEADERS_GAME_WIDGETS}     DESTINATION "include/stmm-games-xml-game/gamewidgets")
install(FILES ${STMMI_HEADERS_XMLUTILE}         DESTINATION "include/stmm-games-xml-game/xmlutile")

install(FILES  "${PROJECT_BINARY_DIR}/stmm-games-xml-game.pc"   DESTINATION "lib/pkgconfig")
