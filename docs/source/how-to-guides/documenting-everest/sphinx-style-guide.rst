.. _everest_doc_sphinx_style_code:

##################
Sphinx Style Guide
##################

*********
Headlines
*********

Example:

.. code-block:: rst

    ###################
    How To: Sphinx (h1)
    ###################

    **************
    Headlines (h2)
    **************

    Headline (h3)
    =============

    Headline (h4)
    -------------

    Headline (h5)
    ^^^^^^^^^^^^^

    Headline (h6)
    """""""""""""

Result:

.. raw:: html

    <div class="highlight-rst"><pre>
    <h1>How To: Sphinx (h1)</h1>
    <h2>Headlines (h2)</h2>
    <h3>Headline (h3)</h3>
    <h4>Headline (h4)</h4>
    <h5>Headline (h5)</h5>
    <h6>Headline (h6)</h6>
    </pre></div>

******
Styles
******

.. code-block:: rst

    **Bold text**
    *Italic text*
    ``Inline literal/code``
    :sup:`super`\ Script
    :sub:`sub`\ Script

.. line-block::

    **Bold text**
    *Italic text*
    ``Inline literal/code``
    :sup:`super`\ Script
    :sub:`sub`\ Script


************
Bullet Lists
************
.. code-blocK:: rst

    * Unordered item
    * Unordered item

        #. Nestes ordered item
        #. Nestes ordered item
        
            #. Nested ordered item

    * Unordered item

* Unordered item
* Unordered item

    #. Nestes ordered item
    #. Nestes ordered item
    
        #. Nested ordered item

* Unordered item

*****************
Targets and Links
*****************

.. code-block:: rst

    .. Anchor target

    .. _anchorbyref:
    .. _Anchor link by text:

    .. External target

    .. _external_link_ref: https://example.com
    .. _External link name: https://example.com

    .. Footnote target

    .. [1] footnote text

    .. Citation target

    .. [cit1] A global citation

    .. External links

    `External link <https://example.com>`_
    `External link name`_
    `Example Text <External link name>`_
    `External link by ref <external_link_ref>`_

    .. Internal links

    `Anchor link by text`_
    `Anchor <Anchor link by text>`_
    `Anchor by ref <anchorbyref>`_
    :ref:`Anchor <anchorbyref>`

    .. Footnote

    Reference a footnote [1]_
    
    .. Citation

    Reference a global citation [cit1]_

    .. Section Link

    Section Heading
    ===============
    `Link <Section Heading>`_

.. anchor target

.. _anchorbyref:
.. _Anchor link by text:

.. external target

.. _external_link_ref: https://example.com
.. _External link name: https://example.com

.. footnote target

.. [1] footnote text

.. citation target

.. [cit1] A global citation

.. External links

| `External link <https://example.com>`_
| `External link name`_
| `Example Text <External link name>`_
| `External link by ref <external_link_ref>`_

.. Internal links

| `Anchor link by text`_
| `Anchor <Anchor link by text>`_
| `Anchor by ref <anchorbyref>`_
| :ref:`Anchor <anchorbyref>`

.. Footnote

Reference a footnote [1]_

.. Citation

Reference a global citation [cit1]_

.. Section Link

Section Heading
===============
`Link <Section Heading_>`_

******
Tables
******

.. code-block:: rst

    +-----------------+-----------------+-----------------+
    | Grid table      | Header 2        | Header 3        |
    |                 |                 |                 |
    +=================+=================+=================+
    | Column 1        | Column 2        | Vertical        |
    +-----------------+-----------------+ column          +
    | Horizontal span                   | span            |
    +-----------------+-----------------+-----------------+

    ============  ========  ========
    Simple table  Header 2  Header 3
    ============  ========  ========
    Column 1      Column 2  Column 3
    Horizontal column span  ...
    ----------------------  --------
    ...           ...       ...
    ============  ========  ========

    .. csv-table:: table title
        :header: "Header 1", "Header 2", "Header 3"
        :widths: 20, 20, 20
        :encoding: utf-8
        :header-rows: 1

        "Row 1, Column 1", "Row 1, Column 2", "Row 1, Column 3"
        "Row 2, Column 1", "Row 2, Column 2", "Row 2, Column 3"

The ``csv-table`` directive can be used to create tables from CSV files:

.. code-block:: rst

    .. csv-table:: table title
        :header: "Header 1", "Header 2", "Header 3"
        :widths: 20, 20, 20
        :encoding: utf-8
        :header-rows: 1
        :file: table.csv

Grid table
==========

+-----------------+-----------------+-----------------+
| Grid table      | Header 2        | Header 3        |
|                 |                 |                 |
+=================+=================+=================+
| Column 1        | Column 2        | Vertical        |
+-----------------+-----------------+ column          +
| Horizontal span                   | span            |
+-----------------+-----------------+-----------------+

Simple table
============

============  ========  ========
Simple table  Header 2  Header 3
============  ========  ========
Column 1      Column 2  Column 3
Horizontal column span  ...
----------------------  --------
...           ...       ...
============  ========  ========

CSV table
=========

.. csv-table:: table title
    :header: "Header 1", "Header 2", "Header 3"
    :widths: 20, 20, 20
    :encoding: utf-8
    :header-rows: 1

    "Row 1, Column 1", "Row 1, Column 2", "Row 1, Column 3"
    "Row 2, Column 1", "Row 2, Column 2", "Row 2, Column 3"



******************
Images and Figures
******************

Figures are images with captions. They support all image options.

.. code-block:: rst

    .. image:: image.png
        :alt: Image alt text
        :width: 150px
        :height: 150px
        :align: center
        :target: target_

    .. figure:: image.png
        :align: center
        :height: 150px
        :name: figure-name

    Figure caption :figure:`figure-name` or `Example <figure-name>`_

Image
=====

.. image:: https://via.placeholder.com/150
    :alt: Image alt text
    :width: 150px
    :height: 150px
    :align: center
    :target: https://example.com

Figure
======

.. figure:: https://via.placeholder.com/150
    :alt: Figure alt text
    :align: center
    :target: https://example.com
    :name: figure-name


Figure caption `figure-name`_ or `Example <figure-name_>`_

********
Comments
********

.. code-block:: rst

    .. comment
        This is a comment

**********
Directives
**********

.. code-block:: rst

    .. directive:: argument
        :option: value

        Directive content

*****************
Table of Contents
*****************

.. code-block:: rst

    .. local table of contents. The ``:local:`` option is optional.

    .. contents:: Table of Contents
        :local:
        :depth: 2

    .. defines global structure and includes all sub toc-trees and tocs
        Can also be set to visible by omitting the ``:hidden:`` option

    .. toc-tree:: Table of Contents
        :maxdepth: 2
        :numbered:
        :hidden:

        file.rst
        second_file
        directory/file

Table of Contents (this document)
=================================

.. contents:: Table of Contents
    :depth: 2
    :class: this-will-duplicate-information-and-it-is-still-useful-here
    :backlinks: none

************************
Content Block Directives
************************

.. contents:: Content Block Directives
    :depth: 1
    :class: this-will-duplicate-information-and-it-is-still-useful-here
    :backlinks: none
    :local:

``.. topic::`` *[title]*
=========================

.. code-block:: rst

    .. topic:: Topic title

        Topic content

.. topic:: Topic

    Topic content

``.. sidebar::`` *[title]*
===========================

.. code-block:: rst

    .. sidebar:: Sidebar title

        Sidebar content

.. sidebar:: Sidebar

    Sidebar content

``.. admonition::`` *[title]*
=============================

.. code-block:: rst

    .. admonition:: Admonition title

        Admonition content

.. admonition:: Admonition title

    Admonition content

``.. attention::``
==================

.. code-block:: rst

    .. attention::

        Attention content

.. attention::
    
        Attention content
    
``.. caution::``
================

.. code-block:: rst

    .. caution::

        Caution content

.. caution::
    
        Caution content
    
``.. danger::``
===============

.. code-block:: rst

    .. danger::

        Danger content

.. danger::

    Danger content

``.. error::``
==============

.. code-block:: rst

    .. error::

        Error content

.. error::

    Error content

``.. hint::``
=============

.. code-block:: rst

    .. hint::

        Hint content

.. hint::

    Hint content

``.. important::``
==================

.. code-block:: rst

    .. important::

        Important content

.. important::

    Important content

``.. note::``
=============

.. code-block:: rst

    .. note::

        Note content

.. note::

    Note content

``.. tip::``
============

.. code-block:: rst

    .. tip::

        Tip content

.. tip::

    Tip content

``.. warning::``
================

.. code-block:: rst

    .. warning::

        Warning content

.. warning::

    Warning content

``.. seealso::``
================

.. code-block:: rst

    .. seealso::

        See also content

.. seealso::

    See also content

``.. versionadded::`` *[version]*
==================================

.. code-block:: rst

    .. versionadded:: 1.0

        Version added content

.. versionadded:: 1.0

    Version added content

``.. versionchanged::`` *[version]*
====================================

.. code-block:: rst

    .. versionchanged:: 1.0

        Version changed content

.. versionchanged:: 1.0

    Version changed content

``.. deprecated::`` *[version]*
===============================

.. code-block:: rst

    .. deprecated:: 1.0

        Deprecated content

.. deprecated:: 1.0

    Deprecated content

``.. math::``
=============

.. code-block:: rst

    .. math::

        \int_{-\infty}^\infty g(x) dx = 1

.. math::

    \int_{-\infty}^\infty g(x) dx = 1

``.. raw::`` *output format*
============================

.. code-block:: rst

    .. raw:: html

        <div>Raw HTML content</div>

.. raw:: html
    
        <div>Raw HTML content</div>

*************
Code Examples
*************

.. code-block:: rst

    .. code-block:: python 
        :linenos:
        :emphasize-lines: 2,3
        :caption: Code caption
        :name: code-name

        // Code example
        some_function();
        any_var = 42;

        // Do another thing
        another_function();

    .. literalinclude:: index.rst
        :language: rst
        :linenos:
        :emphasize-lines: 2-5
        :dedent: 4

.. code-block:: python
    :linenos:
    :emphasize-lines: 2,3
    :caption: Code caption
    :name: code-name

    // Code example
    some_function();
    any_var = 42;

    // Do another thing
    another_function();

.. literalinclude:: index.rst
    :language: rst
    :linenos:
    :emphasize-lines: 2-5
    :dedent: 0
