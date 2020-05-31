# File: libstmm-games-gtk/CMakeLists.txt

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

cmake_minimum_required(VERSION 3.0)

project(stmm-games-gtk CXX)

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
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-games-gtk")
# Header files
set(STMMI_HEADERS_ANIMATIONS
        "${STMMI_HEADERS_DIR}/animations/backgroundthanifactory.h"
        "${STMMI_HEADERS_DIR}/animations/explosionthanifactory.h"
        "${STMMI_HEADERS_DIR}/animations/imagesequencethanifactory.h"
        "${STMMI_HEADERS_DIR}/animations/plaintextthanifactory.h"
        "${STMMI_HEADERS_DIR}/animations/staticgridthanifactory.h"
        )
#
set(STMMI_HEADERS_MODIFIERS
        "${STMMI_HEADERS_DIR}/modifiers/alphamodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/casemodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/fademodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/fillmodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/growmodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/ifelsemodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/imagemodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/maskmodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/nextthememodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/rotatemodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/selectanimodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/selectcasemodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/selectedimagemodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/maskmodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/stopmodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/textmodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/tileanimodifier.h"
        )
#
set(STMMI_HEADERS_GTKUTIL
        "${STMMI_HEADERS_DIR}/gtkutil/cachedsurfaces.h"
        "${STMMI_HEADERS_DIR}/gtkutil/ccsave.h"
        "${STMMI_HEADERS_DIR}/gtkutil/dynanimation.h"
        "${STMMI_HEADERS_DIR}/gtkutil/frame.h"
        "${STMMI_HEADERS_DIR}/gtkutil/image.h"
        "${STMMI_HEADERS_DIR}/gtkutil/segmentedfunction.h"
        "${STMMI_HEADERS_DIR}/gtkutil/tileani.h"
        "${STMMI_HEADERS_DIR}/gtkutil/tilesizing.h"
        )
#
set(STMMI_HEADERS_WIDGET_IMPL
        "${STMMI_HEADERS_DIR}/widgetimpl/immutablethwidgetimpl.h"
        "${STMMI_HEADERS_DIR}/widgetimpl/mutablethwidgetimpl.h"
        "${STMMI_HEADERS_DIR}/widgetimpl/relsizedthwidgetimpl.h"
        )
#
set(STMMI_HEADERS_WIDGETS
        "${STMMI_HEADERS_DIR}/widgets/actionsboxthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/widgets/actionthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/widgets/backgroundthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/widgets/boxthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/widgets/inputboxthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/widgets/levelshowthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/widgets/previewthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/widgets/progressthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/widgets/transparentthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/widgets/varthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/widgets/volatilethwidgetfactory.h"
        )
#
set(STMMI_HEADERS
        "${STMMI_HEADERS_DIR}/boxthemewidgetfactory.h"
        "${STMMI_HEADERS_DIR}/commontheme.h"
        "${STMMI_HEADERS_DIR}/containermodifier.h"
        "${STMMI_HEADERS_DIR}/levelshowthemewidget.h"
        "${STMMI_HEADERS_DIR}/stmm-games-gtk-config.h"
        "${STMMI_HEADERS_DIR}/mainwindow.h"
        "${STMMI_HEADERS_DIR}/stdtheme.h"
        "${STMMI_HEADERS_DIR}/stdthemeanimationfactory.h"
        "${STMMI_HEADERS_DIR}/stdthemeanimationfactories.h"
        "${STMMI_HEADERS_DIR}/stdthemecontext.h"
        "${STMMI_HEADERS_DIR}/stdthemedrawingcontext.h"
        "${STMMI_HEADERS_DIR}/stdthememodifier.h"
        "${STMMI_HEADERS_DIR}/stdthemewidgetfactory.h"
        "${STMMI_HEADERS_DIR}/stdthemewidgetfactories.h"
        "${STMMI_HEADERS_DIR}/theme.h"
        "${STMMI_HEADERS_DIR}/themeanimation.h"
        "${STMMI_HEADERS_DIR}/themecontainerwidget.h"
        "${STMMI_HEADERS_DIR}/themecontext.h"
        "${STMMI_HEADERS_DIR}/themelayout.h"
        "${STMMI_HEADERS_DIR}/themeloader.h"
        "${STMMI_HEADERS_DIR}/themesound.h"
        "${STMMI_HEADERS_DIR}/themewidget.h"
        "${STMMI_HEADERS_DIR}/themewidgetinteractive.h"
        )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
            #"${STMMI_SOURCES_DIR}/stdthemedrawingcontext.cc"

        "${STMMI_SOURCES_DIR}/boxthemewidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/commontheme.cc"
        "${STMMI_SOURCES_DIR}/containermodifier.cc"
        "${STMMI_SOURCES_DIR}/gamegtkdrawingarea.h"
        "${STMMI_SOURCES_DIR}/gamegtkdrawingarea.cc"
        "${STMMI_SOURCES_DIR}/gamewindow.h"
        "${STMMI_SOURCES_DIR}/gamewindow.cc"
        "${STMMI_SOURCES_DIR}/levelshowthemewidget.cc"
        "${STMMI_SOURCES_DIR}/mainwindow.cc"
        "${STMMI_SOURCES_DIR}/stdlevelview.h"
        "${STMMI_SOURCES_DIR}/stdlevelview.cc"
        "${STMMI_SOURCES_DIR}/stdtheme.cc"
        "${STMMI_SOURCES_DIR}/stdthemeanimationfactory.cc"
        "${STMMI_SOURCES_DIR}/stdthemecontext.cc"
        "${STMMI_SOURCES_DIR}/stdthemedrawingcontext.cc"
        "${STMMI_SOURCES_DIR}/stdthememodifier.cc"
        "${STMMI_SOURCES_DIR}/stdthemesound.h"
        "${STMMI_SOURCES_DIR}/stdthemesound.cc"
        "${STMMI_SOURCES_DIR}/stdthemewidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/stdview.h"
        "${STMMI_SOURCES_DIR}/stdview.cc"
        "${STMMI_SOURCES_DIR}/stdviewlayout.h"
        "${STMMI_SOURCES_DIR}/stdviewlayout.cc"
        "${STMMI_SOURCES_DIR}/theme.cc"
        "${STMMI_SOURCES_DIR}/themeanimation.cc"
        "${STMMI_SOURCES_DIR}/themecontainerwidget.cc"
        "${STMMI_SOURCES_DIR}/themecontext.cc"
        "${STMMI_SOURCES_DIR}/themelayout.cc"
        "${STMMI_SOURCES_DIR}/themeloader.cc"
        "${STMMI_SOURCES_DIR}/themesound.cc"
        "${STMMI_SOURCES_DIR}/themewidget.cc"
        "${STMMI_SOURCES_DIR}/themewidgetinteractive.cc"
        #
        "${STMMI_SOURCES_DIR}/dialogs/gamedialog.h"
        "${STMMI_SOURCES_DIR}/dialogs/gamedialog.cc"
        "${STMMI_SOURCES_DIR}/dialogs/highscoredialog.h"
        "${STMMI_SOURCES_DIR}/dialogs/highscoredialog.cc"
        "${STMMI_SOURCES_DIR}/dialogs/inputstrings.h"
        "${STMMI_SOURCES_DIR}/dialogs/inputstrings.cc"
        "${STMMI_SOURCES_DIR}/dialogs/playersdialog.h"
        "${STMMI_SOURCES_DIR}/dialogs/playersdialog.cc"
        "${STMMI_SOURCES_DIR}/dialogs/themedialog.h"
        "${STMMI_SOURCES_DIR}/dialogs/themedialog.cc"
        #
        "${STMMI_SOURCES_DIR}/animations/backgroundthanifactory.cc"
        "${STMMI_SOURCES_DIR}/animations/explosionthanifactory.cc"
        "${STMMI_SOURCES_DIR}/animations/imagesequencethanifactory.cc"
        "${STMMI_SOURCES_DIR}/animations/plaintextthanifactory.cc"
        "${STMMI_SOURCES_DIR}/animations/staticgridthanifactory.cc"
        #
        "${STMMI_SOURCES_DIR}/modifiers/alphamodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/casemodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/fademodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/fillmodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/growmodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/ifelsemodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/imagemodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/maskmodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/nextthememodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/rotatemodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/selectanimodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/selectcasemodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/selectedimagemodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/maskmodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/stopmodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/textmodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/tileanimodifier.cc"
        #
        "${STMMI_SOURCES_DIR}/gtkutil/ccsave.cc"
        "${STMMI_SOURCES_DIR}/gtkutil/cachedsurfaces.cc"
        "${STMMI_SOURCES_DIR}/gtkutil/dynanimation.cc"
        "${STMMI_SOURCES_DIR}/gtkutil/frame.cc"
        "${STMMI_SOURCES_DIR}/gtkutil/gtkutilpriv.h"
        "${STMMI_SOURCES_DIR}/gtkutil/gtkutilpriv.cc"
        "${STMMI_SOURCES_DIR}/gtkutil/image.cc"
        "${STMMI_SOURCES_DIR}/gtkutil/segmentedfunction.cc"
        "${STMMI_SOURCES_DIR}/gtkutil/tileani.cc"
        "${STMMI_SOURCES_DIR}/gtkutil/tilesizing.cc"
        #
        "${STMMI_SOURCES_DIR}/widgetimpl/immutablethwidgetimpl.cc"
        "${STMMI_SOURCES_DIR}/widgetimpl/mutablethwidgetimpl.cc"
        "${STMMI_SOURCES_DIR}/widgetimpl/relsizedthwidgetimpl.cc"
        #
        "${STMMI_SOURCES_DIR}/widgets/actionsboxthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/widgets/actionthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/widgets/backgroundthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/widgets/boxthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/widgets/inputboxthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/widgets/levelshowthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/widgets/previewthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/widgets/progressthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/widgets/transparentthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/widgets/varthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/widgets/volatilethwidgetfactory.cc"
        )

# Define library
add_library(stmm-games-gtk ${STMMI_SOURCES} "${PROJECT_BINARY_DIR}/stmm-games-gtk-config.cc")

include("stmm-games-gtk-defs.cmake")

target_include_directories(stmm-games-gtk        PUBLIC "${STMMGAMESGTK_EXTRA_INCLUDE_DIRS}")
target_include_directories(stmm-games-gtk SYSTEM PUBLIC "${STMMGAMESGTK_EXTRA_INCLUDE_SDIRS}")
target_include_directories(stmm-games-gtk BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-games-gtk>  # <prefix>/include/stmm-games-gtk
)

#target_link_libraries(stmm-games-gtk stmm-games)
target_link_libraries(stmm-games-gtk ${STMMGAMESGTK_EXTRA_LIBRARIES})

set_target_properties(stmm-games-gtk PROPERTIES  ${CMAKE_BUILD_TYPE}_POSTFIX "")
set_target_properties(stmm-games-gtk PROPERTIES
        VERSION ${STMM_GAMES_GTK_VERSION}
        SOVERSION ${STMM_GAMES_GTK_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-games-gtk-${STMM_GAMES_GTK_VERSION}"
        )

DefineTargetPublicCompileOptions(stmm-games-gtk)

# Set version for stmm-games-gtk-config.cc.in
set(STMMI_PKG_VERSION "${STMM_GAMES_GTK_VERSION}")
# Create stmm-games-gtk-config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-games-gtk-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-games-gtk-config.cc" @ONLY)

# Set install dir for pkg-config file
set(STMMI_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
# Create pkg-config file for the library
configure_file("${PROJECT_SOURCE_DIR}/stmm-games-gtk.pc.in"
               "${PROJECT_BINARY_DIR}/stmm-games-gtk.pc" @ONLY)

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-games-gtk was configured with the following options:")
message(STATUS " STMMI_HEADERS:                    ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                    ${STMMI_SOURCES}")
message(STATUS " STMMGAMESGTK_EXTRA_INCLUDE_DIRS:  ${STMMGAMESGTK_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMGAMESGTK_EXTRA_INCLUDE_SDIRS: ${STMMGAMESGTK_EXTRA_INCLUDE_SDIRS}")
message(STATUS " STMMGAMESGTK_EXTRA_LIBRARIES:     ${STMMGAMESGTK_EXTRA_LIBRARIES}")
message(STATUS " CMAKE_BUILD_TYPE:                 ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:            ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:       ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                  ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                   ${CMAKE_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                       ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                    ${BUILD_TESTING}")
endif()

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-games-gtk doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)

install(TARGETS stmm-games-gtk LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS}             DESTINATION "include/stmm-games-gtk")
install(FILES ${STMMI_HEADERS_ANIMATIONS}  DESTINATION "include/stmm-games-gtk/animations")
install(FILES ${STMMI_HEADERS_MODIFIERS}   DESTINATION "include/stmm-games-gtk/modifiers")
install(FILES ${STMMI_HEADERS_GTKUTIL}     DESTINATION "include/stmm-games-gtk/gtkutil")
install(FILES ${STMMI_HEADERS_WIDGET_IMPL} DESTINATION "include/stmm-games-gtk/widgetimpl")
install(FILES ${STMMI_HEADERS_WIDGETS}     DESTINATION "include/stmm-games-gtk/widgets")

install(FILES  "${PROJECT_BINARY_DIR}/stmm-games-gtk.pc"   DESTINATION "lib/pkgconfig")
