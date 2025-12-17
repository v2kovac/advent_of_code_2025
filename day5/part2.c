#include "../common.h"
#include <string.h>

typedef struct {
    u64 start;
    u64 end;
} Range;

static void print(Range* ranges, int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("Range(%llu, %llu)", ranges[i].start, ranges[i].end);
        if (i != size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}

int compare_ranges(const void* a, const void* b) {
    Range first = *(Range*)a;
    Range second = *(Range*)b;
    if (first.start < second.start) {
        return -1;
    } else if (first.start > second.start) {
        return 1;
    } else {
        return 0;
    }
}

int main(int argc, char** argv) {
    assert(argc == 2);
    Arena arena = init_arena(GB(1));
    char* filename = argv[1];
    char* buffer = read_entire_file(&arena, filename);
    assert(buffer != NULL);

    Range* ranges = (Range*)(arena.base + arena.used);
    s32 ranges_size = 0;

    char* main_ctx;
    for(char* line = strtok_r(buffer, "\n", &main_ctx); line; line = strtok_r(NULL, "\n", &main_ctx)) {
        char* inner_ctx;
        char* first = strtok_r(line, "-", &inner_ctx);
        char* last = strtok_r(NULL, "-", &inner_ctx);

        if (last) {
            Range* range = push_array(&arena, Range, 1);
            range->start = (u64)atoll(first);
            range->end = (u64)atoll(last);
        } else {
            if (ranges_size == 0) {
                // figure out how to do these allocations a bit better
                ranges_size = (s32)((Range*)(arena.base + arena.used) - ranges);
            }
            break;
        }
    }

    qsort(ranges, (size_t)ranges_size, sizeof(Range), compare_ranges);

    for (int i = 0; i < ranges_size-1; i++) {
        Range* a = ranges + i;
        if (a->start == 0) continue;

        for (int j = i + 1; j < ranges_size; j++) {
            Range* b = ranges + j;
            if (b->start >= a->start && b->start <= a->end) {
                a->start = a->start < b->start ? a->start : b->start;
                a->end = a->end > b->end ? a->end : b->end;
                b->start = 0;
                b->end = 0;
            } else {
                break;
            }
        }
    }

    u64 result = 0;
    for (int i = 0; i < ranges_size; i++) {
        Range r = ranges[i];
        if (r.start == 0) continue;
        result = result + (r.end - r.start) + 1;
    }

    printf("answer: %llu\n", result);
    assert(result == 343329651880509);
    return 0;
} 

