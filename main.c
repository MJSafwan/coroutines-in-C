#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "routine.h"
#include <time.h>

#define ROUTINE_CAPACITY 256
#define NS 1e6
#define routine_await(r) \
    do {\
        while (routine_run(r, NULL) != ROUTINE_DONE) {\
            routine_yield(); \
        } \
    } while(0)

#define PROMISE(T) struct { T arg; void *out; }

typedef struct {
    routine_t id;
    bool sleeping;
    uint64_t sleep_ns;
} routine_status;

typedef struct {
    routine_status items[ROUTINE_CAPACITY];
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
    routine_status ro = {0};
    ro.id = r;
    s->items[s->count++] = ro;
}

void routine_remove(routine_list *s, routine_t r) {
    size_t i = 0;
    for (i = 0; s->items[i].id != r; ++i);
    
    routine_status tmp = s->items[s->count-1];
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

void baz(void *args) {
    rctx_t context = routine_init(1);

    printf("[baz] Waiting for 5 second...\n");
    routine_sleep(5000);
    printf("[baz] Finished waiting!\n");
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

uint64_t get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 * NS + (uint64_t)ts.tv_nsec;
}

int main(void) {
    routine_list s = {0};

    rctx_t context = routine_init(ROUTINE_CAPACITY);
    routine_append(&s, routine(context, foo, NULL));
    routine_append(&s, routine(context, bar, NULL));
    routine_append(&s, routine(context, baz, NULL));

    uint64_t t = get_time_ns();
    uint64_t prev = t;
    uint64_t dt = 0;
    while (s.count != 0) {
        for (size_t i = 0; i < s.count; ++i) {
            routine_status *r = &s.items[i];
            uint64_t sleep_ms = 0;
            if (r->sleeping == true) {
                if (r->sleep_ns <= dt) { 
                    r->sleeping = false;
                }
                r->sleep_ns -= dt;
            } else {
                int res = routine_run(r->id, &sleep_ms);
                if (res == ROUTINE_DONE) {
                    routine_remove(&s, r->id);
                }
                if (res == ROUTINE_SLEEP) {
                    r->sleeping = true;
                    r->sleep_ns = sleep_ms * NS;
                }
            }
        }
        prev = t;
        t = get_time_ns();
        dt = t - prev;
    }

    return 0;
}
