#include "pla_function.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


void pla_function_alloc_values(pla_function* this) {
    this->variables_ = malloc(this->num_lines_ * sizeof(char*));
    for (int i = 0; i < this->num_lines_; i++) {
        this->variables_[i] = malloc(this->var_count_ * sizeof(char));
    }
    this->fun_values_ = malloc(this->num_lines_ * sizeof(char));
}

void pla_function_init(pla_function* this, int var_count, int line_count) {
    this->num_lines_ = line_count;
    this->var_count_ = var_count;
    this->fun_val_count_[0] = 0;
    this->fun_val_count_[1] = 0;
    pla_function_alloc_values(this);
}

void pla_function_free_values(pla_function* this) {
    for (int i = 0; i < this->num_lines_; i++) {
        free(this->variables_[i]);
    }
    free(this->variables_);
    free(this->fun_values_);
}

void pla_function_destroy(pla_function* this) {
    pla_function_free_values(this);
    this->variables_ = NULL;
    this->fun_values_ = NULL;
    this->num_lines_ = 0;
    this->var_count_ = 0;
}

void pla_function_assign(pla_function *this, pla_function *other) {
    pla_function_free_values(this);
    this->num_lines_ = pla_function_get_num_lines(other);
    this->var_count_ = pla_function_get_var_count(other);
    this->fun_val_count_[0] = pla_function_get_fun_val_count(other)[0];
    this->fun_val_count_[1] = pla_function_get_fun_val_count(other)[1];
    this->variables_ = malloc(this->num_lines_ * sizeof(char*));
    for (int i = 0; i < this->num_lines_; i++) {
        this->variables_[i] = malloc(this->var_count_ * sizeof(char));
        memcpy(this->variables_[i], pla_function_get_variables(other)[i], this->var_count_ * sizeof(char));
    }

    this->fun_values_ = malloc(this->num_lines_ * sizeof(char));
    memcpy(this->fun_values_, pla_function_get_function_values(other), this->num_lines_ * sizeof(char));
}

char ** pla_function_get_variables(pla_function *this) {
    return this->variables_;
}

char * pla_function_get_function_values(pla_function *this) {
    return this->fun_values_;
}

int pla_function_get_num_lines(pla_function *this) {
    return this->num_lines_;
}

int pla_function_get_var_count(pla_function *this) {
    return this->var_count_;
}

int * pla_function_get_fun_val_count(pla_function *this) {
    return this->fun_val_count_;
}

void pla_function_add_line(pla_function* this, const char* new_vars, char value, int line_num) {
    memcpy(*(this->variables_ + line_num), new_vars, this->var_count_ * sizeof(char));
    *(this->fun_values_ + line_num) = value;
    this->fun_val_count_[value - '0']++;
}

void pla_function_print_function(pla_function* this) {
    char** var_ptr = this->variables_;
    char* fun_ptr = this->fun_values_;
    for (int i = 0; i < this->num_lines_; i++, var_ptr++, fun_ptr++) {
        printf("%.*s\t%c\n", this->var_count_, *var_ptr, *fun_ptr);
    }
}

void pla_function_replace_char(const char* before, int before_length, int position, const char* input, int input_length, char* result) {
    if (position == 0) {
        memcpy(result, input, input_length);
        memcpy(result + input_length, before + 1, before_length - 1);
    } else {
        memcpy(result, before, position);
        memcpy(result + position, input, input_length);
        memcpy(result + position + input_length, before + position + 1, before_length - position - 1);
    }
}

void pla_function_free_sort(char*** sorted, int group_count) {
    if (sorted) {
        for (int i = 0; i < group_count; i++) {
            free(sorted[i]);
        }
    }
    free(sorted);
}

char*** pla_function_sort_by_function(pla_function* this) {
    char*** sorted = malloc(2 * sizeof(char**));

    for (int i = 0; i < 2; i++) {
        *(sorted + i) = malloc(*(this->fun_val_count_ + i) * sizeof(char*));
    }

    char** var_ptr_curr = this->variables_;
    char** var_ptr_end = this->variables_ + this->num_lines_;
    char* fun_ptr_curr = this->fun_values_;
    char* fun_ptr_end = this->fun_values_ + this->num_lines_;
    char** sorted_0_curr = *sorted;
    char** sorted_1_curr = *(sorted + 1);
    while (var_ptr_curr != var_ptr_end && fun_ptr_curr != fun_ptr_end) {
        if (*fun_ptr_curr == '0') {
            *sorted_0_curr++ = *var_ptr_curr;
        } else if (*fun_ptr_curr == '1') {
            *sorted_1_curr++ = *var_ptr_curr;
        }
        var_ptr_curr++;
        fun_ptr_curr++;
    }

    return sorted;
}

char*** pla_function_sort_by_position(pla_function* this, int position, int* match_count) {
    if (!match_count) { return NULL; }
    char*** sorted = malloc(3 * sizeof(char**));

    char** var_ptr = this->variables_;
    for (int i = 0; i < this->num_lines_; i++, var_ptr++) {
        char current_char = *(*var_ptr + position);
        if (current_char == '0') {
            match_count[0]++;
        } else if (current_char == '1') {
            match_count[1]++;
        } else if (current_char == '-') {
            match_count[2]++;
        }
    }

    for (int group = 0; group < 3; group++) {
        sorted[group] = malloc(match_count[group] * sizeof(char*));
    }

    var_ptr = this->variables_;
    int indexes[3] = {0, 0, 0};
    for (int i = 0; i < this->num_lines_; i++, var_ptr++) {
        char* current_string = *var_ptr;
        char current_char = *(current_string + position);
        if (current_char == '0') {
            sorted[0][indexes[0]++] = current_string;
        } else if (current_char == '1') {
            sorted[1][indexes[1]++] = current_string;
        } else if (current_char == '-') {
            sorted[2][indexes[2]++] = current_string;
        }
    }

    return sorted;
}

void pla_function_input_variables(pla_function* this, pla_function* other, int position) {
    if (pla_function_get_var_count(other) <= 0) {
        return;
    }
    int* match_count = calloc(3, sizeof(int));
    char*** my_vars = pla_function_sort_by_position(this, position, match_count);
    char*** additional_vars = pla_function_sort_by_function(other);

    int other_var_count = pla_function_get_var_count(other);
    int* other_fun_val_count = pla_function_get_fun_val_count(other);

    char whatever_input[other_var_count];
    memset(whatever_input, '-', sizeof(whatever_input));

    int new_var_count = other_var_count + this->var_count_ - 1;
    int new_line_count = match_count[0] * other_fun_val_count[0] + match_count[1] * other_fun_val_count[1] + match_count[2];
    char new_vars[new_var_count];

    pla_function new_pla;
    pla_function_init(&new_pla, new_var_count, new_line_count);

    int line_num = 0;
    for (int group = 0; group < 3; group++) {
        for (int i = 0; i < match_count[group]; i++) {
            char* temp_line = my_vars[group][i];
            char fun_value = pla_function_get_fun_value(this, temp_line);
            if (group < 2) {
                for (int j = 0; j < other_fun_val_count[group]; j++) {
                    char* input_line = additional_vars[group][j];
                    pla_function_replace_char(temp_line, this->var_count_, position, input_line, other_var_count, new_vars);
                    pla_function_add_line(&new_pla, new_vars, fun_value, line_num);
                    line_num++;
                }
            } else {
                pla_function_replace_char(temp_line,this->var_count_, position, whatever_input, other_var_count, new_vars);
                pla_function_add_line(&new_pla, new_vars, fun_value, line_num);
                line_num++;
            }
        }
    }

    pla_function_assign(this, &new_pla);

    pla_function_destroy(&new_pla);
    free(match_count);
    pla_function_free_sort(my_vars, 3);
    pla_function_free_sort(additional_vars, 2);
}

char pla_function_get_fun_value(pla_function* this, const char* indexed_variables) {
    char** var_ptr = this->variables_;
    char* fun_ptr = this->fun_values_;

    for (int i = 0; i < this->num_lines_; i++, var_ptr++, fun_ptr++) {
        if (memcmp(*var_ptr, indexed_variables, this->var_count_) == 0) {
            return *fun_ptr;
        }
    }

    return '0';
}


size_t pla_function_serialize(void *payload, void **serialized_payload) {
    pla_function *this = (pla_function *)payload;

    size_t total_size = 0;

    total_size += sizeof(int) * 2;
    total_size += sizeof(int) * 2;
    total_size += this->num_lines_ * this->var_count_;
    total_size += this->num_lines_;

    *serialized_payload = malloc(total_size);

    char *current_ptr = *serialized_payload;

    memcpy(current_ptr, this->fun_val_count_, sizeof(int) * 2);
    current_ptr += sizeof(int) * 2;

    memcpy(current_ptr, &this->num_lines_, sizeof(int));
    current_ptr += sizeof(int);
    memcpy(current_ptr, &this->var_count_, sizeof(int));
    current_ptr += sizeof(int);

    for (int i = 0; i < this->num_lines_; i++) {
        memcpy(current_ptr, this->variables_[i], this->var_count_);
        current_ptr += this->var_count_;
    }

    memcpy(current_ptr, this->fun_values_, this->num_lines_);

    return total_size;
}

void * pla_function_deserialize(const void *serialized_payload, size_t size) {
    if (!serialized_payload || size == 0) {
        return NULL;
    }

    const char* buffer = (const char*)serialized_payload;

    pla_function* deserialized = malloc(sizeof(pla_function));

    memcpy(deserialized->fun_val_count_, buffer, sizeof(int) * 2);
    buffer += sizeof(int) * 2;

    memcpy(&deserialized->num_lines_, buffer, sizeof(int));
    buffer += sizeof(int);
    memcpy(&deserialized->var_count_, buffer, sizeof(int));
    buffer += sizeof(int);

    deserialized->variables_ = malloc(deserialized->num_lines_ * sizeof(char*));

    deserialized->fun_values_ = malloc(deserialized->num_lines_ * sizeof(char));

    for (int i = 0; i < deserialized->num_lines_; i++) {
        deserialized->variables_[i] = malloc(deserialized->var_count_ * sizeof(char));
        memcpy(deserialized->variables_[i], buffer, deserialized->var_count_ * sizeof(char));
        buffer += deserialized->var_count_ * sizeof(char);
    }

    memcpy(deserialized->fun_values_, buffer, deserialized->num_lines_ * sizeof(char));

    return deserialized;
}

