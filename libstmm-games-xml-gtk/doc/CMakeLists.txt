# stmm-games-xml-gtk: doc/CMakeLists.txt

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

include(FindDoxygen)

if (DOXYGEN_FOUND)
    if (NOT DOXYGEN_DOT_FOUND) 
        message(WARNING "Graphviz doesn't seem to be installed. Doxygen will not be able to generate graphs. Consider installing this package.")
    endif(NOT DOXYGEN_DOT_FOUND) 

    include(CommonDoxy)
    set(STMMI_TEMP_DOC_LIB_LIST "stmm-games-xml-game;stmm-games-xml-base;stmm-games-gtk;stmm-games-file;stmm-games;stmm-input-au;stmm-input-ev;stmm-input-base;stmm-input")
    CreateLibDoxy("stmm-games-xml-gtk"  ${STMM_GAMES_XML_GTK_VERSION}  "${STMMI_TEMP_DOC_LIB_LIST}")

    if ("$ENV{STMM_DEBIAN_PACKAGING}" STREQUAL "ON")
        list(APPEND STMMI_TEMP_DOC_LIB_LIST stmm-games-fake)
        list(APPEND STMMI_TEMP_DOC_LIB_LIST stmm-games-xml-gtk)
        list(APPEND STMMI_TEMP_DOC_LIB_LIST stmm-input-fake)

        set(STMMI_DOC_DIRS_AND_FILES)
        list(APPEND STMMI_DOC_DIRS_AND_FILES
                ${PROJECT_SOURCE_DIR}/doc-all
                )
        CreateLibDoxyDetailed("stmm-games-doc"  ${STMM_GAMES_XML_GTK_VERSION}  "${STMMI_TEMP_DOC_LIB_LIST}"
                                                ${STMMI_DOC_DIRS_AND_FILES} "htmlall" "docu-config-all.doxygen" "ON")
#         # install examples
#         install(DIRECTORY ${CMAKE_SOURCE_DIR}/examples DESTINATION share/doc/libstmm-games-doc
#                 PATTERN "build" EXCLUDE
#                 PATTERN ".gitignore" EXCLUDE)
#         install(DIRECTORY ${CMAKE_SOURCE_DIR}/../libstmm-games-fake/examples DESTINATION share/doc/libstmm-games-doc
#                 PATTERN "build" EXCLUDE
#                 PATTERN ".gitignore" EXCLUDE)
    endif()
else(DOXYGEN_FOUND)
    add_custom_target(doc COMMAND echo "Doxygen not found")
endif(DOXYGEN_FOUND)
