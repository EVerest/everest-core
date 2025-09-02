from dagger import (
    dag,
    Container,
    Directory,
)

def setup_cpm_cache(
    container: Container,
    path: str,
    volume: str,
    source: Directory,

) -> Container:
    """
    Set up the CPM cache in the given container
    
    Parameters
    ----------
    container: Container
        The container to set up the CPM cache in.
    path: str
        The directory path for the CPM cache.
    volume: str
        The name of the cache volume for CPM.
    source: Directory
        The directory handle to use for the CPM cache.

    Returns
    -------
    Container
        The container with the CPM cache set up.
    """

    container = (
        container
        .with_mounted_cache(
            path=path,
            cache=dag.cache_volume(volume),
            source=source,
        )
        .with_env_variable("CPM_SOURCE_CACHE", path)
    )

    return container

def setup_ccache(
    container: Container,
    path: str,
    volume: str,
    source: Directory,
) -> Container:
    """
    Set up ccache in the given container
    
    Parameters
    ----------
    container: Container
        The container to set up ccache in.
    path: str
        The directory path for the ccache.
    volume: str
        The name of the cache volume for ccache.
    source: Directory
        The directory handle to use for the ccache.

    Returns
    -------
    Container
        The container with ccache set up.
    """

    container = (
        container
        .with_mounted_cache(
            path=path,
            cache=dag.cache_volume(volume),
            source=source,
        )
        .with_env_variable("CCACHE_DIR", path)
    )

    return container
