# Win32 Fiber Async/await Demo

This is an example of using fibers to await on any kernel object handle.
Due to limitations of the Win32 API, only 64 coroutines can run at once.

## API

```c
/**
 * Initialize the async/await system.
 * Must be called before any other async/await functions.
 */
void async_initialize(void);

/**
 * Tear down the async-await system.
 * This should not be run until all coroutines have terminated.
 */
void async_shutdown(void);

/**
 * Runs the main scheduler loop until all coroutines have terminated.
 * This function can be called more than once, though it only makes
 * sense if at least one coroutine has been started with async_start().
 */
void async_run(void);

/**
 * Create and start a new coroutine.
 * The coroutine is immediately started. Returns the first time the new
 * coroutine blocks or exits. Returns 1 on success, or 0 if too many
 * coroutines are running. Only 64 coroutines can run at once.
 */
int async_start(void (*)(void *), void *);

/**
 * Await on a kernel object, destroying it via CloseHandle() when done.
 */
void async_await(HANDLE);

/**
 * Terminate the current coroutine.
 */
void async_exit(void);

/**
 * Return a handle that signals after the given time.
 * The returned a handle suitable for async_await() and is useful for
 * pausing the current coroutine for a time period.
 */
HANDLE async_sleep(double seconds);
```
