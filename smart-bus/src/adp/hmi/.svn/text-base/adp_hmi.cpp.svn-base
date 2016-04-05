#include "adp_hmi.h"

HmiAdp::HmiAdp()
{
}

HmiAdp::~HmiAdp()
{
}

// initial Modbus DB
eRet HmiAdp::dataServerInit(U8 _serverId)
{
    eRet ret = eOK;
    // DBG(DBG_INFO, "enter dataServerInit!");
    int rc;
    int header_length;

    this->ctx = modbus_new_rtu(COMM_PORT, BAUDRATE, PARITY, DATA_BITS, STOP_BIT);
    modbus_set_slave(this->ctx, _serverId);
    this->query = (uint8_t *)malloc(MODBUS_RTU_MAX_ADU_LENGTH);
    header_length = modbus_get_header_length(this->ctx);
    // DBG(DBG_INFO, "Modbus server header_length: 0x%x", header_length);

    modbus_set_debug(this->ctx, FALSE);

    this->mb_mapping = modbus_mapping_new(0, 0, this->DBSize, 0);
    if (this->mb_mapping == NULL) {
        DBG(DBG_ERROR, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(this->ctx);
        return eFail;
    }

    /* Unit tests of modbus_mapping_new (tests would not be sufficient if two
       nb_* were identical) */

    if (this->mb_mapping->nb_registers != this->DBSize)
    {
        DBG(DBG_ERROR, "Invalid nb registers: %d\n", this->DBSize);
        modbus_free(this->ctx);
        return eFail;
    }

    // connect
    rc = modbus_connect(this->ctx);
    if (rc == -1) {
        DBG(DBG_ERROR, "Unable to connect %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return eFail;
    }
    return ret;
}

// listen for Rx
eRet HmiAdp::dataServerRun()
{
    eRet ret = eOK;
    int rc;
    int rc2;

    // DBG(DBG_INFO, "enter dataServerRun!");
    do {
        // DBG(DBG_INFO, "modbus_receiving!");
        rc = modbus_receive(this->ctx, this->query);
        /* Filtered queries return 0 */
    } while (rc == 0);

    /* The connection is not closed on errors which require on reply such as
       bad CRC in RTU. */
    if (rc == -1 && errno != EMBBADCRC) {
        DBG(DBG_WARN, "modbus_receive error !");
        // return eFail;
    }
    else
    {

        rc2 = modbus_reply(this->ctx, this->query, rc, this->mb_mapping);
        if (rc2 == -1) {
            DBG(DBG_ERROR, "modbus_reply error!");
            // return eFail;
        }
    }

    return ret;

}

eRet HmiAdp::dataServerStop()
{
    eRet ret = eOK;
    int rc;

    // DBG(DBG_INFO, "enter dataServerStop!");

    modbus_mapping_free(this->mb_mapping);
    free(this->query);
    /* For RTU */
    modbus_close(this->ctx);
    modbus_free(this->ctx);
    return ret;
}

eRet HmiAdp::getVal(U16 _id, U16 *_pDst)
{
    eRet ret = eOK;
    // DBG(DBG_INFO, "enter HmiAdp getVal!");
    *_pDst = this->mb_mapping->tab_registers[_id];
    return ret;
}

eRet HmiAdp::setVal(U16 _id, U16 src)
{
    eRet ret = eOK;
    // DBG(DBG_INFO, "enter HmiAdp setVal!");
    this->mb_mapping->tab_registers[_id] = src;
    return ret;

}

eRet HmiAdp::setVals(U16 _id, U16 *pSrc, U16 size)
{
    eRet ret = eOK;
    U32 idx = 0;
    // DBG(DBG_INFO, "enter HmiAdp setVals!");
    for(idx = 0; idx < size; idx++)
    {
        this->mb_mapping->tab_registers[_id + idx] = *(pSrc++);
    }
    return ret;
}

