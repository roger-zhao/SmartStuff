/*
 * gps_app.cpp
 *
 *  Created on: Jun 1, 2015
 *      Author: yangmi
 */
#include <math.h>
#include "app_gps.h"

extern char *buff[];
cGPSApp::cGPSApp()
{
	m_gpsAdp = new cGPSAdp();
	m_revState = 2;

	memset(&m_station,0,sizeof(m_station));
/*	m_station[0].stationId = 1;
	strcpy(m_station[0].stationName,"station1");
	//m_station[0].stationName = "station1";
	m_station[0].latitude = 1000;
	m_station[0].longitude = 1000;

	m_station[1].stationId = 2;
	strcpy(m_station[1].stationName,"station2");
	//m_station[1].stationName = "station2";
	m_station[1].latitude = 2000;
	m_station[1].longitude = 2000;*/
}

cGPSApp::~cGPSApp() {
	if (m_gpsAdp != NULL)
		delete m_gpsAdp;
}

/**
 * Calculate distance between two points
 *
 * @param from_pos a pointer to the from position (in radians)
 * @param to_pos a pointer to the to position (in radians)
 * @return distance in meters
 */
double cGPSApp:: GetDistance(double a_lat, double a_lon, double b_lat, double b_lon)
{
#if 0
	return ((double) NMEA_EARTHRADIUS_M) * acos(sin(b_lat) * sin(a_lat) + cos(
			b_lat) * cos(a_lat) * cos(b_lon - a_lon));
#else
	double retV = 0;
	double a = abs(a_lat - b_lat);
	double b = abs(a_lon - b_lon);
	retV = sqrt(a*a+b*b);
	return retV;
#endif
}
void cGPSApp::run()
{
    // printf("GPSAPP run in\n");
    // while (m_run_falg) // del by ZhaoYJ for controlling run outside (in main container)
    {
        this->m_gpsAdp->gpsAdpReceive(&m_gpsInfo);
    }
}
void cGPSApp::init()
{
    this->m_run_falg = 1;
}
void cGPSApp::stop()
{
    this->m_run_falg = 0;
}
char cGPSApp::DistanceGenRptStaMsg(double a_lat, double a_lon,ReportMsg* rptMsg)
{
	if (rptMsg == NULL)
		return 0;
	char retV = 0;
	double distance = 0;
	for (int i = 0; i < STATION_MAX_COUNT; i++)
	{
		distance = this->GetDistance(a_lat, a_lon, m_station[i].latitude,m_station[i].longitude);
		if (distance <= REPORT_DISTANCE)
		{
			rptMsg->cnt = 3;//report this message cnt times
			memset(rptMsg->reprotText,0,sizeof(rptMsg->reprotText));
			sprintf(rptMsg->reprotText,"arrived %s",m_station[i].stationName);
			retV = 1;
			break;
		}
	}

	return retV;
}

int cGPSApp::getGpsInfo(GPS_INFO* gpsInfo)
{
	if (gpsInfo == NULL)
		return (-1);

#if 1
	gpsInfo->D.day = m_gpsInfo.D.day;
	m_gpsInfo.D.hour = m_gpsInfo.D.hour;
	gpsInfo->D.minute =m_gpsInfo.D.minute;
	gpsInfo->D.month=m_gpsInfo.D.month;
	gpsInfo->D.second =m_gpsInfo.D.second;
	gpsInfo->D.year =m_gpsInfo.D.year;
	gpsInfo->EW = m_gpsInfo.EW;
	gpsInfo->NS=m_gpsInfo.NS;
	gpsInfo->high =m_gpsInfo.high;
	gpsInfo->latitude=m_gpsInfo.latitude;
	gpsInfo->EW = m_gpsInfo.EW;
	gpsInfo->longitude =m_gpsInfo.longitude;
	gpsInfo->speed=m_gpsInfo.speed;
	gpsInfo->status=m_gpsInfo.status;
	memcpy(gpsInfo->seq,m_gpsInfo.seq,5);
    // printf("Day: %d\n", m_gpsInfo.D.day);
    // printf("Hour: %d\n", m_gpsInfo.D.hour);
    // printf("Minutes: %d\n", m_gpsInfo.D.minute);
    // printf("Month: %d\n", m_gpsInfo.D.month);
    // printf("Seconds: %d\n", m_gpsInfo.D.second);
    // printf("Year: %d\n", m_gpsInfo.D.year);
    // printf("latitude: %f\n", m_gpsInfo.latitude);
    // printf("longitude: %f\n", m_gpsInfo.longitude);
    // sleep(2);
#endif
	return 0;
}
int cGPSApp::updateGPSInfo() {
	if (m_gpsAdp == NULL)
		return (-1);
#if 0
    static unsigned int pos = 0;
    char tmpbuf[256];
    int num = 0;

    num = m_gpsAdp->gpsAdpReceive(tmpbuf, 256);
    if (num > 0)
    {
		memcpy(m_buf + pos, tmpbuf, num);
		pos += num;
		if (pos >= GPS_RECEIVE_BUF_SIZE)
			pos = pos - GPS_RECEIVE_BUF_SIZE;

		m_gpsAdp->gpsAdpParse(buff[0], 512,&m_gpsInfo);
		return 0;
	}
    else
    	return (-1);
#endif
    return 0;
}
