from typing import (
    Callable,
    Awaitable,
    ParamSpec,
    TypeVar,
    overload,
)
from functools import wraps
import inspect
import asyncio
from dagger import (
    Container,
)

P = ParamSpec("P")
R = TypeVar("R")
SyncFunc = Callable[P, R]
AsyncFunc = Callable[P, Awaitable[R]]
SyncOrAsyncFunc = SyncFunc | AsyncFunc
SyncKeyFunc = Callable[P, str | None]
AsyncKeyFunc = Callable[P, Awaitable[str | None]]
SyncOrAsyncKeyFunc = SyncKeyFunc | AsyncKeyFunc
WrapFunc = Callable[[SyncOrAsyncFunc], AsyncFunc]


async def _build_cache_key(
    func: SyncOrAsyncFunc,
    key_func: SyncOrAsyncKeyFunc | None = None,
    *args: P.args,
    **kwargs: P.kwargs,
) -> str:
    """
    Construct a unique cache key for the given function call.

    The base key is the function's name. If a `key_func` is provided, its
    result is appended to the key to distinguish between different calls.
    The `key_func` may be either synchronous or asynchronous.

    Parameters
    ----------
    func : SyncOrAsyncFunc
        The function for which the cache key is being generated.
    key_func : SyncOrAsyncKeyFunc | None
        Optional function to generate a key suffix based on
        `P.args` and `P.kwargs`.
    args : P.args
        Positional arguments passed to the function.
    kwargs : P.kwargs
        Keyword arguments passed to the function.

    Returns
    -------
    str
        The generated cache key.
    """
    key = func.__name__
    if key_func is not None:
        if inspect.iscoroutinefunction(key_func):
            key_part = await key_func(*args, **kwargs)
        else:
            key_part = key_func(*args, **kwargs)
        if key_part is not None:
            key = f"{key}-{key_part}"
    return key


@overload
def cached_task(
    func: SyncFunc,
) -> AsyncFunc:
    ...


@overload
def cached_task(
    func: AsyncFunc,
) -> AsyncFunc:
    ...


@overload
def cached_task(
    *,
    key_func: SyncOrAsyncKeyFunc | None = None,
) -> WrapFunc:
    ...


def cached_task(
    func: SyncOrAsyncFunc | None = None,
    *,
    key_func: SyncOrAsyncKeyFunc | None = None
) -> WrapFunc | AsyncFunc:
    """
    Decorator to cache and reuse asyncio Tasks for async or sync methods.

    This decorator ensures that for a given method (optionally distinguished
    by a key function), only one task is created and shared for concurrent
    calls, preventing redundant executions.

    Supports decorating both asynchronous (`async def`) and synchronous
    (`def`) functions. Synchronous functions are executed asynchronously
    using `asyncio.to_thread`.


    Parameters
    ----------
    func : SyncOrAsyncFunc | None
        The function to decorate. If `None`, the decorator is used without
        parameters, and the function to decorate is provided later.
    key_func : SyncOrAsyncKeyFunc | None
        Optional function to generate a cache key suffix based on the
        decorated function’s arguments. Can be synchronous or asynchronous.


    Returns
    -------
    WrapFunc | AsyncFunc
        If `func` is provided, returns an async wrapper function that manages
        cached tasks per key. If `func` is `None`, returns a decorator that
        can be used to wrap a function later.
        In other terms, used as `@cached_task(...)` when `func` is `None`
        or `@cached_task` when `func` is provided.

    Examples
    --------
    ```python
    # Example 1: Using the decorator directly on async functions without
    # parameters
    @cached_task
    async def some_async_function(self):
        ...

    # Example 2: Using the decorator directly on sync functions without
    # parameters
    @cached_task
    def sync_function_without_param(self):
        ...

    # Example 3: Using the decorator with a key function on async functions
    # with parameters
    @cached_task(key_func=lambda self, param: param.id)
    async def async_function_with_param(self, param):
        ...

    # Example 4: Using the decorator with an async key function on async
    # functions with parameters
    async def async_key_func(self, param):
        return param.id
    @cached_task(
        key_func=async_key_func
    )
    async def async_function_with_param(self, param):
        ...
    ```

    """
    def wrap_function(
        func: SyncOrAsyncFunc
    ) -> AsyncFunc:
        # check if func is a method of a class
        if (
            not inspect.isfunction(func)
            and not inspect.iscoroutinefunction(func)
        ):
            raise ValueError(
                "cached_task must be used on a method of a class, "
                "not a standalone function."
            )
        if not inspect.signature(func).parameters:
            raise ValueError(
                "cached_task must be used on a method with at least one "
                "argument (the instance itself)."
            )
        first_param = next(iter(inspect.signature(func).parameters.values()))
        if first_param.name not in ("self"):
            raise ValueError(
                "cached_task must be used on a method of a class, "
                "not a standalone function."
            )

        @wraps(func)
        async def wrapper(*args: P.args, **kwargs: P.kwargs) -> R:
            self = args[0]
            if not hasattr(self, "_cached_tasks_lock"):
                self._cached_tasks_lock = asyncio.Lock()
            async with self._cached_tasks_lock:
                if not hasattr(self, "_cached_tasks"):
                    self._cached_tasks = {}
                key = await _build_cache_key(
                    func, key_func, *args, **kwargs
                )
                if key not in self._cached_tasks:
                    if inspect.iscoroutinefunction(func):
                        self._cached_tasks[key] = asyncio.create_task(
                            func(*args, **kwargs)
                        )
                    else:
                        self._cached_tasks[key] = asyncio.create_task(
                            asyncio.to_thread(func, *args, **kwargs)
                        )
            try:
                return await self._cached_tasks[key]
            except Exception:
                async with self._cached_tasks_lock:
                    self._cached_tasks.pop(key, None)
                raise
        return wrapper

    if func is None:
        return wrap_function  # used as @cached_task(...)
    else:
        return wrap_function(func)  # used as @cached_task


async def create_key_from_container(
    self,
    container: Container | None = None
) -> str:
    """Create a unique key for the container based on its properties."""
    if container is None:
        return None
    return await container.id()
