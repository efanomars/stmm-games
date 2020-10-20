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

# File:   libstmm-games-fake/test/CMakeLists.txt

# See   git.com/Barthelemy/CppProjectTemplate

include(CommonTesting)

option(BUILD_TESTING "Build tests" OFF)

if (BUILD_TESTING)

    set(TESTFAKELIBS "${STMMGAMESFAKE_EXTRA_LIBRARIES}")
    list(APPEND TESTFAKELIBS "stmm-games-fake")

    # Tests dir
    set(STMMI_FAKE_TEST_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test")
    # Test sources need to end with .cxx, helper sources with .h .cc
    set(STMMI_FAKE_TEST_SOURCES
            #${STMMI_FAKE_TEST_SOURCES_DIR}/testFakeMockEvent.cxx
            )

    TestFiles("${STMMI_FAKE_TEST_SOURCES}" ""
              "" "${TESTFAKELIBS}"
              FALSE FALSE FALSE)

    include(CTest)
endif()
