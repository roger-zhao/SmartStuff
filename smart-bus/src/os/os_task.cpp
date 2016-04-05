/*
 * os_task.cpp
 *
 *  Created on: 03-29-2015
 *      Author: yanjie 
 */

#include "os_task.h"


OsTask::OsTask()
{
}

OsTask::~OsTask()
{
}

eRet OS_taskInit(OsTaskInfo info)
{
    // TODO add rt-preempt 
#ifndef RT_PREMPT
    // if(0 != pthread_attr_init())
#endif
    return eOK;
}

eRet OsTask::OS_taskCreate()
{
    OsTaskInfo *pInfo = &this->cInfo;
    // TODO add rt-preempt 
    if(0 != pthread_create(&pInfo->tId, NULL, pInfo->tdInfo.entry, &pInfo->u8TaskIdx)) {
        DBG(DBG_FATAL, "error to create task: %s\n", pInfo->tdInfo.sTaskName.c_str());
        return eFail;
    }
    return eOK;
    // RT-preempt
    /*
     * s = pthread_attr_init(&attr);
               if (s != 0)
                   handle_error_en(s, "pthread_attr_init");

               s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
               if (s != 0)
                   handle_error_en(s, "pthread_attr_setdetachstate");

               s = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
               if (s != 0)
                   handle_error_en(s, "pthread_attr_setinheritsched");

               stack_size = strtoul(argv[1], NULL, 0);

               s = posix_memalign(&sp, sysconf(_SC_PAGESIZE), stack_size);
               if (s != 0)
                   handle_error_en(s, "posix_memalign");

               printf("posix_memalign() allocated at %p\n", sp);

               s = pthread_attr_setstack(&attr, sp, stack_size);
               if (s != 0)
                   handle_error_en(s, "pthread_attr_setstack");
     */
}
eRet OsTask::OS_taskSuspend()
{
    // OsTaskInfo info = this->cInfo;
    return eOK;
}
eRet OsTask::OS_taskResume()
{
    // OsTaskInfo info = this->cInfo;
    return eOK;
}
eRet OsTask::OS_taskExit()
{
    // OsTaskInfo info = this->cInfo;
    return eOK;
}

eRet OsTask::OS_taskDelay(UINT32 u32DelayMs)
{
    // OsTaskInfo info = this->cInfo;
#ifdef RT_PREMPT
#else
    usleep(u32DelayMs*1000);
#endif
    return eOK;
}

