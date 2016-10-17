
# copy all files to binary dir
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/software_manual DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/hardware_manual DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/user_manual DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#each command generates a documment
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/software_manual/main.pdf
	COMMAND make
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/software_manual
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/hardware_manual/main.pdf
	COMMAND make
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/hardware_manual
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/user_manual/main.pdf
	COMMAND make
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/user_manual
)

# the rule 'manual' executes all three manuals
add_custom_target(manual ALL
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/software_manual/main.pdf ${CMAKE_CURRENT_BINARY_DIR}/hardware_manual/main.pdf ${CMAKE_CURRENT_BINARY_DIR}/user_manual/main.pdf
	COMMENT "Generating manuals with Latex"
)

# where the doc files are installed 
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/software_manual/main.pdf
	DESTINATION ${DONNIE_PATH}/docs/
	RENAME software_manual.pdf
)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/hardware_manual/main.pdf
	DESTINATION ${DONNIE_PATH}/docs/
	RENAME hardware_manual.pdf
)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/user_manual/main.pdf
	DESTINATION ${DONNIE_PATH}/docs/
	RENAME user_manual.pdf
)
