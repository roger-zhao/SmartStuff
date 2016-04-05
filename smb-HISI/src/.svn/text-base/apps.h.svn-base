#ifndef APPS_H
#define APPS_H
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "smb_types.h"
#include "debug.h"

#define MAX_ARGC_PROC 32
#define MAX_PROC_NAME_LEN 64 
#define MAX_PROC_NUM 32 

// structure of procs' info
//

enum confPattern
{
    eHead = 0,
    eProcInfo
};

enum manageProc 
{
    eNoManage = 0,
    eStop,
    eStart,
    eRestart,
    eKill,
};


typedef struct
{
    UCHAR ucProcIdx;
    INT32 procPID;
    enum manageProc eManageFlag;
    CHAR cProcCmd[MAX_PROC_NAME_LEN];
    UCHAR ucProcArgc;
    INT32 iProcArgv[MAX_ARGC_PROC];
}tProc;
typedef struct
{
    UCHAR ucProcNum;
    tProc proc[MAX_PROC_NUM];
}tProcInfo;


extern int parse_conf_file(CHAR* conFileName, tProcInfo* ptProcInfo);


/**
 * \brief Read configuration from config file
 */
extern int read_conf_file(tProcInfo* ptProcInfo);

extern UCHAR startProcs(tProcInfo* ptProcInfo);


#endif 
