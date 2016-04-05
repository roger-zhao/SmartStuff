#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "smb_types.h"
#include "debug.h"

#define MSG_GPS_ID                                     0x00000001
//#define MSG_SYS_ID                                   0x00000002

typedef struct
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} date_time;

typedef struct
{
    date_time D;//时间
    char status; //接收状态
    double latitude; //纬度
    double longitude; //经度
    char NS; //南北极
    char EW; //东西
    double speed; //速度
    double high; //高度
    char seq[5];
} GPS_INFO;

typedef struct
{
    unsigned int msgid; //identify what it is
    char message[512];
    unsigned int size;
} MessageInfo;



#endif
