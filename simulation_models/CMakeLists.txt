MESSAGE( STATUS "Stage simulation files" )
FILE( GLOB all_files "*" )

INSTALL(FILES ${all_files} 
        DESTINATION ${DONNIE_PATH}/test/stage
)
