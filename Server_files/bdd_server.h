#ifndef BDD_SERVER_H
#define BDD_SERVER_H
#include <arpa/inet.h>
#include "../Shared/array_list.h"
#include "../Shared/bdd_message.h"

#define MAX_CLIENTS 10

/**
 * @brief Represents a server for managing client connections.
 *
 * Fields:
 * - client_sockets_: List of client socket descriptors.
 * - server_id_: Server's socket descriptor.
 * - server_addr_: Server's address and port information.
 */
typedef struct bdd_server {
    array_list client_sockets_;
    int server_id_;
    struct sockaddr_in server_addr_;
} bdd_server;

/**
 * @brief Holds arguments for threads handling client connections.
 *
 * Fields:
 * - server_: Pointer to the server instance.
 * - mutex_: Pointer to a mutex for synchronizing threads.
 * - client_id_: ID of the client associated with the thread.
 */
typedef struct thread_args {
    bdd_server* server_;
    pthread_mutex_t* mutex_;
    int client_id_;
} thread_args;

/**
 * @brief Initializes a thread_args structure.
 * @param this Pointer to the thread_args structure.
 * @param server Pointer to the server.
 * @param mutex Pointer to the mutex.
 * @param client_id ID of the client.
 */
void thread_args_init(thread_args *this, bdd_server *server, pthread_mutex_t *mutex, int client_id);

/**
 * @brief Destroys a thread_args structure.
 * @param this Pointer to the thread_args structure.
 */
void thread_args_destroy(thread_args *this);

/**
 * @brief Retrieves the server from thread_args.
 * @param this Pointer to the thread_args structure.
 * @return Pointer to the server.
 */
bdd_server* thread_args_get_server(thread_args* this);

/**
 * @brief Retrieves the mutex from thread_args.
 * @param this Pointer to the thread_args structure.
 * @return Pointer to the mutex.
 */
pthread_mutex_t* thread_args_get_mutex(thread_args *this);

/**
 * @brief Retrieves the client ID from thread_args.
 * @param this Pointer to the thread_args structure.
 * @return Client ID.
 */
int thread_args_get_client_id(thread_args *this);

/**
 * @brief Initializes a bdd_server instance.
 * @param this Pointer to the server instance.
 */
void bdd_server_init(bdd_server* this);

/**
 * @brief Destroys a bdd_server instance and closes all client connections.
 * @param this Pointer to the server instance.
 */
void bdd_server_destroy(bdd_server* this);

/**
 * @brief Ends a specific client session.
 * @param this Pointer to the server instance.
 * @param client_id ID of the client to disconnect.
 */
void bdd_server_end_session(bdd_server *this, int client_id);

/**
 * @brief Ends all client sessions.
 * @param this Pointer to the server instance.
 */
void bdd_server_end_sessions(bdd_server* this);

/**
 * @brief Retrieves the number of connected clients.
 * @param this Pointer to the server instance.
 * @return Number of connected clients.
 */
int bdd_server_get_client_count(bdd_server *this);

/**
 * @brief Binds the server to a specific address and port.
 * @param this Pointer to the server instance.
 * @param port Port to bind.
 * @param server_address Server's IP address ("any" for all interfaces).
 * @return true if successful, false otherwise.
 */
bool bdd_server_bind_server(bdd_server *this, int port, char *server_address);

/**
 * @brief Prepares the server to accept client connections.
 * @param this Pointer to the server instance.
 * @param client_count Number of clients to wait for.
 */
void bdd_server_standby(bdd_server* this, int client_count);

/**
 * @brief Sends a message to a specific client.
 * @param this Pointer to the server instance.
 * @param receiver_id ID of the receiving client.
 * @param message Pointer to the message to send.
 * @param mutex Optional mutex for synchronization.
 */
void bdd_server_send_message(bdd_server* this, int receiver_id, bdd_message* message, pthread_mutex_t *mutex);

/**
 * @brief Receives a message from a specific client.
 * @param this Pointer to the server instance.
 * @param sender_id ID of the sending client.
 * @param message Pointer to the message to receive.
 * @param mutex Optional mutex for synchronization.
 */
void bdd_server_receive_message(bdd_server *this, int sender_id, bdd_message* message, pthread_mutex_t *mutex);

/**
 * @brief Forwards a message from one client to another.
 * @param this Pointer to the server instance.
 * @param client_id ID of the sending client.
 * @param forwarded_message Pointer to the message to forward.
 * @param mutex Optional mutex for synchronization.
 */
void bdd_server_forward_message(bdd_server *this, int client_id, bdd_message* forwarded_message, pthread_mutex_t *mutex);

/**
 * @brief Thread function for handling message forwarding.
 * @param args Pointer to thread arguments.
 * @return Pointer to a result message (or NULL).
 */
void* bdd_server_forwarding_mode(void* args);

/**
 * @brief Sends modules to clients based on their distribution.
 * @param this Pointer to the server instance.
 * @param modules Pointer to the array list of modules.
 * @param distribution Array tracking module distribution across clients.
 */
void bdd_server_send_modules(bdd_server *this, array_list *modules, int* distribution);

/**
 * @brief Sends instructions to all clients.
 * @param this Pointer to the server instance.
 * @param instructions Array of instruction strings for each client.
 * @return true if all clients are still connected while receiving instructions
 * @return false if any of clients are no longer connected to server
 */
_Bool bdd_server_send_instructions(bdd_server *this, char **instructions);

/**
 * @brief Executes instructions by forwarding messages between clients.
 * @param this Pointer to the server instance.
 * @param result Pointer to store the result message.
 */
void bdd_server_execute_instructions(bdd_server *this, bdd_message *result);

/**
 * @brief Finds out whether is client with that fd still connected.
 *
 * @param client_fd id of client socket
 * @return true if is client connected (most probably)
 * @return false if client disconnected
 */
bool is_client_connected(int client_fd);

/**
 * @brief Tests the connections to all clients and removes any disconnected clients.
 *
 * @param this Pointer to the bdd_server instance.
 * @return the number of inactive and thus removed clients.
 */
int bdd_server_test_connections(bdd_server* this);

#endif //BDD_SERVER_H
