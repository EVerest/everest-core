/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 /******************************************************************************
 *
 *  The original Work has been changed by NXP Semiconductors.
 *
 *  Copyright 2021 NXP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/*
 *  Communicate with a peer using NFC-DEP, LLCP, SNEP.
 */
#pragma once
#ifndef LINUX
#include <utils/RefBase.h>
#include <utils/StrongPointer.h>
#endif
#include <string>
#include "NfcJniUtil.h"
#include "SyncEvent.h"
#include "nfa_p2p_api.h"

class P2pServer;
class P2pClient;
class NfaConn;
#define MAX_NFA_CONNS_PER_SERVER 5

/*****************************************************************************
**
**  Name:           PeerToPeer
**
**  Description:    Communicate with a peer using NFC-DEP, LLCP, SNEP.
**
*****************************************************************************/
class PeerToPeer {
 public:
  typedef unsigned int tJNI_HANDLE;

  /*******************************************************************************
  **
  ** Function:        PeerToPeer
  **
  ** Description:     Initialize member variables.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  PeerToPeer();

  /*******************************************************************************
  **
  ** Function:        ~PeerToPeer
  **
  ** Description:     Free all resources.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  ~PeerToPeer();

  /*******************************************************************************
  **
  ** Function:        getInstance
  **
  ** Description:     Get the singleton PeerToPeer object.
  **
  ** Returns:         Singleton PeerToPeer object.
  **
  *******************************************************************************/
  static PeerToPeer& getInstance();

  /*******************************************************************************
  **
  ** Function:        initialize
  **
  ** Description:     Initialize member variables.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  void initialize();

  /*******************************************************************************
  **
  ** Function:        llcpActivatedHandler
  **
  ** Description:     Receive LLLCP-activated event from stack.
  **                  nat: JVM-related data.
  **                  activated: Event data.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  void llcpActivatedHandler(nfc_jni_native_data* nativeData,
                            tNFA_LLCP_ACTIVATED& activated);

  /*******************************************************************************
  **
  ** Function:        llcpDeactivatedHandler
  **
  ** Description:     Receive LLLCP-deactivated event from stack.
  **                  nat: JVM-related data.
  **                  deactivated: Event data.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  void llcpDeactivatedHandler(nfc_jni_native_data* nativeData,
                              tNFA_LLCP_DEACTIVATED& deactivated);

  void llcpFirstPacketHandler(nfc_jni_native_data* nativeData);

  /*******************************************************************************
  **
  ** Function:        connectionEventHandler
  **
  ** Description:     Receive events from the stack.
  **                  event: Event code.
  **                  eventData: Event data.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  void connectionEventHandler(uint8_t event, tNFA_CONN_EVT_DATA* eventData);

  /*******************************************************************************
  **
  ** Function:        registerServer
  **
  ** Description:     Let a server start listening for peer's connection
  *request.
  **                  jniHandle: Connection handle.
  **                  serviceName: Server's service name.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool registerServer(tJNI_HANDLE jniHandle, const char* serviceName);

  /*******************************************************************************
  **
  ** Function:        deregisterServer
  **
  ** Description:     Stop a P2pServer from listening for peer.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool deregisterServer(tJNI_HANDLE jniHandle);

  /*******************************************************************************
  **
  ** Function:        accept
  **
  ** Description:     Accept a peer's request to connect.
  **                  serverJniHandle: Server's handle.
  **                  connJniHandle: Connection handle.
  **                  maxInfoUnit: Maximum information unit.
  **                  recvWindow: Receive window size.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool accept(tJNI_HANDLE serverJniHandle, tJNI_HANDLE connJniHandle,
              int maxInfoUnit, int recvWindow);

  /*******************************************************************************
  **
  ** Function:        createClient
  **
  ** Description:     Create a P2pClient object for a new out-bound connection.
  **                  jniHandle: Connection handle.
  **                  miu: Maximum information unit.
  **                  rw: Receive window size.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool createClient(tJNI_HANDLE jniHandle, uint16_t miu, uint8_t rw);

  /*******************************************************************************
  **
  ** Function:        connectConnOriented
  **
  ** Description:     Establish a connection-oriented connection to a peer.
  **                  jniHandle: Connection handle.
  **                  serviceName: Peer's service name.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool connectConnOriented(tJNI_HANDLE jniHandle, const char* serviceName);

  /*******************************************************************************
  **
  ** Function:        connectConnOriented
  **
  ** Description:     Establish a connection-oriented connection to a peer.
  **                  jniHandle: Connection handle.
  **                  destinationSap: Peer's service access point.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool connectConnOriented(tJNI_HANDLE jniHandle, uint8_t destinationSap);

  /*******************************************************************************
  **
  ** Function:        send
  **
  ** Description:     Send data to peer.
  **                  jniHandle: Handle of connection.
  **                  buffer: Buffer of data.
  **                  bufferLen: Length of data.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool send(tJNI_HANDLE jniHandle, uint8_t* buffer, uint16_t bufferLen);

  /*******************************************************************************
  **
  ** Function:        receive
  **
  ** Description:     Receive data from peer.
  **                  jniHandle: Handle of connection.
  **                  buffer: Buffer to store data.
  **                  bufferLen: Max length of buffer.
  **                  actualLen: Actual length received.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool receive(tJNI_HANDLE jniHandle, uint8_t* buffer, uint16_t bufferLen,
               uint16_t& actualLen);

  /*******************************************************************************
  **
  ** Function:        disconnectConnOriented
  **
  ** Description:     Disconnect a connection-oriented connection with peer.
  **                  jniHandle: Handle of connection.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool disconnectConnOriented(tJNI_HANDLE jniHandle);

  /*******************************************************************************
  **
  ** Function:        getRemoteMaxInfoUnit
  **
  ** Description:     Get peer's max information unit.
  **                  jniHandle: Handle of the connection.
  **
  ** Returns:         Peer's max information unit.
  **
  *******************************************************************************/
  uint16_t getRemoteMaxInfoUnit(tJNI_HANDLE jniHandle);

  /*******************************************************************************
  **
  ** Function:        getRemoteRecvWindow
  **
  ** Description:     Get peer's receive window size.
  **                  jniHandle: Handle of the connection.
  **
  ** Returns:         Peer's receive window size.
  **
  *******************************************************************************/
  uint8_t getRemoteRecvWindow(tJNI_HANDLE jniHandle);

  /*******************************************************************************
  **
  ** Function:        setP2pListenMask
  **
  ** Description:     Sets the p2p listen technology mask.
  **                  p2pListenMask: the p2p listen mask to be set?
  **
  ** Returns:         None
  **
  *******************************************************************************/
  void setP2pListenMask(tNFA_TECHNOLOGY_MASK p2pListenMask);

  /*******************************************************************************
  **
  ** Function:        getP2pListenMask
  **
  ** Description:     Get the set of technologies that P2P is listening.
  **
  ** Returns:         Set of technologies.
  **
  *******************************************************************************/
  tNFA_TECHNOLOGY_MASK getP2pListenMask();

  /*******************************************************************************
  **
  ** Function:        resetP2pListenMask
  **
  ** Description:     Reset the p2p listen technology mask to initial value.
  **
  ** Returns:         None.
  **
  *******************************************************************************/
  void resetP2pListenMask();

  /*******************************************************************************
  **
  ** Function:        enableP2pListening
  **
  ** Description:     Start/stop polling/listening to peer that supports P2P.
  **                  isEnable: Is enable polling/listening?
  **
  ** Returns:         None
  **
  *******************************************************************************/
  void enableP2pListening(bool isEnable);

  /*******************************************************************************
  **
  ** Function:        handleNfcOnOff
  **
  ** Description:     Handle events related to turning NFC on/off by the user.
  **                  isOn: Is NFC turning on?
  **
  ** Returns:         None
  **
  *******************************************************************************/
  void handleNfcOnOff(bool isOn);

  /*******************************************************************************
  **
  ** Function:        getNextJniHandle
  **
  ** Description:     Get a new JNI handle.
  **
  ** Returns:         A new JNI handle.
  **
  *******************************************************************************/
  tJNI_HANDLE getNewJniHandle();

  /*******************************************************************************
  **
  ** Function:        nfaServerCallback
  **
  ** Description:     Receive LLCP-related events from the stack.
  **                  p2pEvent: Event code.
  **                  eventData: Event data.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  static void nfaServerCallback(tNFA_P2P_EVT p2pEvent,
                                tNFA_P2P_EVT_DATA* eventData);

  /*******************************************************************************
  **
  ** Function:        nfaClientCallback
  **
  ** Description:     Receive LLCP-related events from the stack.
  **                  p2pEvent: Event code.
  **                  eventData: Event data.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  static void nfaClientCallback(tNFA_P2P_EVT p2pEvent,
                                tNFA_P2P_EVT_DATA* eventData);

 private:
  static const int sMax = 10;
  static PeerToPeer sP2p;

  // Variables below only accessed from a single thread
  uint16_t mRemoteWKS;   // Peer's well known services
  bool mIsP2pListening;  // If P2P listening is enabled or not
  tNFA_TECHNOLOGY_MASK mP2pListenTechMask;  // P2P Listen mask

  // Variable below is protected by mNewJniHandleMutex
  tJNI_HANDLE mNextJniHandle;

  // Variables below protected by mMutex
  // A note on locking order: mMutex in PeerToPeer is *ALWAYS*
  // locked before any locks / guards in P2pServer / P2pClient
  Mutex mMutex;
#ifndef LINUX
  android::sp<P2pServer> mServers[sMax];
  android::sp<P2pClient> mClients[sMax];
#endif
  // Synchronization variables
  SyncEvent mSetTechEvent;  // completion event for NFA_SetP2pListenTech()
  SyncEvent mSnepDefaultServerStartStopEvent;  // completion event for
                                               // NFA_SnepStartDefaultServer(),
                                               // NFA_SnepStopDefaultServer()
  SyncEvent
      mSnepRegisterEvent;    // completion event for NFA_SnepRegisterClient()
  Mutex mDisconnectMutex;    // synchronize the disconnect operation
  Mutex mNewJniHandleMutex;  // synchronize the creation of a new JNI handle

  /*******************************************************************************
  **
  ** Function:        ndefTypeCallback
  **
  ** Description:     Receive NDEF-related events from the stack.
  **                  ndefEvent: Event code.
  **                  eventData: Event data.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  static void ndefTypeCallback(tNFA_NDEF_EVT event,
                               tNFA_NDEF_EVT_DATA* evetnData);

#ifndef LINUX
  /*******************************************************************************
  **
  ** Function:        findServer
  **
  ** Description:     Find a PeerToPeer object by connection handle.
  **                  nfaP2pServerHandle: Connectin handle.
  **
  ** Returns:         PeerToPeer object.
  **
  *******************************************************************************/
  android::sp<P2pServer> findServerLocked(tNFA_HANDLE nfaP2pServerHandle);
  /*******************************************************************************
  **
  ** Function:        findServer
  **
  ** Description:     Find a PeerToPeer object by connection handle.
  **                  serviceName: service name.
  **
  ** Returns:         PeerToPeer object.
  **
  *******************************************************************************/
  android::sp<P2pServer> findServerLocked(tJNI_HANDLE jniHandle);

  /*******************************************************************************
  **
  ** Function:        findServer
  **
  ** Description:     Find a PeerToPeer object by service name
  **                  serviceName: service name.
  **
  ** Returns:         PeerToPeer object.
  **
  *******************************************************************************/
  android::sp<P2pServer> findServerLocked(const char* serviceName);

#endif
  /*******************************************************************************
  **
  ** Function:        removeServer
  **
  ** Description:     Free resources related to a server.
  **                  jniHandle: Connection handle.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  void removeServer(tJNI_HANDLE jniHandle);

  /*******************************************************************************
  **
  ** Function:        removeConn
  **
  ** Description:     Free resources related to a connection.
  **                  jniHandle: Connection handle.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  void removeConn(tJNI_HANDLE jniHandle);

  /*******************************************************************************
  **
  ** Function:        createDataLinkConn
  **
  ** Description:     Establish a connection-oriented connection to a peer.
  **                  jniHandle: Connection handle.
  **                  serviceName: Peer's service name.
  **                  destinationSap: Peer's service access point.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool createDataLinkConn(tJNI_HANDLE jniHandle, const char* serviceName,
                          uint8_t destinationSap);
#ifndef LINUX
  /*******************************************************************************
  **
  ** Function:        findClient
  **
  ** Description:     Find a PeerToPeer object with a client connection handle.
  **                  nfaConnHandle: Connection handle.
  **
  ** Returns:         PeerToPeer object.
  **
  *******************************************************************************/
  android::sp<P2pClient> findClient(tNFA_HANDLE nfaConnHandle);

  /*******************************************************************************
  **
  ** Function:        findClient
  **
  ** Description:     Find a PeerToPeer object with a client connection handle.
  **                  jniHandle: Connection handle.
  **
  ** Returns:         PeerToPeer object.
  **
  *******************************************************************************/
  android::sp<P2pClient> findClient(tJNI_HANDLE jniHandle);

  /*******************************************************************************
  **
  ** Function:        findClientCon
  **
  ** Description:     Find a PeerToPeer object with a client connection handle.
  **                  nfaConnHandle: Connection handle.
  **
  ** Returns:         PeerToPeer object.
  **
  *******************************************************************************/
  android::sp<P2pClient> findClientCon(tNFA_HANDLE nfaConnHandle);

  /*******************************************************************************
  **
  ** Function:        findConnection
  **
  ** Description:     Find a PeerToPeer object with a connection handle.
  **                  nfaConnHandle: Connection handle.
  **
  ** Returns:         PeerToPeer object.
  **
  *******************************************************************************/
  android::sp<NfaConn> findConnection(tNFA_HANDLE nfaConnHandle);

  /*******************************************************************************
  **
  ** Function:        findConnection
  **
  ** Description:     Find a PeerToPeer object with a connection handle.
  **                  jniHandle: Connection handle.
  **
  ** Returns:         PeerToPeer object.
  **
  *******************************************************************************/
  android::sp<NfaConn> findConnection(tJNI_HANDLE jniHandle);
#endif
};
/*****************************************************************************
**
**  Name:           NfaConn
**
**  Description:    Store information about a connection related to a peer.
**
*****************************************************************************/
#ifndef LINUX
class NfaConn : public android::RefBase {
#else
class NfaConn  {
#endif
 public:
  tNFA_HANDLE mNfaConnHandle;          // NFA handle of the P2P connection
  PeerToPeer::tJNI_HANDLE mJniHandle;  // JNI handle of the P2P connection
  uint16_t mMaxInfoUnit;
  uint8_t mRecvWindow;
  uint16_t mRemoteMaxInfoUnit;
  uint8_t mRemoteRecvWindow;
  SyncEvent mReadEvent;           // event for reading
  SyncEvent mCongEvent;           // event for congestion
  SyncEvent mDisconnectingEvent;  // event for disconnecting

  /*******************************************************************************
  **
  ** Function:        NfaConn
  **
  ** Description:     Initialize member variables.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  NfaConn();
};
/*****************************************************************************
**
**  Name:           P2pServer
**
**  Description:    Store information about an in-bound connection from a peer.
**
*****************************************************************************/
#ifndef LINUX
class P2pServer : public android::RefBase {
#else
class P2pServer  {
#endif
 public:
  static const std::string sSnepServiceName;

  tNFA_HANDLE mNfaP2pServerHandle;     // NFA p2p handle of local server
  PeerToPeer::tJNI_HANDLE mJniHandle;  // JNI Handle
  SyncEvent mRegServerEvent;           // for NFA_P2pRegisterServer()
  SyncEvent mConnRequestEvent;         // for accept()
  std::string mServiceName;

  /*******************************************************************************
  **
  ** Function:        P2pServer
  **
  ** Description:     Initialize member variables.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  P2pServer(PeerToPeer::tJNI_HANDLE jniHandle, const char* serviceName);

  /*******************************************************************************
  **
  ** Function:        registerWithStack
  **
  ** Description:     Register this server with the stack.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool registerWithStack();

  /*******************************************************************************
  **
  ** Function:        accept
  **
  ** Description:     Accept a peer's request to connect.
  **                  serverJniHandle: Server's handle.
  **                  connJniHandle: Connection handle.
  **                  maxInfoUnit: Maximum information unit.
  **                  recvWindow: Receive window size.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool accept(PeerToPeer::tJNI_HANDLE serverJniHandle,
              PeerToPeer::tJNI_HANDLE connJniHandle, int maxInfoUnit,
              int recvWindow);

  /*******************************************************************************
  **
  ** Function:        unblockAll
  **
  ** Description:     Unblocks all server connections
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  void unblockAll();
#ifndef LINUX
  /*******************************************************************************
  **
  ** Function:        findServerConnection
  **
  ** Description:     Find a P2pServer that has the handle.
  **                  nfaConnHandle: NFA connection handle.
  **
  ** Returns:         P2pServer object.
  **
  *******************************************************************************/
  android::sp<NfaConn> findServerConnection(tNFA_HANDLE nfaConnHandle);

  /*******************************************************************************
  **
  ** Function:        findServerConnection
  **
  ** Description:     Find a P2pServer that has the handle.
  **                  jniHandle: JNI connection handle.
  **
  ** Returns:         P2pServer object.
  **
  *******************************************************************************/
  android::sp<NfaConn> findServerConnection(PeerToPeer::tJNI_HANDLE jniHandle);
#endif
  /*******************************************************************************
  **
  ** Function:        removeServerConnection
  **
  ** Description:     Remove a server connection with the provided handle.
  **                  jniHandle: JNI connection handle.
  **
  ** Returns:         True if connection found and removed.
  **
  *******************************************************************************/
  bool removeServerConnection(PeerToPeer::tJNI_HANDLE jniHandle);

 private:
  Mutex mMutex;
#ifndef LINUX
  // mServerConn is protected by mMutex
  android::sp<NfaConn> mServerConn[MAX_NFA_CONNS_PER_SERVER];

  /*******************************************************************************
  **
  ** Function:        allocateConnection
  **
  ** Description:     Allocate a new connection to accept on
  **                  jniHandle: JNI connection handle.
  **
  ** Returns:         Allocated connection object
  **                  NULL if the maximum number of connections was reached
  **
  *******************************************************************************/
  android::sp<NfaConn> allocateConnection(PeerToPeer::tJNI_HANDLE jniHandle);
#endif
};

/*****************************************************************************
**
**  Name:           P2pClient
**
**  Description:    Store information about an out-bound connection to a peer.
**
*****************************************************************************/
#ifndef LINUX
class P2pClient : public android::RefBase {
#else
class P2pClient  {
#endif
 public:
  tNFA_HANDLE mNfaP2pClientHandle;  // NFA p2p handle of client
  bool mIsConnecting;               // Set true while connecting
#ifndef LINUX
  android::sp<NfaConn> mClientConn;
#endif
  SyncEvent mRegisteringEvent;  // For client registration
  SyncEvent mConnectingEvent;   // for NFA_P2pConnectByName or Sap()
  SyncEvent mSnepEvent;         // To wait for SNEP completion

  /*******************************************************************************
  **
  ** Function:        P2pClient
  **
  ** Description:     Initialize member variables.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  P2pClient();

  /*******************************************************************************
  **
  ** Function:        ~P2pClient
  **
  ** Description:     Free all resources.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  ~P2pClient();

  /*******************************************************************************
  **
  ** Function:        unblock
  **
  ** Description:     Unblocks any threads that are locked on this connection
  **
  ** Returns:         None
  **
  *******************************************************************************/
  void unblock();
};
