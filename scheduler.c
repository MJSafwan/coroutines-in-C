#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdargs.h>
#include "routine.h"
#include "scheduler.h"

typedef enum {
    RS_NORMAL,
    RS_SLEEPING,
    RS_AWAITING,
} routine_state;
typedef struct {
    routine_t id;
    routine_state state;
    routine_t a_routine;
    uint64_t sleep_ns;
} routine_status;
typedef struct {
    routine_status items[ROUTINE_CAPACITY];
    size_t count;
} routine_list;

static rctx_t sc_context; 
static routine_list sc_rl = {0};
static uint64_t sc_dt = 0;

__attribute__ ((format (printf, 1, 2)))
[[noreturn]] xfatal(const char *fmt, ...) {
    fprintf(stderr, "[Fatal] ");
    va_list l;
    va_start(l, fmt);
    vfprintf(stderr, fmt, l);
    va_end(l);
    exit(1);
}

void *xcalloc(size_t count, size_t size) {
    size_t total = count * size;
    if (total == 0)
        xfatal("Trying to initialize zero bytes!\n");
    void *c = calloc(count, size);
    if (c == NULL)
        xfatal("Failed to calloc %d bytes!\n", total);
    return c;
}

rctx_t routine_init(size_t ns) {
    return xcalloc(ns, ROUTINE_STACK_SIZE);
}

void routine_append(routine_list *s, routine_t r) {
    if (s->count == ROUTINE_CAPACITY)
        return;
    routine_status ro = {0};
    ro.id = r;
    s->items[s->count++] = ro;
}

int routine_get(const routine_list *s, routine_t r) {
    for (size_t i = 0; i < s->count; ++i) {
        if (s->items[i].id == r)
            return i;
    }
    return -1;
}

bool routine_exists(const routine_list *s, routine_t r) {
    return routine_get(s, r) == -1 ? false : true;
}

void routine_remove(routine_list *s, routine_t r) {
    size_t i = routine_get(s, r);
    if (i == -1)
        return;
    
    routine_status tmp = s->items[s->count-1];
    s->items[s->count-1] = s->items[i];
    s->items[i] = tmp;
    s->count--;
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
                if (res == ROUTINE_PANIC) {
                    *(uint64_t *)out = rr.panic_code;
                    return ROUTINE_PANIC;
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

void scheduler_init(void) {
    routine_list sc_rl = {0};
    sc_context = routine_init(ROUTINE_CAPACITY);
}

void scheduler_uninit(void) {
    free(sc_context);
}

rctx_t scheduler_get_context() {
    return sc_context;
}

void scheduler_append(void *f, void* args) {
    routine_append(&sc_rl, routine(sc_context, f, args));
}

uint64_t scheduler_get_dt(void) {
    return sc_dt;
}

void scheduler_run(void) {
    uint64_t t = get_time_ns();
    uint64_t prev = t;
    while (sc_rl.count != 0) {
        bool all_asleep = true;
        uint64_t min_sleep = UINT64_MAX;
        for (size_t i = 0; i < sc_rl.count; ++i) {
            uint64_t out = 0;
            routine_state old_state = routine_transition(&sc_rl, &sc_rl.items[i], sc_dt, &out);
            if (old_state == ROUTINE_PANIC) {
                fprintf(stderr, "A routine has panice with code %llu.\n", out);
                fprintf(stderr, "Stopping scheduler...\n");
                return;
            }
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
        sc_dt = t - prev;
    }
}
