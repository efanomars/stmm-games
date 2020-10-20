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

# File:   libstmm-games-xml-gtk/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(stmm-games-xml-gtk CXX)

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
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-games-xml-gtk")
# Header files
set(STMMI_HEADERS_MODIFIERS
        "${STMMI_HEADERS_DIR}/modifiers/xmlalphamodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmlcasemodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmlfademodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmlfillmodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmlgrowmodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmlifelsemodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmlimagemodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmlmaskmodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmlnextthememodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmlrotatemodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmlselectanimodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmlselectcasemodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmlselectedimagemodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmlselectvarmodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmlstopmodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmltextmodifier.h"
        "${STMMI_HEADERS_DIR}/modifiers/xmltileanimodifier.h"
        )
#
set(STMMI_HEADERS_THEME_ANIMATIONS
        "${STMMI_HEADERS_DIR}/themeanimations/xmlbackgroundthanimationfactory.h"
        "${STMMI_HEADERS_DIR}/themeanimations/xmlexplosionthanimationfactory.h"
        "${STMMI_HEADERS_DIR}/themeanimations/xmlimageseqthanimationfactory.h"
        "${STMMI_HEADERS_DIR}/themeanimations/xmlplaintextthanimationfactory.h"
        "${STMMI_HEADERS_DIR}/themeanimations/xmlstaticgridthanimationfactory.h"
        )
#
set(STMMI_HEADERS_THEME_WIDGETS
        "${STMMI_HEADERS_DIR}/themewidgets/xmlactionsboxthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/themewidgets/xmlactionthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/themewidgets/xmlbackgroundthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/themewidgets/xmlboxthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/themewidgets/xmlinputboxthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/themewidgets/xmllevelshowthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/themewidgets/xmlpreviewthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/themewidgets/xmlprogressthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/themewidgets/xmltransparentthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/themewidgets/xmlvarthwidgetfactory.h"
        "${STMMI_HEADERS_DIR}/themewidgets/xmlvolatilethwidgetfactory.h"
        )
#
set(STMMI_HEADERS_XML_GTK_UTIL
        "${STMMI_HEADERS_DIR}/xmlgtkutil/xmlelapsedmapperparser.h"
        )
#
set(STMMI_HEADERS
        "${STMMI_HEADERS_DIR}/gamediskfiles.h"
        "${STMMI_HEADERS_DIR}/stmm-games-xml-gtk-config.h"
        "${STMMI_HEADERS_DIR}/themectx.h"
        "${STMMI_HEADERS_DIR}/xmlmodifierparser.h"
        "${STMMI_HEADERS_DIR}/xmlstdmodifierparsers.h"
        "${STMMI_HEADERS_DIR}/xmlstdthanimationparsers.h"
        "${STMMI_HEADERS_DIR}/xmlstdthwidgetparsers.h"
        "${STMMI_HEADERS_DIR}/xmlthanimationfactoryparser.h"
        "${STMMI_HEADERS_DIR}/xmlthemeimageparser.h"
        "${STMMI_HEADERS_DIR}/xmlthemeloader.h"
        "${STMMI_HEADERS_DIR}/xmlthwidgetfactoryparser.h"
        )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
            #"${STMMI_SOURCES_DIR}/xmlvariantsetparser.cc"
        #
        "${STMMI_SOURCES_DIR}/modifiers/xmlalphamodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmlcasemodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmlfademodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmlfillmodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmlgrowmodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmlifelsemodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmlimagemodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmlmaskmodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmlnextthememodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmlrotatemodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmlselectanimodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmlselectcasemodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmlselectedimagemodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmlselectvarmodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmlstopmodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmltextmodifier.cc"
        "${STMMI_SOURCES_DIR}/modifiers/xmltileanimodifier.cc"
        #
        "${STMMI_SOURCES_DIR}/themeanimations/xmlbackgroundthanimationfactory.cc"
        "${STMMI_SOURCES_DIR}/themeanimations/xmlexplosionthanimationfactory.cc"
        "${STMMI_SOURCES_DIR}/themeanimations/xmlimageseqthanimationfactory.cc"
        "${STMMI_SOURCES_DIR}/themeanimations/xmlplaintextthanimationfactory.cc"
        "${STMMI_SOURCES_DIR}/themeanimations/xmlstaticgridthanimationfactory.cc"
        #
        "${STMMI_SOURCES_DIR}/themewidgets/xmlactionsboxthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/themewidgets/xmlactionthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/themewidgets/xmlbackgroundthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/themewidgets/xmlboxthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/themewidgets/xmlinputboxthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/themewidgets/xmllevelshowthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/themewidgets/xmlpreviewthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/themewidgets/xmlprogressthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/themewidgets/xmltransparentthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/themewidgets/xmlvarthwidgetfactory.cc"
        "${STMMI_SOURCES_DIR}/themewidgets/xmlvolatilethwidgetfactory.cc"
        #
        "${STMMI_SOURCES_DIR}/xmlgtkutil/xmlelapsedmapperparser.cc"
        #
        "${STMMI_SOURCES_DIR}/gamediskfiles.cc"
        "${STMMI_SOURCES_DIR}/themectx.cc"
        "${STMMI_SOURCES_DIR}/xmlmodifierparser.cc"
        "${STMMI_SOURCES_DIR}/xmlstdmodifierparsers.cc"
        "${STMMI_SOURCES_DIR}/xmlstdthanimationparsers.cc"
        "${STMMI_SOURCES_DIR}/xmlstdthwidgetparsers.cc"
        "${STMMI_SOURCES_DIR}/xmlthanimationfactoryparser.cc"
        "${STMMI_SOURCES_DIR}/xmlthemeaniparser.h"
        "${STMMI_SOURCES_DIR}/xmlthemeaniparser.cc"
        "${STMMI_SOURCES_DIR}/xmlthemeassignparser.h"
        "${STMMI_SOURCES_DIR}/xmlthemeassignparser.cc"
        "${STMMI_SOURCES_DIR}/xmlthemeimageparser.cc"
        "${STMMI_SOURCES_DIR}/xmlthemeimagesparser.h"
        "${STMMI_SOURCES_DIR}/xmlthemeimagesparser.cc"
        "${STMMI_SOURCES_DIR}/xmlthemeloader.cc"
        "${STMMI_SOURCES_DIR}/xmlthememodifiersparser.h"
        "${STMMI_SOURCES_DIR}/xmlthememodifiersparser.cc"
        "${STMMI_SOURCES_DIR}/xmlthemeparser.h"
        "${STMMI_SOURCES_DIR}/xmlthemeparser.cc"
        "${STMMI_SOURCES_DIR}/xmlthemesoundsparser.h"
        "${STMMI_SOURCES_DIR}/xmlthemesoundsparser.cc"
        "${STMMI_SOURCES_DIR}/xmlthemewidgetparser.h"
        "${STMMI_SOURCES_DIR}/xmlthemewidgetparser.cc"
        "${STMMI_SOURCES_DIR}/xmlthwidgetfactoryparser.cc"
        "${STMMI_SOURCES_DIR}/xmlutilfile.h"
        "${STMMI_SOURCES_DIR}/xmlutilfile.cc"
        )

# Data dir
set(STMMI_DATA_DIR  "${PROJECT_SOURCE_DIR}/data")
# Data files
set(STMMI_DATA_FILES
        "${STMMI_DATA_DIR}/themes/common/images/background0.svg"
        "${STMMI_DATA_DIR}/themes/common/images/background1.svg"
        "${STMMI_DATA_DIR}/themes/common/images/background2.svg"
        "${STMMI_DATA_DIR}/themes/common/images/background3.svg"
        "${STMMI_DATA_DIR}/themes/common/images/background4.svg"
        "${STMMI_DATA_DIR}/themes/common/images/background5.svg"
        "${STMMI_DATA_DIR}/themes/common/images/background6.svg"
        "${STMMI_DATA_DIR}/themes/common/images/background7.svg"
        "${STMMI_DATA_DIR}/themes/common/images/background8.svg"
        #
        "${STMMI_DATA_DIR}/themes/common/sounds/bonc.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/explosion.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/gameover.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/hat.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n00.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n01.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n02.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n03.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n04.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n05.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n06.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n07.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n08.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n09.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n10.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n11.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n12.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n13.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n14.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n15.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n16.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n17.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n18.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n19.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n20.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n21.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n22.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n23.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n24.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n25.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n26.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n27.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n28.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n29.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/n30.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/systemtest.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/tic.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/tac.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/toc.mp3"
        "${STMMI_DATA_DIR}/themes/common/sounds/wrong.mp3"
        #
        "${STMMI_DATA_DIR}/themes/simple.thm/images/bomb.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/cmdframe-b.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/cmdframe-bl.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/cmdframe-br.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/cmdframe-l.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/cmdframe-r.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/cmdframe-t.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/cmdframe-tl.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/cmdframe-tr.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/cracksmask1.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/cracksmask2.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/explosion1.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/explosion2.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/explosion3.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/explosion4.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/framelineh.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/framelinev.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/gradient_0.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/gradient_1.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/gradient_2.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/gradient_3.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/gradient_5.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/gradient_6.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/gradient_7.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/iconDown.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/iconLeft.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/iconRight.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/iconUp.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/squaremask.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/squareplayer0.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/squareplayer1.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/squareplayer2.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/squareplayer3.svg"
        "${STMMI_DATA_DIR}/themes/simple.thm/images/squareplayer4.svg"
        #
        "${STMMI_DATA_DIR}/themes/simple.thm/theme.xml"
        )

# Define library
add_library(stmm-games-xml-gtk ${STMMI_SOURCES} "${PROJECT_BINARY_DIR}/stmm-games-xml-gtk-config.cc")

include("stmm-games-xml-gtk-defs.cmake")

target_include_directories(stmm-games-xml-gtk        PUBLIC ${STMMGAMESXMLGTK_EXTRA_INCLUDE_DIRS})
target_include_directories(stmm-games-xml-gtk SYSTEM PUBLIC ${STMMGAMESXMLGTK_EXTRA_INCLUDE_SDIRS})
target_include_directories(stmm-games-xml-gtk BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-games-xml-gtk>  # <prefix>/include/stmm-games-xml-gtk
)
target_include_directories(stmm-games-xml-gtk       PRIVATE "${PROJECT_SOURCE_DIR}/src")

target_link_libraries(stmm-games-xml-gtk ${STMMGAMESXMLGTK_EXTRA_LIBRARIES})

set_target_properties(stmm-games-xml-gtk PROPERTIES ${CMAKE_BUILD_TYPE}_POSTFIX "")
set_target_properties(stmm-games-xml-gtk PROPERTIES
        VERSION ${STMM_GAMES_XML_GTK_VERSION}
        SOVERSION ${STMM_GAMES_XML_GTK_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-games-xml-gtk-${STMM_GAMES_XML_GTK_VERSION}"
        )

DefineTargetPublicCompileOptions(stmm-games-xml-gtk)

# Set version and dirs for stmm-games-xml-gtk-config.cc.in
set(STMMI_PKG_VERSION "${STMM_GAMES_XML_GTK_VERSION}")
include(GNUInstallDirs)
# usually:
#   CMAKE_INSTALL_DATADIR=share
#   CMAKE_INSTALL_FULL_DATADIR=/usr/share
set(STMMI_HOME_REL_DATA_DIR ".local/${CMAKE_INSTALL_DATADIR}/stmm-games")
if (NOT ("$ENV{STMM_SNAP_PACKAGING}" STREQUAL "ON"))
    # the following will be prefixed with $HOME at runtime (ex. "/home/myuser" + "/" + "share/stmm-games")
    # it is where prefs and highscores are stored
    set(STMMI_PKG_USER_DATA_DIR "${STMMI_HOME_REL_DATA_DIR}")
    # the installation path (ex. "/usr/local/share/stmm-games")
    # the result is the installation path
    set(STMMI_PKG_DATA_DIR      "${CMAKE_INSTALL_FULL_DATADIR}/stmm-games")
    # the system paths from which games and themes are loaded
    set(STMMI_RO_DATA_DIR_1     "/usr/local/${CMAKE_INSTALL_DATADIR}/stmm-games")
    set(STMMI_RO_DATA_DIR_2     "/usr/${CMAKE_INSTALL_DATADIR}/stmm-games")
    set(STMMI_RO_DATA_DIR_3     "${STMMI_PKG_DATA_DIR}")
else()
    # the following will be prefixed with $SNAP_USER_DATA at runtime (ex. "/home/myuser/snap/stmg/x4" + "/" + "share/stmm-games")
    # it is where prefs and highscores are stored
    set(STMMI_PKG_USER_DATA_DIR "${CMAKE_INSTALL_DATADIR}/stmm-games")
    # the following will be prefixed with $SNAP at runtime (ex. "/snap/stmg/x4" + "/" + "/usr/share/stmm-games")
    # the result is the installation path
    set(STMMI_PKG_DATA_DIR      "${CMAKE_INSTALL_FULL_DATADIR}/stmm-games")
    # the following will be prefixed with $SNAP at runtime (ex. "/snap/stmg/x4" + "/" + "/usr/share/stmm-games")
    # the path from which games and themes can be loaded
    set(STMMI_RO_DATA_DIR_1     "${STMMI_PKG_DATA_DIR}")
    # "/homw/myuser" + "/" + ${STMMI_HOME_REL_DATA_DIR} is also used as an additional path
    # from which games and themes are loaded
endif()

# Create config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-games-xml-gtk-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-games-xml-gtk-config.cc" @ONLY)

# Set install dir for pkg-config file
set(STMMI_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
# Create pkg-config file for the library
configure_file("${PROJECT_SOURCE_DIR}/stmm-games-xml-gtk.pc.in"
               "${PROJECT_BINARY_DIR}/stmm-games-xml-gtk.pc" @ONLY)

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-games-xml-gtk was configured with the following options:")
message(STATUS " STMMI_HEADERS:                       ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                       ${STMMI_SOURCES}")
message(STATUS " STMMGAMESXMLGTK_EXTRA_INCLUDE_DIRS:  ${STMMGAMESXMLGTK_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMGAMESXMLGTK_EXTRA_INCLUDE_SDIRS: ${STMMGAMESXMLGTK_EXTRA_INCLUDE_SDIRS}")
message(STATUS " STMMGAMESXMLGTK_EXTRA_LIBRARIES:     ${STMMGAMESXMLGTK_EXTRA_LIBRARIES}")
message(STATUS " CMAKE_BUILD_TYPE:                    ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:               ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:          ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                     ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                      ${CMAKE_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                          ${BUILD_DOCS}")
message(STATUS " ENV{STMM_DEBIAN_PACKAGING}:          $ENV{STMM_DEBIAN_PACKAGING}")
message(STATUS " BUILD_TESTING:                       ${BUILD_TESTING}")
message(STATUS " STMMI_PKG_DATA_DIR:                  ${STMMI_PKG_DATA_DIR}")
message(STATUS " STMMI_PKG_USER_DATA_DIR:             ${STMMI_PKG_USER_DATA_DIR}")
endif()

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS OR ("$ENV{STMM_DEBIAN_PACKAGING}" STREQUAL "ON"))
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-games-xml-gtk doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)

install(TARGETS stmm-games-xml-gtk LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS}                  DESTINATION "include/stmm-games-xml-gtk")
install(FILES ${STMMI_HEADERS_MODIFIERS}        DESTINATION "include/stmm-games-xml-gtk/modifiers")
install(FILES ${STMMI_HEADERS_THEME_ANIMATIONS} DESTINATION "include/stmm-games-xml-gtk/themeanimations")
install(FILES ${STMMI_HEADERS_THEME_WIDGETS}    DESTINATION "include/stmm-games-xml-gtk/themewidgets")
install(FILES ${STMMI_HEADERS_XML_GTK_UTIL}     DESTINATION "include/stmm-games-xml-gtk/xmlgtkutil")

install(FILES  "${PROJECT_BINARY_DIR}/stmm-games-xml-gtk.pc"   DESTINATION "lib/pkgconfig")

foreach (STMMI_DATA_CUR_FILE  ${STMMI_DATA_FILES})
    file(RELATIVE_PATH  STMMI_DATA_CUR_REL_FILE  "${STMMI_DATA_DIR}"  "${STMMI_DATA_CUR_FILE}")
    get_filename_component(STMMI_DATA_CUR_DIR "${STMMI_PKG_DATA_DIR}/${STMMI_DATA_CUR_REL_FILE}" DIRECTORY)
    #message(STATUS "STMMI_DATA_CUR_FILE         ${STMMI_DATA_CUR_FILE}")
    #message(STATUS "STMMI_DATA_CUR_REL_FILE     ${STMMI_DATA_CUR_REL_FILE}")
    #message(STATUS "STMMI_DATA_CUR_DIR          ${STMMI_DATA_CUR_DIR}")
    install(FILES ${STMMI_DATA_CUR_FILE}  DESTINATION   "${STMMI_DATA_CUR_DIR}")
endforeach (STMMI_DATA_CUR_FILE  ${STMMI_DATA_FILES})
