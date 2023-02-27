//                  Copyright (c) 2016-2017 QUALCOMM Technologies Inc.
//                              All Rights Reserved.

#include "Thread.h"
#include "VkSampleFramework.h" // for LOGE

///////////////////////////////////////////////////////////////////////////////

Thread::Thread(ThreadFunction fx, void* params, uint32_t threadID)
{
    mThreadID           = threadID;
    mThreadFunction     = fx;
    mThreadParameters   = params;
    mThreadState        = UNINTIALIZED_THREAD_STATE;
}

///////////////////////////////////////////////////////////////////////////////

Thread::~Thread()
{

}

///////////////////////////////////////////////////////////////////////////////

bool Thread::Run()
{
    int32_t result;

    // Do not try to start thread if there is no function
    if (mThreadFunction == NULL)
    {
        LOGE("Thread::Run() no thread function defined\n");
        return false;
    }

    result = pthread_create(&mThread, NULL, mThreadFunction, mThreadParameters);
    if (result != 0)
    {
        LOGE("Thread::Run() pthread_create failed. Error = 0x%08X.\n", result);
        return false;
    }

    mThreadState = RUNNING_THREAD_STATE;

    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool Thread::Join()
{
    pthread_join(mThread, NULL);
    mThreadState = COMPLETED_THREAD_STATE;
}