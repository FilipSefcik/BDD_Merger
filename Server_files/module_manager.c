#include "module_manager.h"
#include <ctype.h>

void module_manager_init(module_manager *this, int client_count) {
    this->client_count_ = client_count;
    this->instructions_ = malloc(this->client_count_ * sizeof(char *));
    for (int i = 0; i < this->client_count_; i++) {
        this->instructions_[i] = malloc(sizeof(char));
        this->instructions_[i][0] = 'X';
    }
    array_list_init(&this->modules_, sizeof(module*));
}

void module_manager_free_module(const void * item) {
    module_destroy(*(module**)item);
    free(*(module**)item);
}

void module_manager_destroy(module_manager *this) {
    for (int i = 0; i < this->client_count_; i++) {
        free(this->instructions_[i]);
    }
    free(this->instructions_);
    array_list_process_all(&this->modules_, module_manager_free_module);
    array_list_destroy(&this->modules_);
    this->client_count_ = 0;
}

array_list* module_manager_get_modules(module_manager *this) {
    return &this->modules_;
}

char ** module_manager_get_instructions(module_manager *this) {
    return this->instructions_;
}

void module_manager_load(module_manager *this, const char *conf_file_path) {
    module_manager_load_modules(this, conf_file_path);
    module_manager_load_plas(this);
}

void module_manager_load_modules(module_manager *this, const char *conf_file_path) {
    FILE *file = fopen(conf_file_path, "r");
    if (!file) {
        perror("Failed to open configuration file");
        return;
    }

    char line[512];

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        line[strcspn(line, "\n")] = '\0';

        if (!strstr(line, ".pla")) {
            break;
        }

        char module_name[16];
        char module_path[256];
        int pla_index = strstr(line, ".pla") - line + 4;

        if (pla_index < 0 || pla_index >= sizeof(line)) {
            fprintf(stderr, "Invalid module line: %s\n", line);
            continue;
        }

        sscanf(line, "%s", module_name);
        strncpy(module_path, line + strlen(module_name) + 1, pla_index - strlen(module_name) - 1);
        module_path[pla_index - strlen(module_name) - 1] = '\0';

        module *mod = malloc(sizeof(module));
        module_init(mod, module_name);
        module_set_path(mod, module_path);

        array_list_add(&this->modules_, &mod);
    }

    do {
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        char module_name[16];
        char mapping[256];
        sscanf(line, "%s %s", module_name, mapping);

        module *mod = NULL;
        array_list_find_by_property(&this->modules_, &mod, module_match_name, module_name);

        int digits = 0;
        for (int i = 0; i < strlen(mapping); i++) {
            if (mapping[i] == 'M') {
                char son_name[16];
                int son_position = i - digits;
                int j = i;

                while (j < strlen(mapping) && (j == i || isdigit(mapping[j]))) {
                    if (isdigit(mapping[j])) {
                        digits++;
                    }
                    son_name[j - i] = mapping[j];
                    j++;
                }
                son_name[j - i] = '\0';
                i = j - 1;

                module *son_mod = NULL;
                array_list_find_by_property(&this->modules_, &son_mod, module_match_name, son_name);
                module_add_son(mod, son_mod, son_position);
            }
        }
    } while (fgets(line, sizeof(line), file));


    fclose(file);
}

void module_manager_load_plas(module_manager *this) {
    array_list_process_all(&this->modules_, module_load_pla);
}

void module_manager_add_instruction(module_manager *this, int client_id, char* instruction) {
    char** client_instructions_ptr = this->instructions_ + client_id;
    char* client_instructions = *client_instructions_ptr;

    if (client_instructions[0] == 'X') {
        size_t new_length = strlen(instruction) + 1;
        *client_instructions_ptr = realloc(client_instructions, new_length);
        memcpy(*client_instructions_ptr, instruction, new_length);
    } else {
        size_t current_length = strlen(client_instructions);
        size_t new_length = current_length + strlen(instruction) + 1;
        *client_instructions_ptr = realloc(client_instructions, new_length);
        memcpy(*client_instructions_ptr + current_length, instruction, strlen(instruction) + 1);
    }
}

void module_manager_create_instructions(module_manager *this, int* distribution, void(*give_instruction)(module *mod, char *instructions[2], int* distribution)) {
    module* temp = NULL;
    char* new_instructions[2];
    array_list_sort(&this->modules_, module_priority_comparator);
    for (int i = 0; i < array_list_get_size(&this->modules_); i++) {
        new_instructions[0] = NULL;
        new_instructions[1] = NULL;

        array_list_try_get(&this->modules_, i, &temp);
        give_instruction(temp, new_instructions, distribution);
        module_manager_add_instruction(this, module_get_assigned_client(temp), new_instructions[0]);
        if (new_instructions[1]) {
            module_manager_add_instruction(this, module_get_assigned_client(module_get_parent(temp)), new_instructions[1]);
        }

        free(new_instructions[0]);
        free(new_instructions[1]);
    }
}

void module_manager_print_instructions(module_manager *this) {
    for (int i = 0; i < this->client_count_; i++) {
        if (this->instructions_[i][0] == 'X') {
            printf("%d:\n%c\n", i, this->instructions_[i][0]);
        } else {
            printf("%d:\n%s\n", i, this->instructions_[i]);
        }
    }
}

void module_manager_print_modules(module_manager *this) {
    for (int i = 0; i < array_list_get_size(&this->modules_); i++) {
        module* mod = NULL;
        array_list_try_get(&this->modules_, i, &mod);
        module_print_out(mod);
    }
}
