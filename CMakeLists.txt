
cmake_minimum_required(VERSION 3.0)

project(Foo VERSION 1.2.3)

#####################################
# cmake options
OPTION (BUILD_DOCS "Build Documents" OFF)
OPTION (BUILD_EXAMPLES "Build Examples" OFF)
OPTION (BUILD_DONNIE_DRIVER "Build Drivers (only if the robot is available)" OFF)


#####################################
# Set the default build type
IF (NOT CMAKE_BUILD_TYPE)
  SET (CMAKE_BUILD_TYPE "release" CACHE STRING 
    "Choose the type of build, options are: release (default) debug profile" FORCE)
ENDIF (NOT CMAKE_BUILD_TYPE)
STRING(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
MESSAGE( STATUS "Build type ${CMAKE_BUILD_TYPE}" )

# Monolithic version
add_subdirectory(driver)
add_subdirectory(client)
add_subdirectory(docs)


######################################
# Externally Compile/Install Player
######################################
# http://mirkokiefer.com/blog/2013/03/cmake-by-example/

ExternalProject_Add(project_docs
  URL http://luajit.org/download/LuaJIT-2.0.1.tar.gz
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/luajit-2.0.1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND make
  INSTALL_COMMAND make install PREFIX=${CMAKE_CURRENT_BINARY_DIR}/luajit-2.0.1
)

ExternalProject_Get_Property(project_luajit install_dir)


add_library(luajit STATIC IMPORTED)
set_property(TARGET luajit PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libluajit-5.1.a)
add_dependencies(luajit project_luajit)

add_executable(myapp main.c)

include_directories(${install_dir}/include/luajit-2.0)
target_link_libraries(myapp luajit)

######################################
# Externally Compile/Install Stage
######################################
