MESSAGE( STATUS "color resource files" )
FILE( GLOB txts "*.txt" )

INSTALL(FILES ${txts} 
        DESTINATION ${DONNIE_PATH}/resources/color_files
)
