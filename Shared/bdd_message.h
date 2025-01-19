#ifndef BDD_MESSAGE_H
#define BDD_MESSAGE_H
#include <stddef.h>

/**
 * @brief Represents a message structure for communication between a client and server.
 *
 * Fields:
 * - payload_: Dynamically allocated data associated with the message.
 * - payload_size_: Size of the payload data.
 * - client_id_: Identifier for the client associated with the message.
 * - serialized_buffer_: Buffer for serialized message data.
 * - serialized_buffer_size_: Size of the serialized buffer.
 */
typedef struct bdd_message {
    void* payload_;
    size_t payload_size_;
    int client_id_;
    void* serialized_buffer_;
    size_t serialized_buffer_size_;
} bdd_message;

/**
 * @brief Initializes a bdd_message instance.
 * @param this Pointer to the bdd_message instance.
 * @param client_id Identifier for the client.
 */
void bdd_message_init(bdd_message* this, int client_id);

/**
 * @brief Destroys a bdd_message instance and frees allocated resources.
 * @param this Pointer to the bdd_message instance.
 */
void bdd_message_destroy(bdd_message* this);

/**
 * @brief Assigns the contents of one bdd_message instance to another.
 * @param this Pointer to the destination bdd_message.
 * @param other Pointer to the source bdd_message.
 */
void bdd_message_assign(bdd_message* this, bdd_message* other);

/**
 * @brief Sets the client ID of the message.
 * @param this Pointer to the bdd_message instance.
 * @param client_id The new client ID.
 */
void bdd_message_set_client_id(bdd_message* this, int client_id);

/**
 * @brief Retrieves the client ID of the message.
 * @param this Pointer to the bdd_message instance.
 * @return The client ID.
 */
int bdd_message_get_client_id(bdd_message* this);

/**
 * @brief Retrieves the size of the payload data.
 * @param this Pointer to the bdd_message instance.
 * @return The size of the payload data.
 */
size_t bdd_message_get_payload_size(bdd_message* this);

/**
 * @brief Retrieves the payload data.
 * @param this Pointer to the bdd_message instance.
 * @return Pointer to the payload data.
 */
void* bdd_message_get_payload(bdd_message* this);

/**
 * @brief Retrieves and clears the payload data.
 * @param this Pointer to the bdd_message instance.
 * @return Pointer to the payload data, or NULL if no data exists.
 */
void* bdd_message_get_unique_payload(bdd_message* this);

/**
 * @brief Retrieves the size of the serialized buffer.
 * @param this Pointer to the bdd_message instance.
 * @return Pointer to the size of the serialized buffer.
 */
size_t* bdd_message_get_buffer_size(bdd_message* this);

/**
 * @brief Retrieves the serialized buffer.
 * @param this Pointer to the bdd_message instance.
 * @return Pointer to the serialized buffer.
 */
void* bdd_message_get_buffer(bdd_message* this);

/**
 * @brief Sets the payload data of the message.
 * @param this Pointer to the bdd_message instance.
 * @param data Pointer to the data to be set.
 * @param data_size Size of the data.
 */
void bdd_message_set_payload(bdd_message* this, const void* data, int data_size);

/**
 * @brief Clears the serialized buffer of the message.
 * @param this Pointer to the bdd_message instance.
 */
void bdd_message_clear_buffer(bdd_message* this);

/**
 * @brief Allocates a serialized buffer of a specified size.
 * @param this Pointer to the bdd_message instance.
 * @param buffer_size The size of the buffer to allocate.
 */
void bdd_message_allocate_buffer(bdd_message* this, size_t buffer_size);

/**
 * @brief Serializes the message into a buffer.
 * @param this Pointer to the bdd_message instance.
 * @param serialize_payload Function to serialize the payload.
 * @return Total size of the serialized message.
 */
size_t bdd_message_serialize(bdd_message* this, size_t (*serialize_payload)(void* payload, void** serialized_payload));

/**
 * @brief Deserializes a message from a buffer.
 * @param this Pointer to the bdd_message instance.
 * @param deserialize_payload Function to deserialize the payload.
 * @return Total size of the deserialized message.
 */
size_t bdd_message_deserialize(bdd_message* this, void* (*deserialize_payload)(const void* serialized_payload, size_t size));

#endif //BDD_MESSAGE_H
