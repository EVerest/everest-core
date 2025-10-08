from enum import Enum
import requests

from typing import (
    Callable,
    Awaitable,
    ParamSpec,
    TypeVar,
    overload,
)

import inspect
from functools import wraps
from ..everest_ci import BaseResultType


class _GithubStatusState(Enum):
    ERRROR = "error"
    FAILURE = "failure"
    PENDING = "pending"
    SUCCESS = "success"


_github_api_base_url = "https://api.github.com/repos"


def _initialize_status(
    *,
    auth_token: str,
    org_name: str,
    repo_name: str,
    sha: str,
    function_name: str,
    run_id: str,
    attempt_number: int,
    description: str | None = None,
) -> None:
    """
    Initialize a GitHub status for a specific commit.

    Parameters
    ----------
    auth_token : str
        GitHub authentication token.
    org_name : str
        Organization name on GitHub.
    repo_name : str
        Repository name on GitHub.
    sha : str
        Commit SHA to initialize the status for.
    function_name : str
        The name of the function for which the status is being initialized.
    run_id : str
        Run ID to link in the GitHub status (e.g., CI job output).
    attempt_number : int
        Attempt number of the GitHub Actions run.
    description : str | None
        Optional short description to include with the GitHub status.

    Raises
    ------
    RuntimeError
        If the GitHub status initialization fails.
    """

    api_url = f"{_github_api_base_url}/{org_name}/{repo_name}/statuses/{sha}"
    headers = {
        "Accept": "application/vnd.github+json",
        "Authorization": f"Bearer {auth_token}",
        "GitHub-Api-Version": "2022-11-28"
    }
    body = {
        "state": _GithubStatusState.PENDING.value,
        "target_url": f"https://github.com/{org_name}/{repo_name}/actions/runs/{run_id}/attempts/{attempt_number}",
        "description": description,
        "context": f"Dagger: {function_name}",
    }
    res = requests.post(api_url, json=body, headers=headers)

    if res.status_code != 201:
        raise RuntimeError(
            (
                f"Failed to initialize GitHub status: {res.status_code} - "
                f"{res.text}"
            )
        )


def _update_status(
    *,
    auth_token: str,
    org_name: str,
    repo_name: str,
    sha: str,
    function_name: str,
    success: bool,
    run_id: str,
    attempt_number: int,
    description: str | None = None,
) -> None:
    """
    Update a GitHub status for a specific commit.

    Parameters
    ----------
    auth_token : str
        GitHub authentication token.
    org_name : str
        Organization name on GitHub.
    repo_name : str
        Repository name on GitHub.
    sha : str
        Commit SHA to update the status for.
    function_name : str
        The name of the function for which the status is being updated.
    success : bool
        Whether the function execution was successful (exit code 0).
    run_id : str
        Run ID to link in the GitHub status (e.g., CI job output).
    attempt_number : int
        Attempt number of the GitHub Actions run.
    description : str | None
        Optional short description to include with the GitHub status.

    Raises
    ------
    RuntimeError
        If the GitHub status update fails.
    """
    if success:
        state = _GithubStatusState.SUCCESS
    else:
        state = _GithubStatusState.FAILURE

    api_url = f"{_github_api_base_url}/{org_name}/{repo_name}/statuses/{sha}"
    headers = {
        "Accept": "application/vnd.github+json",
        "Authorization": f"Bearer {auth_token}",
        "GitHub-Api-Version": "2022-11-28"
    }
    body = {
        "state": state.value,
        "target_url": f"https://github.com/{org_name}/{repo_name}/actions/runs/{run_id}/attempts/{attempt_number}",
        "description": description,
        "context": f"Dagger: {function_name}",
    }

    res = requests.post(api_url, json=body, headers=headers)

    if res.status_code != 201:
        raise RuntimeError(
            f"Failed to update GitHub status: {res.status_code} - {res.text}"
        )


P = ParamSpec("P")
R = TypeVar("R", bound=BaseResultType)
SyncFunc = Callable[P, R]
AsyncFunc = Callable[P, Awaitable[R]]
SyncOrAsyncFunc = SyncFunc | AsyncFunc
WrapFunc = Callable[[SyncOrAsyncFunc], SyncOrAsyncFunc]

ValueType = TypeVar("ValueType")


def _resolve_callable_value(
    value: ValueType | Callable[P, ValueType] | None,
    func: SyncOrAsyncFunc,
    *args: P.args,
    **kwargs: P.kwargs,
) -> ValueType:
    if not callable(value):
        return value
    sig = inspect.signature(func)
    bound = sig.bind(*args, **kwargs)
    bound.apply_defaults()
    return value(**bound.arguments)


def create_github_status(
    *,
    condition: bool | Callable[P, bool],
    function_name: str | Callable[P, str],
    auth_token: str | Callable[P, str] | None,
    org_name: str | Callable[P, str] | None,
    repo_name: str | Callable[P, str] | None,
    sha: str | Callable[P, str] | None,
    run_id: str | Callable[P, str] | None,
    attempt_number: int | Callable[P, int] | None = None,
    description: str | Callable[P, str] | None = None,
) -> WrapFunc:
    """
    Decorator factory to automatically create and update GitHub commit status
    around a function execution.

    This decorator sends a "pending" status before the function runs and
    updates the status to "success" or "failure" depending on the `exit_code`
    of the returned `BaseResultType` object.

    Works with both synchronous and asynchronous functions. The decorated
    function must return an instance of `BaseResultType`.

    Parameters
    ----------
    condition : bool | Callable[P, bool]
        If `False`, the decorator does nothing and the function runs without
        creating or updating any GitHub status.
        If `True`, the decorator will create and update the GitHub status.
        This allows for easy toggling of the status reporting feature.
    auth_token : str | Callable[P, str] | None
        GitHub personal access token with repo status permissions.
        If condition is `False`, this can be `None`
    org_name : str | Callable[P, str] | None
        Name of the GitHub organization.
        If condition is `False`, this can be `None`
    repo_name : str | Callable[P, str] | None
        Name of the GitHub repository.
        If condition is `False`, this can be `None`
    sha : str | Callable[P, str] | None
        Commit SHA to which the status should be attached.
        If condition is `False`, this can be `None`
    function_name : str | Callable[P, str]
        The name to use for the GitHub status context.
    run_id : str | Callable[P, str] | None
        Run ID to link in the GitHub status (e.g., CI job output).
        If condition is `False`, this can be `None`
    attempt_number : int | Callable[P, int] | None
        Attempt number of the GitHub Actions run.
        If condition is `False`, this can be `None`
    description : str | Callable[P, str] | None
        Optional short description to include with the GitHub status.

    Returns
    -------
    WrapFunc
        A decorator that wraps a function and reports GitHub status updates
        before and after execution.

    Raises
    ------
    TypeError
        If the decorated function does not return an instance of
        `BaseResultType`.

    Examples
    --------
    ```python
    # Example 1
    @create_github_status(
        auth_token="ghp_xxx",
        org_name="my-org",
        repo_name="my-repo",
        sha="abc123",
        function_name="my-task",
        run_id="123",
        attempt_number=1,
        description="Running my task"
    )
    def run_step() -> MyResult:
        return MyResult(exit_code=0)

    # Example 2
    @create_github_status(
        condition=lambda self: self.get_github_config().is_github_run,
        auth_token=lambda self: self.get_github_config().github_token,
        org_name=lambda self: self.get_github_config().org_name,
        repo_name=lambda self: self.get_github_config().repo_name,
        sha=lambda self: self.get_github_config().sha,
        function_name="run_step",
        run_id=lambda self: self.get_github_config().run_id,
        attempt_number=lambda self: self.get_github_config().attempt_number,
        description=None,
    )
    async def run_step(self) -> MyResult:
        return MyResult(exit_code=0)
    ```
    """

    @overload
    def wrap_function(
        func: SyncFunc,
    ) -> SyncFunc:
        ...

    @overload
    def wrap_function(
        func: AsyncFunc,
    ) -> AsyncFunc:
        ...

    def wrap_function(
        func: SyncOrAsyncFunc,
    ) -> SyncOrAsyncFunc:
        if inspect.iscoroutinefunction(func):
            @wraps(func)
            async def async_wrapper(
                *args: P.args,
                **kwargs: P.kwargs,
            ) -> R:
                nonlocal condition, auth_token, org_name, repo_name, sha, \
                    function_name, run_id, attempt_number, description
                condition = _resolve_callable_value(
                    condition, func, *args, **kwargs
                )
                auth_token = _resolve_callable_value(
                    auth_token, func, *args, **kwargs
                )
                org_name = _resolve_callable_value(
                    org_name, func, *args, **kwargs
                )
                repo_name = _resolve_callable_value(
                    repo_name, func, *args, **kwargs
                )
                sha = _resolve_callable_value(
                    sha, func, *args, **kwargs
                )
                function_name = _resolve_callable_value(
                    function_name, func, *args, **kwargs
                )
                run_id = _resolve_callable_value(
                    run_id, func, *args, **kwargs
                )
                attempt_number = _resolve_callable_value(
                    attempt_number, func, *args, **kwargs
                )
                description = _resolve_callable_value(
                    description, func, *args, **kwargs
                )
                if condition:
                    if auth_token is None:
                        raise ValueError(
                            (
                                "auth_token must be provided when "
                                "condition is True"
                            )
                        )
                    if org_name is None:
                        raise ValueError(
                            "org_name must be provided when condition is True"
                        )
                    if repo_name is None:
                        raise ValueError(
                            "repo_name must be provided when condition is True"
                        )
                    if sha is None:
                        raise ValueError(
                            "sha must be provided when condition is True"
                        )
                    if run_id is None:
                        raise ValueError(
                            "run_id must be provided when condition is True"
                        )
                    if attempt_number is None:
                        raise ValueError(
                            "attempt_number must be provided when condition is True"
                        )
                    _initialize_status(
                        auth_token=auth_token,
                        org_name=org_name,
                        repo_name=repo_name,
                        sha=sha,
                        function_name=function_name,
                        description=description,
                        run_id=run_id,
                        attempt_number=attempt_number,
                    )
                result = await func(*args, **kwargs)
                if not isinstance(result, BaseResultType):
                    raise TypeError(
                        "The decorated function must return an instance of "
                        "BaseResultType"
                    )
                if condition:
                    success = result.exit_code == 0
                    _update_status(
                        auth_token=auth_token,
                        org_name=org_name,
                        repo_name=repo_name,
                        sha=sha,
                        function_name=function_name,
                        success=success,
                        run_id=run_id,
                        attempt_number=attempt_number,
                        description=description,
                    )
                return result
            return async_wrapper
        else:
            @wraps(func)
            def sync_wrapper(
                *args: P.args,
                **kwargs: P.kwargs,
            ) -> R:
                nonlocal condition, auth_token, org_name, repo_name, sha, \
                    function_name, run_id, attempt_number, description
                condition = _resolve_callable_value(
                    condition, func, *args, **kwargs
                )
                auth_token = _resolve_callable_value(
                    auth_token, func, *args, **kwargs
                )
                org_name = _resolve_callable_value(
                    org_name, func, *args, **kwargs
                )
                repo_name = _resolve_callable_value(
                    repo_name, func, *args, **kwargs
                )
                sha = _resolve_callable_value(
                    sha, func, *args, **kwargs
                )
                function_name = _resolve_callable_value(
                    function_name, func, *args, **kwargs
                )
                run_id = _resolve_callable_value(
                    run_id, func, *args, **kwargs
                )
                attempt_number = _resolve_callable_value(
                    attempt_number, func, *args, **kwargs
                )
                description = _resolve_callable_value(
                    description, func, *args, **kwargs
                )
                if condition:
                    if auth_token is None:
                        raise ValueError(
                            (
                                "auth_token must be provided when "
                                "condition is True"
                            )
                        )
                    if org_name is None:
                        raise ValueError(
                            "org_name must be provided when condition is True"
                        )
                    if repo_name is None:
                        raise ValueError(
                            "repo_name must be provided when condition is True"
                        )
                    if sha is None:
                        raise ValueError(
                            "sha must be provided when condition is True"
                        )
                    if run_id is None:
                        raise ValueError(
                            "run_id must be provided when condition is True"
                        )
                    if attempt_number is None:
                        raise ValueError(
                            "attempt_number must be provided when condition is True"
                        )
                    _initialize_status(
                        auth_token=auth_token,
                        org_name=org_name,
                        repo_name=repo_name,
                        sha=sha,
                        function_name=function_name,
                        description=description,
                        run_id=run_id,
                        attempt_number=attempt_number,
                    )
                result = func(*args, **kwargs)
                if not isinstance(result, BaseResultType):
                    raise TypeError(
                        "The decorated function must return an instance of "
                        "BaseResultType"
                    )
                if condition:
                    success = result.exit_code == 0
                    _update_status(
                        auth_token=auth_token,
                        org_name=org_name,
                        repo_name=repo_name,
                        sha=sha,
                        function_name=function_name,
                        success=success,
                        run_id=run_id,
                        attempt_number=attempt_number,
                        description=description,
                    )
                return result
            return sync_wrapper
    return wrap_function
