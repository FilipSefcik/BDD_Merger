#include <ctype.h>
#include <locale.h>

#include "../Server_files/bdd_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../Server_files/server_interface.h"
#include "../Server_files/server_utils.h"
#include "../Shared/comm_utils.h"
#include "../Server_files/module_manager.h"

/**
 * @file main.c
 * @brief Entry point for the BDD server application.
 *
 * This program initializes and starts the server interface, which manages
 * the server's lifecycle, including binding, accepting clients, and executing
 * instructions.
 *
 * Includes:
 * - Initialization of the server interface.
 * - Menu-driven interaction for managing server functions.
 * - Cleanup of server resources upon termination.
 */
int main(int argc, char *argv[]) {
    server_interface interface;
    server_interface_init(&interface);
    server_interface_start_interface(&interface);
    server_interface_destroy(&interface);
    return 0;
}