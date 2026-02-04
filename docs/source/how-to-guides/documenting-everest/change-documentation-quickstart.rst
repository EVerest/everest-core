.. _howto_document:

######################
Documenting Quickstart
######################

This is a short how-to for writing documentation in EVerest. Please refer to the
`Documentation README <https://github.com/EVerest/everest-core/blob/main/docs/README.md>`_
for build instructions for the documentation.

To get more detailed information, see
:ref:`Documenting EVerest <documenting_everest>`.

1. Decide which type of documentation you want to create:

   a. If you are not familiar with the Diátaxis way of organizing documentation,
      read the subsection :ref:`Structure of the Documentation <exp_the_everest_documentation_structure_of_doc>`.
   b. If you cannot clearly decide which category your contribution belongs to,
      consider splitting it up to align with the Diátaxis philosophy.

2. Decide where to place the documentation

   a. Module documentation goes into the modules ``docs`` directory. Provide
      at least an index.rst file in this directory.
   b. If you want to document some partial aspects of your code (like a
      specific algorithm you use), you can add a section in the ``README.md``
      close to the source code. That could for example be in the ``lib/everest/...``
      directory of the everest-core repository or in the corresponding GitHub repository
      if the code is not part of everest-core.
   c. For documentation that is required to understand an important part or
      concept of EVerest, place the new documentation in a proper location in
      the ``docs`` directory of the
      `EVerest main repository <https://github.com/EVerest/everest-core>`_.
   d. When in doubt, use the EVerest main repository.

3. Create an issue (in case of bigger documentation changes).

   Consider to create a documentation issue inside of the
   everest-core GitHub repository you just have chosen.
   Describe the most important aspects of the topic to be documented.

4. Create a Git branch like ``docs/name-of-topic`` in the EVerest main
   repository.
   
   Put a note in the issue to inform the community that you start working on
   new documentation to solve that issue.

5. Create the documentation.

   You can use existing ``.rst`` files as template for creating new
   documentation pages. See this page for getting an idea how to use
   reStructuredText:
   https://www.sphinx-doc.org/en/master/usage/restructuredtext/index.html
   
   Also have a look at our
   :ref:`best practices page for using Sphinx in EVerest <everest_doc_sphinx_style_code>`.

6. Create pull request (PR).

   After having finished your work, create a PR and set a reference to the
   originating issue (if existing).
   The maintainers of the repository will get informed automatically.
   Alternatively, you can try to find people who have the required knowledge in
   and also have the time to review your PR.
   You might find them via Zulip or the working groups.
