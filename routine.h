#include <stdint.h>

#ifndef ROUTINE_H_
#define ROUTINE_H_

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

#endif // ROUTINE_H_
