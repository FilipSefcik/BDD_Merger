cmake_minimum_required(VERSION 3.25)
project(BDD_Project C)

set(CMAKE_C_STANDARD 11)

# Define source directories
set(SHARED_DIR ${CMAKE_SOURCE_DIR}/Shared)
set(SERVER_FILES_DIR ${CMAKE_SOURCE_DIR}/Server_files)
set(KLIENT_FILES_DIR ${CMAKE_SOURCE_DIR}/Klient_files)
set(KLIENT_DIR ${CMAKE_SOURCE_DIR}/Klient)
set(SERVER_DIR ${CMAKE_SOURCE_DIR}/Server)

# Shared files
set(SHARED_SOURCES
        ${SHARED_DIR}/array_list.c
        ${SHARED_DIR}/array_list.h
        ${SHARED_DIR}/bdd_message.c
        ${SHARED_DIR}/bdd_message.h
        ${SHARED_DIR}/pla_function.h
        ${SHARED_DIR}/pla_function.c
        ${SHARED_DIR}/module.c
        ${SHARED_DIR}/module.h
        ${SHARED_DIR}/comm_utils.h
        ${SHARED_DIR}/comm_utils.c
)

# Server-specific files
set(SERVER_FILES_SOURCES
        ${SERVER_FILES_DIR}/bdd_server.c
        ${SERVER_FILES_DIR}/bdd_server.h
        ${SERVER_FILES_DIR}/module_manager.c
        ${SERVER_FILES_DIR}/module_manager.h
        ${SERVER_FILES_DIR}/server_utils.c
        ${SERVER_FILES_DIR}/server_utils.h
        ${SERVER_FILES_DIR}/server_interface.c
        ${SERVER_FILES_DIR}/server_interface.h
)

set (KLIENT_FILES_SOURCES
        ${KLIENT_FILES_DIR}/bdd_klient.c
        ${KLIENT_FILES_DIR}/bdd_klient.h
        ${KLIENT_FILES_DIR}/klient_interface.c
        ${KLIENT_FILES_DIR}/klient_interface.h
)

# Add Server executable
add_executable(Server
        ${SERVER_DIR}/main.c
        ${SHARED_SOURCES}
        ${SERVER_FILES_SOURCES}
)

# Add Klient executable
add_executable(Klient
        ${KLIENT_DIR}/main.c
        ${SHARED_SOURCES}
        ${KLIENT_FILES_SOURCES}
)

# Include directories
target_include_directories(Server PUBLIC ${SHARED_DIR} ${SERVER_FILES_DIR})
target_include_directories(Klient PUBLIC ${SHARED_DIR} ${KLIENT_FILES_DIR})
