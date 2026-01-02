#include "../common.h"

typedef struct {
    u64* items;
    size_t count;
    size_t capacity;
} U64s;


int main(int argc, char** argv) {
    assert(argc == 2 && "Missing filename");
    char* filename = argv[1];

    StringBuilder sb = {0};
    sb_read_entire_file(filename, &sb);
    StringView file_contents = to_string_view(sb.items, sb.count);
    //printf("file: \n"SV_FMT"\n", SV_ARG(file_contents));

    StringViews lines = sv_split_alloc(&file_contents, '\n');

    u64 result = 0;
    U64s nums = {0};
    for (int char_idx = (int)lines.items[0].count - 1; char_idx >= 0; char_idx--) {
        u64 num = 0;
        for (size_t line_idx = 0; line_idx < lines.count-1; line_idx++) {
            char current_char = lines.items[line_idx].data[char_idx];
            if (current_char != ' ') {
                u64 digit = (u64)(current_char - '0');
                num = (num * 10) + digit;
            }
        }
        //printf("%llu\n", num);
        da_append(&nums, num);

        char current_char = lines.items[lines.count-1].data[char_idx];
        if (current_char == '+' || current_char == '*') {
            u64 inner_result = 0;
            for (size_t k = 0; k < nums.count; k++) {
                if (current_char == '*') {
                    if (inner_result == 0) inner_result = 1;
                    inner_result *= nums.items[k];
                } else {
                    inner_result += nums.items[k];
                }
            }
            //printf("inner result: %llu\n", inner_result);
            result += inner_result;
            da_resize(&nums, 0);
        }
    }


    printf("answer %llu\n", result);
    assert(result == 8907730960817);
    return 0;
}











