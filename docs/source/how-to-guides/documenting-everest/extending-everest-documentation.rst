.. _extending_everest_documentation:

###################################
Extending the EVerest Documentation
###################################

If you want to start documenting quickly without the need of reading through
all the theory about current documentation structure and best practices, have
a look at our :ref:`How to write EVerest documentation <howto_document>`.

.. note::
  For doing quick changes in existing documentation pages, the "How to" might
  be a good choice. You also can use the "How to" for creating completely new
  pages. But doing this, prepare for getting more change requests by other
  community members during the review process. To avoid this, read through
  the page you are currently reading to get more theory.

********************************
Process of EVerest documentation
********************************

Preparing a new documentation page
==================================

Let's suppose, you are aware of a brand-new EVerest feature that is still not
documented. Or you found some aspect of EVerest that still lacks a
corresponding documentation page.

This is what to do:

1. Check the existing documentation for similar sections.

   a. Search https://everest.github.io/nightly/index.html
   b. Is it a module that you want to add documentation to? Then have a look
      at the ``everest-core`` repository in the ``modules`` directory and check
      if any documentation pages already do exist there.
   c. Use GitHub search with ``org:EVerest`` and your keywords to check if you
      can find existing documentation snippets near the source code of the
      feature. 
   
   If you can find something that is related to the topic on your mind, please
   decide, whether a new documentation section should be added or the existing
   page should be updated.

2. Create a GitHub issue

   a. In the repository https://github.com/EVerest/EVerest, click on ``Issues``
      and then ``New issue``.
   b. Choose ``Documentation change request`` and fill out the title and
      the description fields. Answer the templated questions, which have already
      been added to the description text area.
   c. Also add a reference to any related documentation pages and describe how
      the new documentation parts shall relate to that (new section, change of
      docs, new page with reference to existing ones etc.).

3. Optionally: Inform others about the issue

   Especially if you do not want to create documentation on your own (due to
   lack of time or knowledge), you can inform others about this new
   documentation requirement (the issue). This is optional as the maintainers
   of the EVerest documentation will get informed about the newly created issue.
   But by taking the topic into an appropriate working group or into the EVerest
   Zulip channels, you could find the right people who have time and knowledge
   to create such a new section in the documentation.

Creating a new documentation page
=================================

Creating a Git branch
---------------------

As with source code feature development, documentation is also organized with
Git branches. The scheme to name a branch should be adhered to

.. code-block:: bash

  docs/name-of-topic

Optionally, to better find your own branches in a list, you could also add
your name initials.

In case your name is Abraham Braveman and you are creating a documentation
about Plug'n'Charge, you could name your branch

.. code-block:: bash

  docs/ab-plug-n-charge

Choosing the type of documentation
----------------------------------

The EVerest documentation follows the Diátaxis philosphy. Find an explanation in
the :ref:`Structure of the Documentation <exp_the_everest_documentation_structure_of_doc>`
section.

Choosing a place to store the docs
----------------------------------

If you want to create a new documentation page, you should first check if
pages with similar topics are already existing. It is a good idea to place
your new page in the same location.

In general, you can decide where to put your documentation pages:

* The repository for the main documentation:
  https://github.com/EVerest/everest-core in directory ``docs/sources/``
* Directly inside of the ``docs`` directory in your modules directory structure.
  The ``index.rst`` in this location will be included into the auto-generated
  ``reference`` documentation page of this module.
* Near the source code which implements the feature that is to be documented.

.. note::
  Don't be afraid to put your documentation at a "wrong" location. It is more
  important that documentation does exist. The maintainers of the EVerest
  documentation will help you to move your docs to a suitable place during the
  PR review phase.

Writing
-------

Best practice is to look at existing documentation sources to get an idea about
how headlines or bullet points are to be handled.

You can create a ``Draft pull request`` on GitHub at an early stage of your
work to let others already get an idea how the new documentation part will look
like and give them the opportunity to comment on your work already.

.. note::
  Consider referencing to existing docs with the same topic and vice versa.

Test the generated html to be correct in formatting an test all the links you
included in your text. Build instructions can be found in `docs/README.md`.

Creating a PR and merge
-----------------------

If you have finished your documentation work, you can create a pull request
for your branch. Don't forget to reference the originating issue (if existing).
The maintainers of the corresponding repository will get informed and will try
to invest time to review your work.

After merging the PR, don't forget to also close the issue and eventually
inform the community about your newly created documentation work.
