#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef size_t GrowableStreamHandle;

struct GrowAbleStream {
    uint32_t size;
    uint32_t used;
    void* data;
};

/**
 * Initializes a growable stream with start size of bytes
*/
void init_growable_stream(struct GrowAbleStream* array, uint32_t bytes);
/**
 * Frees the allocated storage for the growable stream
*/
void free_growable_stream(struct GrowAbleStream* array);
/**
 * Appends a buffer with the specified size at the end of the growable stream.
 * Warning: the pointer is only guaranteed to be valid until the next commit call.
 * Use imperative handles if you want to hold on to your pointer for a longer period.
*/
void* commit_to_growable_stream(struct GrowAbleStream* array, uint32_t bytes);
/**
 * Creates an imperative handle for a void*.
 * Warning: when misused will return an invalid handle.
*/
GrowableStreamHandle get_imperative_handle(struct GrowAbleStream* array, void* handle);
/**
 * Converts an imperative handle to the pointer.
 * This pointer is only guaranteed to bevalid until the buffer grows.
*/
void* imperative_handle_to_pointer(struct GrowAbleStream* array, GrowableStreamHandle handle);