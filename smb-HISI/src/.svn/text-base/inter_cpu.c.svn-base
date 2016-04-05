#include "inter_cpu.h"

#define LIBMODBUS_WITH_TIMEOUT // with timeout when wait a indication for select in receive

modbus_t *ctx;

modbus_mapping_t *mb_mapping;

U8 serverAlive = 1;
U8 clientConnect = 0;
    int tmpPid = -1;
    int sonPid = -1;
    int status = 0;

INT32 s = -1;

// shm for login 
#define FILE_MODE (S_IRUSR | S_IWUSR) 
UINT32 g_loginOK = LOGINOK;
UCHAR* loginPtrModbus = NULL;
UCHAR* loginPtrApp = NULL;

void interCPUComm();
void keepAlive();

void sigHandler()
{
    DBG(DBG_ERROR,"get sigpipe!");
}

pthread_t tidInterCPUComm;
pthread_t tidKeepAlive;

UCHAR startInterCPUComm()
{
    UCHAR ret = EXIT_SUCCESS;
    pthread_t id;

	signal(SIGPIPE, sigHandler);


    ret=pthread_create(&tidInterCPUComm,NULL,(void *)interCPUComm,NULL);
    if(ret!=0){
        DBG(DBG_ERROR,"Create pthread error!");
        ret = EXIT_FAILURE;
    }

#ifndef LIBMODBUS_WITH_TIMEOUT 
    ret=pthread_create(&tidKeepAlive,NULL,(void *)keepAlive,NULL);
    if(ret!=0){
        DBG(DBG_ERROR,"Create pthread error!");
        ret = EXIT_FAILURE;
    }
#endif

    return ret;
}

#define BBBDEAD 10 

#ifndef LIBMODBUS_WITH_TIMEOUT 
void keepAlive()
{
    U8 retry = 0;
    int ret = 0;
    char szRecvBuf[10] = {0};

    struct tcp_info info; 
    int len=sizeof(info); 
    int oldtype;

    while(1)
    {
    getsockopt(modbus_get_socket(ctx), IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len); 
    if((info.tcpi_state==1)) 
    { 
    }
    else
    {
        // DBG(DBG_WARN, "keepAlive: info tcp state not established!");
    }
        if(serverAlive)
        {
            if(clientConnect)
            {
                // test
                if(-1 != s) // just for make sure
                {
                    while((ret = recv(modbus_get_socket(ctx), szRecvBuf, 1, MSG_PEEK)) <= 0)
                    {
                        DBG(DBG_WARN, "keepAlive: recv error, sleep 5s then retry(%d)...", retry);
                        if(retry++ >= BBBDEAD)
                        {
                            DBG(DBG_WARN, "keepAlive: seems that BBB is dead now, close server socket...");
                            retry = 0;
                            usleep(500*1000);
                            close(s);
                            modbus_close(ctx);
                            // reOpenServer();
                            pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&oldtype);
                            usleep(10*1000);
                            pthread_cancel(tidInterCPUComm);
                            usleep(50*1000);
                            clientConnect = 0;
                            ret=pthread_create(&tidInterCPUComm,NULL,(void *)interCPUComm,NULL);
                            if(ret!=0){
                                DBG(DBG_ERROR,"Create pthread error!");
                                ret = EXIT_FAILURE;
                            }
                        }
                        usleep(1000*1000);
                    }
                }
                

            }
        }

        // wait
        // DBG(DBG_WARN, "keep alive: wait 1s...");
        usleep(1000*1000);
    }

}
#endif

#ifdef LIBMODBUS_WITH_TIMEOUT 
void interCPUComm()
{
    INT32 rc;
    INT32 addr;
    U16 val = 0;
    INT32 i = 0;
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    tRegVal recv[MAX_MODBUS_REGS_NUM];
    struct timeval timeout;
    int cnt = 0;


   // printf("logintoBBB in\n");

    // modbus mapping
    mb_mapping = modbus_mapping_new(0, 0, MODREG_END, 0);
    if(mb_mapping == NULL) 
    {
        DBG(DBG_ERROR, "Failed to allocate the mapping: %s", modbus_strerror(errno));
        modbus_free(ctx);
        return EXIT_FAILURE;
    }
  

    // req
    mb_mapping->tab_registers[LOGIN_MODREG] = LOGIN_REQ;
    modbus_set_debug(ctx, FALSE);
    
    while(serverAlive)
    {

        if(!clientConnect)
        {

            s = modbus_tcp_listen(ctx, MAX_MODBUS_CLIENT);
            if(-1 == s)
            {
                DBG(DBG_ERROR, "error: bad sockFD(%d): %s!", s, strerror(errno));
                continue;
            }

            // wait for connection
            if(-1 == modbus_tcp_accept(ctx, &s))
            {
                DBG(DBG_ERROR, "error modbus server accept: %s!", strerror(errno));
                usleep(500*1000);
                continue;
            }
            else
            {
                clientConnect = 1;
                mb_mapping->tab_registers[LOGIN_MODREG] = LOGIN_REQ;
                printf("modbus server accept connection!\n");
                DBG(DBG_INFO, "modbus server accept connection!");
            }
        }

  
        while(clientConnect)
        {

            rc = modbus_receive(ctx, query);
            
            if (rc > 0) 
            {
               // printf("receive msg\n");
                /* LOGIN_MODREG */
                if(!(g_loginOK))
                {
                    if(LOGIN_RES != mb_mapping->tab_registers[LOGIN_MODREG])
                    {
                        DBG(DBG_WARN, " login res err!");
                    }
                    else
                    {
                        g_loginOK = LOGINOK;
                        DBG(DBG_INFO, " login done!");
                    }
                }
                else // handle modbus msg
                {
                    // refresh reg RD
                    if(EXIT_FAILURE == refreshInfoToBBB())
                    {
                        DBG(DBG_ERROR, " refresh info err!");
                    }

                    // parse current modbus msg and update DB
                    // for now, just use multi-reg wr 
                    INT32 headerLen = modbus_get_header_length(ctx);
                    U16 regAddr = 0;
                    U16 regNum = 0;
#define REG_ADDR_OFFSET 1
#define REG_NUM_OFFSET 3
                    if (query[headerLen] == MODBUS_FC_WRITE_MULTIPLE_REGISTERS)
                    {

                        regAddr = MODBUS_GET_INT16_FROM_INT8(query, headerLen + REG_ADDR_OFFSET);
                        regNum = MODBUS_GET_INT16_FROM_INT8(query, headerLen + REG_NUM_OFFSET);
                        U16 rr = 0;
                        for(rr = 0; rr < regNum; rr++)
                        {
                            interCPUInfo.rx[rr].regAddr = regAddr + rr;
                            interCPUInfo.rx[rr].regVal = mb_mapping->tab_registers[regAddr + rr];
                            interCPUInfo.rx[rr].eFlag = eWrite;
                            interCPUInfo.ullRxCnt = regNum;
                            // printf("recv: reg[%d]-value[0x%x]\n", interCPUInfo.rx[rr].regAddr, interCPUInfo.rx[rr].regVal);
                        }
                    }

                }

                /* rc is the query size */
                modbus_reply(ctx, query, rc, mb_mapping);

            } else if (rc == -1) {
              /* Connection closed by the client or error */
                DBG(DBG_WARN, "conn closed: %s", modbus_strerror(errno));
                clientConnect = 0;
                break;
            }
        }
  
        // quit loop
        if (s != -1)
        {
          close(s);
        }
        
        modbus_close(ctx);
    } // while(serverAlive)
}
#else
void interCPUComm()
{
    INT32 rc;
    INT32 addr;
    U16 val = 0;
    INT32 i = 0;
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    tRegVal recv[MAX_MODBUS_REGS_NUM];
    struct timeval timeout;
    int cnt = 0;


   // printf("logintoBBB in\n");

    // modbus mapping
    mb_mapping = modbus_mapping_new(0, 0, MODREG_END, 0);
    if(mb_mapping == NULL) 
    {
        DBG(DBG_ERROR, "Failed to allocate the mapping: %s", modbus_strerror(errno));
        modbus_free(ctx);
        return EXIT_FAILURE;
    }
  

    // req
    mb_mapping->tab_registers[LOGIN_MODREG] = LOGIN_REQ;

    
    while(serverAlive)
    {

        if(!clientConnect)
        {

            // wait for connection
            s = modbus_tcp_listen(ctx, MAX_MODBUS_CLIENT);
            if(-1 == s)
            {
                DBG(DBG_ERROR, "while bad sockFD(%d): %s!", s, strerror(errno));
            }
            if(-1 == modbus_tcp_accept(ctx, &s))
            {
                mb_mapping->tab_registers[LOGIN_MODREG] = LOGIN_REQ;
                DBG(DBG_ERROR, "Error modbus server accept: %s!", strerror(errno));
                usleep(500*1000);
                continue;
            }
            else
            {
                clientConnect = 1;
                printf("modbus server accept connection!\n");
                DBG(DBG_INFO, "modbus server accept connection!");
            }
        }

    // set socket timeout for keepalive thread
        timeout.tv_sec=5;    
        timeout.tv_usec=0;
        int result = setsockopt(modbus_get_socket(ctx), SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout.tv_sec,sizeof(struct timeval));
        if (result < 0)
        {
            DBG(DBG_ERROR, "setsockopt error!");
        }
        else
        {
            DBG(DBG_ERROR, "setsockopt ok!");
        }
        // debug
        // modbus_set_debug(ctx, TRUE);

  
    while(clientConnect)
    {

        rc = modbus_receive(ctx, query);
        
        if (rc > 0) 
        {
           // printf("receive msg\n");
            /* LOGIN_MODREG */
            if(!(g_loginOK))
            {
                if(LOGIN_RES != mb_mapping->tab_registers[LOGIN_MODREG])
                {
                    DBG(DBG_WARN, " login res err!");
                }
                else
                {
                    g_loginOK = LOGINOK;
                    DBG(DBG_INFO, " login done!");
                }
            }
            else // handle modbus msg
            {
                // refresh reg RD
                if(EXIT_FAILURE == refreshInfoToBBB())
                {
                    DBG(DBG_ERROR, " refresh info err!");
                }

                // parse current modbus msg and update DB
                // for now, just use multi-reg wr 
                INT32 headerLen = modbus_get_header_length(ctx);
                U16 regAddr = 0;
                U16 regNum = 0;
#define REG_ADDR_OFFSET 1
#define REG_NUM_OFFSET 3
                if (query[headerLen] == MODBUS_FC_WRITE_MULTIPLE_REGISTERS)
                {

                    regAddr = MODBUS_GET_INT16_FROM_INT8(query, headerLen + REG_ADDR_OFFSET);
                    regNum = MODBUS_GET_INT16_FROM_INT8(query, headerLen + REG_NUM_OFFSET);
                    U16 rr = 0;
                    for(rr = 0; rr < regNum; rr++)
                    {
                        interCPUInfo.rx[rr].regAddr = regAddr + rr;
                        interCPUInfo.rx[rr].regVal = mb_mapping->tab_registers[regAddr + rr];
                        interCPUInfo.rx[rr].eFlag = eWrite;
                        interCPUInfo.ullRxCnt = regNum;
                        // printf("recv: reg[%d]-value[0x%x]\n", interCPUInfo.rx[rr].regAddr, interCPUInfo.rx[rr].regVal);
                    }
                }

            }

            /* rc is the query size */
            modbus_reply(ctx, query, rc, mb_mapping);

        } else if (rc == -1) {
          /* Connection closed by the client or error */
            DBG(DBG_WARN, "conn closed: %s", modbus_strerror(errno));
            clientConnect = 0;
            break;
        }
    }
  
   // printf("serverAlive = %d, quit modbus serverclosed\n", serverAlive);
    // quit loop
    if (s != -1)
    {
      close(s);
    }
    
    modbus_close(ctx);
    }
}

#endif

// real-time info
#define TIME_SEC_DIFF(x, y) ((x > y)?(x - y):(x - y + 60))// seconds
#define RPT_SYS_INFO_PERIOD 10 // seconds
UCHAR refreshInfoToBBB()
{
    UCHAR ret = EXIT_SUCCESS;
    struct timeval tv;
    static UINT32 ts = 0xdeadbeef;
    static UINT32 interval = 0;

    gettimeofday(&tv, NULL); 

    // first time
    if(0xdeadbeef == ts)
    {
        ts = tv.tv_sec;
        interval = 0;
    }
    else
    {
        interval += TIME_SEC_DIFF(tv.tv_sec, ts);
        ts = tv.tv_sec;
    }


    DBG(DBG_INFO, "%s in", __FUNCTION__);

    // people cnt
    if(EXIT_FAILURE == getAPCinfo(&mb_mapping->tab_registers[RPT_INFO_APC_CNT_IN_MODREG]))
    {
        DBG(DBG_ERROR, " get APC info err!");
        return EXIT_FAILURE;
    }

    // printf("%s interval: %d\n", __FUNCTION__, interval);
    // sys info
    if(interval > RPT_SYS_INFO_PERIOD)
    {
        // CPU load/usage
        if(EXIT_FAILURE == getCPUUse(&mb_mapping->tab_registers[RPT_INFO_CPUINFO_LOAD1MIN_MODREG]))
        {
            DBG(DBG_ERROR, " get APC info err!");
            return EXIT_FAILURE;
        }

        // printf("cpu use 0\n");
        // printf("cpu use1min: %d\n", mb_mapping->tab_registers[RPT_INFO_CPUINFO_LOAD1MIN_MODREG]);
        // printf("cpu use5min: %d\n", mb_mapping->tab_registers[RPT_INFO_CPUINFO_LOAD5MIN_MODREG]);
        // printf("cpu use15min: %d\n", mb_mapping->tab_registers[RPT_INFO_CPUINFO_LOAD15MIN_MODREG]);
        // printf("cpu use 1\n");

        // CPU mem 
        if(EXIT_FAILURE == getMemUse(&mb_mapping->tab_registers[RPT_INFO_CPUINFO_MEM_MODREG]))
        {
            DBG(DBG_ERROR, " get Mem usage err!");
            return EXIT_FAILURE;
        }

        // printf("mem use: %d\n", mb_mapping->tab_registers[RPT_INFO_CPUINFO_MEM_MODREG]);

        // CPU disk 
        if(EXIT_FAILURE == getDiskUse(&mb_mapping->tab_registers[RPT_INFO_CPUINFO_DISK0_MODREG]))
        {
            DBG(DBG_ERROR, " get Disk usage err!");
            // return EXIT_FAILURE;
        }

        // printf("disk use0: %d\n", mb_mapping->tab_registers[RPT_INFO_CPUINFO_DISK0_MODREG]);
        // printf("disk use1: %d\n", mb_mapping->tab_registers[RPT_INFO_CPUINFO_DISK1_MODREG]);
        // printf("disk use2: %d\n", mb_mapping->tab_registers[RPT_INFO_CPUINFO_DISK2_MODREG]);

        // back to initial state
        ts = 0xdeadbeef;

    }
    return ret;
}

void stopModbusServer(void)
{
    serverAlive = 0;

    if (s != -1)
    {
      close(s);
    }
    
    modbus_mapping_free(mb_mapping);
    modbus_close(ctx);
    modbus_free(ctx);
}

