cmake_minimum_required(VERSION 3.16)

# Make a Version header file
# Pass the target library name library type 
function(version_header TARGET_LIB TARGET_TYPE)
	#find_package (Bash REQUIRED COMPONENTS Interpreter)
	set(VERSION_BINARY_DIR "${CMAKE_CURRENT_LIST_DIR}/generated")
	set(VERSION_HEADER "${VERSION_BINARY_DIR}/version.h")
	set(TARGET_VERSION "${TARGET_LIB}_version_header")

	add_custom_target(${TARGET_VERSION} DEPENDS ${VERSION_HEADER})
	add_custom_command(
	    OUTPUT ${VERSION_HEADER}
	    COMMAND ${CMAKE_SOURCE_DIR}/script/version.sh ${CMAKE_SOURCE_DIR}
	    COMMENT "Generate version header"
	    VERBATIM
	)

	add_dependencies(${TARGET_LIB}
	    ${TARGET_VERSION}
	)

	target_include_directories(${TARGET_LIB} ${TARGET_TYPE}
	    ${VERSION_BINARY_DIR}
	)
endfunction()


project(ncmqtt C ASM)

set(SOURCE_FILES
    src/main.c
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

# Imposta la variabile PAHO_MQTT_DIR
set(PAHO_MQTT_DIR "" CACHE PATH "Percorso alla cartella Paho MQTT")

if (APPLE)
	set(LIBRARY_EXT ".dylib")
else()
	set(LIBRARY_EXT ".so")
endif()


if (NOT PAHO_MQTT_DIR)
    find_package(PahoMqtt REQUIRED)
else()
    # Imposta la variabile PAHO_MQTT_INCLUDE_DIR manualmente
    set(PAHO_MQTT_INCLUDE_DIR "${PAHO_MQTT_DIR}/src")

    # Imposta la variabile PAHO_MQTT_LIBRARIES manualmente
    set(PAHO_MQTT_LIBRARIES "${PAHO_MQTT_DIR}/build/src/libpaho-mqtt3c${LIBRARY_EXT}")  # Sostituisci con il percorso esatto alla libreria Paho MQTT

    # Assicurati che le variabili PAHO_MQTT_INCLUDE_DIR e PAHO_MQTT_LIBRARIES siano dichiarate come cache
    set(PAHO_MQTT_INCLUDE_DIR ${PAHO_MQTT_INCLUDE_DIR} CACHE INTERNAL "Percorso alla cartella delle intestazioni di Paho MQTT")
    set(PAHO_MQTT_LIBRARIES ${PAHO_MQTT_LIBRARIES} CACHE INTERNAL "Percorso alla libreria Paho MQTT")
endif()


include_directories(
	include
	${PAHO_MQTT_INCLUDE_DIR}
)


add_executable(ncmqtt ${SOURCE_FILES})

add_dependencies(ncmqtt version_header)

add_custom_target(version_header DEPENDS ${VERSION_HEADER})

version_header(ncmqtt PRIVATE)

# Link al percorso delle librerie Paho MQTT

target_link_libraries(ncmqtt PRIVATE ${PAHO_MQTT_LIBRARIES} util m)

