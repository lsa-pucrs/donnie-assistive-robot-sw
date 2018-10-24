MESSAGE( STATUS "Configuring Player drivers" )


IF(NOT ROBOT_COMPUTER)
	# if it is compiling for the desktop computer, then compile the following modules
	# some drivers are only related to the robot, like the raspicam and donnie driver
	add_subdirectory(gtts)
	add_subdirectory(soxplayer)
	add_subdirectory(genericblobfinder)
	add_subdirectory(vib_belt)
	add_subdirectory(floorplan_description)
ELSE()
	# if it is compiling for robot`s computer (RPi)
	# all the drivers are compiled
	add_subdirectory(donnie)
	add_subdirectory(genericblobfinder)
	add_subdirectory(gtts)
	add_subdirectory(raspicamdriver)
	add_subdirectory(soxplayer)
	add_subdirectory(vib_belt)
	#add_subdirectory(visual_odometry) // include this one only when its cmake is updated
	add_subdirectory(floorplan_description)
ENDIF()


