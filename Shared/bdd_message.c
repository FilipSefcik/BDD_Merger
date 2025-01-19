#include "bdd_message.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void bdd_message_init(bdd_message* this, int client_id) {
        this->payload_ = NULL;
        this->payload_size_ = 0;
        this->client_id_ = client_id;
        this->serialized_buffer_ = NULL;
        this->serialized_buffer_size_ = 0;
}

void bdd_message_destroy(bdd_message* this) {
    if (this->payload_) {
        free(this->payload_);
        this->payload_ = NULL;
    }
    this->payload_size_ = 0;
    this->client_id_ = 0;
    bdd_message_clear_buffer(this);
}

void bdd_message_assign(bdd_message *this, bdd_message *other) {
    if (!this || !other) {
        return;
    }

    if (this->serialized_buffer_) {
        free(this->serialized_buffer_);
        this->serialized_buffer_ = NULL;
    }

    this->serialized_buffer_size_ = other->serialized_buffer_size_;
    this->client_id_ = other->client_id_;
    this->payload_size_ = other->payload_size_;

    if (other->serialized_buffer_ && other->serialized_buffer_size_ > 0) {
        this->serialized_buffer_ = malloc(other->serialized_buffer_size_);
        memcpy(this->serialized_buffer_, other->serialized_buffer_, other->serialized_buffer_size_);
    }

    if (other->payload_ && other->payload_size_ > 0) {
        this->payload_ = malloc(other->payload_size_);
        memcpy(this->payload_, other->payload_, other->payload_size_);
    } else {
        this->payload_ = NULL;
    }
}

void bdd_message_set_client_id(bdd_message *this, int client_id) {
    this->client_id_ = client_id;
}

int bdd_message_get_client_id(bdd_message *this) {
    return this->client_id_;
}

size_t bdd_message_get_payload_size(bdd_message *this) {
    return this->payload_size_;
}

void * bdd_message_get_payload(bdd_message *this) {
    return this->payload_;
}

void * bdd_message_get_unique_payload(bdd_message *this) {
    void* temp = this->payload_;
    this->payload_ = NULL;
    this->payload_size_ = 0;
    return temp;
}

size_t* bdd_message_get_buffer_size(bdd_message *this) {
    return &this->serialized_buffer_size_;
}

void * bdd_message_get_buffer(bdd_message *this) {
    return this->serialized_buffer_;
}

void bdd_message_set_payload(bdd_message* this, const void* data, int data_size) {
    if (this->payload_) {
        free(this->payload_);
    }
    this->payload_size_ = data_size;
    this->payload_ = malloc(this->payload_size_);
    if (this->payload_ && data) {
        memcpy(this->payload_, data, data_size);
    }
}

void bdd_message_clear_buffer(bdd_message* this) {
    if (this->serialized_buffer_) {
        free(this->serialized_buffer_);
        this->serialized_buffer_ = NULL;
        this->serialized_buffer_size_ = 0;
    }
}

void bdd_message_allocate_buffer(bdd_message *this, size_t buffer_size) {
    if (this->serialized_buffer_) {
        bdd_message_clear_buffer(this);
    }
    this->serialized_buffer_size_ = buffer_size;
    this->serialized_buffer_ = malloc(this->serialized_buffer_size_);
    memset(this->serialized_buffer_, 0, this->serialized_buffer_size_);
}

size_t bdd_message_serialize(
    bdd_message* this,
    size_t (*serialize_payload)(void* payload, void** serialized_payload)
) {
    void* serialized_payload = NULL;
    size_t serialized_payload_size = serialize_payload(this->payload_, &serialized_payload);

    if (serialized_payload_size == 0) {
        return 0;
    }

    size_t total_size = sizeof(this->client_id_) + sizeof(serialized_payload_size) + serialized_payload_size;

    bdd_message_allocate_buffer(this, total_size);

    size_t offset = 0;

    memcpy((char*)this->serialized_buffer_ + offset, &this->client_id_, sizeof(this->client_id_));
    offset += sizeof(this->client_id_);

    memcpy((char*)this->serialized_buffer_ + offset, &serialized_payload_size, sizeof(serialized_payload_size));
    offset += sizeof(serialized_payload_size);

    memcpy((char*)this->serialized_buffer_ + offset, serialized_payload, serialized_payload_size);

    free(serialized_payload);
    return total_size;
}

size_t bdd_message_deserialize(
    bdd_message* this,
    void* (*deserialize_payload)(const void* serialized_payload, size_t size)
) {
    size_t offset = 0;

    if (!this->serialized_buffer_ || this->serialized_buffer_size_ < sizeof(this->client_id_) + sizeof(this->payload_size_)) {
        return 0;
    }

    memcpy(&this->client_id_, (char*)this->serialized_buffer_ + offset, sizeof(this->client_id_));
    offset += sizeof(this->client_id_);

    size_t payload_size;
    memcpy(&payload_size, (char*)this->serialized_buffer_ + offset, sizeof(payload_size));
    offset += sizeof(payload_size);

    if (this->serialized_buffer_size_ < offset + payload_size) {
        return 0;
    }

    if (this->payload_) {
        free(this->payload_);
    }

    if (deserialize_payload) {
        this->payload_ = deserialize_payload((char*)this->serialized_buffer_ + offset, payload_size);
        if (!this->payload_) {
            return 0;
        }
    } else {
        this->payload_ = malloc(payload_size);
        if (!this->payload_) {
            return 0;
        }
        memcpy(this->payload_, (char*)this->serialized_buffer_ + offset, payload_size);
    }

    this->payload_size_ = payload_size;
    offset += payload_size;

    return offset;
}


