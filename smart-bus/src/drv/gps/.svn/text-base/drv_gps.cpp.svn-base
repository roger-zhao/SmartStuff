#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "drv_gps.h"

cGPSDrv::cGPSDrv() {

}

cGPSDrv::~cGPSDrv() {
	// TODO Auto-generated destructor stub
	//RS232_GPS_CloseComport(Cport);
	CloseDevicce();
}




struct termios new_port_settings,
       old_port_settings[38];

char comports[50][16] =
{
		"/dev/ttyO0", "/dev/ttyO1", "/dev/ttyO2", "/dev/ttyO3", "/dev/ttyO4",
		"/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3", "/dev/ttyS4",
		"/dev/ttyS5", "/dev/ttyS6", "/dev/ttyS7", "/dev/ttyS8", "/dev/ttyS9",
		"/dev/ttyS10", "/dev/ttyS11", "/dev/ttyS12", "/dev/ttyS13",
		"/dev/ttyS14", "/dev/ttyS15", "/dev/ttyUSB0", "/dev/ttyUSB1",
		"/dev/ttyUSB2", "/dev/ttyUSB3", "/dev/ttyUSB4", "/dev/ttyUSB5",
		"/dev/ttyAMA0", "/dev/ttyAMA1", "/dev/ttyACM0", "/dev/ttyACM1",
		"/dev/rfcomm0", "/dev/rfcomm1", "/dev/ircomm0", "/dev/ircomm1",
		"/dev/cuau0", "/dev/cuau1", "/dev/cuau2", "/dev/cuau3", "/dev/cuaU0",
		"/dev/cuaU1", "/dev/cuaU2", "/dev/cuaU3"
};



/*using Serial Class */
#if 1
int cGPSDrv::OpenDevice()
{
	int retV = 0;
	retV = m_serialObj.Open(comports[16], 9600);
	if (retV == 1)
		retV = 0;
	return retV;
}
int cGPSDrv::OpenDevice(int port,int baute)
{
	int retV = 0;
	retV = m_serialObj.Open(comports[port], baute);
	if (retV == 1)
		retV = 0;
	return retV;
}
// Read a string from the serial device
int cGPSDrv::ReadDevice(char *buf,char FinalChar,unsigned int MaxNbBytes,unsigned int TimeOut_ms)
{
	int retv = 0;
	retv = m_serialObj.ReadString(buf,FinalChar,MaxNbBytes,TimeOut_ms);
	return retv;
}

int cGPSDrv::WriteDevice()
{
	return 0;
}

int cGPSDrv::CloseDevicce()
{
	m_serialObj.Close();
	return 0;
}
#endif


int cGPSDrv::RS232_GPS_OpenComport(int comport_number, int baudrate, const char *mode)
{
  int baudr,
      status;
  printf("enter RS232_GPS_OpenComport \n");
  if((comport_number>37)||(comport_number<0))
  {
    printf("illegal comport number\n");
    return(1);
  }

  switch(baudrate)
  {
    case      50 : baudr = B50;
                   break;
    case      75 : baudr = B75;
                   break;
    case     110 : baudr = B110;
                   break;
    case     134 : baudr = B134;
                   break;
    case     150 : baudr = B150;
                   break;
    case     200 : baudr = B200;
                   break;
    case     300 : baudr = B300;
                   break;
    case     600 : baudr = B600;
                   break;
    case    1200 : baudr = B1200;
                   break;
    case    1800 : baudr = B1800;
                   break;
    case    2400 : baudr = B2400;
                   break;
    case    4800 : baudr = B4800;
                   break;
    case    9600 : baudr = B9600;
                   break;
    case   19200 : baudr = B19200;
                   break;
    case   38400 : baudr = B38400;
                   break;
    case   57600 : baudr = B57600;
                   break;
    case  115200 : baudr = B115200;
                   break;
    case  230400 : baudr = B230400;
                   break;
    case  460800 : baudr = B460800;
                   break;
    case  500000 : baudr = B500000;
                   break;
    case  576000 : baudr = B576000;
                   break;
    case  921600 : baudr = B921600;
                   break;
    case 1000000 : baudr = B1000000;
                   break;
    case 1152000 : baudr = B1152000;
                   break;
    case 1500000 : baudr = B1500000;
                   break;
    case 2000000 : baudr = B2000000;
                   break;
    case 2500000 : baudr = B2500000;
                   break;
    case 3000000 : baudr = B3000000;
                   break;
    case 3500000 : baudr = B3500000;
                   break;
    case 4000000 : baudr = B4000000;
                   break;
    default      : printf("invalid baudrate\n");
                   return(1);
                   break;
  }

  int cbits=CS8,
      cpar=0,
      ipar=IGNPAR,
      bstop=0;

  if(strlen(mode) != 3)
  {
    printf("invalid mode \"%s\"\n", mode);
    return(1);
  }

  switch(mode[0])
  {
    case '8': cbits = CS8;
              break;
    case '7': cbits = CS7;
              break;
    case '6': cbits = CS6;
              break;
    case '5': cbits = CS5;
              break;
    default : printf("invalid number of data-bits '%c'\n", mode[0]);
              return(1);
              break;
  }

  switch(mode[1])
  {
    case 'N':
    case 'n': cpar = 0;
              ipar = IGNPAR;
              break;
    case 'E':
    case 'e': cpar = PARENB;
              ipar = INPCK;
              break;
    case 'O':
    case 'o': cpar = (PARENB | PARODD);
              ipar = INPCK;
              break;
    default : printf("invalid parity '%c'\n", mode[1]);
              return(1);
              break;
  }

  switch(mode[2])
  {
    case '1': bstop = 0;
              break;
    case '2': bstop = CSTOPB;
              break;
    default : printf("invalid number of stop bits '%c'\n", mode[2]);
              return(1);
              break;
  }

  Cport[comport_number] = open(comports[comport_number], O_RDWR | O_NOCTTY | O_NDELAY);
  if(Cport[comport_number]==-1)
  {
    perror("unable to open comport ");
    return(1);
  }

  error = tcgetattr(Cport[comport_number], old_port_settings + comport_number);
  if(error==-1)
  {
    close(Cport[comport_number]);
    perror("unable to read portsettings ");
    return(1);
  }
  memset(&new_port_settings, 0, sizeof(new_port_settings));  /* clear the new struct */

  new_port_settings.c_cflag = cbits | cpar | bstop | CLOCAL | CREAD;
  new_port_settings.c_iflag = ipar;
  new_port_settings.c_oflag = 0;
  new_port_settings.c_lflag = 0;
  new_port_settings.c_cc[VMIN] = 0;      /* block untill n bytes are received */
  new_port_settings.c_cc[VTIME] = 0;     /* block untill a timer expires (n * 100 mSec.) */

  cfsetispeed(&new_port_settings, baudr);
  cfsetospeed(&new_port_settings, baudr);

  error = tcsetattr(Cport[comport_number], TCSANOW, &new_port_settings);
  if(error==-1)
  {
    close(Cport[comport_number]);
    perror("unable to adjust portsettings ");
    return(1);
  }

  if(ioctl(Cport[comport_number], TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
    return(1);
  }

  status |= TIOCM_DTR;    /* turn on DTR */
  status |= TIOCM_RTS;    /* turn on RTS */

  if(ioctl(Cport[comport_number], TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
    return(1);
  }

  return(0);
}


int cGPSDrv::RS232_GPS_PollComport(int comport_number, char *buf, int size)
{
  int n;

  n = read(Cport[comport_number], buf, size);

  return(n);
}


int cGPSDrv::RS232_GPS_SendByte(int comport_number, unsigned char byte)
{
  int n;

  n = write(Cport[comport_number], &byte, 1);
  if(n<0)  return(1);

  return(0);
}


int cGPSDrv::RS232_GPS_SendBuf(int comport_number, unsigned char *buf, int size)
{
  return(write(Cport[comport_number], buf, size));
}


void cGPSDrv::RS232_GPS_CloseComport(int comport_number)
{
  int status;

  if(ioctl(Cport[comport_number], TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
  }

  status &= ~TIOCM_DTR;    /* turn off DTR */
  status &= ~TIOCM_RTS;    /* turn off RTS */

  if(ioctl(Cport[comport_number], TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
  }

  tcsetattr(Cport[comport_number], TCSANOW, old_port_settings + comport_number);
  close(Cport[comport_number]);
}

/*
Constant  Description
TIOCM_LE        DSR (data set ready/line enable)
TIOCM_DTR       DTR (data terminal ready)
TIOCM_RTS       RTS (request to send)
TIOCM_ST        Secondary TXD (transmit)
TIOCM_SR        Secondary RXD (receive)
TIOCM_CTS       CTS (clear to send)
TIOCM_CAR       DCD (data carrier detect)
TIOCM_CD        see TIOCM_CAR
TIOCM_RNG       RNG (ring)
TIOCM_RI        see TIOCM_RNG
TIOCM_DSR       DSR (data set ready)

http://man7.org/linux/man-pages/man4/tty_ioctl.4.html
*/

int cGPSDrv::RS232_GPS_IsDCDEnabled(int comport_number)
{
  int status;

  ioctl(Cport[comport_number], TIOCMGET, &status);

  if(status&TIOCM_CAR) return(1);
  else return(0);
}

int cGPSDrv::RS232_GPS_IsCTSEnabled(int comport_number)
{
  int status;

  ioctl(Cport[comport_number], TIOCMGET, &status);

  if(status&TIOCM_CTS) return(1);
  else return(0);
}

int cGPSDrv::RS232_GPS_IsDSREnabled(int comport_number)
{
  int status;

  ioctl(Cport[comport_number], TIOCMGET, &status);

  if(status&TIOCM_DSR) return(1);
  else return(0);
}

void cGPSDrv::RS232_GPS_enableDTR(int comport_number)
{
  int status;

  if(ioctl(Cport[comport_number], TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
  }

  status |= TIOCM_DTR;    /* turn on DTR */

  if(ioctl(Cport[comport_number], TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
  }
}

void cGPSDrv::RS232_GPS_disableDTR(int comport_number)
{
  int status;

  if(ioctl(Cport[comport_number], TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
  }

  status &= ~TIOCM_DTR;    /* turn off DTR */

  if(ioctl(Cport[comport_number], TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
  }
}

void cGPSDrv::RS232_GPS_enableRTS(int comport_number)
{
  int status;

  if(ioctl(Cport[comport_number], TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
  }

  status |= TIOCM_RTS;    /* turn on RTS */

  if(ioctl(Cport[comport_number], TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
  }
}

void cGPSDrv::RS232_GPS_disableRTS(int comport_number)
{
  int status;

  if(ioctl(Cport[comport_number], TIOCMGET, &status) == -1)
  {
    perror("unable to get portstatus");
  }

  status &= ~TIOCM_RTS;    /* turn off RTS */

  if(ioctl(Cport[comport_number], TIOCMSET, &status) == -1)
  {
    perror("unable to set portstatus");
  }
}

void cGPSDrv::RS232_GPS_cputs(int comport_number, const char *text)  /* sends a string to serial port */
{
  while(*text != 0)  RS232_GPS_SendByte(comport_number, *(text++));
}


