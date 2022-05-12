===================
Basic Driver Design
===================

This is an implementation of the ISO 15188-3 EVSE side SLAC protocol.
The general flow of operation will be like this:

- start of operation begins with a control pilot transition from state
  A, E or F to Bx, Cx or Dx
- tbc


====
Todo
====

- implement D-LINK.Terminate, D-LINK.Error and D-LINK.Pause
- make use of the enable flag in the reset command or drop it, if not needed
- handle CM_VALIDATE.REQ message
