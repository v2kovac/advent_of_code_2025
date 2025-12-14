#include "../common.h"
#include <string.h>

static int num_digits(int64_t num) {
    if (num == 0) return 1;

    int result = 0;
    while (num > 0) {
        num = num / 10;
        ++result;
    }
    return result;
}

static void s64_to_string(char* buffer, int buffer_size, int64_t num) {
    assert(num > 0);
    buffer[buffer_size--] = '\0';
    while (num > 0) {
        buffer[buffer_size--] = '0' + (num % 10);
        num /= 10;
    }
}

int main(int argc, char** argv) {
    assert(argc == 2);
    char* filename = argv[1];
    Arena arena = init_arena(GB(1));

    char* buffer = read_entire_file(&arena, filename);
    assert(buffer != NULL);
    printf("string: %s\n", buffer);

    uint64_t result = 0;
    char* first;
    char* second;
    for (first = strtok(buffer, "-"); first; first = strtok(NULL, "-")) {
        second = strtok(NULL, ",\n");
        // printf("%s, %d, %d ; %s, %d, %d\n", first, first_num, size_first, second, second_num, size_second);

        int64_t first_num = atoll(first);
        int64_t second_num = atoll(second);
        for (int64_t i = first_num; i <= second_num; ++i) {
            int size = num_digits(i);
            char buff[size];
            s64_to_string(buff, size, i);

            int half_size = size / 2;
            char* p1 = buff;
            char* p2 = buff + half_size;
            bool match = true;

            if (size % 2 == 1) {
                match = false;
            }
            for (int j = 0; j < half_size && match; j++) {
                if (*p1++ != *p2++) {
                    match = false;
                }
            }
            if (match) {
                //printf("match: %lld\n", i);
                result += (uint64_t)i;
            }
        }
    }

    printf("\nanswer: %llu\n", result);
    assert(result == 38437576669);

    return 0;
}
