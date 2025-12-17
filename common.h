#pragma once

#include <sys/errno.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef float f32;
typedef double f64;

#define KB(value) ((value) * 1024ULL)
#define MB(value) (KB(value) * 1024ULL)
#define GB(value) (MB(value) * 1024ULL)
#define TB(value) (GB(value) * 1024ULL)

// memory stuff
typedef struct {
    size_t size;
    size_t used;
    uint8_t* base;
} Arena;

static Arena init_arena(size_t size) {
    Arena arena = {
        .size = size,
        .used = 0,
        .base = malloc(size),
    };
    return arena;
}

#define push_struct(arena, type) (type*)push_(arena, sizeof(type))
#define push_array(arena, type, count) (type*)push_(arena, (count) * sizeof(type))
static void* push_(Arena* arena, size_t size) {
    assert((arena->used + size) <= arena->size);
    void* result = arena->base + arena->used;
    arena->used += size;
    return result;
}

// general helpers
static int mod(int a, int b) {
    assert(b > 0);
    return ((a % b) + b) % b;
}

static char* read_entire_file(Arena* arena, char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) < 0) {
        perror("Failed to seek to end");
        return NULL;
    }

    long file_size = ftell(file);
    if (file_size < 0) {
        perror("Failed ftell");
        return NULL;
    }

    rewind(file);
    if (errno) {
        perror("Rewind failed");
        return NULL;
    }

    char* buffer = push_array(arena, char, (size_t)file_size + 1);
    size_t bytes_read = fread(buffer, 1, (unsigned long)file_size, file);
    if (ferror(file) != 0) {
        perror("fread failed");
        return NULL;
    }

    *(buffer + bytes_read) = '\0';

    return buffer;
}
