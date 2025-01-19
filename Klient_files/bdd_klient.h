#ifndef BDD_KLIENT_H
#define BDD_KLIENT_H
#include "../Shared/bdd_message.h"
#include "../Shared/array_list.h"
#include "../Shared/module.h"

/**
 * @brief Represents a client connected to a BDD server.
 *
 * Fields:
 * - modules_: List of modules managed by the client.
 * - server_socket_: Socket descriptor for the server connection.
 * - instructions: String containing the client's instructions.
 */
typedef struct bdd_klient {
    array_list modules_;
    int server_socket_;
    char* instructions;
} bdd_klient;

/**
 * @brief Initializes a BDD client.
 * @param this Pointer to the client instance.
 */
void bdd_klient_init(bdd_klient *this);

/**
 * @brief Destroys a BDD client and frees its resources.
 * @param this Pointer to the client instance.
 */
void bdd_klient_destroy(bdd_klient *this);

/**
 * @brief Frees clients resources.
 * @param this Pointer to the client instance.
 */
void bdd_klient_clear_klient(bdd_klient* this);

/**
 * @brief Connects the client to the server.
 * @param this Pointer to the client instance.
 * @param server_ip IP address of the server.
 * @param server_port Port of the server.
 * @return true if the connection is successful, false otherwise.
 */
_Bool bdd_klient_connect(bdd_klient *this, char *server_ip, int server_port);

/**
 * @brief Disconnects the client from the server.
 * @param this Pointer to the client instance.
 */
void bdd_klient_disconnect(bdd_klient *this);

/**
 * @brief Sends a message to the server.
 * @param this Pointer to the client instance.
 * @param message Pointer to the message to send.
 */
void bdd_klient_send_message(bdd_klient *this, bdd_message *message);

/**
 * @brief Receives a message from the server.
 * @param this Pointer to the client instance.
 * @param message Pointer to the message to receive.
 */
void bdd_klient_receive_message(bdd_klient *this, bdd_message *message);

/**
 * @brief Receives instructions and modules from the server.
 * @param this Pointer to the client instance.
 * @return true if successful, false otherwise.
 */
_Bool bdd_klient_receive_info(bdd_klient *this);

/**
 * @brief Receives instructions from the server.
 * @param this Pointer to the client instance.
 * @return true if successful, false otherwise.
 */
bool bdd_klient_receive_instructions(bdd_klient *this);

/**
 * @brief Receives modules from the server.
 * @param this Pointer to the client instance.
 */
void bdd_klient_receive_modules(bdd_klient *this);

/**
 * @brief Finds and retrieves a module by name.
 * @param this Pointer to the client instance.
 * @param module_name Name of the module to retrieve.
 * @return Pointer to the found module, or NULL if not found.
 */
module* bdd_klient_get_module(bdd_klient* this, char* module_name);

/**
 * @brief Merges two modules based on an instruction.
 * @param this Pointer to the client instance.
 * @param instruction Instruction specifying the merge operation.
 */
void bdd_klient_merge_modules(bdd_klient *this, char *instruction);

/**
 * @brief Sends a module to another client based on an instruction.
 * @param this Pointer to the client instance.
 * @param instruction Instruction specifying the operation.
 */
void bdd_klient_send_instruction(bdd_klient *this, char *instruction);

/**
 * @brief Receives a module from another client based on an instruction.
 * @param this Pointer to the client instance.
 * @param instruction Instruction specifying the operation.
 */
void bdd_klient_recv_instruction(bdd_klient *this, char *instruction);

/**
 * @brief Marks the end of processing for a module based on an instruction.
 * @param this Pointer to the client instance.
 * @param instruction Instruction specifying the operation.
 */
void bdd_klient_end_instruction(bdd_klient *this, char *instruction);

/**
 * @brief Sends a finish signal to the server.
 * @param this Pointer to the client instance.
 */
void bdd_klient_finish_instruction(bdd_klient *this);

/**
 * @brief Executes instructions provided by the server.
 * @param this Pointer to the client instance.
 * @param execute_instruction Callback function to handle specific instructions.
 */
void bdd_klient_execute_instructions(bdd_klient *this, void(*execute_instruction)(bdd_klient *bdd_klient_, char *instruction));

/**
 * @brief Prints the modules managed by the client.
 * @param this Pointer to the client instance.
 */
void bdd_klient_print_modules(bdd_klient *this);

/**
 * @brief Prints the instructions received by the client.
 * @param this Pointer to the client instance.
 */
void bdd_klient_print_instructions(bdd_klient *this);

/**
 * @brief Finds out whether is server still connected.
 *
 * @param this Pointer to bdd_klient struct
 * @return true if connection is alive (most probably)
 * @return false if disconnected
 */
_Bool bdd_klient_test_connection(bdd_klient* this);


#endif //BDD_KLIENT_H
