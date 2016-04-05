
#include <UTouch.h>
#include <Keypad.h>
#include <ModbusMaster.h>
#include <UTFT.h>
#include <UTFT_Buttons.h>



#include "smb_menu.h"

#include "smb_hmi.h"

#define imagedatatype  unsigned int

// #define HAVE_RTC

#ifdef HAVE_RTC
#include <DS1307RTC.h>
#include <Time.h>
#include <Wire.h> 
#endif

// global variables define
// dataSource define: the data exchange with BBB via Modbus in real-time
// instantiate ModbusMaster object, serial port 0, Modbus slave ID 1
#define SERIAL_PORT 3
#define MODBUS_SLAVE_ID 1
// ModbusMaster SMBModMaster(SERIAL_PORT_1, MODBUS_SLAVE_ID);
modbusDB modDB(SERIAL_PORT, MODBUS_SLAVE_ID);

#define MAX_SIZE_BAOZHAN_FONT  36864 // 64 stations * 8 name * 72 bytes/char (24x24 pixels)
#define MODBUS_RD_TIME_BAOZHAN_FONT 288 // 36864/(64*2): 64 regs per time in this modbus lib


// RTC
#ifdef HAVE_RTC
tmElements_t tm;


const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#endif

// top & bottom: special menu
Menu *pTopZone;
Menu *pBottZone;

Menu *pDefMenu;
// tabs are merged into each menus
Menu *pXinXi, *pBaoZhan, *pSheZhi;


Menu *last;        // Single entry for back button
Menu *current;     // Current menu location

// device define
#define LCD_SIZE_WIDTH 480
#define LCD_SIZE_HEIGHT 320
#define MIDDLE_X 240 // LCD_SIZE_WIDTH/2
#define MIDDLE_Y 160 // LCD_SIZE_HEIGHT/2

SMBprint SMBprt(HX8357B, 38, 39, 40, 41, &modDB);
const byte numRows = 4; //number of rows on the keypad
const byte numCols = 4; //number of columns on the keypad

//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = { 9, 8, 7, 6 }; //Rows 0 to 3
byte colPins[numCols] = { 5, 4, 3, 2 }; //Columns 0 to 3

//initializes an instance of the Keypad class
SMBKeypad SMBKPad = SMBKeypad(makeKeymap(keymap), rowPins, colPins, numRows,
                          numCols);

// one-key alarm: bind to int.2

/*
Board	int.0	int.1	int.2	int.3	int.4	int.5 	 	 	 
Mega2560	2	3		21		20		19		18
*/
#define INT_NUM 5
#define ONE_KEY_ALARM_PIN 18




// button panel for menus
typedef UTFT_Buttons SMBTabPanel;

SMBTabPanel tabPanel(&SMBprt, (UTouch *) 0);


// fonts pixels declaration
// extern

extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];

extern uint8_t tabFont[];
extern uint8_t vendorName[];
extern uint8_t thinNumber[];
extern uint8_t vendorLogo[];
extern uint8_t vendorVision[];
extern uint8_t errorFont[];

// bitmap
//extern imagedatatype vendorLogo[];
//extern imagedatatype vendorVision[];

// menu list: menu---contents not top, bottom, border or tab
// MENU overview:
// ----------------------------------------------------------------------------
//									TOP: bus info, time
// ----------------------------------------------------------------------------
// *閳ユ柡锟斤拷(*:refreshZoneStart: (rzStart_x, rzStart_y))
// 娣囷拷 |
// 閹拷 |				------------		-			|
// 閳ユ柡锟斤拷					 |			  -   -			|
// 閹讹拷 |					 |			 -	    -		|-------
// 缁旓拷 |					 | 			-	     -		|		\
// 閳ユ柡锟斤拷					 |		   ------------		|		 /
// 鐠侊拷 |				     |		  -	     	   -	|		/
// 缂冿拷 |					 |	  	  -	     	   -	--------
// 閳ユ柡锟斤拷																	       *(*:refreshZoneEnd: (rzEnd_x, rzEnd_y))
// ----------------------------------------------------------------------------
//									BOTTOM閿涙endor, logo
// ----------------------------------------------------------------------------
// TODO: shezhi: can using MENWIZ totally if treat refreshZone as a TEXT screen when future needed

void localInitMenu() {

  SMBTabId tId = 0;
  UC idxLabel = 0;

  // create menu
  pTopZone = new Menu("top", idxLabel++, Menu::LCDOUT, Menu::KEYIN,
                      Menu::REDRAW);
  pBottZone = new Menu("bott", idxLabel++, Menu::LCDOUT, Menu::KEYIN,
                       Menu::REDRAW);

  pXinXi = new Menu("xinXi", idxLabel++, Menu::LCDOUT, Menu::KEYIN,
                    Menu::REDRAW);
  /* pBaoZhan = new Menu("baoZhan", idxLabel++, Menu::LCDOUT, Menu::KEYIN,
                      Menu::REDRAW);*/
  pSheZhi = new Menu("sheZhi", idxLabel++, Menu::LCDOUT, Menu::KEYIN,
                     Menu::REDRAW);

  // link menu
  pXinXi->prev = pSheZhi;
  pXinXi->next = pSheZhi;
  // pBaoZhan->prev = pXinXi;
  // pBaoZhan->next = pSheZhi;
  pSheZhi->prev = pXinXi;
  pSheZhi->next = pXinXi;
  // bind key processing
  pXinXi->processKey = SMBKeypad::procKeyXinXi;
  // pBaoZhan->processKey = procKeyBaoZhan;
  pSheZhi->processKey = SMBKeypad::procKeySheZhi;

  // bind display
  pXinXi->display = displayXinXi;
  // pBaoZhan->display = displayBaoZhan;
  pSheZhi->display = displaySheZhi;

  // create tabs for menu
  // addButton(uint16_t x, uint16_t y, uint16_t width, uint16_t height閿涳拷
  tabPanel.setTextFont(tabFont);
  // tabPanel.setSymbolFont(Dingbats1_XL);
  tabPanel.setButtonColors(VGA_YELLOW, VGA_RED, VGA_YELLOW, VGA_BLUE,
                           VGA_GRAY);
  // labelStr = "xx"; // 娣団剝浼� 
  tId = tabPanel.addButton(10, 20, 50, 100, "01");
  // add to xinXi menu
  pXinXi->tabId = tId;

  // labelStr = "bz"; // 閹躲儳鐝�  // tId = tabPanel.addButton(10, 20, 300, 30, "bz");
  // add to baoZhan menu
  // pBaoZhan->tabId = tId;

  // labelStr = "sz"; // 鐠佸墽鐤� 
  tId = tabPanel.addButton(10, 121, 50, 100, "23");
  // add to baoZhan menu
  pSheZhi->tabId = tId;


}

void oneKeyAlarm()
{
	static UC keyPressTime = 0;
	
	Serial.println(F("One-key alarm in "));
	// for mis-operation
	if(keyPressTime++ >= 2)
	{
		
		Serial.println(F("One-key alarm activate!!!!!!!! press times: "));
		Serial.println(keyPressTime);
		keyPressTime = 0;
		// repeat 3 times for sure
		modDB.sendOneKeyAlarm();
		Alarm();
		modDB.sendOneKeyAlarm();
		Alarm();
		modDB.sendOneKeyAlarm();
		Alarm();
	}
	
}

// RTC
#ifdef HAVE_RTC
bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

#endif


int Buzzerpin = 11;

void Alarm() {
  for (int i = 0; i < 100; i++) {
    digitalWrite(Buzzerpin, HIGH); 
	delay(2);
    digitalWrite(Buzzerpin, LOW); 
    delay(2); 
    }
}




void setup() {

  UINT idx = 0;
  // int degree = 0;
  Menu *pMenuTmp;
    bool parse=false;
  bool config=false;

  // device initial
  pinMode(Buzzerpin, OUTPUT);
  pinMode(ONE_KEY_ALARM_PIN, INPUT_PULLUP);


  // debug serial0
  Serial.begin(9600);
  
  #ifdef HAVE_RTC
  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }



    while (!Serial) 
    {
      Serial.print(" wait for Arduino Serial Monitor"); // wait for Arduino Serial Monitor
    }
  delay(200);
  if (parse && config) {
    Serial.print("DS1307 configured Time=");
    Serial.print(__TIME__);
    Serial.print("Date=");
    Serial.println(__DATE__);
  } else if (parse) {
    Serial.println("DS1307 Communication Error :-{");
    Serial.println("Please check your circuitry");
  } else {
    Serial.print("Could not parse info from the compiler, Time=");
    Serial.print(__TIME__);
    Serial.print("Date=");
    Serial.print(__DATE__);
    Serial.println("");
  }
#endif
  SMBprt.InitLCD();
  SMBprt.clrScr();


  // modbus serial3
  modDB.SMBModMaster.begin(9600);

  

  // get infos (Font, ...) from BBB via modbus
  // meanwhile, vendor AD show

#define INITIAL 0
#define WAIT 1
	SMBprt.printVendorLogo(INITIAL);

#define ROTATE_DEG 1

  idx = 0;
  do {
  
  // wait for BBB ready
  	while (OK != modDB.isBBBReady())
  	{
  		SMBprt.printVendorLogo(WAIT);
  	}
  	
  	Serial.println(F("BBB ready now!"));
  	// get infos from BBB
    if (0 == idx) // first time
    {
      while (OK != modDB.loginBBB());
      modDB.getInfosFromBBB();
    }


    SMBprt.printVendorLogo(WAIT);
    // degree += ROTATE_DEG;

  }while(0);


  // real menu create
  localInitMenu();

  SMBprt.clrScr();
  SMBprt.printBorder();
  SMBprt.printTop();
  SMBprt.printBottom();
 
 

  // main menus
  // default menu show: high-lighten current menu tab
  // initial def menu
  pDefMenu = pXinXi;

  current = last = pXinXi;

  pDefMenu->display(*pDefMenu, *last);

}

// still need one key interrupt for ONE-key alarm
void loop() {

  // if key pressed
  US X_POS = 10;
  char userInNumber[3] = { };
  UC userInNumberCnt = 0;
  UC userInDone = 0;

  UC *pTotalStations = &(modDB.tBusInfo.totalStations);
  UC *pManuCurrStation = &(modDB.tBusInfo.manuCurrStation);
  UC *pManuAPC = &(modDB.tBusInfo.manuPassengerCount);
  *pManuCurrStation = 0xff; // inintial as invalid value
  *pManuAPC = 0xff;
  
  SMBprt.setColor(255, 0, 0);
  SMBprt.setFont(SevenSegNumFont);
  
  
  while (1) {
  
	//Serial3.print(F("this is Serial3 in Arduino!"));
	//Serial.print(F("Start: "));
	//Serial.println(millis());
    char keypressed = SMBKPad.getKey();

    if (keypressed != NO_KEY) {
      Serial.print(keypressed);
      // only baoZhan will handle user input number
      if ((isdigit(keypressed) || ('#' == keypressed)) && (current == pSheZhi)) {
        // data key
        if (userInNumberCnt < 2) // max input number is 2
        {
          userInNumber[userInNumberCnt] = keypressed;
          userInNumber[userInNumberCnt + 1] = '\0';
          userInNumberCnt++;
          // SMBprt.popMsg(userInNumber, MIDDLE_X, MIDDLE_Y, SevenSegNumFont);
        } else {
          Serial.print("input number is exceeded than 2!\n");
        }
         if ('#' == keypressed) // 绾喛顓�     
        {
          userInDone = 1;
          Serial.print("confirm station number!\n");
        }
 

      } else {

       // control key
        if ('A' == keypressed) // 娑撳﹣绔寸粩锟�      
        {
                	Serial.print(F("manul station from: "));
        	Serial.print(*pManuCurrStation);
          *pManuCurrStation = (*pManuCurrStation - 1 + *pTotalStations) % *pTotalStations;
               	Serial.print(F(" to:  "));
        	Serial.print(*pManuCurrStation);   
        	Serial.println(F(""));
        } 
        else if ('B' == keypressed) // 娑撳绔寸粩锟�      
        {
        	Serial.print(F("manul station from: "));
        	Serial.print(*pManuCurrStation);
          *pManuCurrStation = (*pManuCurrStation + 1) % *pTotalStations;
               	Serial.print(F(" to:  "));
        	Serial.print(*pManuCurrStation);   
        	Serial.println(F(""));
        } 
        else
        {
   /*normal control key*/
        switch (keypressed) {
          case 'C': // 娑撳﹣绔撮懣婊冨礋
            if ((Menu *)0 != current->prev)
            {
              last = current;
              current = current->prev;
              
              // redraw menu
              current->disFlg = Menu::REDRAW;
            }
            Serial.print("prev menu!\n");
            break;
          case 'D': // 娑撳绔撮懣婊冨礋
            if ((Menu *)0 != current->next)
            {
            	last = current;
              current = current->next;
              // redraw menu
              current->disFlg = Menu::REDRAW;
            }
            Serial.print("next menu!\n");
            break;
          case '*': // 鏉╂柨娲�           
            current = pDefMenu;
            current->disFlg = Menu::REDRAW;
            Serial.print("return to default!\n");
            break;

        }
        }

      }
      // current menu proc key pressed: data key
      if (userInDone) {
        userInDone = 0;
        current->processKey(*current, userInNumber);
      }
       
    }
	//Serial.print(F("Key done: "));
	//Serial.println(millis());
          // if menu need to be displayed, then display: first time, key pressed, display time out (back to default), new data updated (need to refresh screen)
      SMBprt.printTop(); // might be need to update data, such as datetime, driver name, ...
      current->display(*current, *last);  
      
      // if errors happen, popmsg
      SMBprt.popMsg(modDB.errNo);
       
    	//Serial.print(F("current menu done: "));
	//Serial.println(millis());
      // update data via Modbus: sys info (date, etc), smb info
      modDB.parseModbusMsg();
	//Serial.print(F("Modbus : "));
	//Serial.println(millis());
      
      
    // one-key alarm
    //Serial.print(F("DI: "));
    //Serial.println(digitalRead(ONE_KEY_ALARM_PIN));
    #if 1
  if(digitalRead(ONE_KEY_ALARM_PIN)==LOW)  //由于本例检测上升沿触发，所以先检测输入是否低电平，
  {
    delay(10); //然后延时一段时间，
    if(digitalRead(ONE_KEY_ALARM_PIN)==HIGH) //然后检测是不是电平变为高了。是的话，就是刚好按钮按下了。
    {
      // digitalWrite(LED,onoff);  //写入当前LED状态onoff，
      //onoff=(!onoff); //然后LED状态反转，以便下一次使用。
      delay(10);  //延时一段时间，防止按钮突然断开再按下。
      while(digitalRead(ONE_KEY_ALARM_PIN)==LOW) //判断按钮状态，如果仍然按下的话，等待松开。防止一直按住导致LED输出端连续反转
      {
        delay(1);
      }
      oneKeyAlarm();
    }
  }     
  #endif     
    
	  
	  // RTC
	    #ifdef HAVE_RTC
	  
	    if (RTC.read(tm)) {
	    
	    // update RTC time
	    if(modDB.tSysInfo.tDateTime.year != tmYearToCalendar(tm.Year))
	    {
	    modDB.tSysInfo.tDateTime.year = tmYearToCalendar(tm.Year);
	    modDB.tSysInfo.tDateTime.displayBitMap.year = 1;
	    }
	    
	    if(modDB.tSysInfo.tDateTime.month != tm.Month)
	    {
	    modDB.tSysInfo.tDateTime.month = tm.Month;
	    modDB.tSysInfo.tDateTime.displayBitMap.month = 1;
	    }
	    	    
	    if(modDB.tSysInfo.tDateTime.day != tm.Day)
	    {
	    modDB.tSysInfo.tDateTime.day = tm.Day;
	    modDB.tSysInfo.tDateTime.displayBitMap.day = 1;
	    }
	    
	    if(modDB.tSysInfo.tDateTime.hour != tm.Hour)
	    {
	    modDB.tSysInfo.tDateTime.hour = tm.Hour;
	    modDB.tSysInfo.tDateTime.displayBitMap.hour = 1;
	    }
	    
	    if(modDB.tSysInfo.tDateTime.min != tm.Minute)
	    {
	    modDB.tSysInfo.tDateTime.min = tm.Minute;
	    modDB.tSysInfo.tDateTime.displayBitMap.min = 1;
	    }
	        

	   
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    // delay(9000);
  }
  #endif

  }

}

