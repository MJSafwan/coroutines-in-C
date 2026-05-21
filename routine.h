#include <stdint.h>

#ifndef ROUTINE_H_
#define ROUTINE_H_

#define ROUTINE_YIELD 0
#define ROUTINE_DONE 1
#define ROUTINE_SLEEP 2

typedef uint64_t routine_t;
typedef void* rctx_t;
/* Implemented in routine.asm */
uint64_t routine(rctx_t ctx, void *fn, void *arg);
void routine_yield(void);
void routine_sleep(uint64_t sleep_ms);
int routine_run(routine_t routine, uint64_t *sleep_ms_pointer);
void routine_finish(void);
extern uint32_t ROUTINE_STACK_SIZE;

#endif // ROUTINE_H_
