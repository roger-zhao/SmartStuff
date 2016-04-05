#include "sys.h"

UCHAR statSysInfo(tSysInfo* ptSysInfo)
{
    UCHAR ret = EXIT_SUCCESS;

    DBG(DBG_INFO, "%s in", __FUNCTION__);

    return ret;
}

UCHAR checkWTD()
{
    UCHAR ret = EXIT_SUCCESS;

    DBG(DBG_INFO, "%s in", __FUNCTION__);

    return ret;
}


//设置IP地址
/*
 * 函数名称 ： int setip(char *ip)
 * 函数功能 ： 设置系统IP地址
 * 参    数 ： 
 *            char *ip ：设置的IP地址，以点分十进制的字符串方式表示，如“192.168.0.5” 
 * 返 回 值 ： 0 : 成功 ；  -1 :  失败 
 */
 
UCHAR setLocalIP(char *ip)
{
    struct ifreq temp;
    struct sockaddr_in *addr;
    int fd = 0;
    UCHAR ret = EXIT_SUCCESS;
    strcpy(temp.ifr_name, ETH_NAME);
    if((fd=socket(AF_INET, SOCK_STREAM, 0))<0)
    {
        return EXIT_FAILURE;
    }
    addr = (struct sockaddr_in *)&(temp.ifr_addr);
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(ip);
    ret = ioctl(fd, SIOCSIFADDR, &temp);
    close(fd);
    if(ret < 0)
        return EXIT_FAILURE;
    return ret;
}

UCHAR getLocalIp(CHAR* ipAddr) 
{        
    int sock;
    struct sockaddr_in sin;
    struct ifreq ifr;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
    {
        DBG(DBG_ERROR, "sock err");
        return -1;                
    }
    
    strncpy(ifr.ifr_name, ETH_NAME, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    
    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
    {
        DBG(DBG_ERROR, "ioctl err");
        return -1;
    }
    
    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
    sprintf(ipAddr, "%s\n", inet_ntoa(sin.sin_addr));
    return 0;

}

UCHAR getMemUse(U16* memUse) 
{
    FILE *fd;          
    CHAR err = 0;
    CHAR *buff;   
    UINT32 len;   
    CHAR memInfoNum = 0;   
    unsigned long memTotal = 0;   
    unsigned long memFree = 0;   

    fd = fopen ("/proc/meminfo", "r"); 
   //// printf("%s 0\n", __FUNCTION__);
    if(NULL == fd)
    {
        DBG(DBG_ERROR, " err: open meminfo");
        return EXIT_FAILURE;
    }
   //// printf("%s 1\n", __FUNCTION__);

    // get first line: total
    while((2 > memInfoNum) && (-1 != (err = getline(&buff, &len, fd))))
    {
        // printf("line: %s\n", buff);
        if(strstr(buff, "MemTotal:"))
        {
            if(-1 == sscanf(buff, "%*s %lu\n", &memTotal)) // &(m->total))) 
            {
                DBG(DBG_ERROR, " err: get total meminfo");
                fclose(fd);     //关闭文件fd
                return EXIT_FAILURE;
            }
            // printf("line: <%lu>\n", memTotal);
            memInfoNum++;
        }
        else if(strstr(buff, "MemFree:"))
        {
            if(-1 == sscanf(buff, "%*s %lu", &memFree)) 
            {
                DBG(DBG_ERROR, " err: get free meminfo");
                fclose(fd);     //关闭文件fd
                return EXIT_FAILURE;
            }
            // printf("line: <%lu>\n", memFree);
            memInfoNum++;
        }

    }
      
    if(2 == memInfoNum)
    {
        // scale with 100
        if((NULL == memUse))
        {
            DBG(DBG_ERROR, " err: mem ptr null");
            fclose(fd);     //关闭文件fd
            return EXIT_FAILURE;
        }
        *memUse = (memTotal - memFree)*100/memTotal;
        
       //// printf("%s 6: usage: %hd\n", __FUNCTION__, *memUse);
        fclose(fd);     //关闭文件fd
    }
    else
    {
        DBG(DBG_ERROR, " err: get meminfo");
        fclose(fd);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}

UCHAR getCPUUse(U16* load)
{
    char  buffer[256];
    FILE* fd;
    float load1Min;
    float load5Min;
    float load15Min;

    /*  get load averages  */ 
    fd  =  fopen("/proc/loadavg" , "r");
    if(NULL == fd)
    {
        DBG(DBG_ERROR, " err: open loadavg");
       //// printf("%s 1\n", __FUNCTION__);
        return EXIT_FAILURE;
    }


    if((NULL == load) || (NULL == load+1) || (NULL == load+2))
    {
        DBG(DBG_ERROR, " err: load ptr null");
        fclose(fd);
        return EXIT_FAILURE;
    }

    fgets (buffer, sizeof(buffer), fd); 
    fclose(fd);

    if(-1 == sscanf (buffer, "%f %f %f", &load1Min, &load5Min, &load15Min))
    {
        DBG(DBG_ERROR, " err: get CPU load");
        return EXIT_FAILURE;
    }

   //// printf("%s 5: %f|%f|%f \n", __FUNCTION__, load1Min, load5Min, load15Min);

    *(load++) = (U16)(load1Min * 100);
    *(load++) = (U16)(load5Min * 100);
    *(load) = (U16)(load15Min * 100);
    return EXIT_SUCCESS;
}

UCHAR getDiskUse(U16* disk)
{
    char buf[256],*ptr; 
    FILE *file; 
    file=fopen("/etc/fstab","r"); 
    if(NULL == file)
    {
        DBG(DBG_ERROR, " err: open fstab");
        return EXIT_FAILURE;
    }
    if((NULL == disk) || (NULL == disk+1) || (NULL == disk+2))
    {
        DBG(DBG_ERROR, " err: mem ptr null");
        fclose(file); 
        return EXIT_FAILURE;
    }
    memset(buf,0,sizeof(buf)); 
    while(fgets(buf,sizeof(buf),file)) 
    { 
        ptr=strtok(buf," "); 
        if(ptr&&((strncmp(ptr,"/dev",4)==0))) 
        { 
            ptr=strtok(NULL," "); 
            *(disk++) = gethd(ptr); 
        } 
    } 

    fclose(file); 

    return EXIT_SUCCESS;

}

U16 gethd(char *path) 
{ 
    struct statvfs stat1; 
    statvfs(path,&stat1); 
    if(stat1.f_flag) 
    {
       // printf("%s: usage: %f\n", path, ((((float)stat1.f_blocks-(float)stat1.f_bfree)/(float)stat1.f_blocks*100)));
        return ((U16)(((float)stat1.f_blocks-(float)stat1.f_bfree)/(float)stat1.f_blocks*100));
    }
}
