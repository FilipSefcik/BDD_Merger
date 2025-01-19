#include "module.h"

void son_name_and_pos_init(son_name_and_pos *this, char *son_name, int son_position) {
    memset(this->son_name_, 0, sizeof(this->son_name_));
    size_t null_index = strlen(son_name) < sizeof(this->son_name_) ? strlen(son_name) : sizeof(this->son_name_) - 1;
    strncpy(this->son_name_, son_name, null_index);
    this->son_name_[null_index] = '\0';
    this->son_position_ = son_position;
}

void son_name_and_pos_destroy(const void *item) {
    son_name_and_pos* this = (son_name_and_pos*)item;
    this->son_name_[0] = '\0';
    this->son_position_ = 0;
}

bool son_name_and_pos_compare_name(const void *item, void *property) {
    return strcmp(((son_name_and_pos*)item)->son_name_, (char*)property) == 0;
}

size_t son_name_and_pos_serialize(void *item, void **serialized_payload) {
    son_name_and_pos* son = (son_name_and_pos*)item;

    size_t serialized_size = sizeof(son->son_name_) + sizeof(son->son_position_);

    *serialized_payload = malloc(serialized_size);
    if (!*serialized_payload) {
        perror("Failed to allocate memory for serialized son_name_and_pos");
        return 0;
    }

    char* cursor = *serialized_payload;
    memcpy(cursor, son->son_name_, sizeof(son->son_name_));
    cursor += sizeof(son->son_name_);
    memcpy(cursor, &son->son_position_, sizeof(son->son_position_));

    return serialized_size;
}

void * son_name_and_pos_deserialize(const void *serialized_payload, size_t size) {
    if (size != sizeof(son_name_and_pos)) {
        fprintf(stderr, "Invalid serialized size for son_name_and_pos: expected %zu, got %zu\n", sizeof(son_name_and_pos), size);
        return NULL;
    }

    son_name_and_pos* son = malloc(sizeof(son_name_and_pos));
    if (!son) {
        perror("Failed to allocate memory for deserialized son_name_and_pos");
        return NULL;
    }

    const char* cursor = serialized_payload;
    memcpy(son->son_name_, cursor, sizeof(son->son_name_));
    cursor += sizeof(son->son_name_);
    memcpy(&son->son_position_, cursor, sizeof(son->son_position_));

    return son;
}


void print_son_map(const void* item) {
    printf("\tSon name: %s", ((son_name_and_pos*)item)->son_name_);
    printf(" son position: %d\n", ((son_name_and_pos*)item)->son_position_);
}


void module_init(module *this, char *name) {
    this->name_ = strdup(name);
    this->assigned_client_ = 0;
    this->priority_ = 0;
    this->parent_ = NULL;
    this->path_ = NULL;
    this->function_ = malloc(sizeof(pla_function));
    this->son_map_ = malloc(sizeof(array_list));
    array_list_init(this->son_map_, sizeof(son_name_and_pos));
}

void module_destroy(module *this) {
    array_list_process_all(this->son_map_, son_name_and_pos_destroy);
    array_list_destroy(this->son_map_);
    free(this->son_map_);
    this->son_map_ = NULL;
    pla_function_destroy(this->function_);
    free(this->function_);
    this->function_ = NULL;
    this->parent_ = NULL;
    free(this->name_);
    this->name_ = NULL;
    free(this->path_);
    this->path_ = NULL;
    this->assigned_client_ = 0;
}

void module_destroy_array_list(const void *item) {
    module_destroy(*(module**)item);
    free(*(module**)item);
}

pla_function * module_get_function(module *this) {
    return this->function_;
}

module * module_get_parent(module *this) {
    return this->parent_;
}

int module_get_son_position(module *this, char* son_name) {
    son_name_and_pos son = { '\0', -1};
    array_list_find_by_property(this->son_map_, &son, son_name_and_pos_compare_name, son_name);
    return son.son_position_;
}

int module_get_priority(module *this) {
    return this->priority_;
}

int module_get_var_count(module *this) {
    return this->function_ ? this->function_->var_count_ : 0;
}

int module_get_assigned_client(module *this) {
    return this->assigned_client_;
}

int module_get_son_count(module *this) {
    return array_list_get_size(this->son_map_);
}

char * module_get_name(module *this) {
    return this->name_;
}

void module_set_client(module *this, int client) {
    this->assigned_client_ = client;
}

void module_set_parent(module* this, module *parent) {
    this->parent_ = parent;
}

void module_set_path(module *this, char *path) {
    if (this->path_) { free(this->path_); }
    this->path_ = strdup(path);
}

int module_priority_comparator(const void *this, const void *other) {
    return (*(module**)this)->priority_ - (*(module**)other)->priority_;
}

bool module_match_name(const void *item, void *property) {
    return strcmp(module_get_name(*(module **)item), (char *)property) == 0;
}

void module_add_priority(module *this, int son_priority) {
    if (this->priority_ == son_priority) {
        this->priority_++;
        if (this->parent_) {
            module_add_priority(this->parent_, this->priority_);
        }
    }
}

void module_add_son(module *this, module *son, int son_position) {
    module_add_son_position(this, module_get_name(son), son_position);
    module_set_parent(son, this);
    module_add_priority(this, module_get_priority(son));
}

void module_create_function(module *this, int var_count, int line_count) {
    pla_function_init(this->function_, var_count, line_count);
}

void module_merge_modules(module *parent, module *son) {
    int position = module_get_son_position(parent, module_get_name(son));
    if (position >= 0) {
        pla_function_input_variables(module_get_function(parent), module_get_function(son), position);
        module_adjust_positions(parent, module_get_name(son), module_get_var_count(son));
    }
}

void module_add_son_position(module *this, char *son_name, int son_position) {
    son_name_and_pos new_son;
    son_name_and_pos_init(&new_son, son_name, son_position);
    array_list_add(this->son_map_, &new_son);
}

void module_adjust_positions(module *this, char *added_son, int son_var_count) {
    son_name_and_pos temp;
    int index = array_list_find_by_property(this->son_map_, &temp, son_name_and_pos_compare_name, added_son);
    if (index >= 0 && index < array_list_get_size(this->son_map_) - 1) {
        for (int i = index + 1; i < array_list_get_size(this->son_map_); i++) {
            son_name_and_pos son;
            array_list_try_get(this->son_map_, i, &son);
            son.son_position_ += son_var_count - 1;
            array_list_set(this->son_map_, i, &son);
        }
    }
}

void module_load_pla(const void *module_ptr) {
    module* this = *(module **)module_ptr;

    if (!this->path_) {
        fprintf(stderr, "Module path is not set. Cannot load PLA file.\n");
        return;
    }

    FILE *file = fopen(this->path_, "r");
    if (!file) {
        perror("Failed to open PLA file");
        return;
    }

    char line[32];
    int var_count = 0, num_outputs = 0, num_lines = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '#' || line[0] == '\0') {
            continue;
        }

        if (strncmp(line, ".i", 2) == 0) {
            sscanf(line + 2, "%d", &var_count);
        } else if (strncmp(line, ".o", 2) == 0) {
            sscanf(line + 2, "%d", &num_outputs);
        } else if (strncmp(line, ".p", 2) == 0) {
            sscanf(line + 2, "%d", &num_lines);
        } else {
            break;
        }
    }

    if (var_count + num_outputs + num_lines < 3) {
        fprintf(stderr, "Not enough info (.i, .o, .p)from PLA file.\n");
        fclose(file);
        return;
    }

    if (!this->function_) {
        this->function_ = malloc(sizeof(pla_function));
    }
    pla_function_init(this->function_, var_count, num_lines);

    char variables[var_count + 1];
    char fun_value;
    int line_index = 0;

    do {
        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '#' || line[0] == '\0') {
            continue;
        }

        if (strcmp(line, ".e") == 0) {
            break;
        }

        sscanf(line, "%s %c", variables, &fun_value);

        pla_function_add_line(this->function_, variables, fun_value, line_index);
        line_index++;
    } while (fgets(line, sizeof(line), file));

    fclose(file);
}

size_t module_serialize(void *payload, void **serialized_payload) {
    module* this = *(module**)payload;
    size_t total_size = 0;
    size_t name_size = strlen(this->name_) + 1;

    total_size += sizeof(size_t) + name_size;

    void* function_payload = NULL;
    size_t function_size = pla_function_serialize(this->function_, &function_payload);
    total_size += sizeof(size_t) + function_size;

    void* son_map_payload = NULL;
    size_t son_map_size = array_list_serialize(this->son_map_, &son_map_payload, son_name_and_pos_serialize);
    total_size += sizeof(size_t) + son_map_size;

    *serialized_payload = malloc(total_size);
    if (!*serialized_payload) {
        perror("Failed to allocate memory for serialized module");
        free(function_payload);
        free(son_map_payload);
        return 0;
    }

    char* cursor = *serialized_payload;

    memcpy(cursor, &name_size, sizeof(size_t));
    cursor += sizeof(size_t);
    memcpy(cursor, this->name_, name_size);
    cursor += name_size;

    memcpy(cursor, &function_size, sizeof(size_t));
    cursor += sizeof(size_t);
    memcpy(cursor, function_payload, function_size);
    cursor += function_size;

    memcpy(cursor, &son_map_size, sizeof(size_t));
    cursor += sizeof(size_t);
    memcpy(cursor, son_map_payload, son_map_size);

    free(function_payload);
    free(son_map_payload);

    return total_size;
}

void * module_deserialize(const void *serialized_payload, size_t size) {
    module* this = malloc(sizeof(module));
    if (!this) {
        perror("Failed to allocate memory for module");
        return NULL;
    }

    const char* cursor = serialized_payload;

    size_t name_size;
    memcpy(&name_size, cursor, sizeof(size_t));
    cursor += sizeof(size_t);

    this->name_ = malloc(name_size);
    if (!this->name_) {
        perror("Failed to allocate memory for name");
        free(this);
        return NULL;
    }
    memcpy(this->name_, cursor, name_size);
    cursor += name_size;

    size_t function_size;
    memcpy(&function_size, cursor, sizeof(size_t));
    cursor += sizeof(size_t);

    this->function_ = pla_function_deserialize(cursor, function_size);
    if (!this->function_) {
        perror("Failed to deserialize function");
        free(this->name_);
        free(this);
        return NULL;
    }
    cursor += function_size;

    size_t son_map_size;
    memcpy(&son_map_size, cursor, sizeof(size_t));
    cursor += sizeof(size_t);

    this->son_map_ = array_list_deserialize(cursor, son_map_size, son_name_and_pos_deserialize);
    if (!this->son_map_) {
        perror("Failed to deserialize son_map");
        pla_function_destroy(this->function_);
        free(this->name_);
        free(this);
        return NULL;
    }

    this->parent_ = NULL;
    this->path_ = NULL;
    this->assigned_client_ = 0;
    this->priority_ = 0;

    return this;
}

void module_print_out(module *this) {
    if (!this) {
        return;
    }
    printf("Name: %s\n", this->name_);
    if (this->path_) {
        printf("Path: %s\n", this->path_);
    }
    if (this->parent_) {
        printf("Parent: %s\n", module_get_name(this->parent_));
    }
    printf("Assigned Client: %d\n", this->assigned_client_);
    printf("Priority: %d\n", this->priority_);
    if (array_list_get_size(this->son_map_) > 0) {
        printf("Son map:\n");
        array_list_process_all(this->son_map_, print_son_map);
    }
    if (this->function_) {
        printf("Function:\n");
        pla_function_print_function(this->function_);
    }

}


