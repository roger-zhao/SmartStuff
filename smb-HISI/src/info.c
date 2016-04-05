#include "info.h"



// shm for login 
#define FILE_MODE (S_IRUSR | S_IWUSR) 
// const CHAR APCInfo[] = "/dev/shm/APCInfo";
// const CHAR VencInfo[] = "/dev/shm/VencInfo";
// const CHAR LiveInfo[] = "/dev/shm/LiveInfo";
const CHAR APCInfo[] = "/APCInfo";
const CHAR VencInfo[] = "/VencInfo";
const CHAR LiveInfo[] = "/LiveInfo";
UCHAR size = 4 * eAPCInfoSize; // each info: 32bits
UINT32* pShmAPC = NULL;
UINT32* pShmVenc = NULL;
UINT32* pShmLive = NULL;
  
UCHAR createShms()
{
    UCHAR ret = EXIT_SUCCESS;
    INT32 fd, status;   
    struct stat stat;

    int flags = O_RDWR | O_CREAT;  
    // open APC shm
    fd = shm_open(APCInfo, flags, FILE_MODE);  
    if (fd < 0) {  
        printf("APCInfo shm_open failed, errormsg=%s errno=%d", strerror(errno), errno);  
        return EXIT_FAILURE;  
    }  
    ftruncate(fd, size);  
    pShmAPC = (UINT32*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);   
    if(NULL == pShmAPC)
    {
        printf("shm mmap failed, errormsg=%s errno=%d", strerror(errno), errno);  
        return EXIT_FAILURE;  
    }

    close(fd);  
    // open Venc shm
    fd = shm_open(VencInfo, flags, FILE_MODE);  
    if (fd < 0) {  
        printf("VencInfo shm_open failed, errormsg=%s errno=%d", strerror(errno), errno);  
        return EXIT_FAILURE;  
    }  
    ftruncate(fd, size);  
    pShmVenc = (UINT32*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);   
    if(NULL == pShmVenc)
    {
        printf("shm mmap failed, errormsg=%s errno=%d", strerror(errno), errno);  
        return EXIT_FAILURE;  
    }

    close(fd);  
    // open Live shm
    fd = shm_open(LiveInfo, flags, FILE_MODE);  
    if (fd < 0) {  
        printf("LiveInfo shm_open failed, errormsg=%s errno=%d", strerror(errno), errno);  
        return EXIT_FAILURE;  
    }  
    ftruncate(fd, size);  
    pShmLive = (UINT32*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);   
    if(NULL == pShmLive)
    {
        printf("shm mmap failed, errormsg=%s errno=%d", strerror(errno), errno);  
        return EXIT_FAILURE;  
    }
    close(fd);  
    return ret;  

}


UCHAR destroyShms()
{
    UCHAR ret = EXIT_SUCCESS;

    //取消映射
    ret = munmap(pShmAPC, size);
    if(-1 == ret)
    {
        printf("munmap APC shm ptr fail\n");
        return EXIT_FAILURE;  
    }
    //删除内存共享
    shm_unlink(APCInfo);
    if(-1 == ret)
    {
        printf("shm_unlink fail\n");
        return EXIT_FAILURE;  
    }

    //取消映射
    ret = munmap(pShmVenc, size);
    if(-1 == ret)
    {
        printf("munmap venc shm ptr fail\n");
        return EXIT_FAILURE;  
    }
    //删除内存共享
    shm_unlink(VencInfo);
    if(-1 == ret)
    {
        printf("shm_unlink fail\n");
        return EXIT_FAILURE;  
    }

    //取消映射
    ret = munmap(pShmLive, size);
    if(-1 == ret)
    {
        printf("munmap live shm ptr fail\n");
        return EXIT_FAILURE;  
    }
    //删除内存共享
    shm_unlink(LiveInfo);
    if(-1 == ret)
    {
        printf("shm_unlink fail\n");
        return EXIT_FAILURE;  
    }
    return ret;  
}

UCHAR getAPCinfo(U16* pAPCinfo)
{
    UCHAR ret = EXIT_SUCCESS;

    if(NULL == pShmAPC)
    {
        DBG(DBG_ERROR, "APC shm pointer is null");
        return EXIT_FAILURE;
    }
    else
    {
        *pAPCinfo = pShmAPC[ePeopleCntIn];
        *(pAPCinfo++) = pShmAPC[ePeopleCntOut];
    }

    return ret;
}

UCHAR sendCmd2APC(UCHAR cmd, INT32 val)
{
    UCHAR ret = EXIT_SUCCESS;

    if(NULL == pShmAPC)
    {
        DBG(DBG_ERROR, "APC shm pointer is null");
        return EXIT_FAILURE;
    }
    else
    {
        pShmAPC[cmd] = val;
    }

    return ret;

}

