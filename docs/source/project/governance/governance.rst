#################################
EVerest project governance policy
#################################

Overview
========

This project aims to be governed in a transparent, accessible way for the
benefit of the community. All participation in this project is open and not
bound to corporate affiliation. Participants are bound to the project's
:doc:`Code of Conduct </project/governance/code-of-conduct>`.

Project roles
=============

Contributor
-----------

The contributor role is the starting role for anyone participating in the
project and wishing to contribute code or documentation.

Process for becoming a contributor
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Review the :doc:`Contribution Guidelines </project/contributing>` to ensure your contribution is inline
  with the project's coding and styling guidelines.
- Submit your code as a PR with the appropriate DCO sign-off.
- Have your submission approved by maintainers and merged into the codebase.

Committer
---------

Committers have write access to EVerest repositories and can merge pull requests once approved.
They participate in project discussions and help with code review, but are not
required to be code owners of specific areas.

Process for becoming a committer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Show your experience with the codebase through contributions and engagement
  on the community channels.
- Request to become a committer by contacting the TSC or an existing committer
  in working group meetings or channels.
- If approved, you will be granted write access to the EVerest repositories.

Committer responsibilities
~~~~~~~~~~~~~~~~~~~~~~~~~~

- Monitor communication channels.
- Triage GitHub issues and review pull requests.
- Help move PRs forward or close stale ones.
- Participate in community discussions and working groups.
- Mentor contributors and help them through the contribution process.

When does a committer lose committer status
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If a committer is no longer interested or cannot perform the committer duties,
they should volunteer to be moved to emeritus status. In extreme cases this can
also occur by a vote of the TSC.

Maintainer
----------

Maintainers are committers with specific code ownership responsibilities. They
are listed in the ``.github/CODEOWNERS`` file and are automatically requested
as reviewers for changes in their areas.

Process for becoming a maintainer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Already be a committer with demonstrated expertise in a specific area.
- Show deep understanding of the codebase in your area through contributions
  and reviews.
- Request maintainer status for a specific area or be nominated by existing
  maintainers.
- TSC approves the maintainer assignment.
- Added to the ``.github/CODEOWNERS`` file for your area.

Maintainer responsibilities
~~~~~~~~~~~~~~~~~~~~~~~~~~~

In addition to committer responsibilities:

- Review and approve pull requests in their code ownership areas.
- Ensure code quality and architectural consistency in their areas.
- Make decisions on technical direction for their components.
- Participate in backport decisions for their areas.
- Respond to questions and issues related to their areas.

Technical Steering Committee (TSC)
----------------------------------

The Technical Steering Committee (TSC) is responsible for the overall project
health and direction, coordination of activities, and working with other
projects and committees as needed for the continued growth of the project.

TSC Members
~~~~~~~~~~~

TSC Members have formal voting rights on TSC decisions. They can be elected by existing
voting members. A simple majority of existing voting members is required for approval.

See the `TSC MEMBERS file <https://github.com/EVerest/EVerest/blob/main/tsc/MEMBERS.md>`_
for the current list of TSC members and their roles.

TSC responsibilities
~~~~~~~~~~~~~~~~~~~~

- Set overall technical direction and roadmap
- Resolve technical disputes
- Approve new committers and maintainers
- Define and update governance policies
- Coordinate releases and major initiatives
- Represent EVerest in the broader community
- Make decisions on project structure and processes

Voting process
~~~~~~~~~~~~~~

Decisions require a simple majority vote of TSC members. Each TSC member receives one vote.
Voting can occur during TSC meetings or asynchronously via the TSC mailing list.
Voting delegation in case of absence can be requested via email to the TSC mailing list.

When does a voting member lose voting status
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If a voting member is no longer interested or cannot perform their TSC duties,
they should volunteer to step down to non-voting member or emeritus status.
In extreme cases, a voting member can be removed by a vote of the other voting
members per the voting process above.

TSC meetings and participation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

TSC meetings are open to the community. See the
`TSC README <https://github.com/EVerest/EVerest/blob/main/tsc/README.md>`_
for meeting minutes and how to participate.

Release Process
===============

Project releases occur on a scheduled basis as agreed to by the TSC.
See :ref:`Release and Versioning <project-release-and-versioning>` for details.

Conflict resolution
===================

In general, we prefer that technical issues and membership decisions are amicably
worked out between the persons involved. If a dispute cannot be decided independently,
the issue can be escalated to the TSC.

Communication
=============

This project, just like all of open source, is a global community. In addition to the
:doc:`Code of Conduct </project/governance/code-of-conduct>`, this project will:

- Keep all communication on open channels (mailing list, forums, chat).
- Be respectful of time and language differences between community members (such as scheduling meetings, email/issue responsiveness, etc.).
- Ensure tools are able to be used by community members regardless of their region.

If you have concerns about communication challenges for this project, please contact the TSC.
