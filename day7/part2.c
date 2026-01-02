#include "../common.h"

typedef struct {
    int y, x;
} Point;

typedef struct {
    Point* items;
    size_t count;
    size_t capacity;
} Points;

typedef struct {
    u64* items;
    size_t count;
    size_t capacity;
} U64s;

static StringViews g_graph;
static Points g_splits;
static U64s g_memo;

static Point to_point(int y, int x) {
    return (Point) {
        .y = y,
        .x = x,
    };
}

static char get_char(Point* p) {
    assert(p->y >= 0 && p->y < (int)g_graph.count);
    assert(p->x >= 0 && p->x < (int)g_graph.items[0].count);
    return g_graph.items[p->y].data[p->x];
}

// linear search w/e
static int find_point(Points* ps, Point* p) {
    for (size_t i = 0; i < ps->count; i++) {
        Point cur_point = ps->items[i];
        if (cur_point.y == p->y && cur_point.x == p->x) {
            return (int)i;
        }
    }
    return -1;
}

static u64 dfs(Point p) {
    if (p.y >= (int)g_graph.count) return 1;

    char c = get_char(&p);
    if (c == '^') {
        int split_idx = find_point(&g_splits, &p);
        if (split_idx >= 0) {
            return g_memo.items[split_idx];
        }
        u64 result = dfs(to_point(p.y, p.x-1)) + dfs(to_point(p.y, p.x+1));
        da_append(&g_splits, p);
        da_append(&g_memo, result);
        return result;
    } else {
        return dfs(to_point(p.y+1, p.x));
    }
}


int main(int argv, char** argc) {
    assert(argv == 2);
    char* filename = argc[1];
    StringBuilder sb = {0};
    if(!sb_read_entire_file(filename, &sb)) {
        return 1;
    }

    StringView sv = to_string_view(sb.items, sb.count);
    g_graph = sv_split_alloc(&sv, '\n');

    Point start;
    for (size_t i = 0; i < g_graph.items[0].count; i++) {
        Point p = to_point(0, (int)i);
        if (get_char(&p) == 'S') {
            start = p;
            break;
        }
    }
    assert(start.x > 0);

    u64 result = dfs(start);

    printf("answer: %llu\n", result);
    assert(result == 40941112789504);
}
