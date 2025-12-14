#include "../common.h"
#include <string.h>

#define SIZE 12

int main(int argc, char** argv) {
    assert(argc == 2);
    Arena arena = init_arena(GB(1));
    char* filename = argv[1];
    char* buffer = read_entire_file(&arena, filename);
    assert(buffer != NULL);

    uint64_t result = 0;

    for (char* line = strtok(buffer, "\n"); line; line = strtok(NULL, "\n")) {
        char max[SIZE + 1];
        memset(max, '0', sizeof(max));
        max[SIZE] = '\0';

        size_t line_len = strlen(line);
        for (size_t i = 0; i < line_len; ++i) {
            char cur = line[i];
            size_t line_len_left = line_len - (i + 1);
            for (size_t j = 0; j < SIZE; j++) {
                char cur_max = max[j];
                size_t max_len_left = SIZE - (j + 1);
                if (cur > cur_max && line_len_left >= max_len_left) {
                    for (size_t k = j+1; k < SIZE; k++) max[k] = '0';
                    max[j] = cur;
                    break;
                }
            }
        }

        result += (uint64_t)atoll(max);
    }

    printf("answer: %llu", result);
    assert(result == 171518260283767);
    return 0;
}
