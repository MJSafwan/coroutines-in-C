#include <stdint.h>

#ifndef ROUTINE_H_
#define ROUTINE_H_
#define ROUTINE_CAPACITY 256

#define ROUTINE_YIELD 0
#define ROUTINE_DONE 1
#define ROUTINE_SLEEP 2
#define ROUTINE_AWAIT 3
#define ROUTINE_PANIC 4

typedef uint64_t routine_t;
typedef void* rctx_t;
typedef union {
    uint64_t sleep_ms;
    uint64_t panic_code;
    routine_t a_routine;
} routine_res;
/* Implemented in routine.asm */
uint64_t routine(rctx_t ctx, void *fn, void *arg);
void routine_yield(void);
void routine_sleep(uint64_t sleep_ms);
void routine_await(routine_t a_routine);
void routine_panic(uint64_t panic_code);
int routine_run(routine_t routine, routine_res *rr);
void routine_finish(void);
extern uint32_t ROUTINE_STACK_SIZE;

#endif // ROUTINE_H_
