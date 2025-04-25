.. _everest_modules_handwritten_SessionAccountantSimulator:

..  This file is a placeholder for an optional single file
    handwritten documentation for the SessionAccountantSimulator module.
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
SessionAccountantSimulator
*******************************************

:ref:`Link <everest_modules_SessionAccountantSimulator>` to the module's reference.
Simulated Session Accountant module for SIL testing. Can provide both, pre-validated and non pre-validated tokens, in order to simulate closed-loop (RFID) and open-loop (VISA, giro, ...) cards.


TODO:
- change manifest: use enum for currency instead of int
- rework internal classes: PaymentSession should create an empty SessionRecord which is updated by a SessionRecorder and handed to a CostCalculator to calculate prices
- add mechanism to change price at any time
- unit test dynamic tariff updates
- make library for SingleEvseAccountant
- taxes?
- fix ordering of methods in cpp-files vs. headers
- add more debug logging
- need to handle sessions without transactions?!
- callback parameter lifetimes!
- do multi-EvseManager SIL to show that this multiple sessions can be handled simultaneously
- need to differentiate between payment methods? (is it a banking/RFID/PnC token?) or treat everything the same way and leave it to subscribers how to handle results?
- rename SingleEvseAccountant folder(?)
- write unit tests
- PaymentSession/CostCalculator should or should not charge money for idle before charging.
- Project Folder: includes in separate folder?
- Use the timestamp from the session_events: TransactionFinished, TransactionStarted

