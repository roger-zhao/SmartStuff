#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <common.h>
#include "../os/os_task.h"
#include "../os/os_ipc.h"

#include "../cmn/omc/omc.h"
#include "../app/test/test.h"
#include "../app/gps/app_gps.h"
#include "../app/hmi/app_hmi.h"
#include "../app/dvr/app_dvr.h"

#include "app_main.h"

#define MSG_SEND_FIFO                                   "msg_send_fifo"
#define MSG_REC_FIFO                                    "msg_rec_fifo"

void* Test_Task(void* args);

void* Sem_Task(void* args);
void* taskFunc(void* args);


// instance of app 
cGPSApp *pcGpsApp = new cGPSApp();
MainLoop *pcMainLoop = new MainLoop();
MsgSend* pcMsgSend = new MsgSend();

// for test 
TestApp *pcTestApp = new TestApp();
// Hmi
HmiApp *pcHmiApp = new HmiApp();
// Dvr
DvrApp *pcDvrApp = new DvrApp();

int msgSendFifo = 0;
pthread_mutex_t counter_lock;

OsTask_pThreadInfo tdinfo[] =
{
    { "TestTask", pcTestApp, taskFunc,   1,      10, 1000},
    { "GPSTask",  pcGpsApp, taskFunc,    1,     8, 1000},
    { "MainLoopTask",  pcMainLoop, taskFunc,    1,     18 , 500},
    { "MessageSendTask",  pcMsgSend, taskFunc,    1,     18 , 500},
    { "HmiTask",  pcHmiApp, taskFunc,    1,     18 , 500},
    { "DvrTask",  pcDvrApp, taskFunc,    1,     18 , 500},
};

#define NUM_THREADS  (sizeof(tdinfo) / sizeof(OsTask_pThreadInfo))

// task array
OsTask cTasks[NUM_THREADS];
OsTask_t thread_id[NUM_THREADS];
OsIpcSem cTestSem("sem1"); 
void* Test_Task(void* args)
{
    while(1)
    {
#if 1
        if(eFail == cTestSem.OS_semGive())
        {
            DBG(DBG_FATAL, "error give sem");
        }
#endif
        DBG(DBG_INFO, "test: give sem ....");
        sleep(4);
        DBG(DBG_INFO, "test: hello mi ....");
    }
}

void* Sem_Task(void* args)
{
    while(1)
    {
        DBG(DBG_INFO, "sem: wait sem ....");
        cTestSem.OS_semTake();
        DBG(DBG_INFO, "sem: take sem ....");
        sleep(1);
    }
}

void* taskFunc(void* args)
{
    U8 u8Idx = *(U8 *)args;
    OsTask *pcTask = &cTasks[u8Idx];
    UINT32 u32Peroid = pcTask->cInfo.tdInfo.u32TaskPeriod;
    // init
    pcTask->cInfo.tdInfo.pcAppInst->init();
    // run
    while(1)
    {
        pcTask->cInfo.tdInfo.pcAppInst->run();
        if(u32Peroid != 0)
        {
            // pcTask->OS_taskDelay(u32Peroid);
        }
    }

    delete pcTask->cInfo.tdInfo.pcAppInst;
    // stop/end
    pcTask->cInfo.tdInfo.pcAppInst->stop();
}

// Ctrl+C
void quitByKeys(INT32 sig)
{

    // release resources by calling stop method 
    OsTask *pcTask = NULL;
	for (UINT32 i = 0; i < NUM_THREADS; i++)
    {
        pcTask = &cTasks[i];
        pcTask->cInfo.tdInfo.pcAppInst->stop();
        // delete
        delete pcTask->cInfo.tdInfo.pcAppInst;
    }
    // print
    DBG(DBG_WARN, "Force to quit by hot-keys!");
    _exit(0);
}

int main()
{
    // capture Ctrl+C
    signal(SIGINT, quitByKeys);
    signal(SIGTERM, quitByKeys);
    // init tasks
    for (UINT32 i = 0; i < NUM_THREADS; i++)
    {
        cTasks[i].cInfo.tdInfo = tdinfo[i];
        cTasks[i].cInfo.u8TaskIdx = i;

    }

    // init IPCs
    // test sem
#if 1
    cTestSem.OS_semCreate();
#endif
#if 1

    int ret = 0;
    int counter = 0;
    char pathFifo[50] = { 0 };

    /****************************create gpsfifo**********************************/
    strcpy(pathFifo, MSG_SEND_FIFO);
    unlink(pathFifo);
    int schd_fifo_status = mkfifo(pathFifo, 0755);
    if (schd_fifo_status < 0)
    {
        DBG(DBG_WARN, "create fifo failed, errormsg=%s errno=%d", strerror(errno), errno);  
        return (1);
    }

    /******************************open fifo************************************/
    msgSendFifo = open(pathFifo, O_RDWR);
    if (msgSendFifo == -1)
    {
        DBG(DBG_WARN, "opend fifo failed\n");
        return (1);
    }

    pthread_mutex_init(&counter_lock, NULL);

    /******************************create tasks************************************/
    for (UINT32 i = 0; i < NUM_THREADS; i++)
    {
        if (cTasks[i].cInfo.tdInfo.u8TaskStart == 1)
        {
            if (eFail == cTasks[i].OS_taskCreate())
            {
                DBG(DBG_FATAL, "create test thread fail!");
                continue;
            }
            else
            {

                DBG(DBG_FATAL, "create thread: %s!",
                        cTasks[i].cInfo.tdInfo.sTaskName.c_str());
            }
        }
    }
#endif

    DBG(DBG_INFO, "SMB first hello!");

    while (1)
    {
        // DBG(DBG_INFO, "SMB in...!");
        printf("time: %d seconds passed in Main\n", (counter * 5));

        sleep(5);
        counter++;
    }

    //pthread_exit(0);

    DBG(DBG_INFO, "SMB bye!");
    return 0;
}

