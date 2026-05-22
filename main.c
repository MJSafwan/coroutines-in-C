#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "routine.h"
#include <time.h>

#define NS 1e6

#define PROMISE(T) struct { T arg; void *out; }


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

bool routine_exists(const routine_list *s, routine_t r) {
    for (size_t i = 0; i < s->count; ++i) {
        if (s->items[i].id == r)
            return true;
    }
    return false;
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
        routine_sleep(100);
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

routine_state routine_transition(
        routine_list *s,
        routine_status *r,
        uint64_t dt,
        void* out) {

    routine_res rr = {0};
    routine_state state = r->state;
    switch (state) {
        case RS_SLEEPING:
            if (r->sleep_ns <= dt) { 
                r->state = RS_NORMAL;
                break;
            }
            r->sleep_ns -= dt;
            *(uint64_t*)out = r->sleep_ns;
            break;
        case RS_NORMAL:
            {
                int res = routine_run(r->id, &rr);
                if (res == ROUTINE_DONE) {
                    routine_remove(s, r->id);
                }
                if (res == ROUTINE_SLEEP) {
                    r->state = RS_SLEEPING;
                    r->sleep_ns = rr.sleep_ms * NS;
                }
                if (res == ROUTINE_AWAIT) {
                    r->state = RS_AWAITING;
                    r->a_routine = rr.a_routine;
                    routine_append(s, rr.a_routine);
                }
                break;
            }
        case RS_AWAITING:
            if (routine_exists(s, r->a_routine) == false) {
                r->state = RS_NORMAL;
            }                    
            break;
        default:
            break;
    }
    return state;
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
        bool all_asleep = true;
        uint64_t min_sleep = UINT64_MAX;
        for (size_t i = 0; i < s.count; ++i) {
            uint64_t out = 0;
            routine_state old_state = routine_transition(&s, &s.items[i], dt, &out);
            if (old_state == RS_SLEEPING && all_asleep == true) {
                if (out < min_sleep) {
                    min_sleep = out;
                }
            } else {
                all_asleep = false;
            }
        }
        if (all_asleep) {
            uint64_t nsec = min_sleep;
            uint64_t sec = nsec/(NS*1000);
            nsec -= sec*1000*NS;
            struct timespec ts;
            ts.tv_sec = sec;
            ts.tv_nsec = nsec;
            nanosleep(&ts, NULL);
        }
        prev = t;
        t = get_time_ns();
        dt = t - prev;
    }

    return 0;
}
