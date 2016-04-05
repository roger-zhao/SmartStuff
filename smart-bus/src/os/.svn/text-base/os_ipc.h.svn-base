/*
 * os_ipc.h
 *
 *  Created on: 03-29-2015
 *      Author: yanjie 
 */

#ifndef OS_IPC_H
#define OS_IPC_H

#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <sys/types.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include "common.h"
#include "base_class.h"


typedef sem_t OsIpcSem_t;
typedef mode_t OsIpc_mode_t;
typedef struct stat   OsIpc_stat;
typedef UINT32 OsIpc_size_t;
typedef UINT32 OsIpc_offset_t;

// semaphore
class OsIpcSem: public SMBase
{
    public:

        OsIpcSem();
        OsIpcSem(string str) : sName(str) {};
        virtual ~OsIpcSem() { delete sem; };
        eRet OS_semCreate(); // sem_open
        eRet OS_semGive(); // sem_post
        eRet OS_semTake(); // sem_wait
        eRet OS_semGet(INT32 *val); // sem_getvalue
        eRet OS_semDestory(); // sem_close && sem_destroy
    private:
        OsIpcSem_t *sem;
        string sName;
};

// shared memory 

// shm attr
class OsIpcShmAttr: public SMBase
{
    public:
        string sName;
        INT32 iFlag;
        OsIpc_mode_t mode;

        OsIpcShmAttr();
        OsIpcShmAttr(string str, INT32 flg, OsIpc_mode_t mod) : sName(str.insert(0, "/")), iFlag(flg), mode(mod) {};
        virtual ~OsIpcShmAttr() {};


};

class OsIpcShm: public SMBase
{
    public:
	    OsIpcShm();
	    OsIpcShm(OsIpcShmAttr cAttrIn) { this->cAttr = cAttrIn; };
	    virtual ~OsIpcShm() {};
	    void init() {};
	    void run() {};
	    void end() {};

        eRet OS_shmCreate(); // shm_open
        eRet OS_shmSetSize(UINT32 u32Size); // ftrucate
        eRet OS_shmMap(void *addr, OsIpc_size_t len, INT32 prot, INT32 flags, OsIpc_offset_t offset); // mmap
        eRet OS_shmUnmap(void *addr, OsIpc_size_t len); // munmap 
        eRet OS_shmGetStat(OsIpc_stat *ptStat); // fstat
        eRet OS_shmDestroy(); // shm_unlink
    private:
        OsIpcShmAttr cAttr;
        INT32 fd;

};

#endif
