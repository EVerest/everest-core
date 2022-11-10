Documentation dev container
===========================

Setup
-----

#. Install `Docker <https://www.docker.com/>`_, `Visual Studio Code
   <https://code.visualstudio.com>`_ and the `Dev Container
   <vscode:extension/ms-vscode-remote.remote-containers>`_ plugin for Visual
   Studio Code.

#.
  Clone the *everest-utils* repository::

    git clone https://github.com/EVerest/everest-utils.git

  Copy the folder ``docker/everest-docu`` to a place, you like.  For the
  remainder of this setup, we'll call that folder ``everest-doc``.

  .. note::

    Right now, there is no straight-forward way to keep this folder in sync with
    `git`.

#. Start Visual Studio Code, press ``F1``, type ``devopen``, select `Dev
   Container: Open folder in Container...` and choose the ``everest-doc``
   folder.  Two notifications, one recommending to "install Microsoft Python
   Extension", and one stating that `python` is missing, might appear - both
   can be ignored.

#. On the left, select and open the file ``source/testpage.rst``. Press
   ``CTRL-K CTRL-R`` and a preview of the test page should appear.


Usage
-----

* `Manually building the documentation`: Open a console in the dev container
  and type::

    make html # for html output
    make latexpdf # for pdf output

* `Line wrapping`: It is good practice to keep the line-length of `rst`
  documents to 79 characters per line.  There is a line rewrap plugin
  pre-installed in the dev container and rulers for 79 and 120 characters line
  length are preconfigured.  To automatically line break some text, click on
  any place of that text in the editor and press ``ALT-Q``.  If you press it
  multiple times, the text will be broken down according to the different line
  lengths.

* `Editing and previewing rst files from external folders`: This is
  unfortunately a bit more complicated, because this whole setup runs in a `Dev
  Container` which can't reach the outside.  One possibility is copy the files
  in the ``everest-docu`` folder, so you can use them directly.  This approach
  might not suite you, because you will need to put your folders into this
  project.

  Another possibility is to add an external folder as a mount point to this
  container.  In order to do so, open the file
  ``.devcontainer/devcontainer.json`` and uncomment the mount configuration.
  There you can insert your local folder, which will then be mounted into this
  container at ``/ext_source``.  The create a symlink into this containers
  source folder by::

    # change into this containers source folder
    cd source
    ln -s /ext_source ./

  Now you can browse the rst files inside the symlinked ``source/ext_source``
  folder, edit and preview them.

  In case you have another complete ``Sphinx`` documentation source (which also
  contains a ``conf.py``), you can add/edit the following setting::

    "esbonio.sphinx.confDir": "${workspaceFolder}/source/ext_source"

  inside the ``.vscode/settings.json`` file.

Usefull resources
-----------------

* `Restructured text primer <https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html>`_
