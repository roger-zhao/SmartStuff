/*
 * app_main.cpp
 *
 *  Created on:
 *      Author: yanjie 
 */

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../os/os_task.h"
#include "../os/os_ipc.h"
#include "../app/gps/app_gps.h"
#include "app_main.h"
#include "../cmn/db/db.h"
#include "../drv/spi/SPIDevice.h"
#include "debug.h"
#include "modbus.h"
#include "../app/hmi/app_hmi.h"
#include <memory.h>
// #define CO_TEST_HISI

#ifdef CO_TEST_HISI

#define LOGIN_REQ 0xA5A5
#define LOGIN_RES 0xB6B6
#define LOGIN_MODREG (0x0)

modbus_t *HisiCtx;
unsigned char modbusTcpSvrAlive;
#endif


#define DDDD     1
#if DDDD
using namespace exploringBB;
#endif

extern cGPSApp *pcGpsApp;
extern int msgSendFifo;
extern HmiApp *pcHmiApp;
extern pthread_mutex_t counter_lock;

unsigned char hmi_display[FRONT_PANEL_CODE_NUM_MAX*2] = { 0 };
unsigned char part1[] = { 0x01, 0xB3, 0xCB, 0xBF, 0xCD, 0xC4, 0xFA, 0xBA, 0xC3, 0xA3, 0xAC }; //chengke ninhao,
unsigned char part2[] = { 0x01, 0xA3 ,0xAC,0xB5, 0xBD, 0xC1, 0xCB, 0xA3, 0xAC, 0xC7, 0xEB, 0xCF, 0xC2, 0xB3, 0xB5, 0xA3, 0xAC};//daole,qingxiache
unsigned char part3[] = { 0x01, 0xCF, 0xC2, 0xD2, 0xBB, 0xD5, 0xBE, 0xCA ,0xC7,0xA3, 0xBA};//xiayizhan

void MainLoop::init() 
{
    DBG(DBG_INFO, "enter MainLoop init!");
    // open data-link: wireless device
    //
    /* TCP */
    
    // parse conf from PC, pc task should be get infos for this via shm & sem

    // init DB files: .mp3/mp4, route map, dirver infos, etc.
    // HW & SW self-check when power on
}
#ifdef CO_TEST_HISI
eRet IsHisiLogin()
{
    // get login request
    int rc;
    U16 loginReq = 0;
    eRet ret = eOK;
    if(!modbusTcpSvrAlive)
    {
        DBG(DBG_WARN, "modbusTcp server not alives, give up login HisiBoard");
        return eFail;
    }

    rc = modbus_read_registers(HisiCtx, 0x0, 1, &loginReq);
    if (rc != 1) {
        DBG(DBG_WARN, "ERROR modbus_read_registers (%d)\n", rc);
        ret = eFail;
    }

    if(LOGIN_REQ == loginReq)
    {
        DBG(DBG_WARN, "HISI board is loginning now (0x%x)!\n", loginReq);
        rc = modbus_write_register(HisiCtx, 0x0, LOGIN_RES);
        if (rc != 1) {
            printf("ERROR modbus_write_register: login (%d)\n", rc);
            ret = eFail;
        }

    }
    else
    {
        ret = eFail;
    }

    return ret;
}
#endif
#if DDDD
char generateHmiBuf(StationDB* curr, StationDB* next,unsigned char* buf);
unsigned char* generateTTSSendBuf(StationDB* curr, StationDB* next,int* len)
{
	unsigned char* rbuf = NULL;

	unsigned short textlen = 0;

	unsigned short len1 = 0;
	unsigned short len2 = 0;
	unsigned short len3 = 0;
	unsigned short len4 = 0;
	unsigned short len5 = 0;

	cout << curr->stationName << endl;
	len1 = sizeof(part1);
	len2 = strlen((const char*) curr->NameCode);
	len3 = sizeof(part2);
	len4 = sizeof(part3);
	len5 = strlen((const char*) next->NameCode);
	textlen = len1 + len2 + len3 + len4 + len5;
	rbuf = (unsigned char*) malloc(textlen + 4);
	memset(rbuf, 0, sizeof(rbuf));
	rbuf[0] = 0xFD;
	rbuf[1] = (unsigned char) ((textlen & 0xFF00) >> 8);
	rbuf[2] = (unsigned char) (textlen & 0xFF);
	rbuf[3] = 0x01;

	strncpy((char*) rbuf + 4, (const char*) part1, len1);
	strncpy((char*) rbuf + 4 + len1, (const char*) curr->NameCode, len2);
	strncpy((char*) rbuf + 4 + len1 + len2, (const char*) part2, len3);
	strncpy((char*) rbuf + 4 + len1 + len2 + len3, (const char*) part3, len4);
	strncpy((char*) rbuf + 4 + len1 + len2 + len3 + len4, (const char*) next->NameCode, len5);
	*len = textlen + 4;
	return rbuf;
}

char generateHmiBuf(StationDB* curr, StationDB* next,unsigned char* buf)
{
	if (buf == NULL)
           return -1;
        memset(buf, 0,FRONT_PANEL_CODE_NUM_MAX*2); 
        memcpy((unsigned char*) buf, (const unsigned char*) curr->frontPanelCode, FRONT_PANEL_CODE_NUM_MAX);
        memcpy((unsigned char*) buf + FRONT_PANEL_CODE_NUM_MAX, (const unsigned char*) next->frontPanelCode, FRONT_PANEL_CODE_NUM_MAX);
        return 0;
}


#endif
char updateSystime(date_time* dt)
{
    struct tm time;
    struct timeval tv;
    time_t setTime;

    time.tm_year = dt->year - 1900; /*year -1900*/
    time.tm_mon = dt->month - 1; /*month - 1*/
    time.tm_mday = dt->day;
    time.tm_hour = dt->hour;
    time.tm_min = dt->minute;
    time.tm_sec = dt->second;
    setTime = mktime(&time);

    tv.tv_sec = setTime;
    tv.tv_usec = 0;
    if (settimeofday(&tv, (struct timezone *) 0) < 0)
    {
        printf("Set system datatime error!\n");
        return -1;
    }
    return 0;
}
void showContent(char* src, int len)
{
    if (src == NULL)
    {
        printf("parameter error, show nothing \n");
        return;
    }
    for (int i = 0; i < len; i++)
    {
        printf("src[%d] = %2x\n", i, *(src + i));
    }
}
void MainLoop::run() 
{
    DBG(DBG_INFO, "enter MainLoop run!");
    // parse conf from PC, pc task should be get infos for this via shm & sem

    GPS_INFO gpsInfo;
    ReportMsg rprmsg;
    MessageInfo msgInfo;
    int index = 0;
    int nextIndex = 0;
    char buf[560] = { 0 };
    
    DevConfigure* devCfg = (DevConfigure *) malloc(sizeof(DevConfigure));
    devCfg->bus_id = BUS_0000;
    devCfg->dev_id = DEV_0000;
    devCfg->route.route_id = 0;
    devCfg->route.station[0].latitude = 100;
    devCfg->route.station[0].longitude = 100;
    sprintf(devCfg->route.station[0].stationName, "%s", "station0");

    devCfg->route.station[1].latitude = 200;
    devCfg->route.station[1].longitude = 200;
    sprintf(devCfg->route.station[1].stationName, "%s", "station1");


    for (int i = 0; i < 2; i++)
    {
        pcGpsApp->m_station[i].latitude = devCfg->route.station[i].latitude;
        pcGpsApp->m_station[i].longitude = devCfg->route.station[i].longitude;
        strcpy(pcGpsApp->m_station[i].stationName,
                devCfg->route.station[i].stationName);
    }
#if DDDD
    RouteDB routeInfo;

    std::string docname = "/home/root/RouteDB.xml";
    parseDoc (docname.c_str(),&routeInfo);
    //std::string docname = "/home/yangmi/workspace_v6_0/xmlparse/src/RouteDB.xml";
    SPIDevice *busDevice = new SPIDevice(1, 0); //Using second SPI bus (both loaded)
    busDevice->setSpeed(20000); // Have access to SPI Device object
    busDevice->setMode(SPIDevice::MODE0);
    unsigned char  receive[200];
    pcHmiApp->setBBBReady(1); 
    pcHmiApp->setAutoNextStation(0);//tell Hmi which Station we are first!
    generateHmiBuf(&(routeInfo.station[0]),&(routeInfo.station[1]),hmi_display);
    pcHmiApp->setStationFont(hmi_display);
#endif
    while(1)
    {


#ifdef CO_TEST_HISI
        // test to sync whti HISI
        while(eOK != IsHisiLogin());
#endif

        pthread_mutex_lock(&counter_lock);
        pcGpsApp->getGpsInfo(&gpsInfo);
        pthread_mutex_unlock(&counter_lock);

        //if (pcGpsApp->m_revState == 0)
        {
            if (1 == pcGpsApp->DistanceGenRptStaMsg(gpsInfo.latitude, gpsInfo.longitude, &rprmsg))
            {
                //printf("%s\n", rprmsg.reprotText);
            }
           //if(_gpsInfo.status == 'A')
            {
               //printf("----------------------gpstime :%d %d \n",gpsInfo.D.year,gpsInfo.D.second);
                if(gpsInfo.D.year == 2015)
                  {
                     printf("gpstime :%d %d \n",gpsInfo.D.year,gpsInfo.D.second);
                     updateSystime(&gpsInfo.D);
                     printf("GPS status OK start to set System time\n");
                  }
            }
            msgInfo.msgid = MSG_GPS_ID;
            memcpy(msgInfo.message, (char*) &gpsInfo, sizeof(gpsInfo));
            msgInfo.size = sizeof(msgInfo);
            memcpy(buf, (char*) &msgInfo, sizeof(msgInfo));
            int cnt = write(msgSendFifo, buf, msgInfo.size);
            if (cnt <= 0)
            {
                printf("write FIFO error\n");
            }
        }
#if DDDD
        int len = 0;
        nextIndex = index + 1;
	if (nextIndex >= 10)
            nextIndex = 0;
	

	pcHmiApp->setAutoNextStation(index);//tell Hmi which Station we are first!
        generateHmiBuf(&(routeInfo.station[index]),&(routeInfo.station[nextIndex]),hmi_display);
        // showContent((char*)hmi_display,FRONT_PANEL_CODE_NUM_MAX*2);
	pcHmiApp->setStationFont(hmi_display);
	sleep(30);

        //unsigned char* sendbuf = generateTTSSendBuf(&(routeInfo.station[index]),&(routeInfo.station[index+1]),&len);
        unsigned char* sendbuf = generateTTSSendBuf(&(routeInfo.station[index]),&(routeInfo.station[nextIndex]),&len);
#if 0
        index++;
	if (index >= 10)
           index = 0;
#endif
        index = nextIndex;	
        busDevice->transfer(sendbuf, receive, len);
        if(sendbuf != NULL)
            free(sendbuf);
#endif
    }
    //
    // parse command from remote terminal/monitoring center, also include intelligence scheduling
    //
    // HW & SW self-check if periodic enabled in PC conf 
    //
    // update DB files: .mp3/mp4, route map, dirver infos (include driver enter pwd to report attending), human face, etc.
    //
    // update & report bus infos via speaker & LEDs (screens & lights): GPS infos, station infos, bus infos (crowd-level, status of running, warnings: mis-match with route & long-time driving & and so on)  
    //
    // face-recognization
    // TODO: might be a task?
}

void MainLoop::stop() 
{
    DBG(DBG_INFO, "enter MainLoop stop!");
}
