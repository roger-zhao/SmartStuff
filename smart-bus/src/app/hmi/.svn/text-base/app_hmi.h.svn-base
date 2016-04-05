#ifndef APP_HMI_H 
#define APP_HMI_H 

#include <common.h>
#include "base_class.h"

#include "../../adp/hmi/adp_hmi.h"
#include "../../os/os_timer.h"


// comm with HMI board info
//
#define HMI_DEAD 3
#define MODBUS_START_REG_ADDR 0x0000

#define LOGIN_REQ 0xA5A5
#define LOGIN_RES 0xB6B6

#define BBBREADY 0xE9E9

#define ONE_KEY_ALARM 0xBEEF

#define KEEP_ALIVE_REQ 0xC7C7
#define KEEP_ALIVE_RES 0xD8D8

#define XINXI_MENU_TOTAL_MODREG_PER_STATION 512 // 1024/2=512
#define XINXI_MENU_TOTAL_MODREG 1024 // 16*128/2=1024

#define MAX_DRIVER_NAME_LEN 640 // 32x32, align with xinxi menu


#define MAX_DRIVER_NAME 5

#define NOW 0
#define ONE_SEC (1*1000)

// #define SELF_TEST

#ifdef SELF_TEST

// #define TEST_TIMER

#define CRITICAL_INFO_REFRESH_INTERVAL (2*1000) // 2s
#define NON_CRITICAL_INFO_REFRESH_INTERVAL (4*1000) // 20s

#else

#define CRITICAL_INFO_REFRESH_INTERVAL (2*1000) // 2s
#define NON_CRITICAL_INFO_REFRESH_INTERVAL (20*1000) // 20s

#endif


enum modbRegAddr
 {
	LOGIN_MODREG = (0x0 + MODBUS_START_REG_ADDR),
	BBB_READY_MODREG,
	ONE_KEY_ALARM_MODREG,
// hisi send this time stamp per 2s to keep alive, mightbe not used because time-sync is also periodic
	KEEP_ALVIE_MODREG = (0x3 + MODBUS_START_REG_ADDR),

// follow modbus reg will be sequency
// time-sync per 2s (needed?)
// date
	TIME_SYNC_YEAR_MODREG,
	TIME_SYNC_MON_MODREG, // (TIME_SYNC_YEAR_MODREG + 0x1)
	TIME_SYNC_DAY_MODREG, // (TIME_SYNC_YEAR_MODREG + 0x1)

// time
	TIME_SYNC_HOUR_MODREG, //  (TIME_SYNC_MON_DAY_MODREG + 0x1)
	TIME_SYNC_MIN_MODREG, //  (TIME_SYNC_MON_DAY_MODREG + 0x1)
// second and millisecond(if neccesary for latter)
	TIME_SYNC_SEC_MODREG, // (TIME_SYNC_HOUR_MIN_MODREG + 0x1)
	TIME_SYNC_MS_MODREG, // (TIME_SYNC_HOUR_MIN_MODREG + 0x1)

	// busInfo
	AUTO_CURR_STATION_MODREG,
	MANUAL_CURR_STATION_MODREG,
	APC_MODREG,
	MANUAL_APC_MODREG,
	BUS_GRP_NUM_MODREG,
	BUS_LINE_NUM_MODREG,
	BUS_NUM_MODREG,
	TOTAL_STATIONS_MODREG,
	DRIVER_ID_MODREG,
	DRIVER_NAME_MODREG, // PIXELS_PER_CHIN_CHAR*MAX_DRIVER_NAME

	// commInfo
	WEATHER_MODREG = (DRIVER_NAME_MODREG + MAX_DRIVER_NAME_LEN),
	TEMPERATURE_MODREG,
	HUMIDITY_MODREG,
	ULTRAVIOLET_MODREG,

	// xinXiFont: current & next station
	CURR_STATION_FONT_START_MODREG,
	NEXT_STATION_FONT_START_MODREG = (CURR_STATION_FONT_START_MODREG + XINXI_MENU_TOTAL_MODREG_PER_STATION),
	NEXT_STATION_FONT_END_MODREG = (NEXT_STATION_FONT_START_MODREG + XINXI_MENU_TOTAL_MODREG_PER_STATION),

	MODREG_END = (NEXT_STATION_FONT_END_MODREG + 32) // 32 for gap
};

typedef struct {
    U16 year;
    U16 month;
    U16 day;
    U16 hour;
    U16 min;
    U16 sec;
}dateTime;

typedef struct {
    U16 weather;
    U16 temperature;
    U16 humidity;
    U16 ultraViolet;
}commInfo;

class HmiApp: public APP 
{
public:
    HmiAdp *pcAdp;
	HmiApp();
	virtual ~HmiApp();

public:
	void run();
	void init();
	void stop();
    
    U8 getBBBReady() { return this->BBBReady;};
    void  setBBBReady(U8 _r) {this->BBBReady = _r; };

    S8 getHmiState() { return this->HmiState ;};
    void  setHmiState(S8 _s) {this->HmiState = _s; };

    U8 getAPCVal() {return this->APCVal; };
    void  setAPCVal(U8 _val) {this->APCVal = _val; };

    U8 getAutoNextStation() {return this->autoNextStation; };
    void  setAutoNextStation(U8 _val) {this->autoNextStation= _val; };

    U8 getManualNextStation() {return this->manualNextStation; };
    void  setManualNextStation(U8 _ns) {this->manualNextStation = _ns; };

    U8 getTotalStations() {return this->totalStations; };
    void  setTotalStations(U8 _ns) {this->totalStations = _ns; };

    dateTime *getDateTime() {return &this->tDT; };
    void  setDateTime(dateTime _dt) {this->tDT = _dt; };

    U32 getHmiPasswd() {return this->hmiPasswd; };
    void  setHmiPasswd(U32 _psswd) {this->hmiPasswd = _psswd; };
    
    U8 getHmiCommPort() {return this->hmiCommPort; };
    void  setHmiCommPort(U8 _cport) {this->hmiCommPort = _cport; };

    U8 *getStationFont() {return this->pStationFont; };
    void  setStationFont(U8 *_pFont) {this->pStationFont = _pFont; };

    U16 getBusNum() {return this->busNum; };
    void  setBusNum(U16 _bn) {this->busNum = _bn; };

    U8 getDriverId() {return this->driverId; };
    void  setDriverId(U8 _id) {this->driverId = _id; };

    commInfo *getCommInfo() {return &this->tCommInfo; };
    void  setCommInfo(commInfo _cInfo) {this->tCommInfo = _cInfo; };

private:
    S8 HmiState; 
    U8 APCVal; 
    U8 manualNextStation; 
    U8 autoNextStation;
    U8 totalStations;
    dateTime tDT; 
    U8 *pStationFont;
    U16 busNum;
    U8 driverId;
    commInfo tCommInfo; 
    U32 hmiPasswd;
    U8 hmiCommPort;
    U8 BBBReady;
    // timer
    OsTimer *pCriticalTimer;
    OsTimer *pNonCriticalTimer;
public:

    // some funcs for refresh
    // APC value, prev/next station index, font, time 
    static void refreshCriticalInfos(union sigval sv)
    {
        time_t now;
        struct tm *timeNow;
        DBG(DBG_INFO, "refreshCriticalInfos enter: %llu", OsTimer::getMs()/1000);
        eRet ret = eFail; 
        U16 keepAlive = 0;
        U8 lostHmi = 0;

        HmiApp *_this = reinterpret_cast<HmiApp*> (sv.sival_ptr);

        // keep alive
        ret = _this->pcAdp->getVal(KEEP_ALVIE_MODREG, &keepAlive);
        ASSERT(eOK == ret);
        if(KEEP_ALIVE_REQ == keepAlive)
        {
            ret = _this->pcAdp->setVal(KEEP_ALVIE_MODREG, KEEP_ALIVE_RES);
            ASSERT(eOK == ret);
             _this->setHmiState(1);
        }
        else
        {
            if(++lostHmi > HMI_DEAD)
            {
                _this->setHmiState(-1);
            }
        }

        // APC
        ret = _this->pcAdp->setVal(APC_MODREG, _this->getAPCVal());
        ASSERT(eOK == ret);
        // next station 
        ret = _this->pcAdp->setVal(AUTO_CURR_STATION_MODREG, _this->getAutoNextStation());
        ASSERT(eOK == ret);
        // station font & idx: need convert char to mod_reg in adapter 
        // prev station & current station updated together
        ret = _this->pcAdp->setVals(CURR_STATION_FONT_START_MODREG, (U16 *)_this->getStationFont(), 2*XINXI_MENU_TOTAL_MODREG_PER_STATION);
        ASSERT(eOK == ret);
        // date time
        time(&now);
        timeNow = localtime(&now);
        dateTime dt;
        dt.year = 1900 + timeNow->tm_year;
        dt.month = 1 + timeNow->tm_mon;
        dt.day = timeNow->tm_mday;
        dt.hour = timeNow->tm_hour;
        dt.min = timeNow->tm_min;
        dt.sec = timeNow->tm_sec;
        _this->setDateTime(dt);
        // DBG(DBG_INFO, "dateTime: %4d-%d-%d %d:%d:%d", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);

        ret = _this->pcAdp->setVals(TIME_SYNC_YEAR_MODREG, (U16 *)_this->getDateTime(), 6);
        ASSERT(eOK == ret);
    }
    
    // bus num, driver Id, wealther, ...
    static void refreshNonCriticalInfos(union sigval sv)
    {
        DBG(DBG_INFO, "refreshNonCriticalInfos enter!");
        // DBG(DBG_INFO, "refreshNonCriticalInfos enter: %llu", OsTimer::getMs()/1000);
        eRet ret = eFail; 
        HmiApp *_this = reinterpret_cast<HmiApp*> (sv.sival_ptr);
        // station num 
        ret = _this->pcAdp->setVal(TOTAL_STATIONS_MODREG, _this->getTotalStations());
        ASSERT(eOK == ret);
        // bus num 
        ret = _this->pcAdp->setVal(BUS_NUM_MODREG, _this->getBusNum());
        ASSERT(eOK == ret);
        // drv ID
        ret = _this->pcAdp->setVal(DRIVER_ID_MODREG,_this->getDriverId());
        ASSERT(eOK == ret);
        // commInfo 
        ret = _this->pcAdp->setVals(WEATHER_MODREG, (U16 *)_this->getCommInfo(), 4);
        ASSERT(eOK == ret);
    }

};
#endif
