#include "../common.h"
#include <string.h>

int main(int argc, char** argv) {
    assert(argc == 2);
    Arena arena = init_arena(GB(1));
    char* filename = argv[1];
    char* buffer = read_entire_file(&arena, filename);
    assert(buffer != NULL);

    u64* ranges = (u64*)(arena.base + arena.used);
    s32 ranges_size = 0;
    u64* items = NULL;
    s32 items_size;

    char* main_ctx;
    for(char* line = strtok_r(buffer, "\n", &main_ctx); line; line = strtok_r(NULL, "\n", &main_ctx)) {
        char* inner_ctx;
        char* first = strtok_r(line, "-", &inner_ctx);
        char* last = strtok_r(NULL, "-", &inner_ctx);

        if (last) {
            u64* a = push_array(&arena, u64, 2);
            *a++ = (u64)atoll(first);
            *a = (u64)atoll(last);
        } else {
            if (ranges_size == 0) {
                ranges_size = (s32)((u64*)(arena.base + arena.used) - ranges);
            }
            if (items == NULL) {
                items = (u64*)(arena.base + arena.used);
            }
            u64* a = push_struct(&arena, u64);
            *a = (u64)atoll(first);
        }
    }
    items_size = (s32)((u64*)(arena.base + arena.used) - items);

    int result = 0;
    for (int i = 0; i < items_size; i++) {
        u64 item = items[i];
        for (int j = 0; j < ranges_size-1; j += 2) {
            u64 left = ranges[j];
            u64 right = ranges[j+1];
            if (item >= left && item <= right) {
                result++;
                break;
            }
        }
    }

    printf("answer: %d\n", result);
    assert(result == 789);
    return 0;
}
