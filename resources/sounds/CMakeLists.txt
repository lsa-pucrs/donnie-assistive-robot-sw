MESSAGE( STATUS "Sound resource files" )
FILE( GLOB wavs "*.wav" )
FILE( GLOB mp3s "*.mp3" )

INSTALL(FILES ${wavs} ${mp3s}
        DESTINATION ${DONNIE_PATH}/resources/sounds
)
