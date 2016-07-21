CMAKE_MINIMUM_REQUIRED (VERSION 2.4 FATAL_ERROR)
PROJECT (googletts)

# Include this CMake module to get most of the settings needed to build
SET (CMAKE_MODULE_PATH "/usr/local/share/cmake/Modules")
INCLUDE (UsePlayerPlugin)


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/bass24-lib)
ADD_LIBRARY(googletts ${CMAKE_CURRENT_SOURCE_DIR}/bass24-lib/x64)
TARGET_LINK_LIBRARIES (googletts ${CMAKE_CURRENT_SOURCE_DIR}/bass24-lib/x64/libbass.so)
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR}) #source files list
TARGET_LINK_LIBRARIES (googletts -lcurl)

PLAYER_ADD_PLUGIN_DRIVER (googletts SOURCES googletts.cc) #source files list


#INSTALL (FILES ${CMAKE_CURRENT_SOURCE_DIR}/server_donnie.cfg DESTINATION /usr/local/share/player/config/)
#INSTALL (FILES ${CMAKE_CURRENT_SOURCE_DIR}/client/client_donnie.cc      DESTINATION /usr/local/share/player/examples/libplayerc++/donnie/)
#INSTALL (FILES ${CMAKE_CURRENT_SOURCE_DIR}/client/CMakeLists.txt DESTINATION /usr/local/share/player/examples/libplayerc++/donnie/)
#INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${THE_TARGET}.so DESTINATION ${PLUGIN_PLAYERC_LIBRARY_DIRS})  #instala na pasta /usr/local/lib64
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libgoogletts.so DESTINATION /usr/local/lib64/) 
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/bass24-lib/x64/libbass.so DESTINATION /usr/lib) 
