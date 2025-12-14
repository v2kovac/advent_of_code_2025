#include "../common.h"
#include <string.h>

int main(int argc, char** argv) {
    assert(argc == 2);
    Arena arena = init_arena(GB(1));
    char* filename = argv[1];
    char* buffer = read_entire_file(&arena, filename);
    assert(buffer != NULL);

    int result = 0;

    for (char* line = strtok(buffer, "\n"); line; line = strtok(NULL, "\n")) {
        char max_a = '0';
        char max_b = '0';
        while (*line) {
            if (*line > max_a && *(line + 1)) {
                max_b = '0';
                max_a = *line;
            } else if (*line > max_b) {
                max_b = *line;
            }
            line++;
        }
        char combined[] = { max_a, max_b, '\0'};
        // printf("combined: %s\n", combined);
        result += atoi(combined);
    }

    printf("answer: %d", result);
    assert(result == 17330);
    return 0;
}
