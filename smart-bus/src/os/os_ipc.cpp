/*
 * os_ipc.cpp
 *
 *  Created on: 03-29-2015
 *      Author: yanjie 
 */

#include "os_ipc.h"

// semaphore

eRet OsIpcSem::OS_semCreate() // sem_open
{
    INT32 iFlg = O_CREAT | O_RDWR;
    OsIpc_mode_t mod = S_IRWXU | S_IRWXG | S_IRWXO; // chmod 777
    INT32 iInitVal = 0;

    if(SEM_FAILED == (this->sem = sem_open(this->sName.c_str(), iFlg, mod, iInitVal)))
    {
        PRT("error to create sem: %s\n", this->sName.c_str());
        return eFail;
    }
    return eOK;
}

eRet OsIpcSem::OS_semGive() // sem_post
{

    if(0 != (sem_post(this->sem)))
    {
        PRT("error to give sem: %s\n", this->sName.c_str());
        return eFail;
    }
    return eOK;
}

eRet OsIpcSem::OS_semTake() // sem_wait
{
    if(0 != (sem_wait(this->sem)))
    {
        PRT("error to take sem: %s\n", this->sName.c_str());
        return eFail;
    }
    return eOK;
}

eRet OsIpcSem::OS_semGet(INT32 *val) // sem_getvalue
{
    if(0 != (sem_getvalue(this->sem, val)))
    {
        PRT("error to take sem: %s\n", this->sName.c_str());
        return eFail;
    }
    return eOK;
}

eRet OsIpcSem::OS_semDestory() // sem_close && sem_destroy
{
    // close
    if(0 != (sem_close(this->sem)))
    {
        PRT("error to close sem: %s\n", this->sName.c_str());
        return eFail;
    }

    // destroy 
    if(0 != (sem_unlink(this->sName.c_str())))
    {
        PRT("error to close sem: %s\n", this->sName.c_str());
        return eFail;
    }
    return eOK;
}


// shared memory

eRet OsIpcShm::OS_shmCreate() // shm_open
{

    if(-1 == (this->fd = shm_open(this->cAttr.sName.c_str(), this->cAttr.iFlag, this->cAttr.mode)))
    {
        PRT("error to create shm: %s\n", this->cAttr.sName.c_str());
        return eFail;
    }
    return eOK;
}

eRet OsIpcShm::OS_shmSetSize(UINT32 u32Size) // ftruncate
{
    if(-1 == (ftruncate(this->fd, u32Size)))
    {
        PRT("error to set size of shm: %s\n", this->cAttr.sName.c_str());
        return eFail;
    }
    return eOK;
    
}

eRet OsIpcShm::OS_shmMap(void *addr, OsIpc_size_t len, INT32 prot, INT32 flags, OsIpc_offset_t offset) // mmap
{
    if(MAP_FAILED == (mmap(addr, len, prot, flags, this->fd, offset)))
    {
        PRT("error to map of shm: %s\n", this->cAttr.sName.c_str());
        return eFail;
    }
    return eOK;
}

eRet OsIpcShm::OS_shmUnmap(void *addr, OsIpc_size_t len) // munmap 
{
    if(0 != (munmap(addr, len)))
    {
        PRT("error to unmap of shm: %s\n", this->cAttr.sName.c_str());
        return eFail;
    }
    return eOK;
}

eRet OsIpcShm::OS_shmGetStat(OsIpc_stat *ptStat) // fstat
{
    if(-1 == (fstat(this->fd, ptStat)))
    {
        PRT("error to get state of shm: %s\n", this->cAttr.sName.c_str());
        return eFail;
    }
    return eOK;
}

eRet OsIpcShm::OS_shmDestroy() // shm_unlink
{
    if(0 != (shm_unlink(this->cAttr.sName.c_str())))
    {
        PRT("error to destroy shm: %s\n", this->cAttr.sName.c_str());
        return eFail;
    }
    return eOK;
}
