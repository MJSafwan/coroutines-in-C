#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "routine.h"

#define ROUTINE_CAPACITY 256
#define routine_await(r) \
    do {\
        while (routine_run(r) != ROUTINE_DONE) {\
            routine_yield(); \
        } \
    } while(0)

#define PROMISE(T) struct { T arg; void *out; }

typedef struct {
    routine_t items[ROUTINE_CAPACITY];
    size_t count;
} routine_list;

rctx_t routine_init(size_t ns) {
    if (ns == 0)
        return NULL;
    return calloc(ns, ROUTINE_STACK_SIZE);
}

void routine_append(routine_list *s, routine_t r) {
    if (s->count == ROUTINE_CAPACITY)
        return;
    s->items[s->count++] = r;
}

void routine_remove(routine_list *s, routine_t r) {
    size_t i = 0;
    for (i = 0; s->items[i] != r; ++i);
    
    routine_t tmp = s->items[s->count-1];
    s->items[s->count-1] = s->items[i];
    s->items[i] = tmp;
    s->count--;
}

void fun(PROMISE(int) *p) {
    int max = p->arg;

    for (int i = 0; i < max; ++i) {
        printf("[fun] %d%% done\n", ((100*i)/max));
        routine_yield();
    }
    *(int *)p->out = 60;
    routine_finish();
}

void foo(void *args) {
    rctx_t context = routine_init(1);

    int res = 0;
    PROMISE(int) p = {0};
    p.arg = 10;
    p.out = &res;

    routine_t r1 = routine(context, fun, &p);
    routine_await(r1);

    printf("[foo] out = %d\n", *(int*)p.out);

    free(context);
    routine_finish();
}

void bar(void *args) {
    printf("[Bar] I have nothing to do with foo or fun.\n");
    for (size_t i = 0; i < 3; ++i) {
        printf("[Bar] Bla bla bla\n");
        routine_yield();
    }
    routine_finish();
}


int main(void) {
    routine_list s = {0};
    rctx_t context = routine_init(ROUTINE_CAPACITY);

    routine_append(&s, routine(context, foo, NULL));
    routine_append(&s, routine(context, bar, NULL));

    while (s.count != 0) {
        for (size_t i = 0; i < s.count; ++i) {
            if (routine_run(s.items[i]) == ROUTINE_DONE) {
                routine_remove(&s, s.items[i]);
            }
        }
    }

    return 0;
}
