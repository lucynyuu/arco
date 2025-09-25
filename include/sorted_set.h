#ifndef SORTEDSET_H
#define SORTEDSET_H

#include <stdbool.h>

typedef struct {
    int values[128];
    int size;
} SortedSet;

static void ss_clear(SortedSet* s) {
    s->size = 0;
}

static int ss_size(const SortedSet* s) {
    return s->size;
}

static bool ss_contains(const SortedSet* s, int value) {
    for (int i = 0; i < s->size; ++i)
        if (s->values[i] == value) 
            return true;
    return false;
}

static bool ss_add(SortedSet* s, int value) {
    if (value < 0 || value > 128) 
        return false;
    int i = 0;
    while (i < s->size && s->values[i] < value) ++i;
    if (i < s->size && s->values[i] == value) 
        return false;
    for (int j = s->size; j > i; --j)
        s->values[j] = s->values[j-1];
    s->values[i] = value;
    ++s->size;
    return true;
}

static void ss_remove(SortedSet* s, int value) {
    for (int i = 0; i < s->size; ++i) {
        if (s->values[i] == value) {
            for (int j = i; j < s->size - 1; ++j)
                s->values[j] = s->values[j+1];
            --s->size;
            return;
        }
    }
}

static int ss_get(const SortedSet* s, int index) {
    if (index < 0 || index >= s->size) 
        return -1;
    return s->values[index];
}

#endif /* SORTEDSET_H */