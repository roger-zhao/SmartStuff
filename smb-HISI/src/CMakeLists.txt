AUX_SOURCE_DIRECTORY(. daemon_src) 

# Make build flags compiler specific
if (CMAKE_COMPILER_IS_GNUCC)
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set (CMAKE_C_FLAGS "-D_REETRANT -ggdb -fPIC -Wall -Wextra -pedantic -O0")
	elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
		set (CMAKE_C_FLAGS "-D_REETRANT -DNDEBUG -fPIC -Wall -Wextra -pedantic -O3")
	endif ()
endif (CMAKE_COMPILER_IS_GNUCC)
# set modbus lib
link_directories(${PROJECT_SOURCE_DIR}/lib)

IF(board)
    message(STATUS "target is board")
    add_executable (smb_hisi ${daemon_src})
    target_link_libraries(smb_hisi modbus rt pthread)
ELSE(board)
    message(STATUS "target is host pc")
    add_executable (smb_hisi ${daemon_src})
    target_link_libraries(smb_hisi modbusPC rt pthread)
ENDIF(board)

# The rule to install smb_hisi binary
install (TARGETS smb_hisi 
	RUNTIME
	DESTINATION .)
