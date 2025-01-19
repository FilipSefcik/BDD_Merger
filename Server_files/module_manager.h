#ifndef MODULE_MANAGER_H
#define MODULE_MANAGER_H
#include "../Shared/array_list.h"
#include "../Shared/module.h"

/**
 * @brief Manages a collection of modules and client instructions.
 *
 * Fields:
 * - modules_: Array list of modules managed by the manager.
 * - instructions_: Array of instruction strings for clients.
 * - client_count_: Number of clients.
 */
typedef struct module_manager {
    array_list modules_;
    char** instructions_;
    int client_count_;
} module_manager;

/**
 * @brief Initializes a module manager.
 * @param this Pointer to the module manager.
 * @param client_count Number of clients.
 */
void module_manager_init(module_manager *this, int client_count);

/**
 * @brief Frees memory allocated for a module in the manager.
 * @param item Pointer to the module to free.
 */
void module_manager_free_module(const void *item);

/**
 * @brief Destroys a module manager and frees its resources.
 * @param this Pointer to the module manager.
 */
void module_manager_destroy(module_manager *this);

/**
 * @brief Retrieves the array list of modules.
 * @param this Pointer to the module manager.
 * @return Pointer to the array list of modules.
 */
array_list* module_manager_get_modules(module_manager *this);

/**
 * @brief Retrieves the array of client instructions.
 * @param this Pointer to the module manager.
 * @return Pointer to the array of client instructions.
 */
char** module_manager_get_instructions(module_manager *this);

/**
 * @brief Loads modules and their configurations from a file.
 * @param this Pointer to the module manager.
 * @param conf_file_path Path to the configuration file.
 */
void module_manager_load(module_manager *this, const char *conf_file_path);

/**
 * @brief Loads module definitions from a configuration file.
 * @param this Pointer to the module manager.
 * @param conf_file_path Path to the configuration file.
 */
void module_manager_load_modules(module_manager *this, const char *conf_file_path);

/**
 * @brief Loads PLA files for all modules managed by the manager.
 * @param this Pointer to the module manager.
 */
void module_manager_load_plas(module_manager *this);

/**
 * @brief Adds an instruction for a specific client.
 * @param this Pointer to the module manager.
 * @param client_id ID of the client.
 * @param instruction Instruction string to add.
 */
void module_manager_add_instruction(module_manager *this, int client_id, char* instruction);

/**
 * @brief Creates instructions for all clients based on module distribution.
 * @param this Pointer to the module manager.
 * @param distribution Array tracking module distribution across clients.
 * @param give_instruction Callback function to generate instructions for a module.
 */
void module_manager_create_instructions(module_manager *this, int* distribution, void(*give_instruction)(module *mod, char *instructions[2], int* distribution));

/**
 * @brief Prints the instructions for all clients.
 * @param this Pointer to the module manager.
 */
void module_manager_print_instructions(module_manager *this);

/**
 * @brief Prints details of all modules managed by the manager.
 * @param this Pointer to the module manager.
 */
void module_manager_print_modules(module_manager *this);


#endif //MODULE_MANAGER_H
