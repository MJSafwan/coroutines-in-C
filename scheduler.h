#include "routine.h"
#include <stdint.h>

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#define NS 1e6
#define PROMISE(T) struct { T arg; void *out; }
void scheduler_init(void);
void scheduler_uninit(void);
void scheduler_append(void *f, void* args);
rctx_t scheduler_get_context();
uint64_t scheduler_get_dt(void);
void scheduler_run(void);
#endif // SCHEDULER_H_
