.. _everest_modules_handwritten_PaymentTerminalSimulator:

..  This file is a placeholder for an optional single file
    handwritten documentation for the PaymentTerminalSimulator module.
    Please decide whether you want to use this single file,
    or a set of files in the doc/ directory.
    In the latter case, you can delete this file.
    In the former case, you can delete the doc/ directory.
    
..  This handwritten documentation is optional. In case
    you do not want to write it, you can delete this file
    and the doc/ directory.

..  The documentation can be written in reStructuredText,
    and will be converted to HTML and PDF by Sphinx.

*******************************************
PaymentTerminalSimulator
*******************************************

:ref:`Link <everest_modules_PaymentTerminalSimulator>` to the module's reference.
Simulated Payment terminal module for SIL testing.
Can provide both, pre-validated and non pre-validated tokens, in order to simulate closed-loop (RFID) and open-loop (VISA, giro, ...) cards.


ToDo
====

* ToDo (CB): How to handle open pre-authorizations?
 * Add a config option for token timeout? (time after presenting the card until the token becomes invalid if no session is started)
 * maybe setup (async) timers in order to close them eventually?
 * Should use new TokenValidationStatus "Withdrawn"
  * ...in order to make sure no charging session can be started, once a pre-authorization is about to be cancelled (e.g. because of a timeout)
  * but does this work? (does sending "Withdrawn" really stop running EvseManager transactions? PG says yes!)
* Does not use the bank_session_token_provider yet, but should do should so (and should be forced to do so)
* A real hardware payment-terminal device requires more robustness:
 * device might timeout
 * device may not be available during the module's init/ready time, but later
 * one should implement an internal ready/not-ready state and only set it when the list of open preauthorizations has been cleaned (maybe add more diagnostics + clear_list command)

