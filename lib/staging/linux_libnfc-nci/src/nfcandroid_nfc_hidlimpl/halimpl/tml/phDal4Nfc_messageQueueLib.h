/*
 * Copyright (C) 2010-2014 NXP Semiconductors
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

/*
 * DAL independent message queue implementation for Android
 */

#ifndef PHDAL4NFC_MESSAGEQUEUE_H
#define PHDAL4NFC_MESSAGEQUEUE_H

#include <linux/ipc.h>
#include <phNfcTypes.h>

intptr_t phDal4Nfc_msgget(key_t key, int msgflg);
void phDal4Nfc_msgrelease(intptr_t msqid);
int phDal4Nfc_msgctl(intptr_t msqid, int cmd, void* buf);
intptr_t phDal4Nfc_msgsnd(intptr_t msqid, phLibNfc_Message_t* msg, int msgflg);
int phDal4Nfc_msgrcv(intptr_t msqid, phLibNfc_Message_t* msg, long msgtyp,
                     int msgflg);

#endif /*  PHDAL4NFC_MESSAGEQUEUE_H  */
