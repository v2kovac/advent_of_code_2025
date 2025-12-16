#include "../common.h"
#include <string.h>

typedef struct {
    char* array;
    int max_x;
    int max_y;
} A2D;

static A2D buffer_to_2d_array(Arena* arena, char* buffer) {
    char* newline = strchr(buffer, '\n');
    int max_x = (int)(newline - buffer);
    int max_y = 0;
    char* array = push_array(arena, char, strlen(buffer));
    int i = 0;

    for (; *buffer; buffer++) {
        if (*buffer == '\n') {
            max_y++;
        } else {
            array[i++] = *buffer;
        }
    }

    return (A2D) {
        .array = array,
        .max_x = max_x,
        .max_y = max_y,
    };
}

static char get2d(A2D a2d, int y, int x) {
    if (y >= a2d.max_y || x >= a2d.max_x) return '\0';
    if (y < 0 || x < 0) return '\0';

    return a2d.array[(y * a2d.max_y) + x];
}

int main(int argc, char** argv) {
    assert(argc == 2);
    Arena arena = init_arena(GB(1));
    char* filename = argv[1];
    char* buffer = read_entire_file(&arena, filename);
    assert(buffer != NULL);

    A2D a2d = buffer_to_2d_array(&arena, buffer);

    int result = 0;
    for (int y = 0; y < a2d.max_y; y++) {
        for (int x = 0; x < a2d.max_x; x++) {
            if (get2d(a2d, y, x) == '@') {
                int total = -1;
                for (int y1 = -1; y1 <= 1; y1++) {
                    for (int x1 = -1; x1 <= 1; x1++) {
                        if (get2d(a2d, y + y1, x + x1) == '@') total++;
                    }
                }
                if (total < 4) result++;
            }
        }
    }


    printf("answer: %d", result);
    assert(result == 1409);
    return 0;
}
