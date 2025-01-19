#include "server_utils.h"

void divider_default_divide(array_list* modules, int client_count, int* distribution) {
    array_list_iterator begin;
    array_list_iterator end;
    array_list_get_begin(modules, &begin);
    array_list_get_end(modules, &end);
    int assigned_client = 0;
    while (!array_list_iterator_is_equal(&begin, &end)) {
        module* temp;
        array_list_iterator_get(&begin, &temp);
        module_set_client(temp, assigned_client % client_count);
        distribution[assigned_client % client_count]++;
        array_list_iterator_next(&begin);
        assigned_client++;
    }
    array_list_iterator_destroy(&begin);
    array_list_iterator_destroy(&end);
}

void give_instruction(module *mod, char *instructions[2], int* distribution) {
    module* parent = module_get_parent(mod);
    if (parent) {
        size_t len0 = 11 + strlen(module_get_name(mod)) + strlen(module_get_name(parent)) + 1;
        instructions[0] = malloc(len0);

        size_t len1 = 20 + 2 * strlen(module_get_name(mod)) + strlen(module_get_name(parent)) + 1;
        instructions[1] = malloc(len1);

        if (module_get_son_count(mod) == 0) {
            distribution[module_get_assigned_client(mod)]--;
            module_set_client(mod, module_get_assigned_client(parent));
            distribution[module_get_assigned_client(mod)]++;
        }

        if (module_get_assigned_client(mod) == module_get_assigned_client(parent)) {
            snprintf(instructions[0], len0, "MERG %s %s\n", module_get_name(parent), module_get_name(mod));
            instructions[1][0] = '\0';
        } else {
            snprintf(instructions[0], len0, "SEND %s %d\n", module_get_name(mod), module_get_assigned_client(parent));
            snprintf(instructions[1], len1, "RECV %s\nMERG %s %s\n", module_get_name(mod), module_get_name(parent), module_get_name(mod));
        }

    } else {
        size_t len0 = 10 + strlen(module_get_name(mod)) + 1;
        instructions[0] = malloc(len0);
        snprintf(instructions[0], len0, "END %s\n", module_get_name(mod));
        instructions[1] = NULL;
    }
}

