#ifndef APP_DVR_H
#define APP_DVR_H

#include <common.h>
#include "base_class.h"

#include "../../adp/dvr/adp_dvr.h"
#include "../../os/os_timer.h"


#define MODBUS_START_REG_ADDR 0x0000 

// Hisi login when poweron with magic num 0xA5A5, then BBB return login OK/KO with magic num 0xB6B6
#define LOGIN_REQ 0xA5A5
#define LOGIN_RES 0xB6B6

enum DvrModbRegAddr
{
    LOGIN_MODREG_DVR =  (0x0 + MODBUS_START_REG_ADDR),

// hisi send this time stamp per 2s to keep alive, mightbe not used because time-sync is also periodic
    KEEP_ALVIE_MODREG_DVR = (0x3 + MODBUS_START_REG_ADDR),

// follow modbus reg will be sequency 
// time-sync per 2s
// date
    TIME_SYNC_YEAR_MODREG_DVR,
    TIME_SYNC_MON_DAY_MODREG_DVR, // (TIME_SYNC_YEAR_MODREG_DVR + 0x1)

// time
    TIME_SYNC_HOUR_MIN_MODREG_DVR, //  (TIME_SYNC_MON_DAY_MODREG_DVR + 0x1)
// second and millisecond(if neccesary for latter)
    TIME_SYNC_SEC_MS_MODREG_DVR, // (TIME_SYNC_HOUR_MIN_MODREG_DVR + 0x1)

// report info: APC count, CPU info 
    RPT_INFO_APC_CNT_IN_MODREG_DVR, //  (TIME_SYNC_SEC_MS_MODREG_DVR + 0x1)
    RPT_INFO_APC_CNT_OUT_MODREG_DVR, //  (TIME_SYNC_SEC_MS_MODREG_DVR + 0x1)
    RPT_INFO_CPUINFO_LOAD1MIN_MODREG_DVR, //  (RPT_INFO_APC_CNT_MODREG_DVR + 0x1)
    RPT_INFO_CPUINFO_LOAD5MIN_MODREG_DVR, //  (RPT_INFO_CPUINFO_LOAD1MIN_MODREG_DVR + 0x1)
    RPT_INFO_CPUINFO_LOAD15MIN_MODREG_DVR, // (RPT_INFO_CPUINFO_LOAD5MIN_MODREG_DVR + 0x1)
    RPT_INFO_CPUINFO_MEM_MODREG_DVR, //  (RPT_INFO_CPUINFO_LOAD15MIN_MODREG_DVR + 0x1)
    RPT_INFO_CPUINFO_DISK0_MODREG_DVR, // (RPT_INFO_CPUINFO_MEM_MODREG_DVR + 0x1)
    RPT_INFO_CPUINFO_DISK1_MODREG_DVR, // (RPT_INFO_CPUINFO_DISK0_MODREG_DVR + 0x1)
    RPT_INFO_CPUINFO_DISK2_MODREG_DVR, // (RPT_INFO_CPUINFO_DISK1_MODREG_DVR + 0x1)

// management of procs: 
// default processes
// bitmap (16bit): 
//        all APC sameple_venc live
// start
// stop
// restart
// kill
    MANAGE_DEF_PROCS_MODREG_DVR, // (RPT_INFO_CPUINFO_DISK2_MODREG_DVR + 0x1)

// bitmap (16bit): 
    MANAGE_PROCS1_MODREG_DVR, // (MANAGE_DEF_PROCS_MODREG_DVR + 0x1)
// bitmap (16bit): 
    MANAGE_PROCS2_MODREG_DVR, // (MANAGE_PROCS1_MODREG_DVR + 0x1)

// management of procs via cmd: 
// sample_venc, vpss_attr
    CMD_VENC_MODREG_DVR, // (MANAGE_PROCS2_MODREG_DVR + 0x1)
    CMD_VENC_CODEC_MODREG_DVR, // (CMD_VENC_MODREG_DVR + 0x1)
    CMD_VPSS_MODREG_DVR, // (CMD_VENC_CODEC_MODREG_DVR + 0x1)
// APC: max_mod, min_mod: R G B
    CMD_APC_CODEC_MODREG_DVR, // (CMD_VPSS_MODREG_DVR + 0x1)
    CMD_APC_DO_COUNTING_MODREG_DVR, // (CMD_APC_CODEC_MODREG_DVR + 0x1)
    CMD_APC_R_MAXMOD_MODREG_DVR, // (CMD_APC_DO_COUNTING_MODREG_DVR + 0x1)
    CMD_APC_R_MINMOD_MODREG_DVR, // (CMD_APC_R_MAXMOD_MODREG_DVR + 0x1)
    CMD_APC_G_MAXMOD_MODREG_DVR, // (CMD_APC_R_MINMOD_MODREG_DVR + 0x1)
    CMD_APC_G_MINMOD_MODREG_DVR, // (CMD_APC_G_MAXMOD_MODREG_DVR + 0x1)
    CMD_APC_B_MAXMOD_MODREG_DVR, // (CMD_APC_G_MINMOD_MODREG_DVR + 0x1)
    CMD_APC_B_MINMOD_MODREG_DVR, // (CMD_APC_B_MAXMOD_MODREG_DVR + 0x1)

// management of system via cmd: 
// IP: 25
    CMD_SYS_IP1_MODREG_DVR, // (CMD_APC_B_MINMOD_MODREG_DVR + 0x1)
    CMD_SYS_IP2_MODREG_DVR, // (CMD_SYS_IP1_MODREG_DVR + 0x1)
// SSH, NFS, ROUTE, REBOOT board
// ...
//
    MODREG_DVR_END = (CMD_SYS_IP2_MODREG_DVR + 32) // 32 for gap
};

/*
    TIME_SYNC_YEAR_MODREG_DVR,
    TIME_SYNC_MON_DAY_MODREG_DVR, // (TIME_SYNC_YEAR_MODREG_DVR + 0x1)

// time
    TIME_SYNC_HOUR_MIN_MODREG_DVR, //  (TIME_SYNC_MON_DAY_MODREG_DVR + 0x1)
// second and millisecond(if neccesary for latter)
    TIME_SYNC_SEC_MS_MODREG_DVR, // (TIME_SYNC_HOUR_MIN_MODREG_DVR + 0x1)
    */

struct timeSync_monthDay {
    U16 day:5;
    U16 mon:4;
    U16 resv:7;
};

union uts_monthDay
{
    struct timeSync_monthDay md; 
    U16 val;
};

struct timeSync_hourMin {
    U16 min:8;
    U16 hour:5;
    U16 resv:3;
};

union uts_hourMin
{
    struct timeSync_hourMin hm; 
    U16 val;
};

struct timeSync_secMs {
    U16 ms:8;
    U16 sec:8;
};

union uts_secMs
{
    struct timeSync_secMs sm; 
    U16 val;
};

typedef struct {
    U16 year;
    U16 monDay;
    U16 hourMin;
    U16 secMs;
}DVR_dateTime;

class DvrApp: public APP 
{
public:
    DvrAdp *pcAdp;
	DvrApp();
	virtual ~DvrApp();
public:

	void run();
	void init();
	void stop();

    U8 getMemUse() { return this->memUse;};
    void setMemUse(U8 _memUse) { this->memUse = _memUse; };

    U8 getCpuLoad() { return this->cpuLoad;};
    void setCpuLoad(U8 _cpuLoad) { this->cpuLoad = _cpuLoad; };

    U16 getAPCnt() { return this->APCnt;};
    void setAPCnt(U16 _cnt) { this->APCnt = _cnt; };

    S8 getDvrState() { return this->DvrState;};
    void setDvrState(S8 _s) { this->DvrState = _s; };

private:
    S8 DvrState;
    U8 memUse; 
    U8 cpuLoad; 
    U16 APCnt; 
    // TODO: control cmd for Hisi board
};


#endif
