

/*
 * smb_hmi.h
 *
 */

#ifndef SMB_HMI_H
#define SMB_HMI_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif
#include <ModbusMaster.h>
#include <UTFT.h>
#include <Keypad.h>
#include "smb_menu.h"

typedef unsigned char UC;
typedef unsigned short US;
typedef unsigned int UINT;
typedef unsigned long long ULL;

class SMBprint;
extern SMBprint SMBprt;

class Menu;
//extern Menu *last;        // Single entry for back button
extern Menu *current;     // Current menu location

// for display: border & layout
#define MAX_X_WIDTH 480
#define MAX_Y_HEIGHT 320
#define MIDDLE_X 240
#define MIDDLE_Y 160

#define TAB_START_X 2
#define TAB_START_Y 42
#define TAB_WIDTH 50
#define TAB_HEIGHT 114

// for display: color


#define TIME32_DIFF(x, y) ((x >= y)?(x - y):(0xFFFFFFFF - y + x))

#define PRT(label, value) do { \
								Serial.print(F(label)); \
								Serial.print(F(": <")); \
								Serial.print(value); \
								Serial.println(F(">")); \
							}while(0)

// for Chinese character

// fonts pixels declaration
// extern

// #define SELF_TEST

extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];

extern uint8_t topFont[];
extern uint8_t vendorName[]; //bottFont[];
extern uint8_t tabFont[];
extern uint8_t xinXiFont[];
extern uint8_t sheZhiFont[];
extern uint8_t thinNumber[];
extern uint8_t vendorLogo[];
extern uint8_t vendorVision[];
extern uint8_t errorFont[];

#ifdef SELF_TEST
extern uint8_t drvNameFont[];

#endif
extern uint8_t HanZi[];

// extern unsigned int vendorLogo[];
// extern unsigned int vendorVision[];

#define TOP_MENU_PIXELS_WIDTH_PER_CHIN_CHAR 32
#define TOP_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR 28
#define TOP_MENUPIXELS_PER_CHIN_CHAR 112 // 32x28 pixels Chinese character: per character will be 112bytes
#define PIXELS_WIDTH_PER_ASCII_CHAR 16
#define PIXELS_HEIGHT_PER_ASCII_CHAR 32

#define XINXI_MENU_PIXELS_WIDTH_PER_CHIN_CHAR 32
#define XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR 32

#define XINXI_MENU_BYTES_PER_CHIN_CHAR 128 // 32x32 pixels Chinese character: per character will be 112bytes
#define XINXI_MENU_CHIN_CHAR_PER_STATION 8
#define XINXI_MENU_BYTES_PER_STATION 1024 // 128*8
#define XINXI_MENU_TOTAL_STATIONS	2 // current & next
#define XINXI_MENU_TOTAL_CHIN_CHAR 16
#define XINXI_MENU_TOTAL_MODREG_PER_STATION 512 // 1024/2=512
#define XINXI_MENU_TOTAL_MODREG 1024 // 16*128/2=1024


#define MAX_DRIVER_NAME 5
#define MAX_DRIVER_NAME_LEN 640 // 32x32, align with xinxi menu

// modbus regs

#define MODBUS_START_REG_ADDR 0x0000

#define OK 0
#define KO 1

#define LOGIN_REQ 0xA5A5
#define LOGIN_RES 0xB6B6

#define BBBREADY 0xE9E9

#define ONE_KEY_ALARM 0xBEEF

#define HAVE_KEEP_ALIVE

#define BBBDEAD 20

#define KEEP_ALIVE_REQ 0xC7C7
#define KEEP_ALIVE_RES 0xD8D8


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
	TIME_SYNC_DAY_MODREG,

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

enum errorNo
{
	eNA,
	eCommLinkError, // modbus comm error
	eBBBDead, // keep alive
};

typedef struct {
	UINT autoCurrStation : 1;
	UINT manuCurrStation : 1;
	UINT autoPassengerCount : 1;
	UINT manuPassengerCount : 1;
	UINT busGrpNum : 1;
	UINT busLineNum : 1;
	UINT busNum : 1;
	UINT totalStations : 1;
	UINT driverId : 1;
	UINT driverName : 1;
	UINT currentStation : 1;
	UINT nextStation : 1;
	UINT resv : 20;
}busDisplayBitMap;

typedef struct {
	UC autoCurrStation;
	UC manuCurrStation;
	UC autoPassengerCount;
	UC manuPassengerCount;
	US busGrpNum;
	US busLineNum;
	US busNum;
	UC totalStations;
	char driverId;
	uint8_t driverName[MAX_DRIVER_NAME_LEN];
	uint8_t currentStation[XINXI_MENU_BYTES_PER_STATION];
	uint8_t nextStation[XINXI_MENU_BYTES_PER_STATION];
	// for display
	busDisplayBitMap displayBitMap;

}busInfo;

typedef struct {
	UINT year : 1;
	UINT month : 1;
	UINT day : 1;
	UINT hour : 1;
	UINT min : 1;
	UINT sec : 1;
	UINT resv : 26;
}dateTimeDisplayBitMap;

typedef struct {
	 US year;
	 UC month;
	 UC day;
	 UC hour;
	 UC min;
	 UC sec;
	 dateTimeDisplayBitMap displayBitMap;
}dateTime;

typedef struct {
	dateTime tDateTime;
}sysInfo;

typedef struct {
	UINT weather : 1; // coding for different weather
	UINT temperature : 1;
	UINT humidity : 1;
	UINT ultraViolet : 1;
	UINT resv : 28;
}commDisplayBitMap;

typedef struct {
	UC weather; // coding for different weather
	UC temperature;
	UC humidity;
	UC ultraViolet;
	commDisplayBitMap displayBitMap;
}commonInfo;


typedef struct {

}deviceSett;

typedef struct {

}commSett;

typedef struct {

}pswdSett;

typedef struct {
	deviceSett tDevSett;
	commSett tCommSett;
	pswdSett tPswdSett;
}settings;

class modbusDB {
public:
	ModbusMaster SMBModMaster;
	UC BBBReady;
	// rx
	busInfo tBusInfo;
	sysInfo tSysInfo;
	commonInfo tCmnInfo;
	// tx
	UC manuCurrStation;
	UC manuPassengerCount;
	settings tSett;
	enum errorNo errNo;

	modbusDB();
	modbusDB(UC _serial_port, UC _slave_id): SMBModMaster(_serial_port, _slave_id){};


	UC loginBBB();
	UC isBBBReady();
	void sendOneKeyAlarm();
	void refreshInfosToBBB();
	void getInfosFromBBB();
	void get64BaoZhanFont(); //
	void parseModbusMsg(); // parse Modbus msg
};

class SMBprint : public UTFT {
public:

	// import ModbusDB
	modbusDB *pDB;
	//UC popMsgFlg;

	SMBprint() {};
	SMBprint(byte model, int RS, int WR, int CS, int RST, modbusDB *_pDB, int SER=0) : UTFT(model, RS, WR, CS, RST, SER=0) {
		this->pDB = _pDB;
	};


	void printVendorLogo(UC flg);
	void printTop();
	void printBottom();
	void printBorder();
	void popMsg(enum errorNo en);
};

//Keypad SMBKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows,
//                          numCols);

class SMBKeypad : public Keypad {
public:

	// SMBKeypad() {};
	SMBKeypad(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols) : Keypad(userKeymap, row, col, numRows, numCols) {};

	~SMBKeypad() {};
	// handler keyprocess func

	static void procKeyXinXi(Menu &_xinXi, char *key);

	// static void procKeyBaoZhan(char *key);

	static void procKeySheZhi(Menu &_sheZhi, char *key);
};


// handler display func

void displayXinXi(Menu &xinXi, Menu &last);

// static void displayBaoZhan();

void displaySheZhi(Menu &sheZhi,  Menu &last);

#endif /* YAMS_H_ */

