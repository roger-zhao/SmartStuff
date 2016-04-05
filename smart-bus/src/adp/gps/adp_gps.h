#ifndef ADP_GPS_H
#define ADP_GPS_H

#include <string.h>
#include <common.h>

#include "../../drv/gps/drv_gps.h"

#define GPRMC_NUM_OF_DOU                12
#define GPGGA_NUM_OF_DOU                14
#define ERRORNUM_OF_DOU                 20
#define MIN_GPRMC_LINE_LEN              30    //12+6+time+date
#define MIN_GPGGA_LINE_LEN              30    //12+6+time+date
#define RING_BUF_DEEPTH                 1
#define RING_BUF_LINE_LEN               128

typedef enum
{

    UnKnown = 0,
    UNINIT,
    OPENED,
    CLOSED
} UART_STATE;


typedef struct
{
	char linebuf[128];
	char writeflag;
	unsigned int updateSeqNum; //how latest or new data it is
	//char newflag;
} line_obj;

typedef struct
{
	line_obj lineobj[RING_BUF_DEEPTH];
	char readToalFlag;
} lineRingBuf;

class cGPSAdp
{
public:
	cGPSAdp();
	virtual ~cGPSAdp();
public:
	//GPS_INFO m_gpsInfo;
	cGPSDrv* m_gpsDrv;
	//int gpsAdpReceive(char* buf,int num);
	int gpsAdpReceive(GPS_INFO* gpsInfo);
	int gpsAdpSend(char* buf,int num);
	int gpsAdpParse(GPS_INFO* gpsInfo);
	//int gpsAdpParse(char* buf,int num,GPS_INFO* gpsInfo);
	int gpsAdpUpdateGpsInfo();
	UART_STATE m_uartstate;
	int reBuildBuf(char* buf,int num);
	lineRingBuf m_ringBuf;
	//char index;
	//char m_buf_pool[7][128];      // 0:$GNRMC 1:$GNGGA 2:$GPTXT  3:$GPGSA 4:$BDGSA 5:$GPGSV 6:$BDGSV
	char* m_pGNRMC;
	char  m_gGNRMCState;          //0 not finish fill and record the stroe pointer ; 1 finish fill,and it is time to parse
	char* m_pGNGGA;
	char  m_gGNGGAState;          //0 not finish fill and record the stroe pointer ; 1 finish fill,and it is time to parse
private:
	int GetComma(int num, char *str);
	double get_locate(double temp);
	double get_double_number(char *s);
	void UTC2BTC(date_time *GPS);
	int GetCommaNum(char *str);
};


#endif
