#pragma once

#include <sys/errno.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/mman.h>

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
#define UNUSED(arg) (void)(arg)
#define TODO(str) do { fprintf(stderr, "%s:%d:0 TODO %s", __FILE__, __LINE__, (str)); abort(); } while(0)
#define UNREACHABLE(str) do { fprintf(stderr, "%s:%d:0 UNREACHABLE %s", __FILE__, __LINE__, (str)); __builtin_unreachable(); } while(0)
#define SV_FMT "%.*s"
#define SV_ARG(sv) (int)(sv).count, (sv).data

// arena stuff
typedef struct {
    size_t size;
    size_t used;
    u8* base;
} Arena;

static Arena init_arena(size_t size) {
    Arena arena = {
        .size = size,
        .used = 0,
        .base = (u8*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0),
    };
    if (errno) {
        perror("mmap failed");
    }
    assert(arena.base != MAP_FAILED);
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
    char* buffer = NULL;
    FILE* file = fopen(filename, "r");
    long file_size = 0;
    size_t bytes_read = 0;
    if (file == NULL) {
        perror("Failed to open file");
        goto defer;
    }

    if (fseek(file, 0, SEEK_END) < 0) {
        perror("Failed to seek to end");
        goto defer;
    }

    file_size = ftell(file);
    if (file_size < 0) {
        perror("Failed ftell");
        goto defer;
    }

    rewind(file);
    if (errno) {
        perror("Rewind failed");
        goto defer;
    }

    buffer = push_array(arena, char, (size_t)file_size + 1);
    bytes_read = fread(buffer, 1, (unsigned long)file_size, file);
    if (ferror(file)) {
        perror("fread failed");
        buffer = NULL;
        goto defer;
    }
    *(buffer + bytes_read) = '\0';

defer:
    fclose(file);
    return buffer;
}

// Dynamic Arrays Stuff

#define DA_INIT_CAP 256
#define da_reserve(da, expected_capacity) \
    do { \
        if ((expected_capacity) > (da)->capacity) { \
            if ((da)->capacity == 0) { \
                (da)->capacity = DA_INIT_CAP; \
            } \
            while ((expected_capacity) > (da)->capacity) { \
                (da)->capacity *= 2; \
            } \
            (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items)); \
            assert((da)->items && "DA not allocated"); \
        } \
    } while(0)

#define da_append(da, item) \
    do { \
        da_reserve((da), (da)->count+1); \
        (da)->items[(da)->count++] = item; \
    } while(0)

#define da_resize(da, size) \
    do { \
        da_reserve((da), size); \
        (da)->count = (size); \
    } while(0)


typedef struct {
    char* items;
    size_t count;
    size_t capacity;
} StringBuilder;

typedef struct {
    const char* data;
    size_t count;
} StringView;

typedef struct {
    StringView* items;
    size_t count;
    size_t capacity;
} StringViews;


static bool sb_read_entire_file(char* filename, StringBuilder* sb) {
    long file_size = 0;
    bool result = false;

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        goto defer;
    }

    if (fseek(file, 0, SEEK_END) < 0) {
        perror("Failed to seek to end");
        goto defer;
    }

    file_size = ftell(file);
    if (file_size < 0) {
        perror("Failed ftell");
        goto defer;
    }

    rewind(file);
    if (errno) {
        perror("Rewind failed");
        goto defer;
    }

    da_reserve(sb, sb->count + (size_t)file_size);
    fread(sb->items + sb->count, 1, (size_t)file_size, file);
    if (ferror(file) != 0) {
        perror("fread failed");
        goto defer;
    }
    sb->count += (size_t)file_size;
    result = true;

defer:
    if (file) fclose(file);
    return result;
}

static StringView to_string_view(const char* data, size_t count) {
    return (StringView) {
        .data = data,
        .count = count,
    };
}

static StringViews sv_split_alloc(StringView* sv, char delim) {
    StringViews result = {0};
    char preamble = true;
    size_t start = 0;
    for (size_t i = 0; i < sv->count; i++) {
        if (preamble && sv->data[i] != delim) {
            preamble = false;
            start = i;
        } else if (!preamble && sv->data[i] == delim) {
            da_append(&result, to_string_view(sv->data + start, i - start));
            preamble = true;
        }
    }

    if (!preamble) {
        da_append(&result, to_string_view(sv->data + start, sv->count - start));
    }

    return result;
}
