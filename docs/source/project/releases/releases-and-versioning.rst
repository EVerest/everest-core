.. _project-release-and-versioning:

##################################
Releases and Versioning of EVerest
##################################

This document describes EVerest's release strategy, versioning scheme, and stability guarantees for the project and its components.

Release Cadence
===============

EVerest follows a time-based release strategy with stable releases every 6 months. Each stable release receives community maintenance
and follows explicit API stability guarantees. Each stable release undergoes a coordinated testing period before being published to
ensure quality and stability.

.. note::

    Monthly snapshot releases have been discontinued in favor of this stable release approach since 2026.

Versioning
==========

Versioning Format
-----------------

EVerest uses calendar-based versioning (CalVer) in the format:

.. code-block::

   yyyy.mm.x

Where:

- ``yyyy`` - Four-digit year of the release
- ``mm`` - Two-digit month of the release (01-12)
- ``x`` - Incremental patch number within the stable release line, starting at 0

Releases are made available through Github Releases and tags in the everest-core repository:

- `everest-core <https://github.com/EVerest/everest-core>`_

Stable Releases
---------------

Stable releases are published every 6 months and represent thoroughly tested versions of EVerest. These releases:

- Follow a coordinated testing period
- Receive community maintenance
- Are tagged in Git with the format ``yyyy.mm.0``
- Create a stable release branch for ongoing maintenance

Patch Releases
--------------

Patch releases address bugs, security issues, and other critical fixes within a stable release line. These releases:

- Increment only the patch number (x)
- Never introduce breaking changes to the :ref:`public API of EVerest <project-release-and-versioning-public-api>`
- Are backported from the main development branch
- Are tagged in Git with the incremented patch number

There is no fixed schedule for patch releases. They are published as needed to address issues in the stable release.

Development Versions
--------------------

The ``main`` branch represents ongoing development work for the next stable release. Development versions may introduce breaking changes
and new features that will be included in the next yyyy.mm.0 release.

.. _project-release-and-versioning-public-api:

Public API Definition
=====================

EVerest's public API includes all interfaces that external users and integrators rely on. These APIs are subject to the stability guarantees
described in this document.

The public API consists of:

- :doc:`External AsyncAPIs </explanation/adapt-everest/apis>`
- Energy Management JSON RPC API (upcoming)
- Configuration and Storage contracts. As of today, this includes:
    - EVerest module configuration files (YAML or SQLite)
    - OCPP configuration (JSON or SQLite)

The individual public API components may maintain their own version numbers independent of the EVerest release version.

Please refer to :ref:`breaking changes <project-breaking-changes>` for detailed definitions of breaking changes within the public API.

.. note::

    Internal EVerest :doc:`interfaces </reference/interfaces_index>` and :doc:`types </reference/types_index>` are explicitly excluded from the public API
    and may change without notice.

Stability Guarantees
====================

Within a stable release branch (e.g., all ``2026.01.x`` versions), no :ref:`breaking changes <project-breaking-changes>` are backported to the 
public API. Patch releases within a stable line maintain full backward compatibility.

This guarantee means:

- All EVerest and OCPP Configuration as well as other file and path dependencies from ``2026.01.0`` work with ``2026.01.5``
- External AsyncAPI clients compatible with ``2026.01.0`` work with all ``2026.01.x`` releases

EVerest makes a **best-effort attempt** to minimize breaking changes across major releases (e.g., from ``2026.01.x`` to ``2026.07.0``). However, breaking
changes may be necessary for:

- Significant architectural enhancements
- Protocol compliance updates
- Deprecation of obsolete features

If upgrading across major releases, integrators should review the release notes and potential migration documentation.

Maintenance Policy
==================

Infrastructure for Maintenance
-------------------------------

The EVerest project **provides infrastructure** for stable release maintenance:

* Each stable release creates a dedicated branch (``stable/yyyy.mm``)
* Stable branches remain open for community contributions
* Critical security patches and bug fixes can be backported by project maintainers
  or community contributions

**Important:** The existence of a stable branch does not guarantee ongoing 
maintenance activity. Actual maintenance depends on community involvement and 
the availability of contributors and maintainers.

Active Maintenance Focus
-------------------------

Project maintainers focus their efforts on:

* The **current stable release branch** - Active bug fixes and updates
* The **main development branch** - New features and next release
* **Security patches** - Backported to the current stable release on a best-effort basis

Older Stable Branches
---------------------

Older stable release branches:

* Remain available for community-driven maintenance
* May receive security patches at maintainer's discretion
* Are supported by the community on a **best-effort basis**
* Have no guaranteed response times or fix schedules

If your deployment relies on an older stable branch, we encourage you to 
contribute maintenance work back to the project.
