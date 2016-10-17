IF(NOT ROBOT_COMPUTER AND BUILD_DOCS)
	# docs are only generated for the desktop computer, not for RPi

	# Add an Option to toggle the generation of the API documentation
	option(BUILD_DOXYGEN "Use Doxygen to create the HTML based API documentation" ON)
	option(BUILD_DOXYGEN_PDF "Create Doxygen Docs in PDF format. Requires Latex" OFF)
	option(BUILD_MANUAL "Generates manual from Latex files " OFF)

	# build doxygen documentation
	if(BUILD_DOXYGEN OR BUILD_DOXYGEN_PDF)
	  FIND_PACKAGE(Doxygen)
	  if (NOT DOXYGEN_FOUND)
		message(FATAL_ERROR
		  "Doxygen is needed to build the documentation. Please install it correctly")
	  endif()
	  
	  # run doxygen for tools
	  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/doxy.conf    ${CMAKE_CURRENT_BINARY_DIR}/doxy.conf  @COPYONLY)
	  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen/doxygen.dox  ${CMAKE_CURRENT_BINARY_DIR}/doxygen.dox  @COPYONLY)

	  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/html/html/index.html ${CMAKE_CURRENT_BINARY_DIR}/html/latex/refman.tex
						 COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxy.conf
						 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
						 COMMENT "Generating API documentation with Doxygen"
						 SOURCES ${CMAKE_CURRENT_BINARY_DIR}/doxy.conf
						)	  
	endif()

	if(BUILD_DOXYGEN)
	  MESSAGE( STATUS "Doxygen document in HTML" )
	  # =====================================
	  # RUN DOXYGEN for HTML
	  # =====================================
	  #-- Add a custom target to run Doxygen when ever the project is built
	  # IF you do NOT want the documentation to be generated EVERY time you build the project
	  # then leave out the 'ALL' keyword from the above command.
	  add_custom_target (doxygen ALL
						 DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/html/html/index.html
						)
	  ## do not generate doc when 'make all'
	  #set_target_properties(doxygen PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)

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

	  # runs Firefox to show the generated docs. However, 
	  # this rule depends on the execution of doxygen before to create the 'html/html/index.html' file
	  add_custom_target (doxygen-show
						 COMMAND ${BROWSER_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/html/html/index.html &
						 DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/html/html/index.html
						)
	  ## do not generate doc when 'make all'
	  set_target_properties(doxygen-show PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
	  
	endif()

	# =====================================
	# RUN DOXYGEN for PDF
	# =====================================
	if(BUILD_DOXYGEN_PDF)
		 MESSAGE( STATUS "Doxygen document in PDF" )
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
			 # runs Latex to generate the PDF file. However, 
			 # this rule depends on the execution of doxygen before to create the 'html/latex/refman.tex' file
			 add_custom_command(
						OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/html/latex/refman.pdf
						COMMAND make all
						WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/latex
						COMMENT "Generating API documentation with Doxygen and Latex"
						SOURCES ${CMAKE_CURRENT_BINARY_DIR}/html/latex/refman.tex
			 )			 
			 add_custom_target(doxygen-pdf ALL
				DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/html/latex/refman.pdf
			 )
			 
			 # install the generated PDF file
			 INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/html/latex/refman.pdf
					DESTINATION ${DONNIE_PATH}/docs/
					RENAME doxygen.pdf
			 )
		 endif()
	endif()

	# =====================================
	# BUILD LATEX MANUALS
	# =====================================
	if(BUILD_MANUAL)
	  MESSAGE( STATUS "Latex manuals in PDF" )
	  find_package(LATEX)
	  if (LATEX_COMPILER)
		 add_subdirectory(manual)
	  else()
		 message(FATAL_ERROR "Latex is required to build Doxygen in PDF. Please run 'sudo apt-get install texlive texlive-lang-english texlive-lang-portuguese texlive-latex-extra' or 'sudo apt-get install texlive-full'")
	  endif()

	endif()

# end robot computer
ENDIF()
