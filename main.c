#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ROUTINE_YIELD 0
#define ROUTINE_DONE 1
#define ROUTINE_CAPACITY 256

typedef uint64_t routine_t;
typedef void* rctx_t;
typedef struct {
    routine_t items[ROUTINE_CAPACITY];
    size_t count;
} routine_list;
/* Implemented in routine.asm */
uint64_t routine(rctx_t ctx, void *fn, void *arg);
void routine_yield(void);
int routine_run(routine_t routine);
void routine_finish(void);
extern uint32_t ROUTINE_STACK_SIZE;

void fun(void *arg) {
    char *name = (char *)arg;
    for (int i = 0; i < 10; ++i) {
        printf("[%s] i = %d\n", name, i);
        routine_yield();
    }
    printf("[%s] FINISH!\n", name);
    routine_finish();
}

void quick(void *arg) {
    printf("[quick] I am speed\n");
    routine_yield();
    printf("[quick] FINISH!\n");
    routine_finish();
}



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
    
    for (size_t j = i; j < s->count-1; ++j) {
        routine_t tmp = s->items[j];
        s->items[j] = s->items[j+1];
        s->items[j+1] = tmp;
    }

    s->count--;
}

int main(void) {
    routine_list s = {0};
    rctx_t context = routine_init(ROUTINE_CAPACITY);
    routine_append(&s, routine(context, fun, "foo"));
    routine_append(&s, routine(context, quick, NULL));
    routine_append(&s, routine(context, fun, "bar"));
    routine_append(&s, routine(context, fun, "baz"));
    while (s.count != 0) {
        for (size_t i = 0; i < s.count; ++i) {
            if (routine_run(s.items[i]) == ROUTINE_DONE) {
                routine_remove(&s, s.items[i]);
            }
        }
    }
    return 0;
}
