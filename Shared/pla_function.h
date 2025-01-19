#ifndef PLA_FUNCTION_H
#define PLA_FUNCTION_H
#include <stddef.h>

/**
 * @brief Represents a PLA (Programmable Logic Array) function.
 *
 * Fields:
 * - variables_: 2D array of input variable combinations.
 * - fun_values_: Array of function output values ('0' or '1').
 * - fun_val_count_: Count of function values ('0' and '1').
 * - num_lines_: Number of lines (rows) in the PLA.
 * - var_count_: Number of input variables.
 */
typedef struct pla_function {
    char** variables_;
    char* fun_values_;
    int fun_val_count_[2];
    int num_lines_;
    int var_count_;
} pla_function;

/**
 * @brief Allocates memory for the PLA function values.
 * @param this Pointer to the PLA function.
 */
void pla_function_alloc_values(pla_function* this);

/**
 * @brief Initializes a PLA function structure.
 * @param this Pointer to the PLA function.
 * @param var_count Number of input variables.
 * @param line_count Number of lines in the PLA.
 */
void pla_function_init(pla_function* this, int var_count, int line_count);

/**
 * @brief Frees memory allocated for the PLA function values.
 * @param this Pointer to the PLA function.
 */
void pla_function_free_values(pla_function* this);

/**
 * @brief Destroys a PLA function structure and frees its memory.
 * @param this Pointer to the PLA function.
 */
void pla_function_destroy(pla_function* this);

/**
 * @brief Assigns the contents of one PLA function to another.
 * @param this Pointer to the destination PLA function.
 * @param other Pointer to the source PLA function.
 */
void pla_function_assign(pla_function* this, pla_function* other);


/**
 * @brief Gets the variables array of the PLA function.
 * @param this Pointer to the PLA function.
 * @return Pointer to the variables array.
 */
char** pla_function_get_variables(pla_function* this);

/**
 * @brief Gets the function values array of the PLA function.
 * @param this Pointer to the PLA function.
 * @return Pointer to the function values array.
 */
char* pla_function_get_function_values(pla_function* this);

/**
 * @brief Gets the number of lines in the PLA function.
 * @param this Pointer to the PLA function.
 * @return Number of lines.
 */
int pla_function_get_num_lines(pla_function* this);

/**
 * @brief Gets the number of input variables in the PLA function.
 * @param this Pointer to the PLA function.
 * @return Number of input variables.
 */
int pla_function_get_var_count(pla_function* this);

/**
 * @brief Gets the count of function values ('0' and '1').
 * @param this Pointer to the PLA function.
 * @return Array containing counts of '0' and '1'.
 */
int* pla_function_get_fun_val_count(pla_function* this);

/**
 * @brief Adds a new line to the PLA function.
 * @param this Pointer to the PLA function.
 * @param new_vars Pointer to the new input variable combination.
 * @param value Function output value ('0' or '1').
 * @param line_num Line number to add the data to.
 */
void pla_function_add_line(pla_function* this, const char* new_vars, char value, int line_num);

/**
 * @brief Prints the PLA function (variables and their corresponding values).
 * @param this Pointer to the PLA function.
 */
void pla_function_print_function(pla_function* this);

/**
 * @brief Replaces a character in a string at a specified position.
 * @param before Original string.
 * @param before_length Length of the original string.
 * @param position Position of the character to replace.
 * @param input Replacement string.
 * @param input_length Length of the replacement string.
 * @param result Output buffer to store the result.
 */
void pla_function_replace_char(const char* before, int before_length, int position, const char* input, int input_length, char* result);

/**
 * @brief Frees memory allocated for sorted arrays.
 * @param sorted Pointer to the sorted arrays.
 * @param group_count Number of groups in the sorted arrays.
 */
void pla_function_free_sort(char*** sorted, int group_count);

/**
 * @brief Sorts the PLA function variables by their function values.
 * @param this Pointer to the PLA function.
 * @return Pointer to an array containing sorted variables by '0' and '1'.
 */
char*** pla_function_sort_by_function(pla_function* this);

/**
 * @brief Sorts the PLA function variables by a specific position.
 * @param this Pointer to the PLA function.
 * @param position Position to sort by.
 * @param match_count Array to store the count of matches for '0', '1', and '-'.
 * @return Pointer to an array containing sorted variables by position.
 */
char*** pla_function_sort_by_position(pla_function* this, int position, int* match_count);

/**
 * @brief Inputs additional variables from another PLA function.
 * @param this Pointer to the target PLA function.
 * @param other Pointer to the source PLA function.
 * @param position Position to insert the variables.
 */
void pla_function_input_variables(pla_function* this, pla_function* other, int position);

/**
 * @brief Gets the function value for a specific variable combination.
 * @param this Pointer to the PLA function.
 * @param indexed_variables Pointer to the variable combination.
 * @return Corresponding function value ('0' or '1').
 */
char pla_function_get_fun_value(pla_function* this, const char* indexed_variables);

/**
 * @brief Serializes a PLA function into a buffer.
 * @param payload Pointer to the PLA function.
 * @param serialized_payload Pointer to the output serialized buffer.
 * @return Size of the serialized buffer.
 */
size_t pla_function_serialize(void* payload, void** serialized_payload);

/**
 * @brief Deserializes a PLA function from a buffer.
 * @param serialized_payload Pointer to the serialized buffer.
 * @param size Size of the serialized buffer.
 * @return Pointer to the deserialized PLA function.
 */
void* pla_function_deserialize(const void* serialized_payload, size_t size);

#endif //PLA_FUNCTION_H
