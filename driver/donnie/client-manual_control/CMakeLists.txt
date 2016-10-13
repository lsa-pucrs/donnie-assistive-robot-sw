CMAKE_MINIMUM_REQUIRED (VERSION 2.4 FATAL_ERROR)
PROJECT (client-donnie)
#SET(LIBDEST "$ENV{DONNIE_PATH}/test/player/")

# Include this CMake module to get most of the settings needed to build
SET (CMAKE_MODULE_PATH "/usr/local/share/cmake/Modules")

INCLUDE (UsePlayerC++)


PLAYER_ADD_PLAYERCPP_CLIENT (client-donnie SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/client-donnie.cc )

target_link_libraries (client-donnie pthread) 
