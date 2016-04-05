#ifndef APP_GPS_H
#define APP_GPS_H

#include <common.h>
#include "base_class.h"

#include "../../adp/gps/adp_gps.h"

#define GPS_RECEIVE_BUF_SIZE 512
#define NMEA_EARTHRADIUS_KM         (6378)                          /**< Earth's mean radius in km */
#define NMEA_EARTHRADIUS_M          (NMEA_EARTHRADIUS_KM * 1000)    /**< Earth's mean radius in m */
#define STATION_MAX_COUNT           2
#define REPORT_DISTANCE             200                             /***unit meters**/


typedef struct
{
	unsigned int stationId;
	char stationName[50];
	double latitude;
	double longitude;
} Station;

typedef enum
{
    DEV_0000 = 0,
    DEV_0001,
    DEV_0002,
    DEV_0003,
    DEV_0004
} DEVICE_ID;

typedef enum
{
    BUS_0000 = 0,
    BUS_0001,
    BUS_0002,
    BUS_0003,
    BUS_0004
} BUS_ID;

typedef struct
{
    unsigned int route_id;
    Station station[20];
} RouteInfo;

typedef struct
{
    DEVICE_ID dev_id;
    BUS_ID bus_id;
    RouteInfo route;
} DevConfigure;


typedef struct
{
	char cnt;                               //report this message cnt times
	char reprotText[256];
} ReportMsg;

class cGPSApp: public APP 
{
public:
	cGPSApp();
	virtual ~cGPSApp();
public:
	GPS_INFO m_gpsInfo;
	cGPSAdp *m_gpsAdp;
	char m_buf[GPS_RECEIVE_BUF_SIZE]; // receive buf, it is a ring buf. aw
	char m_revState;
	Station m_station[STATION_MAX_COUNT];
	unsigned char m_run_falg;

	void run();
	void init();
	void stop();
	double GetDistance(double a_lat, double a_lon, double b_lat, double b_lon);
	char DistanceGenRptStaMsg(double a_lat, double a_lon,ReportMsg* rptMsg);
	int getGpsInfo(GPS_INFO* gpsInfo);
	int updateGPSInfo();
};


#endif
