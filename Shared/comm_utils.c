#include "comm_utils.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

size_t serialize_string(void* payload, void** serialized_payload) {
    if (!payload) {
        return 0;
    }

    char* string_payload = (char*)payload;
    size_t string_length = strlen(string_payload) + 1;

    *serialized_payload = malloc(string_length);
    if (!*serialized_payload) {
        perror("Chyba pri alokácii serializovaného bufferu");
        return 0;
    }

    memcpy(*serialized_payload, string_payload, string_length);

    return string_length;
}

size_t serialize_int(void* payload, void** serialized_payload) {
    if (!payload) {
        return 0;
    }

    *serialized_payload = malloc(sizeof(int));
    if (!*serialized_payload) {
        perror("Failed to allocate memory for serialized integer");
        return 0;
    }

    memcpy(*serialized_payload, payload, sizeof(int));
    return sizeof(int);
}

void* deserialize_string(const void* serialized_payload, size_t size) {
    if (!serialized_payload || size <= 0) {
        printf("deserialize_string: Invalid serialized payload or size\n");
        return NULL;
    }

    char* deserialized_string = malloc(size);
    if (!deserialized_string) {
        perror("Memory allocation failed for deserialized string");
        return NULL;
    }

    memcpy(deserialized_string, serialized_payload, size);
    deserialized_string[size - 1] = '\0';

    return deserialized_string;
}

void* deserialize_int(const void* serialized_payload, size_t size) {
    if (!serialized_payload || size != sizeof(int)) {
        printf("Invalid serialized payload or size for integer deserialization\n");
        return NULL;
    }

    int* deserialized_int = malloc(sizeof(int));
    if (!deserialized_int) {
        perror("Failed to allocate memory for deserialized integer");
        return NULL;
    }

    memcpy(deserialized_int, serialized_payload, sizeof(int));
    return deserialized_int;
}

void print_buffer(const void* buffer, size_t size) {
    const unsigned char* buf = (const unsigned char*)buffer;

    printf("Buffer contents (size: %zu):\n", size);
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", buf[i]);

        if ((i + 1) % 16 == 0 || i == size - 1) {
            printf(" | ");
            for (size_t j = (i / 16) * 16; j <= i; j++) {
                printf("%c", isprint(buf[j]) ? buf[j] : '.');
            }
            printf("\n");
        }
    }
}

ssize_t send_all(int socket, const void* buffer, size_t length) {
    size_t total_sent = 0;
    while (total_sent < length) {
        ssize_t sent = send(socket, (const char*)buffer + total_sent, length - total_sent, 0);
        if (sent <= 0) {
            return sent;
        }
        total_sent += sent;
    }
    return total_sent;
}

ssize_t recv_all(int socket, void* buffer, size_t length) {
    size_t total_received = 0;
    while (total_received < length) {
        ssize_t received = recv(socket, (char*)buffer + total_received, length - total_received, 0);
        if (received <= 0) {
            return received;
        }
        total_received += received;
    }
    return total_received;
}