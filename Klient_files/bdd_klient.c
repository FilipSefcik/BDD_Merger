#include "bdd_klient.h"
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../Shared/comm_utils.h"

void bdd_klient_init(bdd_klient *this) {
    this->instructions = NULL;
    this->server_socket_ = 0;
    array_list_init(&this->modules_, sizeof(module*));
}

void bdd_klient_destroy(bdd_klient *this) {
    if (this->server_socket_ != 0) {
        bdd_klient_disconnect(this);
    }
    bdd_klient_clear_klient(this);
    array_list_destroy(&this->modules_);
}

void bdd_klient_clear_klient(bdd_klient *this) {
    free(this->instructions);
    this->instructions = NULL;
    array_list_process_all(&this->modules_, module_destroy_array_list);
    array_list_clear(&this->modules_);
}

void bdd_klient_send_message(bdd_klient *this, bdd_message *message) {
    send_all(this->server_socket_, &message->serialized_buffer_size_, sizeof(message->serialized_buffer_size_));
    send_all(this->server_socket_, message->serialized_buffer_, message->serialized_buffer_size_);
}

void bdd_klient_receive_message(bdd_klient *this, bdd_message *message) {
    recv_all(this->server_socket_, &message->serialized_buffer_size_, sizeof(message->serialized_buffer_size_));
    bdd_message_allocate_buffer(message, message->serialized_buffer_size_);
    recv_all(this->server_socket_, message->serialized_buffer_, message->serialized_buffer_size_);
}

_Bool bdd_klient_connect(bdd_klient *this, char *server_ip, int server_port) {
    if (this->server_socket_ != 0) {
        printf("Server socket already open\n");
        return false;
    }
    struct sockaddr_in server_addr;
    if ((this->server_socket_ = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Chyba pri vytváraní socketu");
        this->server_socket_ = 0;
        return false;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(this->server_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Chyba pri pripojení k serveru");
        close(this->server_socket_);
        this->server_socket_ = 0;
        return false;
    }

    return true;
}

void bdd_klient_disconnect(bdd_klient *this) {
    close(this->server_socket_);
    this->server_socket_ = 0;
}

_Bool bdd_klient_receive_info(bdd_klient *this) {
    if (!bdd_klient_receive_instructions(this)) {
        return false;
    }
    bdd_klient_receive_modules(this);
    return true;
}

bool bdd_klient_receive_instructions(bdd_klient *this) {
    bdd_message msg;
    bdd_message_init(&msg, this->server_socket_);
    bdd_klient_receive_message(this, &msg);
    bdd_message_deserialize(&msg, deserialize_string);
    this->instructions = bdd_message_get_unique_payload(&msg);
    bdd_message_destroy(&msg);
    return strcmp(this->instructions, "Client closing") != 0;
}

void bdd_klient_receive_modules(bdd_klient *this) {
    bdd_message message;
    bdd_message_init(&message, this->server_socket_);
    bdd_klient_receive_message(this, &message);
    bdd_message_deserialize(&message, deserialize_int);
    int module_count = *(int*)bdd_message_get_payload(&message);
    bdd_message_destroy(&message);
    bdd_message_init(&message, this->server_socket_);

    for (int i = 0; i < module_count; i++) {
        bdd_klient_receive_message(this, &message);
        bdd_message_deserialize(&message, module_deserialize);
        module* result = bdd_message_get_unique_payload(&message);
        array_list_add(&this->modules_, &result);
        bdd_message_clear_buffer(&message);
    }

}

module* bdd_klient_get_module(bdd_klient* this, char* module_name) {
    module* temp = NULL;
    array_list_find_by_property(&this->modules_, &temp, module_match_name, module_name);
    return temp;
}

void bdd_klient_merge_modules(bdd_klient *this, char *instruction) {
    if (strncmp(instruction, "MERG", 4) == 0) {
        char* son_name = NULL;
        char* parent_name = NULL;
        sscanf(instruction, "MERG %ms %ms", &parent_name, &son_name);
        module* parent = bdd_klient_get_module(this, parent_name);
        module* son = bdd_klient_get_module(this, son_name);
        module_merge_modules(parent, son);
        free(son_name);
        free(parent_name);
    }
}

void bdd_klient_send_instruction(bdd_klient * this, char * instruction) {
    char* module_name = NULL;
    int client_id = 0;
    sscanf(instruction, "SEND %ms %d", &module_name, &client_id);
    module* mod = bdd_klient_get_module(this, module_name);
    bdd_message msg;
    bdd_message_init(&msg, client_id);
    bdd_message_set_payload(&msg, &mod, sizeof(module*));
    bdd_message_serialize(&msg, module_serialize);
    bdd_klient_send_message(this, &msg);
    bdd_message_destroy(&msg);
    free(module_name);
}

void bdd_klient_recv_instruction(bdd_klient * this, char * instruction) {
    char* module_name = NULL;
    sscanf(instruction, "RECV %ms", &module_name);
    bdd_message msg;
    bdd_message_init(&msg, this->server_socket_);
    bdd_klient_receive_message(this, &msg);
    bdd_message_deserialize(&msg, module_deserialize);
    module* mod = bdd_message_get_unique_payload(&msg);
    if (strcmp(module_name, module_get_name(mod)) != 0) {
        printf("Received bad module!! Got %s instead of %s\n", module_get_name(mod), module_name);
    }
    array_list_add(&this->modules_, &mod);
    bdd_message_destroy(&msg);
    free(module_name);
}

void bdd_klient_end_instruction(bdd_klient * this, char * instruction) {
    char* module_name = NULL;
    sscanf(instruction, "END %ms", &module_name);
    module* mod = bdd_klient_get_module(this, module_name);
    bdd_message msg;
    bdd_message_init(&msg, -2);
    bdd_message_set_payload(&msg, &mod, sizeof(module*));
    bdd_message_serialize(&msg, module_serialize);
    bdd_klient_send_message(this, &msg);
    bdd_message_destroy(&msg);
    free(module_name);
}

void bdd_klient_finish_instruction(bdd_klient *this) {
    int temp = 0;
    bdd_message msg;
    bdd_message_init(&msg, -1);
    bdd_message_set_payload(&msg, &temp, sizeof(int));
    bdd_message_serialize(&msg, serialize_int);
    bdd_klient_send_message(this, &msg);
    bdd_message_destroy(&msg);
}

void bdd_klient_execute_instructions(bdd_klient *this, void(*execute_instruction)(bdd_klient *bdd_klient_, char *instruction)) {
    FILE *stream = fmemopen(this->instructions, strlen(this->instructions), "r");
    if (!stream) {
        perror("Failed to open memory stream");
        return;
    }

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stream) != -1) {
        line[strcspn(line, "\n")] = '\0';

        if (strncmp(line, "SEND", 4) == 0) {
            bdd_klient_send_instruction(this, line);
        } else if (strncmp(line, "RECV", 4) == 0) {
            bdd_klient_recv_instruction(this, line);
        } else if (strncmp(line, "END", 3) == 0) {
            bdd_klient_end_instruction(this, line);
            free(line);
            fclose(stream);
            return;
        } else {
            execute_instruction(this, line);
        }
    }
    
    bdd_klient_finish_instruction(this);

    free(line);
    fclose(stream);
}

void bdd_klient_print_modules(bdd_klient *this) {
    for (int i = 0; i < array_list_get_size(&this->modules_); i++) {
        module* mod = NULL;
        array_list_try_get(&this->modules_, i, &mod);
        module_print_out(mod);
    }
}

void bdd_klient_print_instructions(bdd_klient *this) {
    printf("%s\n", this->instructions);
}

bool bdd_klient_test_connection(bdd_klient *this) {
    char buf;
    ssize_t ret = recv(this->server_socket_, &buf, 1, MSG_PEEK | MSG_DONTWAIT);

    if (ret == 0) {
        return false;
    }
    if (ret < 0) {
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            return false;
        }
    }

    return true;
}
