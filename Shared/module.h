#ifndef MODULE_H
#define MODULE_H

#include "array_list.h"
#include "pla_function.h"

/**
 * @brief Represents a mapping of a son's name to its position.
 *
 * Fields:
 * - son_name_: Name of the son (max 7 characters + null terminator).
 * - son_position_: Position of the son in the hierarchy.
 */
typedef struct son_name_and_pos {
    char son_name_[8];
    int son_position_;
} son_name_and_pos;

/**
 * @brief Initializes a son_name_and_pos structure.
 * @param this Pointer to the structure.
 * @param son_name Name of the son.
 * @param son_position Position of the son.
 */
void son_name_and_pos_init(son_name_and_pos *this, char *son_name, int son_position);

/**
 * @brief Destroys a son_name_and_pos structure.
 * @param item Pointer to the structure to destroy.
 */
void son_name_and_pos_destroy(const void* item);

/**
 * @brief Compares the name of a son with a given property.
 * @param item Pointer to the son_name_and_pos structure.
 * @param property Pointer to the name to compare.
 * @return true if names match, false otherwise.
 */
_Bool son_name_and_pos_compare_name(const void *item, void *property);

/**
 * @brief Serializes a son_name_and_pos structure into a buffer.
 * @param item Pointer to the structure to serialize.
 * @param serialized_payload Pointer to the output serialized buffer.
 * @return Size of the serialized buffer.
 */
size_t son_name_and_pos_serialize(void* item, void** serialized_payload);

/**
 * @brief Deserializes a buffer into a son_name_and_pos structure.
 * @param serialized_payload Pointer to the serialized buffer.
 * @param size Size of the serialized buffer.
 * @return Pointer to the deserialized structure.
 */
void* son_name_and_pos_deserialize(const void* serialized_payload, size_t size);

/**
 * @brief Represents a module with parent-child relationships and functionality.
 *
 * Fields:
 * - parent_: Pointer to the parent module.
 * - function_: Pointer to the module's function (PLA structure).
 * - son_map_: Array list mapping son's names to their positions.
 * - name_: Name of the module.
 * - path_: File path associated with the module.
 * - assigned_client_: ID of the client the module is assigned to.
 * - priority_: Priority level of the module.
 */
typedef struct module {
    struct module* parent_;
    pla_function* function_;
    array_list* son_map_;
    char* name_;
    char* path_;
    int assigned_client_;
    int priority_;
} module;

/**
 * @brief Initializes a module.
 * @param this Pointer to the module.
 * @param name Name of the module.
 */
void module_init(module* this, char* name);

/**
 * @brief Destroys a module and its resources.
 * @param this Pointer to the module.
 */
void module_destroy(module* this);

/**
 * @brief Destroys a module stored in an array list.
 * @param item Pointer to the module to destroy.
 */
void module_destroy_array_list(const void* item);

/**
 * @brief Gets the function of a module.
 * @param this Pointer to the module.
 * @return Pointer to the module's function.
 */
pla_function* module_get_function(module* this);

/**
 * @brief Gets the parent of a module.
 * @param this Pointer to the module.
 * @return Pointer to the parent module.
 */
module* module_get_parent(module* this);

/**
 * @brief Gets the position of a son module by name.
 * @param this Pointer to the module.
 * @param son_name Name of the son module.
 * @return Position of the son module, or -1 if not found.
 */
int module_get_son_position(module* this, char* son_name);

/**
 * @brief Gets the priority of a module.
 * @param this Pointer to the module.
 * @return Priority of the module.
 */
int module_get_priority(module* this);

/**
 * @brief Gets the number of variables in the module's function.
 * @param this Pointer to the module.
 * @return Number of variables.
 */
int module_get_var_count(module* this);

/**
 * @brief Gets the assigned client ID of the module.
 * @param this Pointer to the module.
 * @return Client ID.
 */
int module_get_assigned_client(module* this);

/**
 * @brief Gets the count of sons in the module's son map.
 * @param this Pointer to the module.
 * @return Count of sons.
 */
int module_get_son_count(module* this);

/**
 * @brief Gets the name of the module.
 * @param this Pointer to the module.
 * @return Name of the module.
 */
char* module_get_name(module* this);

/**
 * @brief Sets the client ID for the module.
 * @param this Pointer to the module.
 * @param client Client ID to assign.
 */
void module_set_client(module* this, int client);

/**
 * @brief Sets the parent of the module.
 * @param this Pointer to the module.
 * @param parent Pointer to the parent module.
 */
void module_set_parent(module* this, module* parent);

/**
 * @brief Sets the path associated with the module.
 * @param this Pointer to the module.
 * @param path File path to set.
 */
void module_set_path(module* this, char* path);


/**
 * @brief Compares the priority of two modules.
 * @param this Pointer to the first module.
 * @param other Pointer to the second module.
 * @return Difference in priority (positive if the first has higher priority).
 */
int module_priority_comparator(const void* this, const void* other);

/**
 * @brief Checks if the name of a module matches a given property.
 * @param item Pointer to the module.
 * @param property Pointer to the name to match.
 * @return true if names match, false otherwise.
 */
bool module_match_name(const void* item, void* property);


/**
 * @brief Adds a son's priority to the module and updates its own priority.
 * @param this Pointer to the module.
 * @param son_priority Priority of the son.
 */
void module_add_priority(module* this, int son_priority);

/**
 * @brief Adds a son module to the current module.
 * @param this Pointer to the module.
 * @param son Pointer to the son module.
 * @param son_position Position of the son module.
 */
void module_add_son(module* this, module* son, int son_position);

/**
 * @brief Adds a son with a specific position to the module.
 * @param this Pointer to the module.
 * @param son_name Name of the son.
 * @param son_position Position of the son.
 */
void module_add_son_position(module* this, char* son_name, int son_position);

/**
 * @brief Adjusts the positions of sons in the module after a new son is added.
 * @param this Pointer to the module.
 * @param added_son Name of the added son.
 * @param son_var_count Number of variables in the added son's function.
 */
void module_adjust_positions(module* this, char* added_son, int son_var_count);


/**
 * @brief Loads a PLA file for the module and initializes its function.
 * @param module_ptr Pointer to the module.
 */
void module_load_pla(const void* module_ptr);

/**
 * @brief Creates a function for the module with specified dimensions.
 * @param this Pointer to the module.
 * @param var_count Number of input variables.
 * @param line_count Number of lines in the function.
 */
void module_create_function(module* this, int var_count, int line_count);


/**
 * @brief Merges a son module into its parent.
 * @param parent Pointer to the parent module.
 * @param son Pointer to the son module.
 */
void module_merge_modules(module* parent, module* son);

/**
 * @brief Serializes a module into a buffer.
 * @param payload Pointer to the module to serialize.
 * @param serialized_payload Pointer to the output serialized buffer.
 * @return Size of the serialized buffer.
 */
size_t module_serialize(void* payload, void** serialized_payload);

/**
 * @brief Deserializes a module from a buffer.
 * @param serialized_payload Pointer to the serialized buffer.
 * @param size Size of the serialized buffer.
 * @return Pointer to the deserialized module.
 */
void* module_deserialize(const void* serialized_payload, size_t size);


/**
 * @brief Prints the sons in the module's son map.
 * @param item Pointer to the son to print.
 */
void print_son_map(const void* item);

/**
 * @brief Prints information about the module and its components.
 * @param this Pointer to the module.
 */
void module_print_out(module* this);


#endif //MODULE_H
