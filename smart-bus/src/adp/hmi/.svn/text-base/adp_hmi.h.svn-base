#ifndef ADP_HMI_H
#define ADP_HMI_H

#include <common.h>
#include "modbus.h"

#ifdef BOARD
#define COMM_PORT "/dev/ttyO1"
#else
#define COMM_PORT "/dev/ttyUSB0"
// #define COMM_PORT "/dev/ttyS0"
#endif

#define BAUDRATE 9600
#define PARITY 'N' 
#define DATA_BITS 8 
#define STOP_BIT 1 

#define DEF_SERVER_ID 1


class HmiAdp 
{
public:
    // libmodbus related
    U32 DBSize;
    modbus_t *ctx;
    uint8_t *query;
    modbus_mapping_t *mb_mapping;

	HmiAdp();
	HmiAdp(U32 _DBSize):DBSize(_DBSize) {};
	virtual ~HmiAdp();
public:
    eRet dataServerInit(U8 _svrId = DEF_SERVER_ID);
    eRet dataServerRun();
    eRet dataServerStop();
    eRet getVal(U16 _id, U16 *_pDst);
    eRet setVal(U16 _id, U16 src);
    eRet setVals(U16 _id, U16 *pSrc, U16 size);
    
};


#endif
