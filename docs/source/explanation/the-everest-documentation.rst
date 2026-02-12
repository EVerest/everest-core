.. _exp_the_everest_documentation:

#########################
The EVerest Documentation
#########################

This section explains how different files in different places are compiled
into to html document you are reading. The general structure that is being
aimed for is also explained. If you only read one subsection on this page
it should be
:ref:`Structure of the Documentation <exp_the_everest_documentation_structure_of_doc>`.

If you only want to modify existing documents, this may well be sufficient.
Practical instructions on working on the documentation are located in the
:ref:`How-to section <documenting_everest>`.

.. _exp_the_everest_documentation_structure_of_doc:

******************************
Structure of the Documentation
******************************

Our documentation is structured according to the `Diátaxis <https://diataxis.fr/>`_ framework:

* *tutorials*: Learn by doing through guided practice.
* *how-to guides*: Practical steps to achieve specific tasks.
* *reference*: Technical facts, APIs, and configuration details.
* *explanations*: Conceptual deep-dives and background theory.

**Tutorials** shall allow a new user to successfully do *something*. No concrete
real-world problem needs to be solved at this point (as the Diátaxis authors put
it: a driving lesson is just not about getting from A to B but about the driving
itself). Deep explanations are to be avoided. It's important to provide a
safe route to some success and allow the reader to gain confidence in his developing
practical skills.

**How-to guides** show how real-world problems are solved by giving practical directions.
The target audience are users which already gained some knowledge through other means.
The instructions are practical and serve to achieve specific goals that many users need.

**Reference** material may be worthless to the novice because it requires an
understanding of the EVerest framework and does not give any practical advice.
For users who are familiar with the basics, the reference is a goal-agnostic, precise and
effective source of facts for all the decisions to be made in everydays work.

**Explanations** provide the necessary context and background to understand.
Things learned in *tutorials* and *how-to guides* will often require further
knowledge to be put in a bigger picture and reveal the *why* of many technical
decisions encoded in the EVerest frameworks architecture.

Please keep this framework in mind when contributing new content. We encourage you
to split your contributions into multiple documents that align with the Diátaxis philosophy.
Linking between these documents ensures users have quick access to related material
without cluttering a single page.

By keeping individual documents focused and concise, they become much more readable.
Tutorials, in particular, should remain brief and link to the Explanations section for
deeper background information.

Since this structure was not chosen from the outset, it is quite possible that
some sections of the EVerest documentation do not conform to this structure in
an exemplary manner. These should therefore not be regarded as good examples that
should be followed without further consideration.

************
Source Files
************

EVerest documentation uses Sphinx as documentation generator. As input format,
reStructuredText is used. See here for more information about Sphinx:
https://www.sphinx-doc.org/en/master/

The :ref:`Sphinx Style Guide <everest_doc_sphinx_style_code>` included in this
documentation serves as a reference for the syntax.

.. note::
  It is not required to get a deep understanding of Sphinx to create
  documentation for EVerest. You can check existing pages and you will
  see how easy it is to start documenting. In the end always make sure
  the end result (html) looks as intended!

The locations of the source files that make up the documentation you are reading,
are within the `EVerest/everest-core repository <https://github.com/EVerest/everest-core>`_.

.. note::
  You will find a number of documentation files that are not part of the documentation you are reading
  but still reside inside the `EVerest/everest-core repository <https://github.com/EVerest/everest-core>`_.
  See :ref:`below <documenting_everest_doc_near_source_code>`.

Main EVerest Documentation
==========================

This is a coherent documentation that helps you with getting a fast overview
of the EVerest framework, the EVerest tools and also contains some tutorials.

Reference Documentation
=======================

EVerest interfaces, modules, types and the EVerest API contain documentation
as part of their definitions, right inside the corresponding yaml files.
Those files may also contain configuration settings along with short explanations.

In the `EVerest/everest-core repository <https://github.com/EVerest/everest-core>`_:

* ``types/*.yaml``: Definitions of the internal EVerest types for inter-module communication.
  This adds to the *reference* section.
* ``interfaces/*.yaml``: Definitions of the internal interfaces for inter-module communication.
  This adds to the *reference* section.
* ``modules/.../manifest.yaml``: Definition of the individual modules. This adds to the *reference*
  section.
* ``docs/source/reference/EVerest_API``: This specific subfolder contains the definitions of the
  *EVerestAPI*. They are transformed to html to become part of the *reference* section.
  
The generated pages can be found in
:ref:`the reference section of the main documentation <everest_reference>`.

Optionally, EVerest modules can contain additional handwritten documentation.
See next subsection for more information on this.

Handwritten Documentation
=========================

Each module directory can contain additional handwritten documentation.

- ``modules/.../docs/index.rst``: Handwritten explanations for individual modules.

The contents will automatically be hyperlinked from the page containing the
automatically generated reference docs (explained in the subsection before).
It's considered good practice to also link back from the handwritten
text to the auto-generated reference page of the respective module.

As an example, see the auto-generated
:ref:`reference page of the EvseManager <everest_modules_EvseManager>`.
In the second paragraph, you see a link to the detailed handwritten
documentation.

General documentation that is not associated with a specific module:

- ``docs/source``: Find *tutorials*, *how-to-guides* and *explanations* source files here.

.. _documenting_everest_doc_near_source_code:

Documentation Near Corresponding Source Code
============================================

The documentation parts explained up to now are all part of the main EVerest
documentation you are reading right now. Some documentation snippets can also
be found directly in different GitHub repositories of the EVerest organisation.
These are often README.md files stored near the corresponding source code.

Those docs snippest are not being pushed to the EVerest main documentation.

Examples:

- md files in certain places the everest-core repository

  - ``docs/README.md``: How to build the documentation you are reading
  - ``applications/utils/everest-testing/README.md``: How to use pytest with EVerest

- md/general doc files in other repos (`everest-admin-panel <https://github.com/EVerest/everest-admin-panel>`_,
  `ext-switchev-iso15118 <https://github.com/EVerest/ext-switchev-iso15118>`_, ...)
