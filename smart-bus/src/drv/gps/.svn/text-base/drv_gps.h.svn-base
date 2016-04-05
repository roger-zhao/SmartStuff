#ifndef DRV_GPS_H
#define DRV_GPS_H


#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#include "../serial/drv_serial.h"

typedef enum
{

    CONN_UART = 0,
    CONN_USB,
    CONN_I2C,
    CONN_UNKONWN,
    CONN_FILE           /****using for test ***/
} GPS_CONNET_TYPE;

typedef enum
{
    TTYS0      =  0,
    TTYS1,
    TTYS2,
    TTYUSB0,
    TTYUSB1,
    TTYUSB2
} SERIAL_PORT;

typedef enum
{
    DRV_OK      =  0,
    DRV_OPEN_ERR,
    DRV_CONN_ERR,
    DRV_REC_ERR,
    DRV_PARSE_ERR,
    DRV_UnKnown
} DRV_STATE;

class cGPSDrv
{
public:
	cGPSDrv();
	virtual ~cGPSDrv();
public:

	//GPS_INFO m_gpsInfo;
	int Cport[38];
    int  error;
    serialib m_serialObj;

    int OpenDevice();
    int OpenDevice(int port,int baute);
    int ReadDevice(char *buf,char FinalChar,unsigned int MaxNbBytes,unsigned int TimeOut_ms);
    int WriteDevice();
    int CloseDevicce();
#if 1
	int RS232_GPS_OpenComport(int comport_number, int baudrate, const char *mode);
	int RS232_GPS_PollComport(int comport_number, char *buf, int size);
	int RS232_GPS_SendByte(int comport_number, unsigned char byte);
	int RS232_GPS_SendBuf(int comport_number, unsigned char *buf, int size);
	void RS232_GPS_CloseComport(int comport_number);
	int RS232_GPS_IsDCDEnabled(int comport_number);
	int RS232_GPS_IsCTSEnabled(int comport_number);
	int RS232_GPS_IsDSREnabled(int comport_number);
	void RS232_GPS_enableDTR(int comport_number);
	void RS232_GPS_disableDTR(int comport_number);
	void RS232_GPS_enableRTS(int comport_number);
	void RS232_GPS_disableRTS(int comport_number);
	void RS232_GPS_cputs(int comport_number, const char *text);
#endif
};


#endif
