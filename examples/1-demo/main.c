#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "routine.h"
#include "scheduler.h"

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
    int res = 0;
    PROMISE(int) p = {0};
    p.arg = 10;
    p.out = &res;

    routine_t r1 = routine_make(fun, &p);
    if (r1 != 0)
        routine_await(r1);

    printf("[foo] out = %d\n", *(int*)p.out);
    routine_finish();
}

void baz(void *args) {
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

void mischief(void  *args) {
    routine_sleep(1000);
    routine_panic(0);
}

int main(void) {
    scheduler_init();
    scheduler_append(foo, NULL);
    scheduler_append(bar, NULL);
    scheduler_append(baz, NULL);
    //scheduler_append(mischief, NULL);
    scheduler_run();
    scheduler_uninit();
    return 0;
}
