CMAKE_MINIMUM_REQUIRED (VERSION 2.4 FATAL_ERROR)
PROJECT (client_donnie)

# Include this CMake module to get most of the settings needed to build
SET (CMAKE_MODULE_PATH "/usr/local/share/cmake/Modules")

INCLUDE (UsePlayerC++)

PLAYER_ADD_PLAYERCPP_CLIENT (client_donnie SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/client_donnie.cc)
