#include <windows.h>
#include "async.h"

struct {
    void *main_fiber;
    HANDLE handles[MAXIMUM_WAIT_OBJECTS];
    void *fibers[MAXIMUM_WAIT_OBJECTS];
    void *dead_fiber;
    int count;
} async_loop;

void
async_initialize(void)
{
    async_loop.main_fiber = ConvertThreadToFiber(0);
    async_loop.count = 0;
    async_loop.dead_fiber = 0;
}

void
async_shutdown(void)
{
    ConvertFiberToThread();
}

void
async_run(void)
{
    while (async_loop.count) {
        DWORD nhandles = async_loop.count;
        HANDLE *handles = async_loop.handles;
        DWORD r = WaitForMultipleObjects(nhandles, handles, 0, INFINITE);
        void *fiber = async_loop.fibers[r];
        CloseHandle(async_loop.handles[r]);
        async_loop.handles[r] = async_loop.handles[nhandles - 1];
        async_loop.fibers[r] = async_loop.fibers[nhandles - 1];
        async_loop.count--;
        SwitchToFiber(fiber);
        if (async_loop.dead_fiber) {
            DeleteFiber(async_loop.dead_fiber);
            async_loop.dead_fiber = 0;
        }
    }
}

void
async_await(HANDLE h)
{
    async_loop.handles[async_loop.count] = h;
    async_loop.fibers[async_loop.count] = GetCurrentFiber();
    async_loop.count++;
    SwitchToFiber(async_loop.main_fiber);
}

void
async_exit(void)
{
    async_loop.dead_fiber = GetCurrentFiber();
    SwitchToFiber(async_loop.main_fiber);
}

struct fiber_wrapper {
    void (*func)(void *);
    void *arg;
};

static void
fiber_wrapper(void *arg)
{
    struct fiber_wrapper *fw = arg;
    fw->func(fw->arg);
    async_exit();
}

int
async_start(void (*func)(void *), void *arg)
{
    if (async_loop.count == MAXIMUM_WAIT_OBJECTS) {
        return 0;
    } else {
        struct fiber_wrapper fw = {func, arg};
        SwitchToFiber(CreateFiber(0, fiber_wrapper, &fw));
        return 1;
    }
}

HANDLE
async_sleep(double seconds)
{
    HANDLE promise = CreateWaitableTimer(0, 0, 0);
    LARGE_INTEGER t;
    t.QuadPart = (long long)(seconds * -10000000.0);
    SetWaitableTimer(promise, &t, 0, 0, 0, 0);
    return promise;
}
