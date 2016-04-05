#include <string.h>
#include "adp_gps.h"

#if 0
char testbuff[14][128] =
{
		"$GNRMC,073508.079,V,,,,,0.00,0.00,090615,,,N*5F\r\n0",
		"$GPTXT,01,01,02,ANTSTATUS=OK*3B\r\n0",
		"$GNGGA,073509.079,,,,,0,0,,,M,,M,,*50\r\n0",
		"$GPGSA,A,1,,,,,,,,,,,,,,,*1E\r\n", "$BDGSA,A,1,,,,,,,,,,,,,,,*0F\r\n0",
		"$GPGSV,1,1,00*79\r\n", "$BDGSV,1,1,00*68\r\n0",
		"$GNRMC,073509.079,V,,,,,0.00,0.00,090615,,,N*5E\r\n0",
		"$GPTXT,01,01,02,ANTSTATUS=OK*3B\r\n0",
		"$GNGGA,073510.079,,,,,0,0,,,M,,M,,*58\r\n0",
		"$GPGSA,A,1,,,,,,,,,,,,,,,*1E\r\n", "$BDGSA,A,1,,,,,,,,,,,,,,,*0F\r\n0",
		"$GPGSV,1,1,00*79\r\n", "$BDGSV,1,1,00*68\r\n0"
};
#endif

extern pthread_mutex_t counter_lock;

//#define GPS_SERIAL_PORT 1
#define GPS_SERIAL_PORT 4

cGPSAdp::cGPSAdp()
{
	m_gpsDrv = new cGPSDrv();
	m_uartstate = UNINIT;
	memset(&m_ringBuf, 0, sizeof(m_ringBuf));
	m_ringBuf.readToalFlag = 0;
#if 0
	for (int i = 0; i < 14; i++)
	{
		memcpy(m_ringBuf.lineobj[i].linebuf,testbuff[i],strlen(testbuff[i]));
		//printf("%s",m_ringBuf.lineobj[i].linebuf);
	}
#endif
}

cGPSAdp::~cGPSAdp()
{
	if (m_gpsDrv != NULL)
	{
		m_gpsDrv->CloseDevicce();
		delete m_gpsDrv;
	}
}

//int cGPSAdp::gpsAdpReceive(char* buf, int num)
int cGPSAdp::gpsAdpReceive(GPS_INFO* gpsInfo)
{
	if (m_gpsDrv == NULL)
		return -1;

	//char tmp[128] = { 0 };
	int num = 0;
	if (m_uartstate != OPENED)
	{
		if (m_gpsDrv->OpenDevice(GPS_SERIAL_PORT,9600) != 0)
		{
			printf("Can not open comport\n");
			return (-1);
		}
		m_uartstate = OPENED;
	}
	// Read a maximum of 127 characters with a timeout of 1.2 seconds
	// The final character of the string must be a line feed ('\n')
	num = m_gpsDrv->ReadDevice(m_ringBuf.lineobj[0].linebuf, '\n', RING_BUF_LINE_LEN -1, 1200);
	if (num > 0)
	{
		m_ringBuf.lineobj[0].linebuf[num] = 0;
		this->gpsAdpParse(gpsInfo);
	}
	return 0;
#if 0
	int  n = 0, cport_nr = 16, /* /dev/ttyUSB0 */  bdrate = 9600; /* 9600 baud */
	char mode[] = { '8', 'N', '1', 0 };
	char c;
	static int bufIndex = 0;
	static int lineIndex = 0;
	static char store = 0;
    static unsigned int upcnt = 0;

    //static unsigned int headcnt = 0;
    //static unsigned int tailcnt = 0;

    /* return directly if there is another thread is reading data,
     * reading thread has higher priority than write thread.
     */
#if 0
     if (m_ringBuf.readToalFlag == 1)
     {
    	bufIndex = 0; //as we have discard one char this time, so this line is un-trust line, we should re-fill it,
    	store = 0;
    	printf("return\n");
		return 0;
     }
#endif
	if (m_uartstate != OPENED)
	{
		if (m_gpsDrv->RS232_GPS_OpenComport(cport_nr, bdrate, mode))
		{
			printf("Can not open comport\n");
			return (0);
		}
		m_uartstate = OPENED;
	}

	/*
	 * read one char each time
	 * speed 9600 b/s
	 * time = 1*1000/(9600/8) = 0.8333 ms
	 */
	n = m_gpsDrv->RS232_GPS_PollComport(cport_nr, &c, 1);
	if (n > 0)
	{
		if (c == '$') // read head char,start store,and reset bufIndex,set some flags
		{
			store = 1;
			bufIndex = 0;
			upcnt++;
#if 0
			if(headcnt != tailcnt)
				bufIndex = 0;

			headcnt++;
            printf("head------%d\n",headcnt);
#endif
			m_ringBuf.lineobj[lineIndex].updateSeqNum = upcnt;
			m_ringBuf.lineobj[lineIndex].writeflag = 1;
		}

		if (store == 1)
		{
			m_ringBuf.lineobj[lineIndex].linebuf[bufIndex] = c; //store it
			//if (c == '\n' || c == '\r') // arrive at end
		    if (c == '\n') // arrive at end
			{
#if 0
				tailcnt++;
			    printf("tail------%d\n",tailcnt);
#endif
#if 1
				if (bufIndex < (RING_BUF_LINE_LEN - 1))
					m_ringBuf.lineobj[lineIndex].linebuf[bufIndex + 1] = 0; //add an end flag
				else if (bufIndex >= RING_BUF_LINE_LEN)
					m_ringBuf.lineobj[lineIndex].linebuf[bufIndex] = 0; //add an end flag
				//clear write flag
				m_ringBuf.lineobj[lineIndex].writeflag = 0;
#endif
				//disable store
				store = 0;
				this->gpsAdpParse(gpsInfo);
				//printf("sec:%d\n",gpsInfo->D.second);
				//printf("%s",m_ringBuf.lineobj[lineIndex].linebuf);
				// increase lineIndex move to next line
#if 0
			    lineIndex++;
				if (lineIndex >= RING_BUF_DEEPTH)
				{
					lineIndex = 0;
					this->gpsAdpParse(gpsInfo);
#if 0
					//using for test
					for (int i = 1; i < 14; i++)
					{
						printf("%s",m_ringBuf.lineobj[i].linebuf);
					}
					m_ringBuf.readToalFlag = 1;
#endif
				}
#endif
			}
			bufIndex++;
			if (bufIndex >= RING_BUF_LINE_LEN)
			{
				bufIndex = 0;
				store = 0;
			}
		}
	}
	//n = m_gpsDrv->RS232_GPS_PollComport(cport_nr, buf, num);
	return 0;
#endif
}
int cGPSAdp::gpsAdpSend(char* buf, int num) {
	return 0;
}
double cGPSAdp::get_double_number(char *s)
{
	char buf[128];
	int i;
	double rev;
	i = GetComma(1, s);
	strncpy(buf, s, i);
	buf[i] = 0;
	rev = atof(buf);

	return rev;
}

double cGPSAdp::get_locate(double temp)
{
	int m;
	double n;
	m = (int) temp / 100;
	n = (temp - m * 100) / 60;
	n = n + m;
	return n;
}

////////////////////////////////////////////////////////////////////////////////
//得到指定序号的逗号位置
int cGPSAdp::GetComma(int num, char *str)
{
	int i, j = 0;
	int len = strlen(str);
	for (i = 0; i < len; i++)
	{
		if (str[i] == ',')
		{
			j++;
		}

		if (j == num)
			return i + 1;
	}
	return 0;
}
#if 0
char* cGPSAdp::GetComma(char* start, char* end, int num, char *str)
{
	char* pbuf = str;
	int n = 0;
	while (*pbuf != '$') {
		pbuf = movePointer(start, pbuf, 1, end);
		if (*pbuf == ',') {
			n++;
		}

		if (n > 20) {
			break;
		}
		if (n == num)
			break;
	}
	return pbuf;
}
#endif
/* get the num of ","*/
int cGPSAdp::GetCommaNum(char *str)
{
	int i, j = 0;
	int len = strlen(str);
	for (i = 0; i < len; i++)
	{
		if (str[i] == ',')
		{
			j++;
		}
	}
	return j;
}
////////////////////////////////////////////////////////////////////////////////
//将世界时转换为北京时
void cGPSAdp::UTC2BTC(date_time *gps_time)
{
	//***************************************************
	//如果秒号先出,再出时间数据,则将时间数据+1秒
	gps_time->second++; //加一秒
	if (gps_time->second > 59)
	{
		gps_time->second = 0;
		gps_time->minute++;
		if (gps_time->minute > 59)
		{
			gps_time->minute = 0;
			gps_time->hour++;
		}
	}

	//***************************************************
	gps_time->hour += 8;
	if (gps_time->hour > 23)
	{
		gps_time->hour -= 24;
		gps_time->day += 1;
		if (gps_time->month == 2 || gps_time->month == 4 || gps_time->month
				== 6 || gps_time->month == 9 || gps_time->month == 11)
		{
			if (gps_time->day > 30)
			{
				gps_time->day = 1;
				gps_time->month++;
			}
		}
		else
		{
			if (gps_time->day > 31)
			{
				gps_time->day = 1;
				gps_time->month++;
			}
		}
		if (gps_time->year % 4 == 0)
		{//
			if (gps_time->day > 29 && gps_time->month == 2)
			{
				gps_time->day = 1;
				gps_time->month++;
			}
		}
		else
		{
			if (gps_time->day > 28 && gps_time->month == 2)
			{
				gps_time->day = 1;
				gps_time->month++;
			}
		}
		if (gps_time->month > 12)
		{
			gps_time->month -= 12;
			gps_time->year++;
		}
	}
}
char* movePointer(char* start,char* pbuf,int num,char* end)
{
	char* preturn = NULL;
	if ((end - pbuf) > num)
		preturn = pbuf + num;
	else
		preturn = start + ((pbuf + num) - end);

	return preturn;
}

int cGPSAdp::reBuildBuf(char* buf,int num)
{
#if 0
    char* pstart = NULL;
    char* pend = NULL;
	char headGNRMC[1][7] = { "$GNRMC" };
	char headGNGGA[1][7] = { "$GNGGA" };
#if 0
	char headGPTXT[1][7] = { "$GPTXT" };
	char headGPGSA[1][7] = { "$GPGSA" };
	char headGPGSV[1][7] = { "$GPGSV" };
	char headBDGSA[1][7] = { "$BDGSA" };
	char headBDGSV[1][7] = { "$BDGSV" };
#endif
	if (buf == NULL || num <= 0 || num > 128)
		return -1;
	//step 1 find head
	pstart = strstr(buf, headGNRMC[0]);
	if(pstart != NULL)
	{
	   // try to find the tail
	   pend = strstr(pstart,"\r\n");
	   if (pend != NULL)
	   {
		    //found it
			strncpy(m_pGNRMC, pstart, pend - pstart);
	   }
		else
		{
			//not found
			strncpy(m_buf_pool[0], pstart, num - (pstart - buf));
		}
	}
#endif
	return 0;

}
/*
 *
$GNRMC,054307.083,V,,,,,0.00,0.00,090615,,,N*56

$GPTXT,01,01,02,ANTSTATUS=OK*3B

$GNGGA,054308.083,,,,,0,0,,,M,,M,,*57

$GPGSA,A,1,,,,,,,,,,,,,,,*1E

$BDGSA,A,1,,,,,,,,,,,,,,,*0F

$GPGSV,1,1,00*79

$BDGSV,1,1,00*68
*
**/

int cGPSAdp::gpsAdpParse(GPS_INFO* gpsInfo)
{

	char *pbuf = NULL;
	char headGNRMC[1][7] = { "$GNRMC" };
	char headGNGGA[1][7] = { "$GNGGA" };
#if 0
	char headGPTXT[1][7] = { "$GPTXT" };
	char headGPGSA[1][7] = { "$GPGSA" };
	char headGPGSV[1][7] = { "$GPGSV" };
	char headBDGSA[1][7] = { "$BDGSA" };
	char headBDGSV[1][7] = { "$BDGSV" };
#endif

    this->m_ringBuf.readToalFlag = 1; //set read flag first to stop write thread operate ringBuf anymore
    /*start parse it,line by line*/
    //for (int i = 0; i < RING_BUF_DEEPTH; i++)
    for (int i = 0; i < 1; i++)
    {

#if 0
    	if (this->m_ringBuf.lineobj[i].writeflag == 1)
    	{
    		//printf("1\n");
			continue;
    	}
#endif
       pbuf = strstr(this->m_ringBuf.lineobj[i].linebuf, headGNRMC[0]); //look for GNRMC
       if (pbuf != NULL)
		{
			//"$GNRMC,054307.083,V,,,,,0.00,0.00,090615,,,N*56"
    	   pthread_mutex_lock(&counter_lock);
			gpsInfo->D.hour = (pbuf[7] - '0') * 10 + (pbuf[8] - '0');
			gpsInfo->D.minute = (pbuf[9] - '0') * 10 + (pbuf[10] - '0');
			gpsInfo->D.second = (pbuf[11] - '0') * 10 + (pbuf[12] - '0');
			int tmp = GetComma(9, pbuf);
			gpsInfo->D.day = (pbuf[tmp + 0] - '0') * 10 + (pbuf[tmp + 1] - '0');
			gpsInfo->D.month = (pbuf[tmp + 2] - '0') * 10 + (pbuf[tmp + 3] - '0');
			gpsInfo->D.year = (pbuf[tmp + 4] - '0') * 10 + (pbuf[tmp + 5] - '0') + 2000;

			gpsInfo->status = pbuf[GetComma(2, pbuf)];
			gpsInfo->latitude = get_locate(get_double_number(&pbuf[GetComma(3,pbuf)]));
			if (pbuf[GetComma(4, pbuf)] != ',')
				gpsInfo->NS = pbuf[GetComma(4, pbuf)];
			else
				gpsInfo->NS = 0;
			gpsInfo->longitude = get_locate(get_double_number(&pbuf[GetComma(5,pbuf)]));
			if (pbuf[GetComma(6, pbuf)] != ',')
				gpsInfo->EW = pbuf[GetComma(6, pbuf)];
			else
				gpsInfo->EW = 0;
			gpsInfo->speed = get_double_number(&pbuf[GetComma(7, pbuf)]);
			//UTC2BTC(&gpsInfo->D);
			memcpy(gpsInfo ->seq, pbuf + GetComma(12, pbuf), 4);
			pthread_mutex_unlock(&counter_lock);
			// printf("second:%d\n",gpsInfo->D.second);
			continue;
		}
        pbuf = strstr(this->m_ringBuf.lineobj[i].linebuf, headGNGGA[0]); ////look for GNGGA
		if (pbuf != NULL)
		{
			//"$GNGGA,054308.083,,,,,0,0,,,M,,M,,*57"
			pthread_mutex_lock(&counter_lock);
			gpsInfo->high = get_double_number(&pbuf[GetComma(9, pbuf)]);
			pthread_mutex_unlock(&counter_lock);
		}
       //printf("second:%d\n",gpsInfo->D.second);
    }
    this->m_ringBuf.readToalFlag = 0;// after parse all info ,restore this flag.
    return 0;
}
#if 0
int cGPSAdp::gpsAdpParse(char* buf, int num,GPS_INFO* gpsInfo) {

    char* pbuf = buf;
    //char* pstart = buf;
	char* pend = buf + num;
	char* pGPRMC = NULL;
	char* pGPGGA = NULL;
	char* pdou_c[20];
	char* pdou_a[20];
	//bool mc_found = false;
	//bool ga_found = false;
	bool gprmc_complete = false;
	bool gpgga_complete = false;
	int doucnt = 0;
	//int linelen = 0;
	char tmpbuf[256];
	char headGNRMC[1][7] = { "$GNRMC" };
	char headGNGGA[1][7] = { "$GNGGA" };
	//char headBDGSV[1][7] = { "$BDGSV" };
	if (buf == NULL || num < 6 || gpsInfo == NULL)
		return(-1);
	for (int i = 0; i < 20; i++)
	{
		pdou_c[i] = NULL;
		pdou_a[i] = NULL;
	}

	while (pbuf < pend)
	{
		if((pend - pbuf) < MIN_GPRMC_LINE_LEN) /* make sure there is one valid line at least */
			break;

		//step 1 find header
		/**************************************parse GNRMC***************************************************/
		if (strncmp(pbuf, headGNRMC[0], 6) == 0)
		{
			/* Get the pGPRMC head pointer*/
			pGPRMC = pbuf;
			//mc_found = true;
			/* judge if it is a complete line */
			/* count its ',' if the number of ',' != 13 ,not complete line ,do not need to parse it */
			//step2 check it
			while(*pGPRMC != '\n')
			{
				if (*pGPRMC == ',') {
					pdou_c[doucnt] = pGPRMC;
					doucnt++;
				}
				if (doucnt >= ERRORNUM_OF_DOU) {
					break;
				}
				pGPRMC++;
				if((pGPRMC >= pend) || (pend - pGPRMC) <  MIN_GPRMC_LINE_LEN)
					break;
			}
			/**************************************************************************************************/
			if (doucnt == GPRMC_NUM_OF_DOU) {
				gprmc_complete = true;
			}
			doucnt = 0;
			pGPRMC = pbuf;
			//step 3 start parse
			if (gprmc_complete == true)
			{
				//"GPRMC"
				//$GNRMC,054307.083,V,,,,,0.00,0.00,090615,,,N*56
				gpsInfo->D.hour = (*(pGPRMC + 7) - '0') * 10 + (*(pGPRMC + 8) - '0');
				gpsInfo->D.minute = (*(pGPRMC + 9) - '0') * 10 + (*(pGPRMC + 10) - '0');
				gpsInfo->D.second = (*(pGPRMC + 11) - '0') * 10 + (*(pGPRMC + 12) - '0');
				//the nith 9 ',' is the date
				gpsInfo->D.day = (*(pdou_c[8] + 1) - '0') * 10 + (*(pdou_c[8] + 2) - '0');
				gpsInfo->D.month = (*(pdou_c[8] + 3) - '0') * 10 + (*(pdou_c[8] + 4) - '0');
				gpsInfo->D.year = (*(pdou_c[8] + 5) - '0') * 10 + (*(pdou_c[8] + 6) - '0') + 2000;
				//the second 2 dou ',' is the status
				if (*(pdou_c[1] + 1) != ',')
					gpsInfo->status = *(pdou_c[1] + 1);
				//the third dou is latitude
				memcpy(tmpbuf, pdou_c[2] + 1, pdou_c[3] - pdou_c[2] - 1);
				tmpbuf[pdou_c[3] - pdou_c[2] - 1] = 0;
				gpsInfo->latitude = get_locate(atof(tmpbuf));
				//the fourth dou is direction
				gpsInfo->NS = *(pdou_c[3] + 1);
				//the fifth dou is longtidue
				memcpy(tmpbuf, pdou_c[4] + 1, pdou_c[5] - pdou_c[4] - 1);
				tmpbuf[pdou_c[5] - pdou_c[4] - 1] = 0;
				gpsInfo->longitude = get_locate(atof(tmpbuf));
				//the sixth dou is direction
				gpsInfo->EW = *(pdou_c[5] + 1);
				//the seventh is speed
				memcpy(tmpbuf, pdou_c[6] + 1, pdou_c[7] - pdou_c[6] - 1);
				tmpbuf[pdou_c[7] - pdou_c[6] - 1] = 0;
				gpsInfo->speed = atof(tmpbuf);
				//get seq num
				memcpy(gpsInfo->seq, pdou_c[11] + 1, 4);
				//chage to BeiJing Time
				UTC2BTC(&gpsInfo->D);
			}
		}
		/**************************************parse GNGGA***************************************************/
		else if (strncmp(pbuf, headGNGGA[0], 6) == 0)
		{
			pGPGGA = pbuf;
			//step2 check it
			while (*pGPGGA != '\n')
			{
				if (*pGPGGA == ',')
				{
					pdou_a[doucnt] = pGPGGA;
					doucnt++;
				}
				if (doucnt >= ERRORNUM_OF_DOU)
					break;
				pGPGGA++;
				if ((pGPGGA >= pend) || (pend - pGPGGA) < MIN_GPRMC_LINE_LEN)
					break;
			}
			pGPGGA = pbuf;
			if (doucnt == GPGGA_NUM_OF_DOU)
				gpgga_complete = true;

			doucnt = 0;
			//step 3 start parse it
			if(gpgga_complete == true)
			{
				//the nineth dou is hight
				memcpy(tmpbuf, pdou_a[8] + 1, pdou_a[9] - pdou_a[8] -1);
				tmpbuf[pdou_a[9] - pdou_a[8] - 1] = 0;
				gpsInfo->high = atof(tmpbuf);
			}

		}

		pbuf++;
	}


#if 0

	/* find head */
// step 1 find head
	while (pbuf < pend) {

		if ((*pbuf) == '$') {
			if (strncmp(pbuf, headGNRMC[0], 6) == 0) {
				pGPRMC = pbuf;
				mc_found = true;
			} else if (strncmp(pbuf, headGNGGA[0], 6) == 0) {
				pGPGGA = pbuf;
				ga_found = true;

			} else if (strncmp(pbuf, headBDGSV[0], 6) == 0) {

			}
		}
		if ((mc_found == true) || (ga_found == true))
			break;
		pbuf++;
	}
// step 2 judge if it is complete sentence
// look for the next $ and calculate the num of ','
	if (pGPRMC != NULL) {
		linelen = strlen(pGPRMC);
		if (linelen > 20 && linelen < num) {
			pbuf = pGPRMC;
			for (int i = 0; i < linelen; i++) {
				if (*pbuf == ',') {
					pdou_c[doucnt] = pbuf;
					doucnt++;
				}
				if (doucnt >= ERRORNUM_OF_DOU) {
					break;
				}
				pbuf++;
				//pbuf = movePointer(buf,pbuf,1,pend);
			}
		}
	}

	if (doucnt == GPRMC_NUM_OF_DOU)
	{
		gprmc_complete = true;
	}
	doucnt = 0;

	if (pGPGGA != NULL) {
		linelen = strlen(pGPGGA);
		if (linelen > 20 && linelen < num) {
			pbuf = pGPGGA;
			for (int i = 0; i < linelen; i++) {
				if (*pbuf == ',') {
					pdou_a[doucnt] = pbuf;
					doucnt++;
				}
				if (doucnt >= ERRORNUM_OF_DOU) {
					break;
				}
			}
			pbuf++;
			//pbuf = movePointer(buf,pbuf,1,pend);
		}
	}

	if (doucnt == GPGGA_NUM_OF_DOU)
	{
		gpgga_complete = true;
	}
	doucnt = 0;

// step 3 parse it

	if (pGPRMC != NULL && gprmc_complete == true)
	{
		//"GPRMC"
		//char* ginfo = movePointer(buf,pGPRMC,7,pend);
		//int ggg =
		gpsInfo->D.hour = (*(movePointer(buf,pGPRMC,7,pend)) - '0') * 10 + (*(movePointer(buf,pGPRMC,8,pend)) - '0');
		gpsInfo->D.minute = (*(movePointer(buf,pGPRMC,9,pend)) - '0') * 10 + (*(movePointer(buf,pGPRMC,10,pend)) - '0');
		gpsInfo->D.second = (*(movePointer(buf,pGPRMC,11,pend)) - '0') * 10 + (*(movePointer(buf,pGPRMC,12,pend)) - '0');
		//the nith ',' is the date
        //char *pp = pdou[8] + 1;
		gpsInfo->D.day = (*(pdou_c[8] + 1) - '0') * 10 + (*(pdou_c[8] + 2) - '0');
		gpsInfo->D.month = (*(pdou_c[8] + 3) - '0') * 10 + (*(pdou_c[8] + 4) - '0');
		gpsInfo->D.year = (*(pdou_c[8] + 5) - '0') * 10 + (*(pdou_c[8] + 6) - '0') + 2000;
        //the second dou is the status
		if (*(pdou_c[1] + 1) != ',')
			gpsInfo->status = *(pdou_c[1] + 1);
		//the third dou is latitude

		memcpy(tmpbuf, pdou_c[2] + 1, pdou_c[3] - pdou_c[2] -1);
		tmpbuf[pdou_c[3] - pdou_c[2] - 1] = 0;
		gpsInfo->latitude = get_locate(atof(tmpbuf));
		//the fourth dou is direction
		gpsInfo->NS = *(pdou_c[3] + 1);
		//the fifth dou is longtidue
		memcpy(tmpbuf, pdou_c[4] + 1, pdou_c[5] - pdou_c[4] - 1);
		tmpbuf[pdou_c[5] - pdou_c[4] - 1] = 0;
		gpsInfo->longitude = get_locate(atof(tmpbuf));
		//the sixth dou is direction
	    gpsInfo->EW = *(pdou_c[5] + 1);
	    //the seventh is speed
	    memcpy(tmpbuf, pdou_c[6] + 1, pdou_c[7] - pdou_c[6] - 1);
	    tmpbuf[pdou_c[7] - pdou_c[6] - 1] = 0;
		gpsInfo->speed = atof(tmpbuf);
		//chage to BeiJing Time
		UTC2BTC(&gpsInfo->D);
	}
	if(pGPGGA != NULL && gpgga_complete == true)
	{
		//the nineth dou is hight
		memcpy(tmpbuf, pdou_a[8] + 1, pdou_a[9] - pdou_a[8] -1);
		tmpbuf[pdou_a[9] - pdou_a[8] - 1] = 0;
		gpsInfo->high = atof(tmpbuf);
	}
#endif
#if 0
	start = strstr(buf, "$"); //find head first
	while (start != NULL)
	{
		linelen = strlen(start);
		if(linelen < 6)
			return(-1);
		c = start[5];
		tmpcnt = GetCommaNum(start);

		if (c == 'C')
		{
			//"GPRMC"
			if (tmpcnt != 11) //make sure it is complete one line
				break;
			gpsInfo->D.hour = (start[7] - '0') * 10 + (start[8] - '0');
			gpsInfo->D.minute = (start[9] - '0') * 10 + (start[10] - '0');
			gpsInfo->D.second = (start[11] - '0') * 10 + (start[12] - '0');
			tmp = GetComma(9, start);
			gpsInfo->D.day = (start[tmp + 0] - '0') * 10 + (start[tmp + 1] - '0');
			gpsInfo->D.month = (start[tmp + 2] - '0') * 10 + (start[tmp + 3] - '0');
			gpsInfo->D.year = (start[tmp + 4] - '0') * 10 + (start[tmp + 5] - '0')
					+ 2000;

			gpsInfo->status = start[GetComma(2, start)];
			gpsInfo->latitude = get_locate(get_double_number(&start[GetComma(3,
					start)]));
			gpsInfo->NS = start[GetComma(4, start)];
			gpsInfo->longitude = get_locate(get_double_number(&start[GetComma(5,
					start)]));
			gpsInfo->EW = start[GetComma(6, start)];
			gpsInfo->speed = get_double_number(&start[GetComma(7, start)]);
			UTC2BTC(&gpsInfo->D);

		}

		if (c == 'A')
		{
			//"$GPGGA"
			if (tmpcnt < 11)
				break;
			gpsInfo->high = get_double_number(&start[GetComma(9, start)]);
		}


		start++;
		if ((start - buf) > num)
			break;
		start = strstr(start, "$");
	}
#endif
    return 0;
}
#endif
int cGPSAdp::gpsAdpUpdateGpsInfo() {
	return 0;
}
