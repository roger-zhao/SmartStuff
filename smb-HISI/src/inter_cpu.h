#ifndef INTER_CPU_H 
#define INTER_CPU_H 
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
#include <pthread.h>
#include <linux/tcp.h>
#include <sys/types.h> 
#include <sys/wait.h>

#include "smb_types.h"
#include "debug.h"
#include "modbus.h"

#include "info.h"
#include "sys.h"

#define MAX_MODBUS_CLIENT 128 

#define MAX_MODBUS_REGS_NUM 32 
#define MODBUS_START_REG_ADDR 0x0000 

// Hisi login when poweron with magic num 0xA5A5, then BBB return login OK/KO with magic num 0xB6B6
#define LOGIN_REQ 0xA5A5
#define LOGIN_RES 0xB6B6
#if 0
#define LOGIN_MODREG (0x0 + MODBUS_START_REG_ADDR)

// hisi send this time stamp per 2s to keep alive, mightbe not used because time-sync is also periodic
#define KEEP_ALVIE_MODREG (0x3 + MODBUS_START_REG_ADDR)

// follow modbus reg will be sequency 
// time-sync per 2s
// date
#define TIME_SYNC_YEAR_MODREG (KEEP_ALVIE_MODREG + 0x1)
#define TIME_SYNC_MON_DAY_MODREG (TIME_SYNC_YEAR_MODREG + 0x1)

// time
#define TIME_SYNC_HOUR_MIN_MODREG (TIME_SYNC_MON_DAY_MODREG + 0x1)
// second and millisecond(if neccesary for latter)
#define TIME_SYNC_SEC_MS_MODREG (TIME_SYNC_HOUR_MIN_MODREG + 0x1)

// report info: APC count, CPU info 
#define RPT_INFO_APC_CNT_MODREG (TIME_SYNC_SEC_MS_MODREG + 0x1)
#define RPT_INFO_CPUINFO_LOAD1MIN_MODREG (RPT_INFO_APC_CNT_MODREG + 0x1)
#define RPT_INFO_CPUINFO_LOAD5MIN_MODREG (RPT_INFO_CPUINFO_LOAD1MIN_MODREG + 0x1)
#define RPT_INFO_CPUINFO_LOAD15MIN_MODREG (RPT_INFO_CPUINFO_LOAD5MIN_MODREG + 0x1)
#define RPT_INFO_CPUINFO_MEM_MODREG (RPT_INFO_CPUINFO_LOAD15MIN_MODREG + 0x1)
#define RPT_INFO_CPUINFO_DISK0_MODREG (RPT_INFO_CPUINFO_MEM_MODREG + 0x1)
#define RPT_INFO_CPUINFO_DISK1_MODREG (RPT_INFO_CPUINFO_DISK0_MODREG + 0x1)
#define RPT_INFO_CPUINFO_DISK2_MODREG (RPT_INFO_CPUINFO_DISK1_MODREG + 0x1)

// management of procs: 
// default processes
// bitmap (16bit): 
//        all APC sameple_venc live
// start
// stop
// restart
// kill
#define MANAGE_DEF_PROCS_MODREG (RPT_INFO_CPUINFO_DISK2_MODREG + 0x1)

// bitmap (16bit): 
#define MANAGE_PROCS1_MODREG (MANAGE_DEF_PROCS_MODREG + 0x1)
// bitmap (16bit): 
#define MANAGE_PROCS2_MODREG (MANAGE_PROCS1_MODREG + 0x1)

// management of procs via cmd: 
// sample_venc, vpss_attr
#define CMD_VENC_MODREG (MANAGE_PROCS2_MODREG + 0x1)
#define CMD_VENC_CODEC_MODREG (CMD_VENC_MODREG + 0x1)
#define CMD_VPSS_MODREG (CMD_VENC_CODEC_MODREG + 0x1)
// APC: max_mod, min_mod: R G B
#define CMD_APC_CODEC_MODREG (CMD_VPSS_MODREG + 0x1)
#define CMD_APC_DO_COUNTING_MODREG (CMD_APC_CODEC_MODREG + 0x1)
#define CMD_APC_R_MAXMOD_MODREG (CMD_APC_DO_COUNTING_MODREG + 0x1)
#define CMD_APC_R_MINMOD_MODREG (CMD_APC_R_MAXMOD_MODREG + 0x1)
#define CMD_APC_G_MAXMOD_MODREG (CMD_APC_R_MINMOD_MODREG + 0x1)
#define CMD_APC_G_MINMOD_MODREG (CMD_APC_G_MAXMOD_MODREG + 0x1)
#define CMD_APC_B_MAXMOD_MODREG (CMD_APC_G_MINMOD_MODREG + 0x1)
#define CMD_APC_B_MINMOD_MODREG (CMD_APC_B_MAXMOD_MODREG + 0x1)

// management of system via cmd: 
// IP: 25
#define CMD_SYS_IP1_MODREG (CMD_APC_B_MINMOD_MODREG + 0x1)
#define CMD_SYS_IP2_MODREG (CMD_SYS_IP1_MODREG + 0x1)
// SSH, NFS, ROUTE, REBOOT board
// ...
//
#define MODREG_START LOGIN_MODREG
#define MODREG_END (CMD_SYS_IP2_MODREG + 32) // 32 for gap
#endif
enum modbRegAddr
{
    LOGIN_MODREG =  (0x0 + MODBUS_START_REG_ADDR),

// hisi send this time stamp per 2s to keep alive, mightbe not used because time-sync is also periodic
    KEEP_ALVIE_MODREG = (0x3 + MODBUS_START_REG_ADDR),

// follow modbus reg will be sequency 
// time-sync per 2s
// date
    TIME_SYNC_YEAR_MODREG,
    TIME_SYNC_MON_DAY_MODREG, // (TIME_SYNC_YEAR_MODREG + 0x1)

// time
    TIME_SYNC_HOUR_MIN_MODREG, //  (TIME_SYNC_MON_DAY_MODREG + 0x1)
// second and millisecond(if neccesary for latter)
    TIME_SYNC_SEC_MS_MODREG, // (TIME_SYNC_HOUR_MIN_MODREG + 0x1)

// report info: APC count, CPU info 
    RPT_INFO_APC_CNT_IN_MODREG, //  (TIME_SYNC_SEC_MS_MODREG + 0x1)
    RPT_INFO_APC_CNT_MODREG, //  (TIME_SYNC_SEC_MS_MODREG + 0x1)
    RPT_INFO_CPUINFO_LOAD1MIN_MODREG, //  (RPT_INFO_APC_CNT_MODREG + 0x1)
    RPT_INFO_CPUINFO_LOAD5MIN_MODREG, //  (RPT_INFO_CPUINFO_LOAD1MIN_MODREG + 0x1)
    RPT_INFO_CPUINFO_LOAD15MIN_MODREG, // (RPT_INFO_CPUINFO_LOAD5MIN_MODREG + 0x1)
    RPT_INFO_CPUINFO_MEM_MODREG, //  (RPT_INFO_CPUINFO_LOAD15MIN_MODREG + 0x1)
    RPT_INFO_CPUINFO_DISK0_MODREG, // (RPT_INFO_CPUINFO_MEM_MODREG + 0x1)
    RPT_INFO_CPUINFO_DISK1_MODREG, // (RPT_INFO_CPUINFO_DISK0_MODREG + 0x1)
    RPT_INFO_CPUINFO_DISK2_MODREG, // (RPT_INFO_CPUINFO_DISK1_MODREG + 0x1)

// management of procs: 
// default processes
// bitmap (16bit): 
//        all APC sameple_venc live
// start
// stop
// restart
// kill
    MANAGE_DEF_PROCS_MODREG, // (RPT_INFO_CPUINFO_DISK2_MODREG + 0x1)

// bitmap (16bit): 
    MANAGE_PROCS1_MODREG, // (MANAGE_DEF_PROCS_MODREG + 0x1)
// bitmap (16bit): 
    MANAGE_PROCS2_MODREG, // (MANAGE_PROCS1_MODREG + 0x1)

// management of procs via cmd: 
// sample_venc, vpss_attr
    CMD_VENC_MODREG, // (MANAGE_PROCS2_MODREG + 0x1)
    CMD_VENC_CODEC_MODREG, // (CMD_VENC_MODREG + 0x1)
    CMD_VPSS_MODREG, // (CMD_VENC_CODEC_MODREG + 0x1)
// APC: max_mod, min_mod: R G B
    CMD_APC_CODEC_MODREG, // (CMD_VPSS_MODREG + 0x1)
    CMD_APC_DO_COUNTING_MODREG, // (CMD_APC_CODEC_MODREG + 0x1)
    CMD_APC_R_MAXMOD_MODREG, // (CMD_APC_DO_COUNTING_MODREG + 0x1)
    CMD_APC_R_MINMOD_MODREG, // (CMD_APC_R_MAXMOD_MODREG + 0x1)
    CMD_APC_G_MAXMOD_MODREG, // (CMD_APC_R_MINMOD_MODREG + 0x1)
    CMD_APC_G_MINMOD_MODREG, // (CMD_APC_G_MAXMOD_MODREG + 0x1)
    CMD_APC_B_MAXMOD_MODREG, // (CMD_APC_G_MINMOD_MODREG + 0x1)
    CMD_APC_B_MINMOD_MODREG, // (CMD_APC_B_MAXMOD_MODREG + 0x1)

// management of system via cmd: 
// IP: 25
    CMD_SYS_IP1_MODREG, // (CMD_APC_B_MINMOD_MODREG + 0x1)
    CMD_SYS_IP2_MODREG, // (CMD_SYS_IP1_MODREG + 0x1)
// SSH, NFS, ROUTE, REBOOT board
// ...
//
    MODREG_END = (CMD_SYS_IP2_MODREG + 32) // 32 for gap
};

// structure of inter CPU info
enum eWrRdFlag
{
    eNA = 0x0,
    eWrite = 0x55,
    eRead = 0x66
};
typedef struct 
{
    U16 regAddr;
    U16 regVal;
    enum eWrRdFlag eFlag;
}tRegVal;

typedef struct 
{
    ULL ullRxCnt;
    ULL ullTxCnt;

    // Rx
    tRegVal rx[MAX_MODBUS_REGS_NUM];
    // Tx
    tRegVal tx[MAX_MODBUS_REGS_NUM];
}tInterCPUInfo;
//
extern modbus_t *ctx;

extern modbus_mapping_t *mb_mapping;

#define LOGINOK 0xFEABCDEF

extern UINT32 g_loginOK;

extern UCHAR* loginPtrModbus;

extern UCHAR* loginPtrApp;

extern UCHAR serverAlive;

extern tInterCPUInfo interCPUInfo;

extern UCHAR startInterCPUComm();

extern UCHAR refreshInfoToBBB();


extern void stopModbusServer();


#endif
