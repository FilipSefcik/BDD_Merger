#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @file array_list.h
 * @brief Implementation of a dynamic array list in C.
 *
 * This module provides a generic implementation of a dynamic array list
 * with functionality for initialization, manipulation, and serialization.
 * The array list supports operations such as adding, inserting, removing,
 * and retrieving elements, as well as resizing and exporting to files.
 *
 * Features:
 * - Dynamic resizing with an initial capacity.
 * - Support for custom element types and sizes.
 * - Serialization and deserialization for binary and text formats.
 * - Iterators for traversing the list.
 *
 * Usage:
 * - Initialize the list with `array_list_init`.
 * - Add, insert, or remove elements as needed.
 * - Serialize or export the list if required.
 * - Destroy the list with `array_list_destroy` to free resources.
 */
typedef struct array_list {
    int element_size_;
    int size_;
    int capacity_;
    void* array_;
} array_list;

typedef struct array_list_iterator {
    const array_list *list_;
    int current_index_;
} array_list_iterator;

void array_list_init(array_list *this, int element_size);
void array_list_destroy(array_list *this);

void array_list_clear(array_list *this);
int array_list_get_size(const array_list* this);
void array_list_add(array_list* this, const void* value);
_Bool array_list_insert(array_list* this, int index, const void* value);
_Bool array_list_set(array_list *this, int index, const void *value);
_Bool array_list_try_get(const array_list* this, int index, void* value);
void array_list_remove_last(array_list* this);
_Bool array_list_remove_at(array_list *this, int index);
_Bool array_list_assign(array_list* this, array_list* other);
void array_list_save_to_file(const array_list *this, FILE *out);
void array_list_load_from_file(array_list *this, FILE *in);
void array_list_export_to_text_file(const array_list *this, FILE* out, const char* (*item_to_string)(const void *item));

/**
 * @brief Processes all elements in the array list.
 * @param this Pointer to the array list.
 * @param process_item Callback function to process each item.
 */
void array_list_process_all(const array_list *this, void (*process_item)(const void* item));

/**
 * @brief Sorts the array list using a custom comparator.
 * @param this Pointer to the array list.
 * @param comparator Comparison function for sorting elements.
 */
void array_list_sort(array_list *this, int (*comparator)(const void* item_a, const void* item_b));

/**
 * @brief Finds an element in the array list based on a property.
 * @param this Pointer to the array list.
 * @param item Output pointer for the found item.
 * @param item_has_property Predicate function to check the property.
 * @param prop Pointer to searched property.
 * @return Index of the matching element, or -1 if not found.
 */
int array_list_find_by_property(array_list* this, void* item, _Bool(*item_has_property)(const void *item, void* property), void* prop);

/**
 * @brief Serializes the array list into a buffer.
 * @param payload Pointer to the array list.
 * @param serialized_payload Output pointer for the serialized buffer.
 * @param serialize_item Function to serialize individual items.
 * @return Size of the serialized data.
 */
size_t array_list_serialize(void* payload, void** serialized_payload, size_t(*serialize_item)(void* item, void** serialized_item));

/**
 * @brief Deserializes a buffer into an array list.
 * @param serialized_payload Pointer to the serialized buffer.
 * @param size Size of the buffer.
 * @param deserialize_item Function to deserialize individual items.
 * @return Pointer to the deserialized array list, or NULL on failure.
 */
void* array_list_deserialize(const void* serialized_payload, size_t size, void* (*deserialize_item)(const void* serialized_item, size_t item_size));


array_list_iterator* array_list_get_begin(const array_list *this, array_list_iterator *begin);
array_list_iterator* array_list_get_end(const array_list *this, array_list_iterator *end);

void array_list_iterator_init(array_list_iterator *this, const array_list *list, int index);
void array_list_iterator_destroy(array_list_iterator *this);

_Bool array_list_iterator_is_equal(const array_list_iterator *this, const array_list_iterator *other);
void array_list_iterator_get(const array_list_iterator *this, void* value);
void array_list_iterator_next(array_list_iterator *this);

#endif //ARRAY_LIST_H
