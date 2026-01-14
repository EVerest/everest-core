.. _htg-choosing-version:

###########################
Choosing an EVerest Version
###########################

This guide helps you choose the right EVerest version for your use case.

Please refer to our :ref:`versioning policy <project-release-and-versioning>` for detailed information on
versioning and release cycles.

.. note::

   This guide provides general guidance but is not exhaustive. Actual version and upgrade 
   requirements  depend on your specific deployment, integrations, and customizations.
   Always thoroughly review release notes and test upgrades in non-production environments.

Production Deployments
======================

For production systems, use the latest stable release (``yyyy.mm.x``). These versions provide:

- Tested and verified functionality
- Ongoing maintenance and security updates
- Stable public APIs
- Community support

Development and Testing
=======================

For development of new features or testing upcoming changes, you may use the ``main`` branch. Be aware that:

- Breaking changes may occur at any time
- APIs may be unstable
- This is not suitable for production deployments

Upgrade Planning
================

Within Stable Lines
-------------------

Upgrading within a stable release line (e.g., ``2026.01.0`` to ``2026.01.3``) should be straightforward:

- Review the release notes for the target version
- Test the upgrade in a non-production environment
- Deploy the new version

No configuration or integration changes should be required.

Across Major Releases
---------------------

Upgrading to a new stable release line (e.g., ``2026.01.x`` to ``2026.07.0``) requires more careful planning:

- Review the release notes and potential migration documentation
- Identify any breaking changes affecting your integration
- Update configurations and integrations as needed
- Thoroughly test in a non-production environment
- Plan a maintenance window for production deployment

Migration guides may be provided by the community for each major release documenting known breaking changes and upgrade paths.

Different major releases may be fully backwards compatible, partially compatible, or incompatible depending on the changes introduced.
It is recommended to review the release notes as well as versions of the public API components when upgrading across major releases.
