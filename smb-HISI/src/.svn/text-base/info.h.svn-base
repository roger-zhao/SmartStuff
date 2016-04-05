#ifndef INFO_H 
#define INFO_H 
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

#include <sys/ipc.h>  
#include <sys/mman.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <errno.h>

#include "smb_types.h"
#include "debug.h"

// for now, only APC need date-exchange with daemon
enum infoAPC
{
    eWTDAPC = 0,
    ePeopleCntIn,
    ePeopleCntOut,
    eCmdDoCounting,
    eCmdReserve0,
    eCmdReserve1,
    eAPCInfoSize
};
extern UINT32* pShmAPC;

extern UCHAR createShms();

extern UCHAR destroyShms();

extern UCHAR getAPCinfo(U16* pPeopleCnt);

extern UCHAR sendCmd2APC(UCHAR cmd, INT32 val);

// venc
enum infoVenc
{
    eWTDVenc = 0,
};
extern UINT32* pShmVenc;


// live555server 
enum infoLive
{
    eWTDLive = 0,
};
extern UINT32* pShmLive;

#endif
