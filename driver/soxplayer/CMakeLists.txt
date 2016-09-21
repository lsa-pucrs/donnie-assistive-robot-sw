SET (DRIVERNAME "soxplayer")
SET (LIBDEST "/usr/local/lib64/")
#SET(LIBDEST "$ENV{DONNIE_PATH}/lib/player/")

CMAKE_MINIMUM_REQUIRED (VERSION 2.4 FATAL_ERROR)

PROJECT (${DRIVERNAME})

SET (CMAKE_MODULE_PATH "/usr/local/share/cmake/Modules")
INCLUDE (UsePlayerPlugin)

# especificar a dependecia de sox adequadamente. ver
# https://cmake.org/Wiki/CMake:How_To_Find_Libraries
# com isso, a dependencia vai ser verificada no ccmake, antes da compilacao.

PLAYER_ADD_PLUGIN_DRIVER (${DRIVERNAME} SOURCES ${DRIVERNAME}.cc) 
TARGET_LINK_LIBRARIES (${DRIVERNAME} -lsox) 
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR})

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${DRIVERNAME}.so DESTINATION ${LIBDEST}) 
