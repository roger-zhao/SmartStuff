#include "adp_dvr.h"


// connect server
eRet DvrAdp::dataClientInit()
{
    eRet ret = eOK;
    DBG(DBG_INFO, "enter dataClientInit!");
    int retry = 0;

    /* TCP */
    // for re-connect, malloc no-neccesary
    if(!this->ctx)
    {
        this->ctx = modbus_new_tcp(HISI_SERVER_IP, MODBUS_TCP_PORT);
        modbus_set_debug(this->ctx, FALSE);
        modbus_set_error_recovery(this->ctx, (modbus_error_recovery_mode)(MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL));
    }
    else
    {
    }


    // 10s for connect(select) timeout
    modbus_set_response_timeout(this->ctx, 10, 0);

    // wait for HISI modbus server ok
    while(modbus_connect(this->ctx) == -1) 
    {
        {
            DBG(DBG_WARN, "Connection failed(retry: %d): %s, wait for 5s...",
                retry, modbus_strerror(errno));
            modbus_close(this->ctx);
            usleep(5000*1000); // 5s
            retry++; 
            // modbus_flush(ctx);
            // FIXME: to heavy for it
            // seems like will be not used now, good thing!
            if(retry > 6*HISI_DEAD)
            {
                retry = 0;
                // restart Network
                system("/etc/init.d/networking restart");
            }
        }
    }


    // 500ms for response timeout
    modbus_set_response_timeout(this->ctx, 0, 500*1000);

    this->serverAlive = 1;

    return ret;
}

// handle HISI board
// 1. normal msg
// 2. if HISI board power off, then re-connect server
eRet DvrAdp::dataClientRun()
{
    eRet ret = eOK;
    int rc;
    U32 retry;
    U16 testReg = 0;
    U16 testRegVal = 0;

    if(this->serverAlive)
    {
        // test with server, if read error. then re-connect
        while((rc = modbus_read_registers(this->ctx, testReg, 1, &testRegVal)) != 1)
        {
            DBG(DBG_WARN, "Warn: modbus_read_registers (%d)", rc);
            if(++retry > HISI_DEAD)
            {
                // notify that server should be dead
                this->serverAlive = 0;
                retry = 0;
                break;
            }
        }
    }

    // re-connect at-once
    retry = 0;
    if(!this->serverAlive)
    {
        DBG(DBG_WARN, "Warn: modbus server not alive");
        // usleep(500*1000); // 0.5s
        // modbus_flush(ctx);
        // usleep(500*1000); // 0.5s
        modbus_close(this->ctx);
        usleep(1000*1000); // 0.5s
        // modbus_free(this->ctx);
        // this->ctx = NULL;
        this->dataClientInit();
    }

    return ret;

}

eRet DvrAdp::dataClientStop()
{
    eRet ret = eOK;

    DBG(DBG_INFO, "enter dataClientStop!");

    modbus_close(this->ctx);
    modbus_free(this->ctx);
    this->ctx = NULL;
    return ret;
}

eRet DvrAdp::getVal(U16 _id, U16 *_pDst)
{
    eRet ret = eOK;
    int rc;
    // DBG(DBG_INFO, "enter DvrAdp getVal!");
    if(this->serverAlive)
    {
    rc = modbus_read_registers(this->ctx, _id, 1, _pDst);
    if (rc != 1) {
        DBG(DBG_WARN, "Warn: DvrAdp getVal(%d)", rc);
    }
    }
    return ret;
}

// FIXME: in DVR board, wr Reg function: MULTIPLE_REGS, so 
// even here wr single Reg, still use wr multi Reg
eRet DvrAdp::setVal(U16 _id, U16 _src)
{
    eRet ret = eOK;
    int rc;

    // DBG(DBG_INFO, "enter DvrAdp setVal!");
    if(this->serverAlive)
    {
    rc = modbus_write_registers(this->ctx, _id, 1, &_src);
    if (rc != 1) {
        DBG(DBG_WARN, "Warn: DvrAdp setVal(%d)", rc);
    }
    }

    return ret;
}

eRet DvrAdp::getVals(U16 _id, U16 *_pDst, U16 _size)
{
    eRet ret = eOK;
    int rc;
    ASSERT(NULL != _pDst);
    // DBG(DBG_INFO, "enter DvrAdp getVal!");
    if(this->serverAlive)
    {
    rc = modbus_read_registers(this->ctx, _id, _size, _pDst);
    if (rc != _size) {
        DBG(DBG_WARN, "Warn: DvrAdp getVals(%d)", rc);
    }
    }
    return ret;
}

eRet DvrAdp::setVals(U16 _id, U16 *_pSrc, U16 _size)
{
    eRet ret = eOK;
    int rc;
    // DBG(DBG_INFO, "enter DvrAdp setVals!");
    ASSERT(NULL != _pSrc);

    if(this->serverAlive)
    {
    rc = modbus_write_registers(this->ctx, _id, _size, _pSrc);
    if (rc != _size) {
        DBG(DBG_WARN, "Warn: DvrAdp setVals(%d)", rc);
    }
    }
    return ret;
}

