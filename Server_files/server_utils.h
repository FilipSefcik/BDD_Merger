#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H
#include "../Shared/module.h"
#include "../Shared/array_list.h"

/**
 * @brief Distributes modules among clients using a round-robin approach.
 * @param modules Pointer to an array list of modules.
 * @param client_count Total number of clients.
 * @param distribution Array to store the distribution of modules per client.
 */
void divider_default_divide(array_list* modules, int client_count, int* distribution);

/**
 * @brief Generates instructions for module communication and merging.
 * @param mod Pointer to the current module.
 * @param instructions Array to store two strings with instructions.
 * @param distribution Array tracking the distribution of modules per client.
 *
 * Instruction format:
 * - "MERG parent_module current_module": Merge current module into parent.
 * - "SEND current_module client_id": Send current module to a client.
 * - "RECV current_module MERG parent_module current_module": Receive and merge.
 * - "END current_module": Indicates end of operation for the module.
 */
void give_instruction(module *mod, char *instructions[2], int* distribution);
#endif //SERVER_UTILS_H
