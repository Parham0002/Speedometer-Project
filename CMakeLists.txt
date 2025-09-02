cmake_minimum_required(VERSION 3.22)
project(av24tr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

# ---------------- paths ----------------
set(CLIENT_DIR desktop/client)
set(SERVER_DIR desktop/server)
set(RES_DIR    ${CLIENT_DIR}/res)
set(FONTS_DIR  "$ENV{HOME}/.local/share/fonts")


# ---------------- Qt components (GLOBAL) ----------------
set(QT_PROTO_COMPONENT_UART "SerialPort" CACHE STRING "Qt6 component for UART")

set(QT_PROTO_COMPONENT "${QT_PROTO_COMPONENT_TCP}")
if(UARTCOM)
  set(QT_PROTO_COMPONENT "${QT_PROTO_COMPONENT_UART}")
endif()

set(QT_COMPONENTS Core Widgets Multimedia ${QT_PROTO_COMPONENT})
find_package(Qt6 REQUIRED COMPONENTS ${QT_COMPONENTS})


# ---------------- client/server lists ----------------
set(CLIENT_SOURCES
  ${CLIENT_DIR}/src/canvas.cpp
  ${CLIENT_DIR}/src/comservice.cpp
  ${CLIENT_DIR}/src/window.cpp
  ${CLIENT_DIR}/main.cpp
)

set(SERVER_SOURCES
  ${SERVER_DIR}/src/comservice.cpp
  ${SERVER_DIR}/src/window.cpp
  ${SERVER_DIR}/main.cpp
)

set(CLIENT_TCP_SOURCES   ${CLIENT_DIR}/src/tcpservice.cpp)
set(SERVER_TCP_SOURCES   ${SERVER_DIR}/src/tcpservice.cpp)

set(CLIENT_UART_SOURCES  ${CLIENT_DIR}/src/uartservice.cpp )
set(SERVER_UART_SOURCES  ${SERVER_DIR}/src/uartservice.cpp)


# ---------------- targets ----------------
add_executable(client ${CLIENT_SOURCES})
target_include_directories(client PUBLIC shared ${CLIENT_DIR}/include)


add_executable(server ${SERVER_SOURCES})
target_include_directories(server PUBLIC shared ${SERVER_DIR}/include)




# ----- (UART or TCP) -----
if(UARTCOM)
    set(SERVER_UPORT "/dev/ttyUSB0")
    set(CLIENT_UPORT "/dev/ttyUSB1")
    add_compile_definitions(UARTCOM UART_SPORT="${SERVER_UPORT}" UART_CPORT="${CLIENT_UPORT}")

    # Add UART sources
    target_sources(client PRIVATE ${CLIENT_UART_SOURCES})
    target_sources(server PRIVATE ${SERVER_UART_SOURCES})

    # Link with SerialPort
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia SerialPort)
    target_link_libraries(client PUBLIC Qt6::Core Qt6::Widgets Qt6::Multimedia Qt6::SerialPort)
    target_link_libraries(server PUBLIC Qt6::Core Qt6::Widgets Qt6::SerialPort)

    # PlatformIO upload targets
    add_custom_target(upload_client
        COMMAND pio run -d ${CMAKE_SOURCE_DIR}/esp32/client -e esp32-c6-devkitc-1 -t upload --upload-port ${CLIENT_UPORT})

    add_custom_target(upload_server
        COMMAND pio run -d ${CMAKE_SOURCE_DIR}/esp32/server -e esp32-c6-devkitc-1 -t upload --upload-port ${SERVER_UPORT})

    add_custom_target(upload)
    add_dependencies(upload upload_client upload_server)


else()

    # Add TCP sources
    target_sources(client PRIVATE ${CLIENT_TCP_SOURCES})
    target_sources(server PRIVATE ${SERVER_TCP_SOURCES})

    # No SerialPort here
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia)
    target_link_libraries(client PUBLIC Qt6::Core Qt6::Widgets Qt6::Multimedia)
    target_link_libraries(server PUBLIC Qt6::Core Qt6::Widgets)
endif()


# ---------------- sound  ----------------
set(WAV_FILE "sound.wav")
if(NOT EXISTS "${CMAKE_BINARY_DIR}/${WAV_FILE}")
    file(COPY ${RES_DIR}/${WAV_FILE} DESTINATION ${CMAKE_BINARY_DIR})
endif()

# ---------------- fonts  ----------------
set(FONT_NAME MaterialIcons.ttf)
set(SOURCE_FONT_PATH "${CMAKE_SOURCE_DIR}/desktop/client/res/${FONT_NAME}")
set(DEST_FONT_DIR "$ENV{HOME}/.local/share/fonts")
set(DEST_FONT_PATH "${DEST_FONT_DIR}/${FONT_NAME}")
if(NOT EXISTS "${DEST_FONT_PATH}")
  message(STATUS "Font saknas – installerar till: ${DEST_FONT_PATH}")
  file(MAKE_DIRECTORY "${DEST_FONT_DIR}")
  file(COPY "${SOURCE_FONT_PATH}" DESTINATION "${DEST_FONT_DIR}")
  execute_process(
    COMMAND fc-cache -f -v "${DEST_FONT_DIR}"
    RESULT_VARIABLE FC_RESULT
    OUTPUT_VARIABLE FC_OUTPUT
    ERROR_VARIABLE FC_ERROR
  )
  if(FC_RESULT EQUAL 0)
    message(STATUS "fc-cache uppdaterad:\n${FC_OUTPUT}")
  else()
    message(WARNING "fc-cache misslyckades:\n${FC_ERROR}")
  endif()
else()
  message(STATUS "Fonten finns redan: ${DEST_FONT_PATH}")
endif()
# --------------------------------------------------------------------------------------------

add_custom_target(use_uart
  COMMAND ${CMAKE_COMMAND} -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR} -DUARTCOM=ON
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target client server --parallel
  USES_TERMINAL
  COMMENT "Switching to UART (UARTCOM=ON) and rebuilding client+server"
)

add_custom_target(use_tcp
  COMMAND ${CMAKE_COMMAND} -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR} -DUARTCOM=OFF
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target client server --parallel
  USES_TERMINAL
  COMMENT "Switching to TCP (UARTCOM=OFF) and rebuilding client+server"
)
# --------------------------------------------------------------------------------------------

  

