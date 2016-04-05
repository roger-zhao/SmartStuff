#ifndef ADP_DVR_H
#define ADP_DVR_H

#include <common.h>
#include "modbus.h"

#ifdef BOARD
#define COMM_PORT "/dev/ttyO1"
#else
#define COMM_PORT "/dev/ttyUSB0"
// #define COMM_PORT "/dev/ttyS0"
#endif

#define HISI_SERVER_IP "192.168.2.20"
#define MODBUS_TCP_PORT 1502 

#define HISI_DEAD 3

class DvrAdp 
{
public:
    // libmodbus related
    modbus_t *ctx;
    U8 serverAlive;

	DvrAdp():serverAlive(0) {};
	~DvrAdp() {};
public:
    eRet dataClientInit();
    eRet dataClientRun();
    eRet dataClientStop();
    eRet getVal(U16 _id, U16 *_pDst);
    eRet getVals(U16 _id, U16 *_pDst, U16 _size);
    eRet setVal(U16 _id, U16 src);
    eRet setVals(U16 _id, U16 *_pSrc, U16 _size);
    
};


#endif


