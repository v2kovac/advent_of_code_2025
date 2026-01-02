#include "../common.h"
#include <string.h>

#define Array(type) struct { type* data; size_t size; }
typedef Array(char) String;
typedef Array(String) StringArray;
typedef Array(StringArray) StringArrayArray;

#define dyn_begin(arena, type) (type*)((arena)->base + (arena)->used)
#define dyn_push(arena, type) push_struct(arena, type)
#define dyn_count(arena, begin, type) ((size_t)(((type*)((arena)->base + (arena)->used)) - ((type*)(begin))))

static StringArray split(Arena* arena, String str, char delim) {
    String* begin = dyn_begin(arena, String);
    size_t last_split_index = 0;

    for (size_t str_index = 0; str_index < str.size; str_index++) {
        if (str.data[str_index] == delim || str_index == str.size - 1) {
            size_t substr_size = str_index - last_split_index;
            if (substr_size > 0) {
                *dyn_push(arena, String) = (String) {
                    .data = str.data + last_split_index,
                    .size = substr_size,
                };
            }
            last_split_index = str_index + 1;
        }
    }

    return (StringArray) {
        .data = begin,
        .size = dyn_count(arena, begin, String),
    };
}

static String to_string(char* buff) {
    return (String) {
        .data = buff,
        .size = strlen(buff),
    };
}

static void print_StringArray(StringArray arr) {
    printf("[");
    for (size_t i = 0; i < arr.size; i++) {
        if (i > 0) printf(", ");
        printf("%.*s", (int)arr.data[i].size, arr.data[i].data);
    }
    printf("]");
}

static void print_StringArrayArray(StringArrayArray arr) {
    printf("[");
    for (size_t i = 0; i < arr.size; i++) {
        if (i == 0) printf("\n");
        printf("  ");
        print_StringArray(arr.data[i]);
        printf(",\n");
    }
    printf("]\n");
}

int main(int argc, char** argv) {
    assert(argc == 2);
    char* filename = argv[1];
    Arena arena = init_arena(GB(1));
    char* buffer = read_entire_file(&arena, filename);
    assert(buffer);

    StringArray lines = split(&arena, to_string(buffer), '\n');
    StringArrayArray arr2d = {
        .size = lines.size,
        .data = push_array(&arena, StringArray, lines.size),
    };
    for (size_t i = 0; i < lines.size; i++) {
        arr2d.data[i] = split(&arena, lines.data[i], ' ');
    }
    // print_StringArrayArray(arr2d);


    u64 result = 0;
    for (size_t x = 0; x < arr2d.data[0].size; x++) {
        u64 row = 0;
        bool add = *arr2d.data[arr2d.size - 1].data[x].data == '+';
        for (size_t y = 0; y < arr2d.size - 1; y++) {
            u64 num = strtoull(arr2d.data[y].data[x].data, NULL, 10);
            if (add) {
                row += num;
            } else {
                if (row == 0) row = 1;
                row *= num;
            }
        }
        result += row;
    }
    printf("answer: %llu\n", result);
    assert(result == 4805473544166);
}











