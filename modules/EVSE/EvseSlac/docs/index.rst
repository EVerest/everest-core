===================
EvseSlac
===================

This is an implementation of the ISO 15118-3 EVSE side SLAC protocol.
The general flow of operation will be like this:

- start of operation begins with a control pilot transition from state
  A, E or F to Bx, Cx or Dx. This is indicated by the EvseManager by calling
  the commands enter_bcd/leave_bcd.
- Once started, it waits for a CM_SLAC_PARM_REQ from the EV side to start the SLAC session.
- In case of success, SLAC finishes with a CM_SLAC_MATCH_RES, which sends a new NMK to the EV side.
  The EV then joins the logical network and the two modems are paired. A dlink_ready(true) is published
  to signal to the EvseManager that the low level PLC link is ready for communication.

If not run as root user, this modules requires the capability CAP_NET_RAW.

====
Todo
====

- make use of the enable flag in the reset command or drop it, if not needed
- handle CM_VALIDATE.REQ message
- implement PLC chip resetting after unplug or failed SLAC sessions (especially for QCA chips)
