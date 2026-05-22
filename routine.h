#include <stdint.h>

#ifndef ROUTINE_H_
#define ROUTINE_H_
#define ROUTINE_CAPACITY 256

#define ROUTINE_YIELD 0
#define ROUTINE_DONE 1
#define ROUTINE_SLEEP 2
#define ROUTINE_AWAIT 3

typedef uint64_t routine_t;
typedef void* rctx_t;
typedef union {
    uint64_t sleep_ms;
    routine_t a_routine;
} routine_res;
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
/* Implemented in routine.asm */
uint64_t routine(rctx_t ctx, void *fn, void *arg);
void routine_yield(void);
void routine_sleep(uint64_t sleep_ms);
void routine_await(routine_t a_routine);
int routine_run(routine_t routine, routine_res *rr);
void routine_finish(void);
extern uint32_t ROUTINE_STACK_SIZE;

#endif // ROUTINE_H_
