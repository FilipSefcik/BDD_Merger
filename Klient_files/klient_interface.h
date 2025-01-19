#ifndef KLIENT_INTERFACE_H
#define KLIENT_INTERFACE_H

#include "bdd_klient.h"

/**
 * @brief Represents the interface for managing a BDD client.
 *
 * Fields:
 * - klient_: Instance of the BDD client.
 * - connected_: Boolean indicating if the client is connected to a server.
 */
typedef struct klient_interface {
    bdd_klient klient_;
    _Bool connected_;
} klient_interface;

/**
 * @brief Initializes the client interface.
 * @param this Pointer to the client interface.
 */
void klient_interface_init(klient_interface *this);

/**
 * @brief Destroys the client interface and frees its resources.
 * @param this Pointer to the client interface.
 */
void klient_interface_destroy(klient_interface *this);

/**
 * @brief Starts the client interface menu loop for user interaction.
 * @param this Pointer to the client interface.
 */
void klient_interface_start_interface(klient_interface *this);

/**
 * @brief Prints the main menu for the client interface.
 */
void klient_interface_print_menu(void);

/**
 * @brief Connects the client to the server.
 * @param this Pointer to the client interface.
 */
void klient_interface_connect(klient_interface *this);

/**
 * @brief Disconnects the client from the server.
 * @param this Pointer to the client interface.
 */
void klient_interface_disconnect(klient_interface *this);

/**
 * @brief Runs the main program for the client, including instruction execution.
 * @param this Pointer to the client interface.
 */
void klient_interface_run(klient_interface *this);

/**
 * @brief Test connections with server, if server is non-active, client disconnects.
 * @param this Pointer to the klient interface.
 */
void klient_interface_test_connection(klient_interface * this);

/**
 * @brief Scans input from user and checks if it is integer.
 * @param this Pointer to klient interface.
 * @param answer Pointer to buffer where input is stored.
 * @return True when int was input, false when not.
 */
_Bool klient_interface_get_int(klient_interface* this, int* answer);


#endif //KLIENT_INTERFACE_H
