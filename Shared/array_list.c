#include "array_list.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_LIST_DEFAULT_CAPACITY 5

static void array_list_resize(array_list *this, int new_capacity) {
    this->array_ = realloc(this->array_, new_capacity * this->element_size_);
    this->capacity_ = new_capacity;
    if (this->size_ > this->capacity_) {
        this->size_ = this->capacity_;
    }
}

void array_list_init(array_list *this, int element_size) {
    this->element_size_ = element_size;
    this->size_ = 0;
    this->capacity_ = ARRAY_LIST_DEFAULT_CAPACITY;
    this->array_ = calloc(this->capacity_, this->element_size_);
}

void array_list_destroy(array_list *this) {
    free(this->array_);
    this->array_ = NULL;
    this->size_ = 0;
    this->capacity_ = 0;
    this->element_size_ = 0;
}

void array_list_clear(array_list *this) {
    this->size_ = 0;
}

int array_list_get_size(const array_list* this) {
    return this->size_;
}

void array_list_add(array_list* this, const void* value) {
    if (this->size_ >= this->capacity_) {
        array_list_resize(this, this->capacity_ * 2);
    }
    memcpy((char*)this->array_ + this->size_ * this->element_size_, value, this->element_size_);
    ++this->size_;
}

_Bool array_list_insert(array_list* this, int index, const void* value) {
    if (index < 0 || index > this->size_) {
        return false;
    }
    if (index == this->size_) {
        array_list_add(this, value);
    } else {
        if (this->size_ >= this->capacity_) {
            array_list_resize(this, this->capacity_ * 2);
        }
        memmove(
            (char*)this->array_ + (index + 1) * this->element_size_,
            (char*)this->array_ + index * this->element_size_,
            (this->size_ - index) * this->element_size_
            );
        memcpy((char*)this->array_ + index * this->element_size_, value, this->element_size_);
        ++this->size_;
    }
    return true;
}

_Bool array_list_set(array_list *this, int index, const void *value) {
    if (index < 0 || index >= this->size_) {
        return false;
    }

    memcpy((char*)this->array_ + index * this->element_size_, value, this->element_size_);
    return true;
}

_Bool array_list_try_get(const array_list* this, int index, void* value) {
    if (index < 0 || index >= this->size_) {
      return false;
    }
    memcpy(value, (char*)this->array_ + index * this->element_size_, this->element_size_);
    return true;
}

void array_list_remove_last(array_list *this) {
    if (this->size_ > 0) {
        --this->size_;
    }
}

_Bool array_list_remove_at(array_list *this, int index) {
    if (index < 0 || index > this->size_) {
        return false;
    }

    if (index == this->size_) {
        array_list_remove_last(this);
    } else {
        memmove(
                (char*)this->array_ + index * this->element_size_,
                (char*)this->array_ + (index + 1) * this->element_size_,
                (this->size_ - index - 1) * this->element_size_
        );
        --this->size_;
    }

    return true;
}

_Bool array_list_assign(array_list *this, array_list *other) {
    if (this == other) {
        return false;
    }

    this->size_ = other->size_;
    this->capacity_ = other->capacity_;
    this->element_size_ = other->element_size_;
    array_list_resize(this, other->capacity_);
    memcpy(this->array_, other->array_, other->size_ * other->element_size_);
    return true;
}

void array_list_save_to_file(const array_list *this, FILE *out) {
    fwrite(&this->size_, sizeof(this->size_), 1, out);
    fwrite(&this->capacity_, sizeof(this->capacity_), 1, out);
    fwrite(&this->element_size_, sizeof(this->element_size_), 1, out);
    fwrite(this->array_, this->element_size_, this->size_, out);
}

void array_list_load_from_file(array_list *this, FILE *in) {
    fread(&this->size_, sizeof(this->size_), 1, in);
    fread(&this->capacity_, sizeof(this->capacity_), 1, in);
    fread(&this->element_size_, sizeof(this->element_size_), 1, in);
    array_list_resize(this, this->capacity_);
    fread(this->array_, this->element_size_, this->size_, in);
}

void array_list_export_to_text_file(const array_list *this, FILE* out, const char* (*item_to_string)(const void *item)) {
    fprintf(out, "%d %d %d ", this->element_size_, this->size_, this->capacity_);
    char* item = this->array_;
    char* end = this->array_ + this->size_ * this->element_size_;
    while (item < end) {
        const char* string = item_to_string(item);
        fprintf(out, "%s ", string);
        item += this->element_size_;
    }
}

void array_list_process_all(const array_list *this, void(*process_item)(const void *item)) {
    char* item = this->array_;
    char* end = this->array_ + this->size_ * this->element_size_;
    while (item < end) {
        process_item(item);
        item += this->element_size_;
    }
}

void array_list_sort(array_list *this, int (*comparator)(const void *item_a, const void *item_b)) {
    qsort(this->array_, this->size_, this->element_size_, comparator);
}

int array_list_find_by_property(array_list *this, void *item, _Bool(*item_has_property)(const void *item, void* property), void* prop) {
    char* curr_item = this->array_;
    char* end = this->array_ + this->size_ * this->element_size_;
    while (curr_item < end) {
        if(item_has_property(curr_item, prop)) {
            memcpy(item, curr_item, this->element_size_);
            return (int)(curr_item - (char*)this->array_) / this->element_size_;
        }
        curr_item += this->element_size_;
    }
    return -1;
}

size_t array_list_serialize(void *payload, void **serialized_payload,
                            size_t(*serialize_item)(void *item, void **serialized_item)) {
      array_list* this = (array_list*)payload;
    size_t total_size = 0;

    total_size += sizeof(int) * 3;

    void** serialized_items = malloc(this->size_ * sizeof(void*));
    if (!serialized_items) {
        perror("Failed to allocate memory for serialized items");
        return 0;
    }

    size_t* item_sizes = malloc(this->size_ * sizeof(size_t));
    if (!item_sizes) {
        perror("Failed to allocate memory for item sizes");
        free(serialized_items);
        return 0;
    }

    for (int i = 0; i < this->size_; i++) {
        void* item = (char*)this->array_ + i * this->element_size_;
        item_sizes[i] = serialize_item(item, &serialized_items[i]);
        total_size += sizeof(size_t) + item_sizes[i];
    }

    *serialized_payload = malloc(total_size);
    if (!*serialized_payload) {
        perror("Failed to allocate memory for serialized array_list");
        for (int i = 0; i < this->size_; i++) {
            free(serialized_items[i]);
        }
        free(serialized_items);
        free(item_sizes);
        return 0;
    }

    char* cursor = *serialized_payload;

    memcpy(cursor, &this->size_, sizeof(int));
    cursor += sizeof(int);
    memcpy(cursor, &this->element_size_, sizeof(int));
    cursor += sizeof(int);
    memcpy(cursor, &this->capacity_, sizeof(int));
    cursor += sizeof(int);

    for (int i = 0; i < this->size_; i++) {
        memcpy(cursor, &item_sizes[i], sizeof(size_t));
        cursor += sizeof(size_t);
        memcpy(cursor, serialized_items[i], item_sizes[i]);
        cursor += item_sizes[i];
        free(serialized_items[i]);
    }

    free(serialized_items);
    free(item_sizes);

    return total_size;
}

void * array_list_deserialize(const void *serialized_payload, size_t size,
    void *(*deserialize_item)(const void *serialized_item, size_t item_size)) {
    const char* cursor = serialized_payload;

    array_list* this = malloc(sizeof(array_list));
    if (!this) {
        perror("Failed to allocate memory for array_list");
        return NULL;
    }

    memcpy(&this->size_, cursor, sizeof(int));
    cursor += sizeof(int);
    memcpy(&this->element_size_, cursor, sizeof(int));
    cursor += sizeof(int);
    memcpy(&this->capacity_, cursor, sizeof(int));
    cursor += sizeof(int);

    this->array_ = malloc(this->capacity_ * this->element_size_);
    if (!this->array_) {
        perror("Failed to allocate memory for array_list array");
        free(this);
        return NULL;
    }

    for (int i = 0; i < this->size_; i++) {
        size_t item_size;
        memcpy(&item_size, cursor, sizeof(size_t));
        cursor += sizeof(size_t);

        void* deserialized_item = deserialize_item(cursor, item_size);
        if (!deserialized_item) {
            perror("Failed to deserialize item");
            free(this->array_);
            free(this);
            return NULL;
        }
        memcpy((char*)this->array_ + i * this->element_size_, deserialized_item, this->element_size_);
        free(deserialized_item);
        cursor += item_size;
    }

    return this;
}

array_list_iterator* array_list_get_begin(const array_list *this, array_list_iterator *begin) {
    array_list_iterator_init(begin, this, 0);
    return begin;
}

array_list_iterator* array_list_get_end(const array_list *this, array_list_iterator *end) {
    array_list_iterator_init(end, this, this->size_);
    return end;
}

//---------- iterator -------------

void array_list_iterator_init(array_list_iterator *this, const array_list *list, int index) {
    this->list_ = list;
    this->current_index_ = index;
}

void array_list_iterator_destroy(array_list_iterator *this) {
    this->list_ = NULL;
    this->current_index_ = 0;
}

_Bool array_list_iterator_is_equal(const array_list_iterator *this, const array_list_iterator *other) {
    return this->current_index_ == other->current_index_ && this->list_ == other->list_;
}

void array_list_iterator_get(const array_list_iterator *this, void *value) {
    array_list_try_get(this->list_, this->current_index_, value);
}

void array_list_iterator_next(array_list_iterator *this) {
    this->current_index_++;
}
