#ifndef SERVER_INTERFACE_H
#define SERVER_INTERFACE_H
#include "bdd_server.h"

/**
 * @brief Represents the interface for managing a BDD server.
 *
 * Fields:
 * - server_: Instance of the BDD server.
 * - binded_: Boolean indicating if the server is bound to an address and port.
 */
typedef struct server_interface {
    bdd_server server_;
    _Bool binded_;
} server_interface;


/**
 * @brief Initializes the server interface.
 * @param this Pointer to the server interface.
 */
void server_interface_init(server_interface* this);

/**
 * @brief Destroys the server interface and its resources.
 * @param this Pointer to the server interface.
 */
void server_interface_destroy(server_interface* this);

/**
 * @brief Starts the server interface menu loop for user interaction.
 * @param this Pointer to the server interface.
 */
void server_interface_start_interface(server_interface* this);

/**
 * @brief Prints the main menu for the server interface.
 */
void server_interface_print_menu(void);

/**
 * @brief Binds the server to a specific IP address and port.
 * @param this Pointer to the server interface.
 */
void server_interface_bind(server_interface* this);

/**
 * @brief Puts the server in standby mode to accept new client connections.
 * @param this Pointer to the server interface.
 */
void server_interface_standby(server_interface* this);

/**
 * @brief Test connections with clients, if there are non-active, they get removed.
 * @param this Pointer to the server interface.
 */
void server_interface_test_connections(server_interface * this);

/**
 * @brief Checks if a file exists at the specified path.
 * @param filename Path to the file.
 * @return true if the file exists, false otherwise.
 */
_Bool file_exists(char* filename);

/**
 * @brief Loads the configuration file path for the server.
 * @param this Pointer to server interfce
 * @param conf_path Pointer to the buffer storing the configuration file path.
 * @param path_size Size of the buffer.
 */
void server_interface_load_conf_file(server_interface* this, char* conf_path, size_t path_size);

/**
 * @brief Runs the main server program, including module management and client communication.
 * @param this Pointer to the server interface.
 */
void server_interface_run(server_interface* this);

/**
 * @brief Scans input from user and checks if it is integer.
 * @param this Pointer to server interface.
 * @param answer Pointer to buffer where input is stored.
 * @return True when int was input, false when not.
 */
_Bool server_interface_get_int(server_interface* this, int* answer);

#endif //SERVER_INTERFACE_H
