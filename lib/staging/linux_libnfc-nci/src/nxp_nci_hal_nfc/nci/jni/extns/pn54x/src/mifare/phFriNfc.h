/*
 * Copyright (C) 2015 The Android Open Source Project
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
 * NFC FRI Main Header.
 */

#ifndef PHFRINFC_H
#define PHFRINFC_H
#include <phNfcTypes.h>

#define LOCK_BITS_CHECK_ENABLE

#define NFCSTATUS_INVALID_DEVICE_REQUEST (0x10F5)

/*
 * Completion Routine
 *
 * NFC-FRI components that work in an overlapped style need to provide a
 * function that is compatible to this definition. It is mandatory to define
 * such a routine for components that interact with other components up or down
 * the stack. Moreover, such components shall provide a function within their
 * API to enable the setting of the Completion Routine address and parameters.
 *
 *      First Parameter: Context
 *      Set to the address of the called instance (component instance context
 * structure). For instance, a component that needs to give control to a
 * component up the stack needs to call the completion routine of the upper
 * component. The value to assign to this parameter is the address of the
 * context structure instance of the called component. Such a structure
 * usually contains all variables, data or state information a component member
 * needs for operation. The address of the upper instance must be known by
 * the lower (completing) instance. The mechanism to ensure that this
 *      information is present involves the structure phFriNfc_CplRt_t . See its
 * documentation for further information.
 *
 *      Second Parameter: Status Value
 *      The lower layer hands over the completion status via this parameter. The
 * completion routine that has been called needs to process the status in a way
 * that is comparable to what a regular function return value would
 * require.
 *
 *       The prototype of the component's Process(ing) functions has to be
 * compatible to this function pointer declaration for components interacting
 * with others. In other cases, where there is no interaction or
 * asynchronous processing the definition of the Process(ing) function can
 * be arbitrary, if present at all.
 */
typedef void (*pphFriNfc_Cr_t)(void*, NFCSTATUS);

/*
 * Completion Routine structure
 *
 * This structure finds itself within each component that requires to report
 * completion to an upper (calling) component. Depending on the actual
 * implementation (static or dynamic completion information) the stack
 * Initialization or the calling component needs to inform the initialized or
 * called component about the completion path. This information is submitted via
 * this structure.
 */
typedef struct phFriNfc_CplRt {
  pphFriNfc_Cr_t CompletionRoutine; /* Address of the upper Layer's Process(ing)
                                     * function to call upon completion. The
                                     * stack initializer (or depending on the
                                     * implementation: the calling component)
                                     *   needs to set this member to the address
                                     * of the function that needs to be within
                                     *   the completion path: A calling
                                     * component would give its own processing
                                     * function address to the lower layer.
                                     */
  void* Context;                    /* Instance address (context) parameter.
                                     * The stack initializer (or depending on the
                                     * implementation: the calling component)
                                     * needs to set this member to the address of
                                     * the component context structure instance
                                     * within the completion path: A
                                     * calling component would give its own
                                     * instance address to the lower layer.
                                     */
} phFriNfc_CplRt_t;

#endif /* __PHFRINFC_H__ */
