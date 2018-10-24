MESSAGE( STATUS "Configuring Player clients" )


IF(NOT ROBOT_COMPUTER)
	# if it is compiling for the desktop computer, then compile the following modules
	add_subdirectory(godonnie)
	add_subdirectory(alerts)
	add_subdirectory(ranger2vib)
	add_subdirectory(floor_desc)
	add_subdirectory(test)
ELSE()
	# if it is compiling for robot`s computer (RPi)
	add_subdirectory(alerts)
	add_subdirectory(ranger2vib)
ENDIF()
