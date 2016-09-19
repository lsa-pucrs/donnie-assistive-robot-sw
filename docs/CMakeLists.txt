# from https://mementocodex.wordpress.com/2013/01/19/how-to-generate-code-documentation-with-doxygen-and-cmake-a-slightly-improved-approach/

#-- Add an Option to toggle the generation of the API documentation
option(BUILD_DOXYGEN "Use Doxygen to create the HTML based API documentation" OFF)
option(BUILD_MANUAL "Generates manual from Latex files " OFF)


if(BUILD_DOXYGEN)
  FIND_PACKAGE(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR
      "Doxygen is needed to build the documentation. Please install it correctly")
  endif()

  # =====================================
  # RUN DOXYGEN 
  # =====================================
  # run doxygen for tools
  configure_file(doxygen/doxy.conf    ${PROJECT_BINARY_DIR}/docs/doxy.conf  @COPYONLY)
  configure_file(doxygen/doxygen.dox  ${PROJECT_BINARY_DIR}/docs/doxygen.dox  @COPYONLY)
  # run doxygen for SystemC model
  configure_file(doxygen/doxy_sc.conf    ${PROJECT_BINARY_DIR}/docs/doxy_sc.conf  @COPYONLY)
  configure_file(doxygen/doxygen_sc.dox  ${PROJECT_BINARY_DIR}/docs/doxygen_sc.dox  @COPYONLY)
  # run doxygen for VHDL model
  configure_file(doxygen/doxy_vhdl.conf   ${PROJECT_BINARY_DIR}/docs/doxy_vhdl.conf  @COPYONLY)
  configure_file(doxygen/doxygen_vhdl.dox ${PROJECT_BINARY_DIR}/docs/doxygen_vhdl.dox  @COPYONLY)
  #-- Add a custom target to run Doxygen when ever the project is built
  # IF you do NOT want the documentation to be generated EVERY time you build the project
  # then leave out the 'ALL' keyword from the above command.
  add_custom_target (doxygen 
                     COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/docs/doxy.conf
                     SOURCES ${PROJECT_BINARY_DIR}/docs/doxy.conf
                     COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/docs/doxy_sc.conf
                     SOURCES ${PROJECT_BINARY_DIR}/docs/doxy_sc.conf
                     COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/docs/doxy_vhdl.conf
                     SOURCES ${PROJECT_BINARY_DIR}/docs/doxy_vhdl.conf
                    )
  ## do not generate doc when 'make all'
  set_target_properties(doxygen PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)

 
  # =====================================
  # RUN BROWSER
  # =====================================
  # Try to find `firefox` 
  find_program(
        BROWSER_EXECUTABLE
        NAMES firefox
        DOC "web browser to open the Doxygen documents"
    )
  if(NOT BROWSER_EXECUTABLE)
        message("WARNING: Firefox browser not found")
  endif()

  add_custom_target (doxygen-show
                     COMMAND ${BROWSER_EXECUTABLE} ${PROJECT_BINARY_DIR}/docs/software_html/html/index.html &
                     COMMAND ${BROWSER_EXECUTABLE} ${PROJECT_BINARY_DIR}/docs/sc_html/html/index.html &
                     COMMAND ${BROWSER_EXECUTABLE} ${PROJECT_BINARY_DIR}/docs/vhdl_html/html/index.html &
                    )
  ## do not generate doc when 'make all'
  set_target_properties(doxygen-show PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
endif()



if(BUILD_MANUAL)
  find_package(LATEX)
  if (NOT LATEX_FOUND)
     # include Latex
     add_subdirectory(manual)
  else()
     message(FATAL_ERROR "Latex is needed to build the documentation. Please install it correctly")
  endif()

endif()
