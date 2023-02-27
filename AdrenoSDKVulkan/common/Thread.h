//                  Copyright (c) 2016-2017 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
#pragma once

#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

typedef pthread_mutex_t     Mutex;
typedef void* (*ThreadFunction)(void*);

enum ThreadState
{
    UNINTIALIZED_THREAD_STATE,
    RUNNING_THREAD_STATE,
    COMPLETED_THREAD_STATE
};

// Defines a thread

class VkSampleFramework;

class Thread
{
public:
    Thread(ThreadFunction fx, void* params, uint32_t threadID=0);
    ~Thread();

    bool Run();
    bool Join();

    uint32_t GetID(){return mThreadID;};

protected:
    ThreadState             mThreadState;
    uint32_t                mThreadID;
    ThreadFunction          mThreadFunction;
    void*                   mThreadParameters;
    pthread_t               mThread;
};
