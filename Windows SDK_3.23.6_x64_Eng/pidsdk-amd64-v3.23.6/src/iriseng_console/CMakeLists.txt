project(irisconsole)

set(SOURCE console.cpp)

add_executable(${PROJECT_NAME} ${SOURCE})

INCLUDE_DIRECTORIES (
                     ${sources_SOURCE_DIR}/irisengine/inc
                     ${sources_SOURCE_DIR}/common/inc
                     ${pidsdk_SOURCE_DIR}/external
					)

target_link_libraries(${PROJECT_NAME} irisengine)

SET_TARGET_PROPERTIES (${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}"
    VERSION 1.0 SOVERSION 1
    CXX_STANDARD 11
    )

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        )    
