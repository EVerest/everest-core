#######################################
Setup the EVerest Development Container
#######################################

There are two variants for the devcontainer setup:

.. grid:: 1 2 2 2
    :gutter: 2

    .. grid-item-card:: Service Containers managed by VSCode
        :link: /tutorials/setup-devcontainer/vscode
        :link-type: doc

        Uses the VSCode Dev Containers extension, which manages both
        the devcontainer and the service containers.

    .. grid-item-card:: Service Containers managed by devrd cli
        :link: /tutorials/setup-devcontainer/devrd
        :link-type: doc

        Uses the devrd cli to manage the service containers.
        This variant can be used if one is not using VSCode or needs dedicated
        control over the service containers.

Also consider these documents: 

- :doc:`troubleshooting section <troubleshooting>` that deals for devcontainer-specific issues.
- :doc:`How-to Guide: How to use a development container for EVerest development and sil testing </how-to-guides/devcontainer-usage/index>`
- :doc:`Internals of the EVerest Development Container </explanation/devcontainer-internal/index>`

.. toctree::
    :maxdepth: 1
    :hidden:

    Managed by devrd CLI <devrd>
    Managed by VSCode <vscode>
    troubleshooting