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

static bool invalid_id(int64_t num) {
    int size = num_digits(num);
    char buff[size];
    s64_to_string(buff, size, num);

    for (int window = 1; window <= size / 2; ++window) {
        if (size % window != 0) continue;
        // printf("num: %llu, window: %d\n", num, window);
        bool invalid = true;
        for (int cur_window_spot = 0; cur_window_spot < window; ++cur_window_spot) {
            char last = buff[cur_window_spot];
            for (int offset = 0; offset < size/window; ++offset) {
                char curr = buff[cur_window_spot + (offset * window)];
                if (last == curr) {
                    // fine
                    last = curr;
                } else {
                    invalid = false;
                    break;
                }
            }
        }
        if (invalid) {
            // printf("invalid: %lld\n", num);
            return true;
        }
    }

    return false;
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
            if (invalid_id(i)) {
                result += (uint64_t)i;
            }
        }
    }

    printf("\nanswer: %llu\n", result);
    assert(result == 49046150754);

    return 0;
}
