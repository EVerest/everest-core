from enum import Enum
import requests

class CIStep(Enum):
    BUILD_KIT = "build-kit"
    LINT = "lint"
    CONFIGURE_CMAKE_GCC = "configure-cmake-gcc"
    BUILD_CMAKE_GCC = "build-cmake-gcc"
    UNIT_TESTS = "unit-tests"
    INSTALL_CMAKE_GCC = "install-cmake-gcc"
    INTEGRATION_TESTS = "integration-tests"
    OCPP_TESTS = "ocpp-tests"

class GithubStatusState(Enum):
    ERRROR = "error"
    FAILURE = "failure"
    PENDING = "pending"
    SUCCESS = "success"

github_api_base_url = "https://api.github.com/repos" \

def initialize_status(
    auth_token: str,
    org_name: str,
    repo_name: str,
    sha: str,
    step: CIStep,
    description: str | None = None,
) -> None:
    """
    Initialize a GitHub status for a specific commit.

    Parameters:
    -----------
    auth_token : str
        GitHub authentication token.
    org_name : str
        Organization name on GitHub.
    repo_name : str
        Repository name on GitHub.
    sha : str
        Commit SHA to set the status for.
    step : CIStep
        The CI step for which the status is being set.
    """
    
    api_url = f"{github_api_base_url}/{org_name}/{repo_name}/statuses/{sha}"
    headers = {
        "Accept": "application/vnd.github+json",
        "Authorization": f"Bearer {auth_token}",
        "GitHub-Api-Version": "2022-11-28"
    }
    body = {
        "state": GithubStatusState.PENDING.value,
        "target_url": None,
        "description": description,
        "context": f"Dagger: {step.value}",
    }
    res = requests.post(api_url, json=body, headers=headers)

    if res.status_code != 201:
        raise RuntimeError(
            f"Failed to initialize GitHub status: {res.status_code} - {res.text}"
        )

def update_status(
    auth_token: str,
    org_name: str,
    repo_name: str,
    sha: str,
    step: CIStep,
    state: GithubStatusState,
    target_url: str,
    description: str | None = None,
) -> None:
    """
    Update a GitHub status for a specific commit.

    Parameters:
    -----------
    auth_token : str
        GitHub authentication token.
    org_name : str
        Organization name on GitHub.
    repo_name : str
        Repository name on GitHub.
    sha : str
        Commit SHA to update the status for.
    step : CIStep
        The CI step for which the status is being updated.
    state : GithubStatusState
        The new state of the status.
    target_url : str
        URL to link to for more details.
    """
    
    api_url = f"{github_api_base_url}/{org_name}/{repo_name}/statuses/{sha}"
    headers = {
        "Accept": "application/vnd.github+json",
        "Authorization": f"Bearer {auth_token}",
        "GitHub-Api-Version": "2022-11-28"
    }
    body = {
        "state": state.value,
        "target_url": target_url,
        "description": description,
        "context": f"Dagger: {step.value}",
    }
    
    res = requests.post(api_url, json=body, headers=headers)

    if res.status_code != 201:
        raise RuntimeError(
            f"Failed to update GitHub status: {res.status_code} - {res.text}"
        )
