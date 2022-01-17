#include "GrowAbleStream.h"

void init_growable_stream(struct GrowAbleStream* stream, uint32_t bytes) {
    stream->size = bytes;
    stream->used = 0;
    stream->data = malloc(bytes);
}

void free_growable_stream(struct GrowAbleStream* stream) {
    stream->size = 0;
    stream->used = 0;
    free(stream->data);
}

void* commit_to_growable_stream(struct GrowAbleStream* stream, uint32_t bytes) {
    if (stream->size <= stream->used + bytes) {
        uint32_t newSize = stream->size;
        while (newSize < stream->size + bytes) newSize *= 2;
        void* result = realloc(stream->data, newSize);
        if (result != NULL) {
            stream->data = result;
            stream->size = newSize;
        } else {
            void* newData = malloc(newSize);
            memmove(newData, stream->data, stream->size);
            free(stream->data);
            stream->data = newData;
            stream->size = newSize;
        }
    }

    void* dataLocation = ((uint8_t*)stream->data) + stream->used;
    stream->used += bytes;
    return dataLocation;
}

GrowableStreamHandle get_imperative_handle(struct GrowAbleStream* array, void* handle) {
    return (uint8_t*)handle - (uint8_t*)array->data;
}

void* imperative_handle_to_pointer(struct GrowAbleStream* array, GrowableStreamHandle handle) {
    return (uint8_t*)array->data + handle;
}