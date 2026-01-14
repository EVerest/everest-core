.. _project-contributing:

#######################
Contributing to EVerest
#######################

Thank you for your interest in contributing to EVerest.

This document explains our contribution process and how to contribute a bug fix
or change in source code. In case you want to contribute to the EVerest
documentation, please see :doc:`Documenting EVerest </explanation/the-everest-documentation>`.

For a description of the roles and responsibilities of the various members of
the EVerest community, see the :doc:`Governance Policy </project/governance/governance>`, and for further details,
see the project's `Technical Charter <https://github.com/lf-energy/foundation/blob/main/project_charters/everest_charter.pdf>`_.

Briefly, Contributors are anyone who submits content to the project, Committers
review and approve such submissions, and the Technical Steering Committee
provides general project oversight.

If you just need help or have a question, refer to the :doc:`Community Channels </project/community>`.

To contribute code to the project, first read over the :doc:`Governance Policy </project/governance/governance>` 
page to understand the roles involved.

Discussing and Communication
-----------------------------

We encourage contributors to engage in discussions, whether it's about
proposing new features, reporting bugs, contributing to the documentation or
seeking clarification on project-related matters.
While GitHub issues are a valuable tool for tracking tasks and issues, we find
that real-time communication often leads to faster resolution and clearer
understanding.

To facilitate such discussions, we utilize Zulip chat as our primary
communication platform. Zulip offers a threaded chat experience that helps
keep conversations organized and searchable, making it easier to follow
discussions even if you join midway.

If a discussion leads to actionable tasks or decisions, summarize the outcome
and create a corresponding GitHub issue. This allows for clear tracking and
enables contributors to start working on the agreed-upon tasks.

Join the Zulip chat here:
`EVerest channel on LF Energy Zulip <https://lfenergy.zulipchat.com/>`_

Issues
------

If you find a bug or want to request a new feature, please create an issue
in the GitHub repository. Make sure to provide a clear and concise description
of the problem or feature request, along with any relevant details or
steps to reproduce the issue.

The repository provides issue templates for bugs and feature requests to help you
provide all necessary information.

Pull requests
-------------

Each code contribution must include:

- Tests and documentation to explain the functionality.
- Any new files have `Copyright and license headers <https://github.com/lf-energy/tac/blob/main/process/contribution_guidelines.md#license>`_
- A `Developer Certificate of Origin signoff <https://github.com/lf-energy/tac/blob/main/process/contribution_guidelines.md#contribution-sign-off>`_.
- Submitted to the project as a pull request.

Each commit message and pull request description should have enough information
in it so that other contributors can understand what has been changed and
eventually which impact the change will have. Issue and pull request templates
of the repositories of the EVerest organization support a proper format
of code contributions.

Every pull request for a feature request must link an issue. Pull requests for
bugfixes do not necessarily need to link an issue, but they should contain a
proper description of the bug that is fixed.

You can use draft pull requests if you want to share your current state
and start a discussion about your changes. As soon as a draft pull request
is converted to be ready for review, the CODEOWNERS will be automatically
notified and start the review process.

If reviewers require changes to your PR, please keep the commit history clean
and do not rebase and force push to your branch, so that the changes can be
tracked and reviewers can quickly identify if the required changes have been
addressed.

Project committers will review the contribution in a timely manner, and advise
of any changes needed to merge the request.

Labels
------

GitHub labels help organize issues and pull requests. EVerest does not yet have
a standardized label system. When contributing:

- Browse existing labels in the repository and use them consistently
- Common labels to look for: ``bug``, ``enhancement``, ``documentation``, 
  ``help-wanted``, ``good-first-issue``
- Backport workflow: Use ``backport-candidate`` to mark PRs that should be
  considered for backporting to stable releases

If you need a label that doesn't exist, discuss with maintainers before creating it.

Coding Style
------------

Each contribution must meet the `Javascript <https://github.com/EVerest/EVerest/blob/main/.eslintrc.json>`_ or
`C++ <https://github.com/EVerest/EVerest/blob/main/.clang-format>`_ *coding style* (part of every repository).

License
-------

EVerest is licensed under the `Apache License 2.0 <https://github.com/EVerest/everest-core/blob/main/LICENSE>`_ license.
Contributions should abide by that standard license.
