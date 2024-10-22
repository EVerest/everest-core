/******************************************************************************
 *
 *  Copyright (C) 2015 NXP Semiconductors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License")
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

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tools.h"


void* framework_AllocMem(size_t size);
void  framework_FreeMem(void *ptr);


/****************** Thread ****************************/


typedef struct tLinuxThread
{
	pthread_t thread;
	void* ctx;
	void* (*threadedFunc)(void *);
	void* mutexCanDelete;
}tLinuxThread_t;

void* thread_object_func(void* obj)
{
	tLinuxThread_t *linuxThread = (tLinuxThread_t *)obj;
	void *res = NULL;
	framework_LockMutex(linuxThread->mutexCanDelete);
	res = linuxThread->threadedFunc(linuxThread->ctx);
	framework_UnlockMutex(linuxThread->mutexCanDelete);

	return res;
}


eResult framework_CreateThread(void** threadHandle, void * (* threadedFunc)(void *) , void * ctx)
{
	tLinuxThread_t *linuxThread = (tLinuxThread_t *)framework_AllocMem(sizeof(tLinuxThread_t));
	
	linuxThread->ctx = ctx;
	linuxThread->threadedFunc = threadedFunc;
	framework_CreateMutex(&(linuxThread->mutexCanDelete));
	
	if (pthread_create(&(linuxThread->thread), NULL, thread_object_func, linuxThread))
	{
		printf("Cannot create Thread\n");
		framework_DeleteMutex(linuxThread->mutexCanDelete);
		framework_FreeMem(linuxThread);
		
		return FRAMEWORK_FAILED;
	}
	pthread_detach(linuxThread->thread);
	
	*threadHandle = linuxThread;
	
	return FRAMEWORK_SUCCESS;
}

void framework_JoinThread(void * threadHandle)
{
	tLinuxThread_t *linuxThread = (tLinuxThread_t*)threadHandle;
	if (pthread_self() != linuxThread->thread)
	{
		// Will cause block if thread still running !!!
		framework_LockMutex(linuxThread->mutexCanDelete);
		framework_UnlockMutex(linuxThread->mutexCanDelete);
		// Thread now just ends up !
	}
}


void framework_DeleteThread(void * threadHandle)
{
	tLinuxThread_t *linuxThread = (tLinuxThread_t*)threadHandle;
	framework_DeleteMutex(linuxThread->mutexCanDelete);
	framework_FreeMem(linuxThread);
}

void * framework_GetCurrentThreadId()
{
	return (void*)pthread_self();
}

void * framework_GetThreadId(void * threadHandle)
{
	tLinuxThread_t *linuxThread = (tLinuxThread_t*)threadHandle;
	return (void*)linuxThread->thread;
}

void framework_MilliSleep(uint32_t ms)
{
	usleep(1000*ms);
}

/****************** Mutex ****************************/

typedef struct tLinuxMutex
{
	pthread_mutex_t *lock;
	pthread_cond_t  *cond;
}tLinuxMutex_t;

eResult framework_CreateMutex(void ** mutexHandle)
{
	tLinuxMutex_t *mutex = (tLinuxMutex_t *)framework_AllocMem(sizeof(tLinuxMutex_t));
	
	mutex->lock = (pthread_mutex_t*)framework_AllocMem(sizeof(pthread_mutex_t));
	mutex->cond = (pthread_cond_t*)framework_AllocMem(sizeof(pthread_cond_t));
	
	pthread_mutex_init(mutex->lock,NULL);
	pthread_cond_init(mutex->cond,NULL);
	
	*mutexHandle = mutex;
	
	return FRAMEWORK_SUCCESS;
}

void framework_LockMutex(void * mutexHandle)
{
	tLinuxMutex_t *mutex = (tLinuxMutex_t*)mutexHandle;
	
	int res = pthread_mutex_lock(mutex->lock);
	if (res)
	{
		printf("lock() failed %s\n",strerror (errno));
	}
}

void framework_UnlockMutex(void * mutexHandle)
{
	tLinuxMutex_t *mutex = (tLinuxMutex_t*)mutexHandle;
	int res = pthread_mutex_unlock(mutex->lock);
	if (res)
	{
		printf("unlock() failed %s\n",strerror (errno));
	}
}

void framework_WaitMutex(void * mutexHandle, uint8_t needLock)
{
	tLinuxMutex_t *mutex = (tLinuxMutex_t*)mutexHandle;
	
	if (needLock)
	{
		framework_LockMutex(mutexHandle);
	}

	pthread_cond_wait(mutex->cond,mutex->lock);
	
	if (needLock)
	{
		framework_UnlockMutex(mutexHandle);
	}
	
}

void framework_NotifyMutex(void * mutexHandle, uint8_t needLock)
{
	tLinuxMutex_t *mutex = (tLinuxMutex_t*)mutexHandle;
	
	if (needLock)
	{
		framework_LockMutex(mutexHandle);
	}

	pthread_cond_broadcast(mutex->cond);
	
	if (needLock)
	{
		framework_UnlockMutex(mutexHandle);
	}
}

void framework_DeleteMutex(void * mutexHandle)
{
	tLinuxMutex_t *mutex = (tLinuxMutex_t*)mutexHandle;
	
	pthread_mutex_destroy(mutex->lock);
	pthread_cond_destroy(mutex->cond);
	
	framework_FreeMem(mutex);
}

/****************** Memory Mgmt ****************************/


typedef struct sMemInfo
{
	uint32_t magic;
	size_t size;
} sMemInfo_t;

typedef struct sMemInfoEnd
{
	uint32_t magicEnd;
} sMemInfoEnd_t;


void* framework_AllocMem(size_t size)
{
	sMemInfo_t *info = NULL;
	sMemInfoEnd_t *infoEnd = NULL;
	uint8_t * pMem = (uint8_t *) malloc(size+sizeof(sMemInfo_t)+sizeof(sMemInfoEnd_t));
	info = (sMemInfo_t*)pMem;
	
	info->magic = 0xDEADC0DE;
	info->size  = size;

	pMem = pMem+sizeof(sMemInfo_t);

	memset(pMem,0xAB,size);

	infoEnd = (sMemInfoEnd_t*)(pMem+size);

	infoEnd->magicEnd = 0xDEADC0DE;
	return pMem;
}

void framework_FreeMem(void *ptr)
{
	if(NULL !=  ptr)
	{
		sMemInfoEnd_t *infoEnd = NULL;
		uint8_t *memInfo = (uint8_t*)ptr;
		sMemInfo_t *info = (sMemInfo_t*)(memInfo - sizeof(sMemInfo_t));

		infoEnd = (sMemInfoEnd_t*)(memInfo+info->size);

		if ((info->magic != 0xDEADC0DE)||(infoEnd->magicEnd != 0xDEADC0DE))
		{
			// Call Debugger
			*(int *)(uintptr_t)0xbbadbeef = 0;
		}else
		{
			memset(info,0x14,info->size+sizeof(sMemInfo_t)+sizeof(sMemInfoEnd_t));
		}
			
		free(info);
	}
}


