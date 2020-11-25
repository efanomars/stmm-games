if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the omni source code and call cmake from there")
endif()

cmake_minimum_required(VERSION 3.0)

project(omni CXX)

set(RUNTIME_OUTPUT_DIRECTORY "build")

include(FindPkgConfig)
find_package(PkgConfig)

# Version
set(OMNI_VERSION "0.29") # !-U-!

# Required libraries
set(OMNI_REQ_STMM_GAMES_XML_GTK_VERSION "0.29") # !-U-!
set(OMNI_REQ_STMM_INPUT_GTK_DM_VERSION "0.16") # !-U-!

# Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
pkg_check_modules(STMMGAMESXMLGTK  REQUIRED  stmm-games-xml-gtk>=${OMNI_REQ_STMM_GAMES_XML_GTK_VERSION})
pkg_check_modules(STMMINPUTGTKDM   REQUIRED  stmm-input-gtk-dm>=${OMNI_REQ_STMM_INPUT_GTK_DM_VERSION})

pkg_check_modules(GTKMM            REQUIRED  gtkmm-3.0>=3.22.0)

list(APPEND OMNI_EXTRA_INCLUDE_DIRS   ${GTKMM_INCLUDE_DIRS})
list(APPEND OMNI_EXTRA_INCLUDE_DIRS   ${STMMINPUTGTKDM_INCLUDE_DIRS})
list(APPEND OMNI_EXTRA_INCLUDE_DIRS   ${STMMGAMESXMLGTK_INCLUDE_DIRS})
list(APPEND OMNI_EXTRA_LIBS           ${GTKMM_LIBRARIES})
list(APPEND OMNI_EXTRA_LIBS           ${STMMINPUTGTKDM_LIBRARIES})
list(APPEND OMNI_EXTRA_LIBS           ${STMMGAMESXMLGTK_LIBRARIES})

# Add include directories of used libraries
include_directories(SYSTEM "${OMNI_EXTRA_INCLUDE_DIRS}")

# Source and headers files
set(OMNI_SOURCES
        ${PROJECT_SOURCE_DIR}/src/inputstrings.h
        ${PROJECT_SOURCE_DIR}/src/inputstrings.cc
        ${PROJECT_SOURCE_DIR}/src/jsonstrings.h
        ${PROJECT_SOURCE_DIR}/src/jsonstrings.cc
        ${PROJECT_SOURCE_DIR}/src/main.cc
        ${PROJECT_SOURCE_DIR}/src/setup.h
        ${PROJECT_SOURCE_DIR}/src/setup.cc
        ${PROJECT_SOURCE_DIR}/src/setupstdconfig.h
        ${PROJECT_SOURCE_DIR}/src/setupstdconfig.cc
        ${PROJECT_SOURCE_DIR}/src/setupxmlgameloader.h
        ${PROJECT_SOURCE_DIR}/src/setupxmlgameloader.cc
        ${PROJECT_SOURCE_DIR}/src/setupxmlthemeloader.h
        ${PROJECT_SOURCE_DIR}/src/setupxmlthemeloader.cc
        )
# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")

set(STMM_COMPILE_WARNINGS "-Wall -Wextra $ENV{STMM_CPP_OPTIONS}")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	string(REPLACE "-Wsuggest-override" "" STMM_COMPILE_WARNINGS ${STMM_COMPILE_WARNINGS})
	string(REPLACE "-Wlogical-op" "" STMM_COMPILE_WARNINGS ${STMM_COMPILE_WARNINGS})
endif()

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${STMM_COMPILE_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STMM_COMPILE_WARNINGS}")

file(COPY "../../../share/thirdparty/nlohmann" DESTINATION ".")
# Define executable
add_executable(omni ${OMNI_SOURCES})

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "omni was configured with the following options:")
message(STATUS " OMNI_SOURCES:                ${OMNI_SOURCES}")
message(STATUS " OMNI_DATA_FILES              ${OMNI_DATA_FILES}")
message(STATUS " OMNI_EXTRA_INCLUDE_DIRS:     ${OMNI_EXTRA_INCLUDE_DIRS}")
message(STATUS " OMNI_EXTRA_LIBS:             ${OMNI_EXTRA_LIBS}")
message(STATUS " CMAKE_BUILD_TYPE:            ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:       ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_FLAGS:             ${CMAKE_CXX_FLAGS}")
message(STATUS " CMAKE_CXX_FLAGS_DEBUG:       ${CMAKE_CXX_FLAGS_DEBUG}")
#message(STATUS " install prefix:              ${CMAKE_INSTALL_PREFIX}")
endif()

target_include_directories(omni PRIVATE "${PROJECT_BINARY_DIR}")

target_link_libraries(omni ${OMNI_EXTRA_LIBS})

#install(TARGETS omni RUNTIME DESTINATION "bin")
