#include <ctype.h>
#include <locale.h>
#include "../Klient_files/klient_interface.h"

/**
 * @file main.c
 * @brief Entry point for the BDD client application.
 *
 * This program initializes and starts the client interface, which manages
 * the client's lifecycle, including connecting to the server, receiving
 * instructions, and executing tasks based on the instructions.
 *
 * Includes:
 * - Initialization of the client interface.
 * - Menu-driven interaction for managing client functions.
 * - Cleanup of client resources upon termination.
 */
int main(int argc, char *argv[]) {
    klient_interface interface;
    klient_interface_init(&interface);
    klient_interface_start_interface(&interface);
    klient_interface_destroy(&interface);
    return 0;
}