MESSAGE( STATUS "Configuring Player drivers" )


IF(NOT ROBOT_COMPUTER)
	# if it is compiling for the desktop computer, then compile the following modules
	# some drivers are only related to the robot, like the raspicam and donnie driver
	add_subdirectory(gtts)
	add_subdirectory(soxplayer)
	add_subdirectory(genericblobfinder)
ELSE()
	# if it is compiling for robot`s computer (RPi)
	# all the drivers are compiled
	add_subdirectory(donnie)
	add_subdirectory(genericblobfinder)
	add_subdirectory(gtts)
	add_subdirectory(raspicamdriver)
	add_subdirectory(soxplayer)
ENDIF()


