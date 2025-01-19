#ifndef COMM_UTILS_H
#define COMM_UTILS_H
#include <stddef.h>
#include <stdio.h>

/**
 * @brief Serializes a string into a dynamically allocated buffer.
 * @param payload Pointer to the string to serialize.
 * @param serialized_payload Pointer to the output serialized buffer.
 * @return Size of the serialized buffer.
 */
size_t serialize_string(void* payload, void** serialized_payload);

/**
 * @brief Serializes an integer into a dynamically allocated buffer.
 * @param payload Pointer to the integer to serialize.
 * @param serialized_payload Pointer to the output serialized buffer.
 * @return Size of the serialized buffer.
 */
size_t serialize_int(void* payload, void** serialized_payload);

/**
 * @brief Deserializes a string from a serialized buffer.
 * @param serialized_payload Pointer to the serialized buffer.
 * @param size Size of the serialized buffer.
 * @return Pointer to the deserialized string (must be freed by the caller).
 */
void* deserialize_string(const void* serialized_payload, size_t size);

/**
 * @brief Deserializes an integer from a serialized buffer.
 * @param serialized_payload Pointer to the serialized buffer.
 * @param size Size of the serialized buffer.
 * @return Pointer to the deserialized integer (must be freed by the caller).
 */
void* deserialize_int(const void* serialized_payload, size_t size);
/**
 * @brief Prints the contents of a buffer in hexadecimal and ASCII format.
 * @param buffer Pointer to the buffer.
 * @param size Size of the buffer.
 */
void print_buffer(const void* buffer, size_t size);
/**
 * @brief Sends all data in the buffer over a socket.
 * @param socket The socket descriptor.
 * @param buffer Pointer to the buffer to send.
 * @param length Length of the buffer.
 * @return Total bytes sent, or -1 on error.
 */
ssize_t send_all(int socket, const void* buffer, size_t length);

/**
 * @brief Receives a specific amount of data over a socket.
 * @param socket The socket descriptor.
 * @param buffer Pointer to the buffer where received data will be stored.
 * @param length Number of bytes to receive.
 * @return Total bytes received, or -1 on error.
 */
ssize_t recv_all(int socket, void* buffer, size_t length);

#endif //COMM_UTILS_H
