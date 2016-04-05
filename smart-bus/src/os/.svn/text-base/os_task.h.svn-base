/*
 * os_task.h
 *
 *  Created on: 03-29-2015
 *      Author: yanjie 
 */

#ifndef OS_TASK_H
#define OS_TASK_H

#include <pthread.h>

#include "common.h"
#include "base_class.h"

typedef struct
{
  string sTaskName;
  APP *pcAppInst;
  void* (*entry)(void*);
  U8 u8TaskStart;
  U8 u8TaskPrio;
  UINT32 u32TaskPeriod; // ms
} OsTask_pThreadInfo;

typedef pthread_t OsTask_t;               
typedef pthread_attr_t OsTask_attr_t;               

class OsTaskInfo : public SMBase
{
    public:
        OsTask_pThreadInfo tdInfo; 
        OsTask_t tId; 
        OsTask_attr_t tAttr; 
        // for taskcreate
        U8 u8TaskIdx; 
        OsTaskInfo() {};
        virtual ~OsTaskInfo() {};
};

class OsTask : public SMBase
{
    public:
        OsTaskInfo cInfo;
	    OsTask();
	    virtual ~OsTask();
	    void init() {};
	    void run() {};
	    void end() {};

        eRet OS_taskInit();
        eRet OS_taskCreate();
        eRet OS_taskSuspend();
        eRet OS_taskResume();
        eRet OS_taskExit();
        eRet OS_taskDelay(UINT32 u32DelayMs);
};

#endif
