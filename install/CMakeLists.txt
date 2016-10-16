MESSAGE( STATUS "Setup file" )

if (ROBOT_COMPUTER)
    SET(setup ${CMAKE_CURRENT_SOURCE_DIR}/setup-rpi.sh )
else ()
    SET(setup ${CMAKE_CURRENT_SOURCE_DIR}/setup.sh )
endif ()

INSTALL(FILES ${setup}
        DESTINATION ${DONNIE_PATH}
        RENAME setup.sh
)
