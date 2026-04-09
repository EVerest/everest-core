#####################
C++ Coding Guidelines
#####################

This how-to-guide describes best practices for developing C++ code for
EVerest.

The aim is to have a consistent approach in order to avoid coding errors
and aid with code maintenance.

This guide is aimed at new code contributions. Existing code can be updated
but this should be in a separate pull request and not part of functionality
changes.

See :doc:`Contributing to EVerest </project/contributing>` for
more information on how to contribute.

Before merging your contribution please ensure that the merged commit message is
meaningful and describes the changes following review.

Code Layout
===========

C++ code in EVerest is formatted via ``Clang Format`` and this is enforced
via the CI pipelines. This consistent approach is essential to aid with
code reviews ensuring that there are minimal (ideally none) formatting only
changes.

It is recommended to use the EVerest clang format container to reformat your
code before submitting a pull request.

The EVerest clang format container ensures a consistent output using a specific
version of clang format. Unfortunately different versions of clang format
give different results even with the same ``.clang-format`` configuration file.

The docker container for clang-format is `here <https://github.com/EVerest/everest-ci>`_.

File Naming
===========

Historically different approaches have been used and it is recommended to be
consistent with the existing code in the repository.

- C++ files should use the extension ``.cpp``
- C++ header files should use the extension ``.hpp``
- C files should use the extension ``.c``
- C header files should use the extension ``.h``
- For EVerest modules file names reflect the class contained within e.g. class
  ``EvseManager`` would have ``EvseManager.cpp`` and ``EvseManager.hpp``
- For library code file names are in snake_case and should reflect the class
  contained within e.g. class ``OpenSSLProvider`` would have
  ``openssl_provider.cpp`` and ``openssl_provider.hpp``
- header files and implementation should be co-located in the same directory
- private/internal headers should be in a separate directory (e.g. a sub-directory)
- avoid mixing PascalCase and snake_case in the same filename
- when a directory contains many files consider creating sub-directories to group
  functionality

Naming
======

- classes, and structures, should be named using PascalCase starting with a capital letter
- functions and methods should be in lower case snake_case
- namespaces should relate to the directory hierarchy
- enum values should be consistently named - i.e. avoid mixing PascalCase and
  snake_case
- member variables should be snake_case starting with  `m_`
- function parameters should not match member variables or local variables
- the `this` pointer should only be used when absolutely required

Example header file called ``ocpp/OcppDataModel.hpp``:

  .. code-block:: cpp

    // SPDX-License-Identifier: Apache-2.0
    // Copyright 2026 Pionix GmbH and Contributors to EVerest

    /// \file Object to manage the OCPP data model

    #pragma once

    namespace ocpp {

    class OcppDataModel : public OcppDataModelBase {
    private:
        int m_num_connectors{-1};

    public:
        OcppDataModel() = default;
        OcppDataModel(int num_connectors) : m_num_connectors(num_connectors) {
        }

        /// \brief check if the object has been configured
        /// \returns true when there is at least one connector
        bool isConfigured() const {
            return m_num_connectors > 0;
        }

        /// \brief initialise with the count of connectors
        /// \param[in] count - the number of detected connectors
        void initialise(int count);

        operator const OcppDataModelBase&() const {
            return *this;
        }
    };

    } // namespace ocpp

Example source file called ``ocpp/OcppDataModel.cpp``:

  .. code-block:: cpp

    // SPDX-License-Identifier: Apache-2.0
    // Copyright 2026 Pionix GmbH and Contributors to EVerest

    namespace {
    int internal_function(int parameter) {
        return parameter * 4 + 1;
    }

    namespace ocpp {

    void OcppDataModel::initialise(int count) {
        m_num_connectors = internal_function(count);
    }

    } // namespace ocpp

Header Files
============

In general inline code should be avoid in header files however C++ templates will
often require this and it may be needed for inlining.

Member variable initialisers should be provided. The following links gives examples
of the different types and their uses:

- `Initialisation <https://en.cppreference.com/w/cpp/language/initialization.html>`_
- `Non-static data members <https://en.cppreference.com/w/cpp/language/data_members.html>`_

e.g.

.. code-block:: cpp

    struct s {
        std::string s3 = "hello";    // 1. copy-initialization
        std::string s4 = {"hello"};  // 2. list-initialization (since C++11)
        std::string s5{'a'};         // 3. list-initialization (since C++11)
    };

List initialisation is preferred. Be consistent with usage and avoid mixing
initialisation types (especially 2. and 3.).

Exceptions
==========

There are always going to be occasions where departure from these guidelines
is warranted. Comments should be used to explain the rationale for the difference.

It is recommended to match the existing style, or create a pull request that
only updates the file to the new standards before making any code changes.
This simplifies reviewing the pull request.

Coding Style
============

- minimise the use of early returns from a function
- try to keep functions short so they can be seen on a single screen
- always include a ``default:`` clause in a switch statement
- add doxygen comments using ``///`` and ``\brief`` …
- try to use meaningful variable names
- use ``#pragma once`` as the include guard
- avoid throwing exceptions - keep use of exceptions contained within your class
- use `RAII <https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization>`_ where possible
- use existing utility functions (*)
- minimise external dependencies and ensure a compatible licence exists
- design with testing in mind - ensure that your code can be unit tested
- consider object life cycles and the impacts of multiple threads
- add unit tests

* there are a growing set of utility classes and functions in the lib directory.
  Including thread safe queues and object protection (monitor) and bit flags based
  on enumerations. Where possible use existing implementations and consider generalising new functions
  for inclusion into the ``util`` library.

If in doubt have a look at existing code and consider the `Google style guide <https://google.github.io/styleguide/cppguide.html>`_

Suggesting a Change
===================

`Zulip <https://lfenergy.zulipchat.com/>`_ can be used as a starting point to interact with the community about
a change, or enhancement. Once there is some consensus on an approach an issue
should be raised in the `EVerest github <https://github.com/EVerest/EVerest/issues>`_.

See :doc:`Contributing to EVerest </project/contributing>` for
more information on how to contribute.

Issues have a status:

- Backlog - issue needs to be looked at
- Ready - issue is ready for implementation
- In Progress - issue is being worked on
- In review - the updates are in review (e.g. pull request)
- Done - the issue is closed

Please follow the issue template and add information such as the fork/branch
where the issue is being looked at along with any pull request IDs.

It is especially important to check (the comments and status) before starting to
work on an issue to ensure that someone else hasn't already started to look at it.
