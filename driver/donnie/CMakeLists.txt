CMAKE_MINIMUM_REQUIRED (VERSION 2.4 FATAL_ERROR)
PROJECT (donnie)

# Include this CMake module to get most of the settings needed to build
SET (CMAKE_MODULE_PATH "/usr/local/share/cmake/Modules")
INCLUDE (UsePlayerPlugin)

PLAYER_ADD_PLUGIN_DRIVER (donnie SOURCES donnie.cc) #source files list
#TARGET_LINK_LIBRARIES (${THE_TARGET})
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR}) #source files list
#LINK_DIRECTORIES () #libraries directories files list
#ADD_LIBRARY () #libraries list ex: -ld

#INSTALL (FILES ${CMAKE_CURRENT_SOURCE_DIR}/server_donnie.cfg DESTINATION /usr/local/share/player/config/)
#INSTALL (FILES ${CMAKE_CURRENT_SOURCE_DIR}/client/client_donnie.cc      DESTINATION /usr/local/share/player/examples/libplayerc++/donnie/)
#INSTALL (FILES ${CMAKE_CURRENT_SOURCE_DIR}/client/CMakeLists.txt DESTINATION /usr/local/share/player/examples/libplayerc++/donnie/)
#INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${THE_TARGET}.so DESTINATION ${PLUGIN_PLAYERC_LIBRARY_DIRS})  #instala na pasta /usr/local/lib64
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libdonnie.so DESTINATION /usr/local/lib64/) 
