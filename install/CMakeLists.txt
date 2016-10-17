MESSAGE( STATUS "Setup file" )

# this renames the setup file such that both the robot and 
# the desktop installation look alike
if (ROBOT_COMPUTER)
    SET(setup ${CMAKE_CURRENT_SOURCE_DIR}/setup-rpi.sh )
else ()
    SET(setup ${CMAKE_CURRENT_SOURCE_DIR}/setup.sh )
endif ()

INSTALL(FILES ${setup}
        DESTINATION ${DONNIE_PATH}
        RENAME setup.sh
)
