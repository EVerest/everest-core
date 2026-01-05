#######################################
Setup the EVerest Development Container
#######################################

To setup the devcontainer one has to decide which variant
of setup. There are two variants:

.. grid:: 1 2 2 2
    :gutter: 2

    .. grid-item-card: Service Containers managed by VSCode
        :link: /tutorials/setup-devcontainer/vscode
        :link-type: doc

        This variant makes use of the VSCode Dev Containers extension.
        Which will allow to run commands in the VSCode built-in terminal.
        It will manage the service containers by starting them together with
        the devcontainer itself. A dedicated control of this services is not
        possible with this variant.

    .. grid-item-card: Service Containers managed by devrd cli
        :link: /tutorials/setup-devcontainer/devrd
        :link-type: doc

        This variant makes use of the devrd cli to manage the service containers.
        So this variant can be used if one is not using VSCode.
        It allows a dedicated control of the service containers independent
        of the devcontainer lifecycle. The devcontainer itself will not start
        or stop any service containers. With this variant the VSCode built-in
        terminal can't be used to run commands in the devcontainer directly.
        For this the devrd cli provides a command to open an interactive session
        in the devcontainer.

.. include:: /snippets/overview-devcontainer.rst
