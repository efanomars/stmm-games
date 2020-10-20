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

# File:   libstmm-games/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(stmm-games CXX)

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
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-games")
# Header files
set(STMMI_HEADERS_ANIMATIONS
        "${STMMI_HEADERS_DIR}/animations/backgroundanimation.h"
        "${STMMI_HEADERS_DIR}/animations/explosionanimation.h"
        "${STMMI_HEADERS_DIR}/animations/imagesequenceanimation.h"
        "${STMMI_HEADERS_DIR}/animations/staticgridanimation.h"
        "${STMMI_HEADERS_DIR}/animations/textanimation.h"
        )
set(STMMI_HEADERS_EVENTS
        "${STMMI_HEADERS_DIR}/events/alarmsevent.h"
        "${STMMI_HEADERS_DIR}/events/arrayevent.h"
        "${STMMI_HEADERS_DIR}/events/backgroundevent.h"
        "${STMMI_HEADERS_DIR}/events/cumulcmpevent.h"
        "${STMMI_HEADERS_DIR}/events/delayedqueueevent.h"
        "${STMMI_HEADERS_DIR}/events/logevent.h"
        "${STMMI_HEADERS_DIR}/events/othersevent.h"
        "${STMMI_HEADERS_DIR}/events/positionerevent.h"
        "${STMMI_HEADERS_DIR}/events/randomevent.h"
        "${STMMI_HEADERS_DIR}/events/scrollerevent.h"
        "${STMMI_HEADERS_DIR}/events/selectevent.h"
        "${STMMI_HEADERS_DIR}/events/showtextevent.h"
        "${STMMI_HEADERS_DIR}/events/soundevent.h"
        "${STMMI_HEADERS_DIR}/events/speedevent.h"
        "${STMMI_HEADERS_DIR}/events/staticgridevent.h"
        "${STMMI_HEADERS_DIR}/events/sysevent.h"
        "${STMMI_HEADERS_DIR}/events/tileanimatorevent.h"
        "${STMMI_HEADERS_DIR}/events/variableevent.h"
        )
set(STMMI_HEADERS_OPTIONS
        "${STMMI_HEADERS_DIR}/options/booloption.h"
        "${STMMI_HEADERS_DIR}/options/enumoption.h"
        "${STMMI_HEADERS_DIR}/options/intoption.h"
        )
set(STMMI_HEADERS_TRAITSETS
        "${STMMI_HEADERS_DIR}/traitsets/alphatraitset.h"
        "${STMMI_HEADERS_DIR}/traitsets/chartraitset.h"
        "${STMMI_HEADERS_DIR}/traitsets/colortraitset.h"
        "${STMMI_HEADERS_DIR}/traitsets/fonttraitset.h"
        "${STMMI_HEADERS_DIR}/traitsets/inttraitset.h"
        "${STMMI_HEADERS_DIR}/traitsets/notemptyinttraitset.h"
        "${STMMI_HEADERS_DIR}/traitsets/tiletraitsets.h"
        )
set(STMMI_HEADERS_UTIL
        "${STMMI_HEADERS_DIR}/util/basictypes.h"
        "${STMMI_HEADERS_DIR}/util/circularbuffer.h"
        "${STMMI_HEADERS_DIR}/util/coords.h"
        "${STMMI_HEADERS_DIR}/util/direction.h"
        "${STMMI_HEADERS_DIR}/util/helpers.h"
        "${STMMI_HEADERS_DIR}/util/intset.h"
        "${STMMI_HEADERS_DIR}/util/namedindex.h"
        "${STMMI_HEADERS_DIR}/util/namedobjindex.h"
        "${STMMI_HEADERS_DIR}/util/randomparts.h"
        "${STMMI_HEADERS_DIR}/util/recycler.h"
        "${STMMI_HEADERS_DIR}/util/util.h"
        "${STMMI_HEADERS_DIR}/util/variant.h"
        "${STMMI_HEADERS_DIR}/util/variantset.h"
        "${STMMI_HEADERS_DIR}/util/xybuffer.h"
        )
set(STMMI_HEADERS_UTILE
        "${STMMI_HEADERS_DIR}/utile/extendedboard.h"
        "${STMMI_HEADERS_DIR}/utile/newrows.h"
        "${STMMI_HEADERS_DIR}/utile/querytileremoval.h"
        "${STMMI_HEADERS_DIR}/utile/randomtiles.h"
        "${STMMI_HEADERS_DIR}/utile/tilebuffer.h"
        "${STMMI_HEADERS_DIR}/utile/tilerect.h"
        "${STMMI_HEADERS_DIR}/utile/tilecoords.h"
        "${STMMI_HEADERS_DIR}/utile/tileselector.h"
        )
set(STMMI_HEADERS_WIDGETS
        "${STMMI_HEADERS_DIR}/widgets/actionsboxwidget.h"
        "${STMMI_HEADERS_DIR}/widgets/actionwidget.h"
        "${STMMI_HEADERS_DIR}/widgets/backgroundwidget.h"
        "${STMMI_HEADERS_DIR}/widgets/boxwidget.h"
        "${STMMI_HEADERS_DIR}/widgets/inputboxwidget.h"
        "${STMMI_HEADERS_DIR}/widgets/levelshowwidget.h"
        "${STMMI_HEADERS_DIR}/widgets/previewwidget.h"
        "${STMMI_HEADERS_DIR}/widgets/progresswidget.h"
        "${STMMI_HEADERS_DIR}/widgets/transparentwidget.h"
        "${STMMI_HEADERS_DIR}/widgets/varwidget.h"
        "${STMMI_HEADERS_DIR}/widgets/volatilewidget.h"
        )
set(STMMI_HEADERS
        "${STMMI_HEADERS_DIR}/appconfig.h"
        "${STMMI_HEADERS_DIR}/appconstraints.h"
        "${STMMI_HEADERS_DIR}/apppreferences.h"
        "${STMMI_HEADERS_DIR}/block.h"
        "${STMMI_HEADERS_DIR}/containerwidget.h"
        "${STMMI_HEADERS_DIR}/event.h"
        "${STMMI_HEADERS_DIR}/game.h"
        "${STMMI_HEADERS_DIR}/gameowner.h"
        "${STMMI_HEADERS_DIR}/gameproxy.h"
        "${STMMI_HEADERS_DIR}/gamesound.h"
        "${STMMI_HEADERS_DIR}/gameview.h"
        "${STMMI_HEADERS_DIR}/gamewidget.h"
        "${STMMI_HEADERS_DIR}/gamewidgets.h"
        "${STMMI_HEADERS_DIR}/highscore.h"
        "${STMMI_HEADERS_DIR}/highscoresdefinition.h"
        "${STMMI_HEADERS_DIR}/keyactionevent.h"
        "${STMMI_HEADERS_DIR}/layout.h"
        "${STMMI_HEADERS_DIR}/level.h"
        "${STMMI_HEADERS_DIR}/levelanimation.h"
        "${STMMI_HEADERS_DIR}/levelanimations.h"
        "${STMMI_HEADERS_DIR}/levelblock.h"
        "${STMMI_HEADERS_DIR}/levellisteners.h"
        "${STMMI_HEADERS_DIR}/levelshow.h"
        "${STMMI_HEADERS_DIR}/levelview.h"
        "${STMMI_HEADERS_DIR}/named.h"
        "${STMMI_HEADERS_DIR}/option.h"
        "${STMMI_HEADERS_DIR}/ownertype.h"
        "${STMMI_HEADERS_DIR}/prefselector.h"
        "${STMMI_HEADERS_DIR}/private-listenerstk.h"
        "${STMMI_HEADERS_DIR}/randomsource.h"
        "${STMMI_HEADERS_DIR}/relsizedgamewidget.h"
        "${STMMI_HEADERS_DIR}/stdconfig.h"
        "${STMMI_HEADERS_DIR}/stdpreferences.h"
        "${STMMI_HEADERS_DIR}/stdrandomsource.h"
        "${STMMI_HEADERS_DIR}/stmm-games-config.h"
        "${STMMI_HEADERS_DIR}/tile.h"
        "${STMMI_HEADERS_DIR}/tileanimator.h"
        "${STMMI_HEADERS_DIR}/traitset.h"
        "${STMMI_HEADERS_DIR}/variable.h"
        )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
            #"${STMMI_SOURCES_DIR}/util/coords.cc"
            #"${STMMI_SOURCES_DIR}/utile/tilecoords.cc"

        "${STMMI_SOURCES_DIR}/animations/backgroundanimation.cc"
        "${STMMI_SOURCES_DIR}/animations/explosionanimation.cc"
        "${STMMI_SOURCES_DIR}/animations/imagesequenceanimation.cc"
        "${STMMI_SOURCES_DIR}/animations/staticgridanimation.cc"
        "${STMMI_SOURCES_DIR}/animations/textanimation.cc"
        #
        "${STMMI_SOURCES_DIR}/events/alarmsevent.cc"
        "${STMMI_SOURCES_DIR}/events/arrayevent.cc"
        "${STMMI_SOURCES_DIR}/events/backgroundevent.cc"
        "${STMMI_SOURCES_DIR}/events/cumulcmpevent.cc"
        "${STMMI_SOURCES_DIR}/events/delayedqueueevent.cc"
        "${STMMI_SOURCES_DIR}/events/logevent.cc"
        "${STMMI_SOURCES_DIR}/events/othersevent.cc"
        "${STMMI_SOURCES_DIR}/events/positionerevent.cc"
        "${STMMI_SOURCES_DIR}/events/randomevent.cc"
        "${STMMI_SOURCES_DIR}/events/scrollerevent.cc"
        "${STMMI_SOURCES_DIR}/events/selectevent.cc"
        "${STMMI_SOURCES_DIR}/events/showtextevent.cc"
        "${STMMI_SOURCES_DIR}/events/soundevent.cc"
        "${STMMI_SOURCES_DIR}/events/speedevent.cc"
        "${STMMI_SOURCES_DIR}/events/staticgridevent.cc"
        "${STMMI_SOURCES_DIR}/events/sysevent.cc"
        "${STMMI_SOURCES_DIR}/events/tileanimatorevent.cc"
        "${STMMI_SOURCES_DIR}/events/variableevent.cc"
        #
        "${STMMI_SOURCES_DIR}/options/booloption.cc"
        "${STMMI_SOURCES_DIR}/options/enumoption.cc"
        "${STMMI_SOURCES_DIR}/options/intoption.cc"
        #
        "${STMMI_SOURCES_DIR}/traitsets/alphatraitset.cc"
        "${STMMI_SOURCES_DIR}/traitsets/chartraitset.cc"
        "${STMMI_SOURCES_DIR}/traitsets/colortraitset.cc"
        "${STMMI_SOURCES_DIR}/traitsets/fonttraitset.cc"
        "${STMMI_SOURCES_DIR}/traitsets/inttraitset.cc"
        "${STMMI_SOURCES_DIR}/traitsets/notemptyinttraitset.cc"
        "${STMMI_SOURCES_DIR}/traitsets/tiletraitsets.cc"
        #
        "${STMMI_SOURCES_DIR}/util/basictypes.cc"
        "${STMMI_SOURCES_DIR}/util/circularbuffer.cc"
        "${STMMI_SOURCES_DIR}/util/coords.cc"
        "${STMMI_SOURCES_DIR}/util/direction.cc"
        "${STMMI_SOURCES_DIR}/util/helpers.cc"
        "${STMMI_SOURCES_DIR}/util/intset.cc"
        "${STMMI_SOURCES_DIR}/util/namedindex.cc"
        "${STMMI_SOURCES_DIR}/util/namedobjindex.cc"
        "${STMMI_SOURCES_DIR}/util/randomparts.cc"
        "${STMMI_SOURCES_DIR}/util/recycler.cc"
        "${STMMI_SOURCES_DIR}/util/util.cc"
        "${STMMI_SOURCES_DIR}/util/variant.cc"
        "${STMMI_SOURCES_DIR}/util/variantset.cc"
        "${STMMI_SOURCES_DIR}/util/xybuffer.cc"
        #
        "${STMMI_SOURCES_DIR}/utile/extendedboard.cc"
        "${STMMI_SOURCES_DIR}/utile/newrows.cc"
        "${STMMI_SOURCES_DIR}/utile/querytileremoval.cc"
        "${STMMI_SOURCES_DIR}/utile/randomtiles.cc"
        "${STMMI_SOURCES_DIR}/utile/tilebuffer.cc"
        "${STMMI_SOURCES_DIR}/utile/tilecoords.cc"
        "${STMMI_SOURCES_DIR}/utile/tilerect.cc"
        "${STMMI_SOURCES_DIR}/utile/tileselector.cc"
        #
        "${STMMI_SOURCES_DIR}/widgets/actionsboxwidget.cc"
        "${STMMI_SOURCES_DIR}/widgets/actionwidget.cc"
        "${STMMI_SOURCES_DIR}/widgets/backgroundwidget.cc"
        "${STMMI_SOURCES_DIR}/widgets/boxwidget.cc"
        "${STMMI_SOURCES_DIR}/widgets/inputboxwidget.cc"
        "${STMMI_SOURCES_DIR}/widgets/levelshowwidget.cc"
        "${STMMI_SOURCES_DIR}/widgets/previewwidget.cc"
        "${STMMI_SOURCES_DIR}/widgets/progresswidget.cc"
        "${STMMI_SOURCES_DIR}/widgets/transparentwidget.cc"
        "${STMMI_SOURCES_DIR}/widgets/varwidget.cc"
        "${STMMI_SOURCES_DIR}/widgets/volatilewidget.cc"
        #
        "${STMMI_SOURCES_DIR}/appconfig.cc"
        "${STMMI_SOURCES_DIR}/appconstraints.cc"
        "${STMMI_SOURCES_DIR}/apppreferences.cc"
        "${STMMI_SOURCES_DIR}/block.cc"
        "${STMMI_SOURCES_DIR}/containerwidget.cc"
        "${STMMI_SOURCES_DIR}/event.cc"
        "${STMMI_SOURCES_DIR}/game.cc"
        "${STMMI_SOURCES_DIR}/gameproxy.cc"
        "${STMMI_SOURCES_DIR}/gamesound.cc"
        "${STMMI_SOURCES_DIR}/gameview.cc"
        "${STMMI_SOURCES_DIR}/gameowner.cc"
        "${STMMI_SOURCES_DIR}/gamewidget.cc"
        "${STMMI_SOURCES_DIR}/gamewidgets.cc"
        "${STMMI_SOURCES_DIR}/highscore.cc"
        "${STMMI_SOURCES_DIR}/highscoresdefinition.cc"
        "${STMMI_SOURCES_DIR}/keyactionevent.cc"
        "${STMMI_SOURCES_DIR}/layout.cc"
        "${STMMI_SOURCES_DIR}/level.cc"
        "${STMMI_SOURCES_DIR}/levelanimation.cc"
        "${STMMI_SOURCES_DIR}/levelanimations.cc"
        "${STMMI_SOURCES_DIR}/levelblock.cc"
        "${STMMI_SOURCES_DIR}/levellisteners.cc"
        "${STMMI_SOURCES_DIR}/levelshow.cc"
        "${STMMI_SOURCES_DIR}/levelview.cc"
        "${STMMI_SOURCES_DIR}/named.cc"
        "${STMMI_SOURCES_DIR}/option.cc"
        "${STMMI_SOURCES_DIR}/ownertype.cc"
        "${STMMI_SOURCES_DIR}/randomsource.cc"
        "${STMMI_SOURCES_DIR}/prefselector.cc"
        "${STMMI_SOURCES_DIR}/private-listenerstk.cc"
        "${STMMI_SOURCES_DIR}/relsizedgamewidget.cc"
        "${STMMI_SOURCES_DIR}/stdconfig.cc"
        "${STMMI_SOURCES_DIR}/stdpreferences.cc"
        "${STMMI_SOURCES_DIR}/stdrandomsource.cc"
        "${STMMI_SOURCES_DIR}/tile.cc"
        "${STMMI_SOURCES_DIR}/tileanimator.cc"
        "${STMMI_SOURCES_DIR}/traitset.cc"
        "${STMMI_SOURCES_DIR}/variable.cc"
        )

# Define library
add_library(stmm-games ${STMMI_SOURCES} "${PROJECT_BINARY_DIR}/stmm-games-config.cc")

include("stmm-games-defs.cmake")

target_include_directories(stmm-games        PUBLIC ${STMMGAMES_EXTRA_INCLUDE_DIRS})
target_include_directories(stmm-games SYSTEM PUBLIC ${STMMGAMES_EXTRA_INCLUDE_SDIRS})
target_include_directories(stmm-games BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-games>  # <prefix>/include/stmm-games
)

target_link_libraries(stmm-games ${STMMGAMES_EXTRA_LIBRARIES})

set_target_properties(stmm-games PROPERTIES ${CMAKE_BUILD_TYPE}_POSTFIX "")
set_target_properties(stmm-games PROPERTIES
        VERSION ${STMM_GAMES_VERSION}
        SOVERSION ${STMM_GAMES_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-games-${STMM_GAMES_VERSION}"
        )

DefineTargetPublicCompileOptions(stmm-games)

# Set version for stmm-games-config.cc.in
set(STMMI_PKG_VERSION "${STMM_GAMES_VERSION}")
# Create config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-games-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-games-config.cc" @ONLY)

include(FindPkgConfig)
if (PKG_CONFIG_FOUND)
    # Set install dir for pkg-config file
    set(STMMI_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
    # Create pkg-config file for the library
    configure_file("${PROJECT_SOURCE_DIR}/stmm-games.pc.in"
                   "${PROJECT_BINARY_DIR}/stmm-games.pc" @ONLY)
endif()

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-games was configured with the following options:")
message(STATUS " STMMI_HEADERS:                 ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                 ${STMMI_SOURCES}")
message(STATUS " STMMGAMES_EXTRA_INCLUDE_DIRS:  ${STMMGAMES_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMGAMES_EXTRA_INCLUDE_SDIRS: ${STMMGAMES_EXTRA_INCLUDE_SDIRS}")
message(STATUS " STMMGAMES_EXTRA_LIBRARIES:     ${STMMGAMES_EXTRA_LIBRARIES}")
message(STATUS " CMAKE_BUILD_TYPE:              ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:         ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:    ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:               ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                ${CMAKE_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                    ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                 ${BUILD_TESTING}")
endif()

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-games doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)


install(TARGETS stmm-games LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS}            DESTINATION "include/stmm-games")
install(FILES ${STMMI_HEADERS_ANIMATIONS} DESTINATION "include/stmm-games/animations")
install(FILES ${STMMI_HEADERS_EVENTS}     DESTINATION "include/stmm-games/events")
install(FILES ${STMMI_HEADERS_OPTIONS}    DESTINATION "include/stmm-games/options")
install(FILES ${STMMI_HEADERS_TRAITSETS}  DESTINATION "include/stmm-games/traitsets")
install(FILES ${STMMI_HEADERS_UTIL}       DESTINATION "include/stmm-games/util")
install(FILES ${STMMI_HEADERS_UTILE}      DESTINATION "include/stmm-games/utile")
install(FILES ${STMMI_HEADERS_WIDGETS}    DESTINATION "include/stmm-games/widgets")
if (PKG_CONFIG_FOUND)
    install(FILES  "${PROJECT_BINARY_DIR}/stmm-games.pc"   DESTINATION "lib/pkgconfig")
endif()
