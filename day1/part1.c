#include <stdio.h>
#include <sys/errno.h>
#include "../common.h"

int main(int argc, char** argv) {
    assert(argc == 2);
    Arena arena = init_arena(GB(1));

    char* buffer = read_entire_file(&arena, argv[1]);
    assert(buffer != NULL);

    int n = 50;
    int result = 0;
    while (*buffer != '\0') {
        char dir = *buffer++;
        int amount = atoi(buffer);

        if (dir == 'L') {
            n -= amount;
        } else if (dir == 'R') {
            n += amount;
        } else {
            assert(false);
        }
        n = mod(n, 100);
        if (n == 0) {
            result++;
        }

        if (amount >= 100) {
            buffer += 4;
        } else if (amount >= 10) {
            buffer += 3;
        } else {
            buffer += 2;
        }
    }


    printf("answer: %d", result);
    return 0;
}
