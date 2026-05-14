#include <stdio.h>

#define ROUTINE_ONE_DONE 1
#define ROUTINE_ALL_DONE 2

void routine(void *fn, void *arg);
void routine_yield(void);
int routine_next(void);
void routine_finish(void);

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

int main(void) {
    routine(fun, "foo");
    routine(quick, NULL);
    routine(fun, "bar");
    routine(fun, "baz");
    while (routine_next() != ROUTINE_ONE_DONE);
    printf("'quick' probably finished, running through the others!\n");
    routine(fun, "fun");
    while (routine_next() != ROUTINE_ALL_DONE);
    printf("All done!\n");
    return 0;
}
