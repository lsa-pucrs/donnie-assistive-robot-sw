CMAKE_MINIMUM_REQUIRED (VERSION 2.8 FATAL_ERROR)
SET (CLIENTNAME "GoDonnie")
PROJECT (${CLIENTNAME})

message(STATUS "Configuring '${CLIENTNAME}'")

#########
# check DONNIE_PATH
#########
if(DEFINED ENV{DONNIE_PATH})
   SET(DONNIE_PATH "$ENV{DONNIE_PATH}")
   #message(STATUS "DONNIE_PATH = ${DONNIE_PATH}")
else()
   message(FATAL_ERROR "DONNIE_PATH is NOT defined")
endif()

# check the compiler
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(FATAL "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
# required to support 'acentos' in Portuguese
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexec-charset=iso-8859-1")


# Include this CMake module to get most of the settings needed to build
SET (CMAKE_MODULE_PATH "/usr/local/share/cmake/Modules")

#########
# check depedencies - java -lreadline -lantlr3c
#########
# required to run antlr
find_package(Java REQUIRED)

#########
# check depedencies - readline
#########
find_path( LREADLINE_INCLUDE_DIR readline.h 
	HINTS /usr/include/readline/ /usr/include/
	DOC "Libreadline include dir" )
find_library( LREADLINE_LIB 
	NAMES readline 
	HINTS /usr/lib/x86_64-linux-gnu/ /usr/lib/arm-linux-gnueabihf
	DOC "Libreadline lib" REQUIRED)

IF( NOT LREADLINE_LIB )
  MESSAGE( FATAL_ERROR "Libreadline not found. Please install 'sudo apt-get install libreadline-dev'. aborting" )
ENDIF()
message(STATUS "Readline Library -- ${LREADLINE_LIB}")
message(STATUS "Readline Include -- ${LREADLINE_INCLUDE_DIR}")

#########
# check depedencies - antlr3c
#########
find_path( LANTLR3_INCLUDE_DIR antlr3.h DOC "Libantlr3 include dir" )
find_library( LANTLR3_LIB 
	NAMES antlr3c
	HINTS /usr/lib/x86_64-linux-gnu/ /usr/lib/arm-linux-gnueabihf
	DOC "Liantlr3 lib" REQUIRED)

IF( NOT LANTLR3_LIB )
  MESSAGE( FATAL_ERROR "Liantlr3 not found. Please install 'sudo apt-get install libantlr3c-dev'. aborting" )
ENDIF()
message(STATUS "ANTLR3 Library -- ${LANTLR3_LIB}")
message(STATUS "ANTLR3 Include -- ${LANTLR3_INCLUDE_DIR}")

#########
# run antlr
#########
ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/GoDonnie.tokens
  COMMAND java -jar ${CMAKE_CURRENT_SOURCE_DIR}/Antlr/antlr-3.2.jar -o ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/GoDonnie.g
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/GoDonnie.g
  COMMENT "Generating GoDonnie parser"
)

ADD_CUSTOM_TARGET(GoDonnieParser ALL
   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/GoDonnie.tokens
)

# hack to break the depedency. This is required if the file does not yet exist in compile time 
# https://samthursfield.wordpress.com/2015/11/21/cmake-dependencies-between-targets-and-files-and-custom-commands/
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/GoDonnieParser.c
    PROPERTIES GENERATED TRUE
    )
    
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/GoDonnieLexer.c
    PROPERTIES GENERATED TRUE
    )

#########
# set Player
#########
INCLUDE (UsePlayerC++)

PLAYER_ADD_PLAYERCPP_CLIENT (${CLIENTNAME} 
	SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Compiler.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/DonnieClient.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/GoDonnie.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/utils.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Exception.cpp ${CMAKE_CURRENT_SOURCE_DIR}/Historic.cpp ${CMAKE_CURRENT_BINARY_DIR}/GoDonnieLexer.c  ${CMAKE_CURRENT_BINARY_DIR}/GoDonnieParser.c
	INCLUDEDIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../../utils/)

# add the includes and libs to build it
TARGET_LINK_LIBRARIES (${CLIENTNAME} ${LREADLINE_LIB} ${LANTLR3_LIB}) 
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR} ${LANTLR3_INCLUDE_DIR} ${LREADLINE_INCLUDE_DIR} )

# godonnie depends on GoDonnieParser. This makes sure that the parser is built before the GoDonnie interpreter
add_dependencies(GoDonnie GoDonnieParser)

# Install library and example files
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${CLIENTNAME} DESTINATION ${DONNIE_PATH}/bin) 
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/square.gd DESTINATION ${DONNIE_PATH}/test/${CLIENTNAME})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/test.gd DESTINATION ${DONNIE_PATH}/test/${CLIENTNAME})
