# from https://mementocodex.wordpress.com/2013/01/19/how-to-generate-code-documentation-with-doxygen-and-cmake-a-slightly-improved-approach/

IF(NOT ROBOT_COMPUTER AND BUILD_DOCS)
	# docs are only generated for the desktop computer, not for RPi

	#-- Add an Option to toggle the generation of the API documentation
	option(BUILD_DOXYGEN "Use Doxygen to create the HTML based API documentation" OFF)
	option(BUILD_MANUAL "Generates manual from Latex files " OFF)


	# build doxygen documentation
	if(BUILD_DOXYGEN)
	  FIND_PACKAGE(Doxygen)
	  if (NOT DOXYGEN_FOUND)
		message(FATAL_ERROR
		  "Doxygen is needed to build the documentation. Please install it correctly")
	  endif()

	  # =====================================
	  # RUN DOXYGEN for HTML
	  # =====================================
	  # run doxygen for tools
	  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/doxy.conf    ${CMAKE_CURRENT_BINARY_DIR}/doxy.conf  @COPYONLY)
	  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/doxygen.dox  ${CMAKE_CURRENT_BINARY_DIR}/doxygen.dox  @COPYONLY)
	  #-- Add a custom target to run Doxygen when ever the project is built
	  # IF you do NOT want the documentation to be generated EVERY time you build the project
	  # then leave out the 'ALL' keyword from the above command.
	  add_custom_target (doxygen 
						 COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxy.conf
						 SOURCES ${CMAKE_CURRENT_BINARY_DIR}/doxy.conf
						)
	  ## do not generate doc when 'make all'
	  set_target_properties(doxygen PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)

	  INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/html
			DESTINATION ${DONNIE_PATH}/docs/
	  )
	 
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
						 COMMAND ${BROWSER_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/html/html/index.html &
						)
	  ## do not generate doc when 'make all'
	  set_target_properties(doxygen-show PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
	  
	  # =====================================
	  # RUN DOXYGEN for PDF
	  # =====================================
	  option(BUILD_DOXYGEN_PDF "Create Doxygen Docs in PDF format. Requires Latex" OFF)
	  if(BUILD_DOXYGEN_PDF)
		  find_package(LATEX)
		  if (NOT LATEX_COMPILER)
			 message(FATAL_ERROR "Latex is required to build Doxygen in PDF. Please run 'sudo apt-get install texlive texlive-lang-english texlive-lang-portuguese texlive-latex-extra' or 'sudo apt-get install texlive-full'")
			 # uses 3.058 MB in disk - not recommended for VMs
			 #sudo apt-get install -y texlive-full
			 # uses 494MB in disk
			 # sudo apt-get install -y texlive texlive-lang-english texlive-lang-portuguese
			 # uses 558MB in disk
			 #sudo apt-get install -y texlive-latex-extra
		  else()
			 add_custom_target(doxygen-pdf
				COMMAND make
				WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/latex/
				DEPENDS doxygen
			 )
			 
			 INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/html/latex/refman.pdf
					DESTINATION ${DONNIE_PATH}/docs/
					RENAME doxygen.pdf
			 )
		  endif()
	  endif()
	  
	endif()


	# build Latex manuals
	if(BUILD_MANUAL)
	  find_package(LATEX)
	  if (LATEX_COMPILER)
		 add_subdirectory(manual)
	  else()
		 message(FATAL_ERROR "Latex is required to build Doxygen in PDF. Please run 'sudo apt-get install texlive texlive-lang-english texlive-lang-portuguese texlive-latex-extra' or 'sudo apt-get install texlive-full'")
	  endif()

	endif()

# end robot computer
ENDIF()

