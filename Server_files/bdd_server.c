#include "bdd_server.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <asm-generic/errno.h>

#include "../Shared/module.h"
#include "../Shared/comm_utils.h"

void thread_args_init(thread_args *this, bdd_server *server, pthread_mutex_t *mutex, int client_id) {
    this->server_ = server;
    this->mutex_ = mutex;
    this->client_id_ = client_id;
}

void thread_args_destroy(thread_args *this) {
    this->mutex_ = NULL;
    this->server_ = NULL;
    this->client_id_ = 0;
}

bdd_server * thread_args_get_server(thread_args* this) {
    return this->server_;
}

pthread_mutex_t * thread_args_get_mutex(thread_args *this) {
    return this->mutex_;
}

int thread_args_get_client_id(thread_args *this) {
    return this->client_id_;
}

void bdd_server_init(bdd_server* this) {
    array_list_init(&this->client_sockets_, sizeof(int));
}

void bdd_server_destroy(bdd_server* this) {
    bdd_server_end_sessions(this);
    array_list_destroy(&this->client_sockets_);
    close(this->server_id_);
}

void bdd_server_end_session(bdd_server *this, int client_id) {
    int unused_client_id;
    array_list_try_get(&this->client_sockets_, client_id, &unused_client_id);
    close(unused_client_id);
    int unused_flag = -1;
    array_list_set(&this->client_sockets_, client_id, &unused_flag);
}

void bdd_server_end_sessions(bdd_server* this) {
    for (int i = 0; i < array_list_get_size(&this->client_sockets_); i++) {
        int client_fd;
        array_list_try_get(&this->client_sockets_, i, &client_fd);
        if (client_fd >= 0) {
            close(client_fd);
        }
    }

    array_list_clear(&this->client_sockets_);
}

int bdd_server_get_client_count(bdd_server *this) {
    return array_list_get_size(&this->client_sockets_);
}

bool bdd_server_bind_server(bdd_server *this, int port, char *server_address) {
    this->server_id_ = socket(AF_INET, SOCK_STREAM, 0);
    if (this->server_id_ == -1) {
        perror("Chyba pri vytváraní socketu");
        return false;
    }

    this->server_addr_.sin_family = AF_INET;
    this->server_addr_.sin_port = htons(port);

    if (strcmp(server_address, "any") == 0) {
        this->server_addr_.sin_addr.s_addr = INADDR_ANY;
    } else {
        if (inet_pton(AF_INET, server_address, &this->server_addr_.sin_addr) <= 0) {
            perror("Neplatná IP adresa");
            close(this->server_id_);
            return false;
        }
    }

    int opt = 1;
    if (setsockopt(this->server_id_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("Chyba pri nastavovaní SO_REUSEADDR");
        close(this->server_id_);
        return false;
    }

    if (bind(this->server_id_, (struct sockaddr*)&this->server_addr_, sizeof(this->server_addr_)) == -1) {
        perror("Chyba pri bindovaní");
        close(this->server_id_);
        return false;
    }

    return true;
}

void bdd_server_standby(bdd_server* this, int client_count) {
    if (listen(this->server_id_, MAX_CLIENTS) == -1) {
        perror("Chyba pri počúvaní");
        close(this->server_id_);
        return;
    }

    printf("Server je pripravený na nové spojeni%s.\n", client_count == 1 ? "e" : "a");

    for (int i = 0; i < client_count; i++) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_id;
        if ((client_id = accept(this->server_id_, (struct sockaddr*)&client_addr, &addr_len)) == -1) {
            printf("Chyba pri pripájaní klienta.\n");
            i--;
            continue;
        }

        printf("%d/%d pripojený!\n", i + 1, client_count);

        array_list_add(&this->client_sockets_, &client_id);
    }

    shutdown(this->server_id_, SHUT_RD);
}

void bdd_server_send_message(bdd_server* this, int receiver_id, bdd_message* message, pthread_mutex_t *mutex) {
    int client_fd;
    if (mutex) { pthread_mutex_lock(mutex); }
    array_list_try_get(&this->client_sockets_, receiver_id, &client_fd);
    if (mutex) { pthread_mutex_unlock(mutex); }

    if (client_fd < 0) {
        printf("Klient %d sa nepoužíva.\n", receiver_id);
        return;
    }

    send_all(client_fd, bdd_message_get_buffer_size(message), sizeof(*bdd_message_get_buffer_size(message)));
    send_all(client_fd, bdd_message_get_buffer(message), *bdd_message_get_buffer_size(message));
}

void bdd_server_receive_message(bdd_server *this, int sender_id, bdd_message* message, pthread_mutex_t *mutex) {
    int client_fd;
    if (mutex) { pthread_mutex_lock(mutex); }
    array_list_try_get(&this->client_sockets_, sender_id, &client_fd);
    if (mutex) { pthread_mutex_unlock(mutex); }

    if (client_fd < 0) {
        printf("Klient %d sa nepoužíva.\n", sender_id);
        return;
    }

    recv_all(client_fd, bdd_message_get_buffer_size(message), sizeof(*bdd_message_get_buffer_size(message)));
    bdd_message_allocate_buffer(message, *bdd_message_get_buffer_size(message));
    recv_all(client_fd, bdd_message_get_buffer(message), *bdd_message_get_buffer_size(message));
}

void bdd_server_forward_message(bdd_server *this, int client_id, bdd_message* forwarded_message, pthread_mutex_t *mutex) {
    bdd_server_receive_message(this, client_id, forwarded_message, mutex);
    bdd_message_deserialize(forwarded_message, NULL);
    if (bdd_message_get_client_id(forwarded_message) < 0) {
        return;
    }
    bdd_server_send_message(this, bdd_message_get_client_id(forwarded_message), forwarded_message, mutex);
    bdd_message_clear_buffer(forwarded_message);
}

void * bdd_server_forwarding_mode(void* args) {
    bdd_server* this = thread_args_get_server((thread_args*)args);
    pthread_mutex_t* mutex = thread_args_get_mutex((thread_args*)args);
    int client_id = thread_args_get_client_id((thread_args*)args);

    bdd_message forwarded_message;
    bdd_message_init(&forwarded_message, 0);
    while (bdd_message_get_client_id(&forwarded_message) >= 0) {
        bdd_server_forward_message(this, client_id, &forwarded_message, mutex);
    }

    bdd_message* final_message = NULL;
    if (bdd_message_get_client_id(&forwarded_message) == -2) {
        final_message = malloc(sizeof(bdd_message));
        bdd_message_init(final_message, 0);
        bdd_message_assign(final_message, &forwarded_message);
    }
    bdd_message_destroy(&forwarded_message);
    return final_message;
}

void bdd_server_send_modules(bdd_server *this, array_list *modules, int* distribution) {

    bdd_message message;
    int client;
    for (int i = 0; i < bdd_server_get_client_count(this); i++) {
        if (distribution[i] > 0) {
            array_list_try_get(&this->client_sockets_, i, &client);
            bdd_message_init(&message, client);
            bdd_message_set_payload(&message, &distribution[i], sizeof(int));
            bdd_message_serialize(&message, serialize_int);
            bdd_server_send_message(this, i, &message, NULL);
            bdd_message_destroy(&message);
        }
    }

    module* temp = NULL;
    int client_id = 0;
    for (int i = 0; i < array_list_get_size(modules); i++) {
        array_list_try_get(modules, i, &temp);
        client_id = module_get_assigned_client(temp);
        bdd_message_init(&message, client_id);
        bdd_message_set_payload(&message, &temp, sizeof(module*));
        bdd_message_serialize(&message, module_serialize);
        bdd_server_send_message(this, client_id, &message, NULL);
        bdd_message_destroy(&message);
    }
}


_Bool bdd_server_send_instructions(bdd_server *this, char **instructions) {
    bdd_message message;
    char* temp = NULL;
    for (int i = 0; i < bdd_server_get_client_count(this); i++) {
        int client_fd;
        array_list_try_get(&this->client_sockets_, i, &client_fd);
        if (!is_client_connected(client_fd)) {
            return false;
        }

        bdd_message_init(&message, -1);
        temp = instructions[i];

        if (temp[0] == 'X') {
            temp = "Client closing";
        }

        bdd_message_set_payload(&message, temp, strlen(temp) + 1);
        bdd_message_serialize(&message, serialize_string);
        bdd_server_send_message(this, i, &message, NULL);
        bdd_message_destroy(&message);

        if (strcmp(temp, "Client closing") == 0) {
            bdd_server_end_session(this, i);
        }
    }
    return true;
}

void bdd_server_execute_instructions(bdd_server *this, bdd_message *result) {
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    thread_args* args = malloc(bdd_server_get_client_count(this) * sizeof(thread_args));
    array_list threads;
    array_list_init(&threads, sizeof(pthread_t*));

    for (int i = 0; i < bdd_server_get_client_count(this); i++) {
        int client_fd;
        array_list_try_get(&this->client_sockets_, i, &client_fd);
        if (client_fd >= 0) {
            thread_args_init(args + i, this, &mutex, i);
            pthread_t* thread= malloc(sizeof(pthread_t));
            array_list_add(&threads, &thread);
            pthread_create(thread, NULL, bdd_server_forwarding_mode, args + i);
        }
    }

    bool found_result = false;

    for (int i = 0; i < array_list_get_size(&threads); i++) {
        pthread_t* thread;
        array_list_try_get(&threads, i, &thread);
        if (!found_result) {
            bdd_message* thread_result = NULL;
            pthread_join(*thread, (void**)&thread_result);
            if (thread_result) {
                bdd_message_assign(result, thread_result);
                bdd_message_destroy(thread_result);
                free(thread_result);
                found_result = true;
            }
        } else {
            pthread_join(*thread, NULL);
        }
        free(thread);
    }

    free(args);
    array_list_destroy(&threads);
    pthread_mutex_destroy(&mutex);
}

bool is_client_connected(int client_fd)
{
    char buf;
    ssize_t ret = recv(client_fd, &buf, 1, MSG_PEEK | MSG_DONTWAIT);

    if (ret == 0) {
        return false;
    } else if (ret < 0) {
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            return false;
        }
    }

    return true;
}

int bdd_server_test_connections(bdd_server *this) {
    if (bdd_server_get_client_count(this) == 0) {
        return 0;
    }

    int disconnected_clients = 0;

    for (int i = 0; i < bdd_server_get_client_count(this); i++) {
        int client_fd;
        array_list_try_get(&this->client_sockets_, i, &client_fd);

        if (client_fd < 0 || !is_client_connected(client_fd)) {
            if (client_fd >= 0) {
                bdd_server_end_session(this, i);
            }
            array_list_remove_at(&this->client_sockets_, i);
            i--;
            disconnected_clients++;
        }
    }

    return disconnected_clients;
}

