MESSAGE( STATUS "Setup file" )

# this set the main variables and renames the setup file such that 
# both the robot and the desktop installation look alike
if (ROBOT_COMPUTER)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup-rpi.sh.in setup.sh @ONLY)
    #SET(setup ${CMAKE_CURRENT_SOURCE_DIR}/setup-rpi.sh )
else ()
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.sh.in setup.sh @ONLY)
    #SET(setup ${CMAKE_CURRENT_SOURCE_DIR}/setup.sh )
endif ()


#INSTALL(FILES ${setup}
INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/setup.sh
        DESTINATION ${DONNIE_PATH}
#        RENAME setup.sh
)

# auxiliar rasp scripts
if (ROBOT_COMPUTER)
	INSTALL(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/softshutdown.py ${CMAKE_CURRENT_SOURCE_DIR}/speechIP.py
			DESTINATION ${DONNIE_PATH}/scripts
	)
endif ()
