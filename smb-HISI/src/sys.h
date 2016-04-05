#ifndef SYS_H 
#define SYS_H 
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
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/vfs.h> 
#include <sys/statvfs.h> 


#include "smb_types.h"
#include "debug.h"

#define ETH_NAME        "eth0"

// structure of system info
typedef struct
{
    UCHAR ucCPULoad;
    UCHAR ucCPUMemUse;
    UCHAR ucCPUDiskUse;
    // ... 
}tSysInfo;
//

typedef struct //定义一个mem occupy的结构体
{
unsigned long total; 
unsigned long free;                       
}MEM_OCCUPY;

extern UCHAR statSysInfo(tSysInfo* ptSysInfo);

extern UCHAR checkWTD();

extern UCHAR getLocalIP(CHAR* ipAddr);

extern UCHAR setLocalIP(CHAR* ipAddr);

extern UCHAR getMemUse(U16* memUse);

extern UCHAR getCPUUse(U16* load);

extern UCHAR getDiskUse(U16* disk);

extern U16 gethd(char *path); 
#endif
