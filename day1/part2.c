#include "../common.h"

#define DIV 100

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
        assert(amount > 0);

        if (amount >= 100) {
            buffer += 4;
        } else if (amount >= 10) {
            buffer += 3;
        } else {
            buffer += 2;
        }

        // printf("n: %d dir:%c amount:%d div:%d result:%d\n", n, dir, amount, amount / DIV, result);
        result += (amount / DIV);
        amount = mod(amount, DIV);

        if (dir == 'L') {
            amount = -amount;
        } else if (dir == 'R') {
            //leave as is
        } else {
            assert(false);
        }

        if (amount != 0) {
            if (n == 0) {
                n += amount;
            } else {
                n += amount;
                if (n <= 0 || n >= 100) {
                    result++;
                }
            }
        }
        n = mod(n, DIV);
    }

    printf("answer: %d\n", result);
    return 0;
}
