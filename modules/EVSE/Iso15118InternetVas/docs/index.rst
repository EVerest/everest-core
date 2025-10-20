.. _everest_modules_handwritten_Iso15118InternetVas:

*******************************************
Iso15118InternetVas
*******************************************

:ref:`Link <everest_modules_Iso15118InternetVas>` to the module's reference.

.. warning::
   This module and its helper scripts are **examples** and not intended for
   production use without modification. The provided ``vas-internet-setup.sh``
   script is a starting point and may not cover all edge cases for your
   specific hardware and network environment. It is the user's responsibility
   to ensure the configuration is secure and robust.

This module implements the ISO 15118-2 Value Added Service (VAS) for providing
internet access to a connected electric vehicle (EV). When an EV requests this
service, the module configures the charger's networking to share its internet
connection with the EV over the power line communication (PLC) interface.

How it works
============

1.  **Service Announcement**: The module advertises the availability of the
    Internet Access service (Service ID 3) to the EV, offering HTTP and/or HTTPS
    access based on the module's configuration.
2.  **Service Selection**: If the EV selects this service, the module initiates
    the network setup.
3.  **Network Setup**: The module executes a helper script,
    ``vas-internet-setup.sh``, to configure all necessary networking components.
    The script's behavior depends on the configuration and the services selected
    by the EV:
    - It enables IPv6 forwarding and sets up NAT using ``ip6tables``.
    - If IPv4 support is enabled in the configuration, it also:
      - Starts a DHCPv4 server (``udhcpd``) on the EV-facing network
        interface (``ev_interface``). This DHCP server provides the EV with an
        IPv4 address and DNS server information.
      - Enables IPv4 forwarding and NAT using ``iptables``.
    - It starts a Router Advertisement Daemon (``radvd``) on the
      ``ev_interface`` to enable the EV to configure an IPv6 address using SLAAC.
      This also includes advertising Recursive DNS Servers (RDNSS) for IPv6.
    - The module determines which ports to open based on the parameter sets
      (HTTP, HTTPS) selected by the EV. The forwarding is then restricted to
      allow only TCP traffic on the selected ports (80 for HTTP, 443 for HTTPS).
    - The example script uses public DNS servers (Google's 8.8.8.8 for IPv4
      and 2001:4860:4860::8888 for IPv6). This can be changed in the script.
4.  **Session Teardown**: When the charging session ends (signaled by the
    ``evse_manager``), the module calls the same script to automatically tear
    down the network configuration, stopping the services and removing all
    forwarding and NAT rules.

Requirements
============

Software
--------

The module relies on the ``vas-internet-setup.sh`` script, which requires the
following external tools to be available in the system's PATH:

- ``radvd``: For IPv6 Router Advertisements.
- ``ip6tables``: For setting up IPv6 NAT and forwarding rules.
- ``ip`` (from the ``iproute2`` package): For network interface configuration.
- ``iptables`` (optional, for IPv4): For IPv4 NAT and forwarding rules.
- ``udhcpd`` (optional, for IPv4): For the DHCPv4 server (typically provided by
  BusyBox).

The EVerest framework must be run with sufficient privileges to allow these
tools to modify network settings. This typically means running as the ``root``
user.

Hardware
--------

- A working internet connection on the charging station.
- A network interface that is connected to the internet (e.g., ``eth0``, ``wwan0``).
- A network interface for the Power Line Communication (PLC) modem that
  communicates with the EV (e.g., a HomePlug Green PHY modem connected via
  Ethernet).

Configuration
=============

.. list-table::
   :widths: 25 75
   :header-rows: 1

   * - Key
     - Description
   * - ``ev_interface``
     - The name of the network interface connected to the EV via the PLC modem.
   * - ``modem_interface``
     - The name of the network interface connected to the internet.
   * - ``http_support``
     - (boolean) Whether to announce support for HTTP (Port 80). Defaults to `true`.
   * - ``https_support``
     - (boolean) Whether to announce support for HTTPS (Port 443). Defaults to `true`.

Example Configuration
---------------------

.. code-block:: yaml

    - module: Iso15118InternetVas
      config:
        ev_interface: eth1
        modem_interface: eth0
        http_support: true
        https_support: true

Provided Interfaces
===================

- **iso15118_vas**: Implements the ``ISO15118_vas`` interface to handle service
  discovery and selection from the EV.

Required Interfaces
===================

- **evse_manager**: The module optionally connects to an ``evse_manager`` to
  monitor the charging session. When the session finishes, it triggers the
  teardown of the internet connection for the EV.