#include <stdio.h>
#include "async.h"

static void
hello(void *arg)
{
    double time = *(double *)arg;
    async_await(async_sleep(time));
    printf("hello %f\n", time);
}

static void
parallel(void *arg)
{
    (void)arg;
    HANDLE promises[1024];
    for (size_t i = 0; i < sizeof(promises) / sizeof(*promises); i++)
        promises[i] = async_sleep(1);
    puts("parallel begin");
    for (size_t i = 0; i < sizeof(promises) / sizeof(*promises); i++)
        async_await(promises[i]);
    puts("parallel end");
}

int
main(void)
{
    async_initialize();

    for (int i = 1; i <= 10; i++) {
        double time = i * 0.25;
        async_start(hello, &time);
    }
    async_run();

    async_start(parallel, 0);
    async_run();

    async_shutdown();

    puts("async done");
}
