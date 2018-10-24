MESSAGE( STATUS "Stage simulation files" )
FILE( GLOB all_files "*.*"  )

INSTALL(FILES ${all_files} 
        DESTINATION ${DONNIE_PATH}/test/stage
)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/maps
	DESTINATION ${DONNIE_PATH}/test/stage
)

#ADD_SUBDIRECTORY(maps)
