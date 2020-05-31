# stmm-games: test/CMakeLists.txt

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

# BEWARE!
#   The tests depend on the libstmm-games-fake project
#   If you change the interface you should build and install the
#   libstmm-games project without testing

include(CommonTesting)

option(BUILD_TESTING "Build tests" OFF)

if (BUILD_TESTING)
    # Test dirs
    set(STMMI_TEST_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test")

    # Test sources should end with .cxx
    set(STMMI_TEST_SOURCES_SIMPLE
            "${STMMI_TEST_SOURCES_DIR}/testAppConstraints.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testBlock.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testCircularBuffer.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testCoords.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testBasicTypes.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testDirection.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testIntSet.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testNamedIndex.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testNamedObjIndex.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testNewRows.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testOption.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testRandomParts.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testRandomTiles.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testRecycler.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testTile.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testTileCoords.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testTileSelector.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testTileTraitSets.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testUtil.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testVariantSet.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testXYBuffer.cxx"
           )

    TestFiles("${STMMI_TEST_SOURCES_SIMPLE}" "${STMMI_TEST_WITH_SOURCES}" "" "stmm-games" FALSE)

    # Test sources should end with .cxx
    set(STMMI_TEST_SOURCES_INPUT
            "${STMMI_TEST_SOURCES_DIR}/testAlarmsEvent.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testArrayEvent.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testBackgroundEvent.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testDelayedQueueEvent.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testGame.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testHighscore.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testHighscoresDefinition.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testLayout.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testLogEvent.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testRandomEvent.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testStdConfig.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testStdPreferences.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testSysEvent.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testTileAnimatorEvent.cxx"
            "${STMMI_TEST_SOURCES_DIR}/testVariableEvent.cxx"
           )

    TestFiles("${STMMI_TEST_SOURCES_INPUT}" "${STMMI_TEST_WITH_SOURCES}" "" "stmm-games;stmm-input-fake" TRUE)

    include(CTest)
endif()
