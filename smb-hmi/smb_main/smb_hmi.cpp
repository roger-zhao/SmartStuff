/*
 * smb_menu.cpp
 *
 */

#include "smb_hmi.h"

// handler with Modbus, etc
// extern ModbusMaster SMBModMaster;

String padStart(String str,int minLength,String padChar){

	String result = str;

 while( result.length()<minLength ){

 result = padChar+result;

 }

 return result;

}

UC modbusDB::loginBBB() {
#ifdef  SELF_TEST
  return OK;
#else

  uint16_t val = 0;
  UC retry = 0;
  UC res = 0;
  UC ret = 0;

  Serial.println(F("loginBBB in"));

  // login to BBB
  while (res = this->SMBModMaster.writeSingleRegister(LOGIN_MODREG, LOGIN_REQ)) {
    Serial.print(F("wr login modreg error: "));
    Serial.println(res);

    if (retry > 2) {

      Serial.print(F("wr login modreg error time exceeded 2: "));
      Serial.print(retry);
      return KO;
    }
    retry++;

    delay(80);
  }

  delay(80);

  retry = 0;
  while (res = this->SMBModMaster.readHoldingRegisters(LOGIN_MODREG, 1)) {
    Serial.print(F("rd login modreg error: "));
    Serial.println(res);
    if (retry > 2) {
      Serial.print(F("rd login modreg error time exceeded 2: "));
      Serial.print(retry);
      return KO;
    }
    retry++;
    delay(80);
  }

  if (LOGIN_RES == (val = this->SMBModMaster.getResponseBuffer(0))) {
    Serial.println(F("login BBB success!"));
    ret = OK;
  } else {
    Serial.println(F("login BBB failed!"));
    ret = KO;
  }

 return ret;
#endif
}


UC modbusDB::isBBBReady() {

#ifdef  SELF_TEST
	static unsigned long lastTime = millis();
	Serial.println(F("isBBBReady in"));
	if(TIME32_DIFF(millis(), lastTime) > 3000)
	{
		Serial.print(F("isBBBReady time out: "));
		Serial.print(F("lastTime: "));
		Serial.println(lastTime);
		Serial.print(F("currTime: "));
		Serial.println(millis());
		return OK;
	}
	else
	{
		Serial.println(F("isBBBReady wait... "));
		return KO;
	}

#else
  uint16_t val = 0;
  UC retry = 0;
  UC res = 0;
  UC ret = 0;

  Serial.println(F("isBBBReady in"));
  delay(30);
  if (res = this->SMBModMaster.readHoldingRegisters(BBB_READY_MODREG, 1)) {
    Serial.print(F("rd BBB ready modreg error: "));
    Serial.println(res);
    if (++retry >= 2) {
      Serial.print(F("rd BBB ready modreg error time exceeded 2: "));
      Serial.println(retry);
      return KO;
    }
  }

  if (BBBREADY == (val = this->SMBModMaster.getResponseBuffer(0))) {
    Serial.println(F("BBB ready!"));
    ret = OK;
  } else {
    Serial.println(F("BBB not ready!"));
    ret = KO;
  }

  return ret;
#endif
}

void modbusDB::sendOneKeyAlarm() {

  uint16_t val = 0;
  UC retry = 0;
  UC res = 0;
  UC ret = 0;

  Serial.println(F("sendOneKeyAlarm in"));
  // delay(2);

  while (res = this->SMBModMaster.writeSingleRegister(ONE_KEY_ALARM_MODREG, ONE_KEY_ALARM)) {
    Serial.print(F("wr one key alarm error: "));
    Serial.println(res);
    if (++retry >= 2) {

      Serial.print(F("wr one key alarm modreg error time exceeded 2: "));
      Serial.print(retry);
      return;
    }
    delay(80);
  }

  return;
}

#define TIME_SYNC_INTERVAL_SEC 2000 // 2sec
#define BUS_INFO_UPDATE_INTERVAL_SEC 3000 // 3sec
#define COMM_INFO_UPDATE_INTERVAL_SEC 200000 // 200sec
#define KEEP_ALIVE_INTERVAL_SEC 2000 // 2sec
enum getInfoState {
  eTimeSync, eBusInfo, eKeepAlive, eCommInfo, stNum
} infoSt;

unsigned long lastTimeStamp[stNum];
UC first = 1;

void modbusDB::refreshInfosToBBB() {
	  US idx = 0;
	  US modRegStartAddr = 0;
	  US modRegNum = 0;
	  UC retry = 0;
	  UC res = 0;
#ifdef  SELF_TEST

#else
	// current station (FIXME: next station in BBB side, which is much more reasonable)
	  //if((0xff != this->tBusInfo.manuCurrStation) && (this->tBusInfo.manuCurrStation != this->tBusInfo.autoCurrStation))
	  retry = 0;
	  {
		  while (res = this->SMBModMaster.writeSingleRegister(MANUAL_CURR_STATION_MODREG, this->tBusInfo.manuCurrStation)) {
			Serial.print(F("wr manuCurrStation error: "));
			Serial.println(res);
			if (++retry >= 2) {

			  Serial.print(F("wr manuCurrStation modreg error time exceeded 2: "));
			  Serial.print(retry);

			  return;
			}
			delay(80);
		  }
	  }

	  //if((0xff != this->tBusInfo.manuPassengerCount) && (this->tBusInfo.manuPassengerCount != this->tBusInfo.autoPassengerCount))
      retry = 0;
	  {
		// APC level: manuPassengerCount is a code for crowd level

		  while (res = this->SMBModMaster.writeSingleRegister(MANUAL_APC_MODREG, this->tBusInfo.manuPassengerCount)) {
		    Serial.print(F("wr manuPassengerCount error: "));
		    Serial.println(res);
		    if (++retry >= 2) {

		      Serial.print(F("wr manuPassengerCount modreg error time exceeded 2: "));
		      Serial.print(retry);
		      return;
		    }
		    delay(80);
		  }
	  }
#endif
}

void modbusDB::getInfosFromBBB() {
#ifdef  SELF_TEST

#else
  US idx = 0;
  US modRegStartAddr = 0;
  US modRegNum = 0;
  UC retry = 0;
  UC retryTotal = 0;
  UC res = 0;
  static UC keepAliveFails = 0;
  enum kpState {eNull, eReq, eResp};
  static UC keepAliveSt = 0xff;

  // update all infos when first time
  if (first) {

	  first = 0;

    lastTimeStamp[eTimeSync] = millis();
    lastTimeStamp[eBusInfo] = millis();
    lastTimeStamp[eKeepAlive] = millis();
    lastTimeStamp[eCommInfo] = millis();

#ifdef HAVE_KEEP_ALIVE
    // KEEP ALIVE
    this->SMBModMaster.setTransmitBuffer(0, KEEP_ALIVE_REQ);
    while (res = this->SMBModMaster.writeSingleRegister(KEEP_ALVIE_MODREG,
    		KEEP_ALIVE_REQ)) {
		  Serial.print(F("wr KEEP ALIVE error: "));
		  Serial.print(res);
		  Serial.println(F(""));
      if (++retry >= 2) {

        Serial.print(F("wr KEEP ALIVE modreg error time exceeded 2: "));
        Serial.print(retry);
        Serial.println(F(""));
        retryTotal += retry;

        return;
      }
    }
    keepAliveSt = eReq;

#endif
    // SYNC TIME
    delay(80);
    retry = 0;
    modRegStartAddr = TIME_SYNC_YEAR_MODREG;
    modRegNum = 6;
    if (res = this->SMBModMaster.readHoldingRegisters(modRegStartAddr,
              modRegNum)) {
      Serial.print(F("rd timesync modreg error: "));
      Serial.println(res);
      if (++retry >= 2) {
        Serial.print(F("rd timesync modreg error time exceeded 2: "));
        Serial.println(retry);
        retryTotal += retry;

        //return;
      }
    }
    delay(80);
    // for(idx = 0; idx < TIME_MODREG_NUM; idx++)
    idx = 0;
    this->tSysInfo.tDateTime.year = this->SMBModMaster.getResponseBuffer(
                                      idx++);
    this->tSysInfo.tDateTime.month =
      (UC) (this->SMBModMaster.getResponseBuffer(idx++) & 0xFF);
    this->tSysInfo.tDateTime.day =
      (UC) (this->SMBModMaster.getResponseBuffer(idx++) & 0xFF);
    this->tSysInfo.tDateTime.hour =
      (UC) (this->SMBModMaster.getResponseBuffer(idx++) & 0xFF);
    this->tSysInfo.tDateTime.min =
      (UC) (this->SMBModMaster.getResponseBuffer(idx++) & 0xFF);
    this->tSysInfo.tDateTime.sec =
      (UC) (this->SMBModMaster.getResponseBuffer(idx++) & 0xFF);

    // busInfo
    /*
     *
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
     */
    retry = 0;
    delay(80);
    modRegStartAddr = AUTO_CURR_STATION_MODREG;
    modRegNum = 9;
    if (res = this->SMBModMaster.readHoldingRegisters(modRegStartAddr,
              modRegNum)) {
      Serial.print(F("rd autoCurrStation(+8) modreg error: "));
      Serial.println(res);
      if (++retry >= 2) {
        Serial.print(F("rd autoCurrStation(+8) modreg error time exceeded 2: "));
        Serial.println(retry);
        retryTotal += retry;

      }
    }

#ifdef HAVE_ERROR_START
    if(retryTotal >= (3*2)) // all read regs failed
    {
    	Serial.println(F("error: rd mod regs failed in getInfosFromBBB, wait 1s and return!"));
    	this->errNo = eCommLinkError;
      	retryTotal = 0;
    	delay(1000);
    	return;
    }
    else // everything is ok or go back to ok
    {
    	// when go back to ok, clr error flg, notify current menu to REDRAW
    	if(eNA != this->errNo)
    	{
    		this->errNo = eNA;
    		current->disFlg = Menu::REDRAW;
    	}
    }
#endif

    // for(idx = 0; idx < TIME_MODREG_NUM; idx++)
    retry = 0;
    idx = 0;
    this->tBusInfo.autoCurrStation =
      (UC) (this->SMBModMaster.getResponseBuffer(idx++) & 0xFF);

    // update manu from auto
    this->tBusInfo.manuCurrStation = this->tBusInfo.autoCurrStation;
    idx++;

    this->tBusInfo.autoPassengerCount =
      (UC) (this->SMBModMaster.getResponseBuffer(idx++) & 0xFF);
    /*this->tBusInfo.manuPassengerCount =
      (UC) (this->SMBModMaster.getResponseBuffer(idx++) & 0xFF);*/
    idx++;
    this->tBusInfo.busGrpNum = this->SMBModMaster.getResponseBuffer(idx++);
    this->tBusInfo.busLineNum = this->SMBModMaster.getResponseBuffer(idx++);
    this->tBusInfo.busNum = this->SMBModMaster.getResponseBuffer(idx++);
    this->tBusInfo.totalStations =
      (UC) (this->SMBModMaster.getResponseBuffer(idx++) & 0xFF);
    this->tBusInfo.driverId = (UC) (this->SMBModMaster.getResponseBuffer(
                                      idx++) & 0xFF);

    // PRT("total stations", this->tBusInfo.totalStations);

#ifdef HAVE_DRV_NAME
    // driver name font
    modRegStartAddr = DRIVER_NAME_MODREG;
    modRegNum = 64;
    retry = 0;
#define DRIVER_NAME_FONT_RD_TIMES 5 // MAX_DRIVER_NAME_LEN/64/2=640/2/64=5
    for (idx = 0; idx < DRIVER_NAME_FONT_RD_TIMES; idx++) {
      if (res = this->SMBModMaster.readHoldingRegisters(modRegStartAddr,
                modRegNum)) {
        Serial.print(F("rd timesync modreg error: "));
        Serial.println(res);
        if (++retry >= 2) {
          Serial.print(F("rd timesync modreg error time exceeded 2: "));
          Serial.println(retry);

          return;
        }
      }
      delay(100);
      modRegStartAddr += modRegNum;
    }

#define MAX_DRIVER_NAME_LEN_WORD 320 // MAX_DRIVER_NAME_LEN/2
    for (idx = 0; idx < MAX_DRIVER_NAME_LEN_WORD;) {
      US resp = this->SMBModMaster.getResponseBuffer(idx);
      this->tBusInfo.driverName[idx++] = (UC) (resp & 0xFF);
      this->tBusInfo.driverName[idx++] = (UC) ((resp >> 8) & 0xFF);
    }
#endif

    // commInfo
    /*WEATHER_MODREG = (DRIVER_NAME_MODREG + MAX_DRIVER_NAME_LEN),
     TEMPERATURE_MODREG,
     HUMIDITY_MODREG,
     ULTRAVIOLET_MODREG,
     *
     */
    modRegStartAddr = WEATHER_MODREG;
    modRegNum = 4;
    if (res = this->SMBModMaster.readHoldingRegisters(modRegStartAddr,
              modRegNum)) {
      Serial.print(F("rd commInfo modreg error: "));
      Serial.println(res);
      if (++retry >= 2) {
        Serial.print(F("rd commInfo modreg error time exceeded 2: "));
        Serial.println(retry);

        return;
      }
    }
    delay(80);
    // for(idx = 0; idx < TIME_MODREG_NUM; idx++)
    idx = 0;
    this->tCmnInfo.weather = (UC) (this->SMBModMaster.getResponseBuffer(
                                     idx++) & 0xFF);
    this->tCmnInfo.temperature = (UC) (this->SMBModMaster.getResponseBuffer(
                                         idx++) & 0xFF);
    this->tCmnInfo.humidity = (UC) (this->SMBModMaster.getResponseBuffer(
                                      idx++) & 0xFF);
    this->tCmnInfo.ultraViolet = (UC) (this->SMBModMaster.getResponseBuffer(
                                         idx++) & 0xFF);



    // font
    // xinXiFont: current & next station
    /*	CURR_STATION_FONT_START_MODREG,
     CURR_STATION_FONT_END_MODREG = (CURR_STATION_FONT_START_MODREG + XINXI_MENU_TOTAL_MODREG_PER_STATION),
     NEXT_STATION_FONT_START_MODREG,
     NEXT_STATION_FONT_END_MODREG = (NEXT_STATION_FONT_START_MODREG + XINXI_MENU_TOTAL_MODREG_PER_STATION),
     */
    modRegStartAddr = CURR_STATION_FONT_START_MODREG;
    modRegNum = 64;
    US stationFontIdx = 0;
    US rdIdx = 0;
    retry = 0;

    //Serial.println(F("Current staion fonts: "));
    //Serial.println(F("***********************************************************************************************************************"));
#define STATION_FONT_RD_TIMES (XINXI_MENU_BYTES_PER_STATION/64/2)
    for (rdIdx = 0; rdIdx < STATION_FONT_RD_TIMES; rdIdx++) {

    	// request infos from modbus server
      if (res = this->SMBModMaster.readHoldingRegisters(modRegStartAddr, modRegNum)) {
        Serial.print(F("rd current station fonts modreg error: "));
        Serial.println(res);
        if (++retry >= 2) {
          Serial.print(F("rd current station fonts modreg error time exceeded 2: "));
          Serial.println(retry);

          return;
        }
      }

      modRegStartAddr += modRegNum;

      //Serial.print(F("modAddr: "));
      //Serial.println(modRegStartAddr, HEX);

      // get & store resp infos
      for (idx = 0; idx < modRegNum; idx++) {
      	US resp = this->SMBModMaster.getResponseBuffer(idx);
      	// font_MSB -> reg_LSB, font_LSB -> reg_MSB (font_MSB->font_LSB: left -> right)
        this->tBusInfo.currentStation[stationFontIdx++] = (UC) (resp & 0xFF); // font_MSB
        //Serial.print(F(",0x"));
        //Serial.print((resp & 0xFF), HEX);
        this->tBusInfo.currentStation[stationFontIdx++] = (UC) ((resp >> 8) & 0xFF); // font_LSB
        //Serial.print(F(",0x"));
        //Serial.print((resp >> 8) & 0xFF, HEX);
        //if(0 == (stationFontIdx%16))
        //{
        	//Serial.println(F(""));
        //}
      }
    }
    //Serial.println(F("***********************************************************************************************************************"));

    // next station
    modRegStartAddr = NEXT_STATION_FONT_START_MODREG;
    modRegNum = 64;
    stationFontIdx = 0;
    rdIdx = 0;
    retry = 0;

#define STATION_FONT_RD_TIMES (XINXI_MENU_BYTES_PER_STATION/64/2)
    for (rdIdx = 0; rdIdx < STATION_FONT_RD_TIMES; rdIdx++) {

    	// request infos from modbus server
      if (res = this->SMBModMaster.readHoldingRegisters(modRegStartAddr, modRegNum)) {
        Serial.print(F("rd next station fonts modreg error: "));
        Serial.println(res);
        if (++retry >= 2) {
          Serial.print(F("rd next station fonts modreg error time exceeded 2: "));
          Serial.println(retry);

          return;
        }
      }

      modRegStartAddr += modRegNum;

      // get & store resp infos
      for (idx = 0; idx < modRegNum; idx++) {
      	US resp = this->SMBModMaster.getResponseBuffer(idx);
      	// font_MSB -> reg_LSB, font_LSB -> reg_MSB (font_MSB->font_LSB: left -> right)
        this->tBusInfo.nextStation[stationFontIdx++] = (UC) (resp & 0xFF); // font_MSB
        this->tBusInfo.nextStation[stationFontIdx++] = (UC) ((resp >> 8) & 0xFF); // font_LSB
        //Serial.println(F("Current staion font: "));
        //Serial.println(resp, HEX);
      }
    }



  } else {



#ifdef HAVE_KEEP_ALIVE
    retry = 0;
    // keep alive with BBB: 2s refresh keep alive in BBB side, 5s refresh
    if (TIME32_DIFF(millis(),
                    lastTimeStamp[eKeepAlive]) >= KEEP_ALIVE_INTERVAL_SEC) {

        // update time stamp
        lastTimeStamp[eKeepAlive] = millis();

      // get res first
      while (res = this->SMBModMaster.readHoldingRegisters(KEEP_ALVIE_MODREG,
                1)) {

        PRT("rd keep alive modreg error", res);
        if (++retry >= 2) {

          PRT("rd keep alive modreg error time exceeded 2", retry);

        	Serial.println(F("error: rd mod regs failed in getInfosFromBBB, wait 1s and return!"));
        	this->errNo = eCommLinkError;
        	retryTotal = 0;
        	delay(1000);
        	return;

        }
      }

    	// when go back to ok, clr comm error flg, notify current menu to REDRAW
    	if(eCommLinkError == this->errNo)
    	{
    		this->errNo = eNA;
    		current->disFlg = Menu::REDRAW;
    	}

      // req wait for resp
      if(eReq == keepAliveSt)
      {
		  if (KEEP_ALIVE_RES == (this->SMBModMaster.getResponseBuffer(0))) {
		      // when go back to ok, clr BBBdead error flg, notify current menu to REDRAW
		  	if(eBBBDead == this->errNo)
		  	{
		  		this->errNo = eNA;
		  		current->disFlg = Menu::REDRAW;
		  	}
		  } else {
			Serial.println(F("keep alive wrong!"));
			keepAliveFails++;
			if(keepAliveFails >= BBBDEAD) //
			{
				Serial.print(F("error: seems that BBB is dead (not keep alive for "));
				Serial.print(keepAliveFails);
				Serial.println(F(" times, wait 1s and return!"));
				this->errNo = eBBBDead;
				keepAliveFails = 0;
				delay(1000);

				return;
			}

		  }
      }





      // send req
      retry = 0;
      keepAliveSt = eReq;

      this->SMBModMaster.setTransmitBuffer(0, KEEP_ALIVE_REQ);
      while (res = this->SMBModMaster.writeSingleRegister(
                     KEEP_ALVIE_MODREG, KEEP_ALIVE_REQ)) {
        Serial.print(F("wr keep alive error: "));
        Serial.println(res);
        if (++retry >= 2) {

          Serial.print(F("wr keep alive error time exceeded 2: "));
          Serial.println(retry);

          return;
        }
      }




    }
#endif




#define UPDATE_VALUE_DISPLAYBITMAP(oldVal, newVal, bitmap) if(oldVal != newVal) {oldVal = newVal; bitmap = 1;}

    // refresh datetime
    if (TIME32_DIFF(millis(), lastTimeStamp[eTimeSync])
        >= TIME_SYNC_INTERVAL_SEC) {
      // get datetime
      // SYNC TIME
      modRegStartAddr = TIME_SYNC_YEAR_MODREG;
      modRegNum = 6;
      retry = 0;
      if (res = this->SMBModMaster.readHoldingRegisters(modRegStartAddr,
                modRegNum)) {
        Serial.print(F("rd timesync modreg error: "));
        Serial.println(res);
        if (++retry >= 2) {
          Serial.print(F("rd timesync modreg error time exceeded 2: "));
          Serial.println(retry);
          retryTotal += retry;

        }
      }
      // for(idx = 0; idx < TIME_MODREG_NUM; idx++)
      idx = 0;
      UPDATE_VALUE_DISPLAYBITMAP(this->tSysInfo.tDateTime.year,
                                 this->SMBModMaster.getResponseBuffer(idx),
                                 this->tSysInfo.tDateTime.displayBitMap.year);
      idx++;
      UPDATE_VALUE_DISPLAYBITMAP(this->tSysInfo.tDateTime.month,
                                 (UC )(this->SMBModMaster.getResponseBuffer(idx) & 0xFF),
                                 this->tSysInfo.tDateTime.displayBitMap.month);
      idx++;
      UPDATE_VALUE_DISPLAYBITMAP(this->tSysInfo.tDateTime.day,
                                 (UC )(this->SMBModMaster.getResponseBuffer(idx) & 0xFF),
                                 this->tSysInfo.tDateTime.displayBitMap.day);
      idx++;
      UPDATE_VALUE_DISPLAYBITMAP(this->tSysInfo.tDateTime.hour,
                                 (UC )(this->SMBModMaster.getResponseBuffer(idx) & 0xFF),
                                 this->tSysInfo.tDateTime.displayBitMap.hour);
      idx++;
      UPDATE_VALUE_DISPLAYBITMAP(this->tSysInfo.tDateTime.min,
                                 (UC )(this->SMBModMaster.getResponseBuffer(idx) & 0xFF),
                                 this->tSysInfo.tDateTime.displayBitMap.min);
      idx++;
      UPDATE_VALUE_DISPLAYBITMAP(this->tSysInfo.tDateTime.sec,
                                 (UC )(this->SMBModMaster.getResponseBuffer(idx) & 0xFF),
                                 this->tSysInfo.tDateTime.displayBitMap.sec);


      // configure RTC

      // update time stamp
      lastTimeStamp[eTimeSync] = millis();
    }



    // refresh station infos
    if (TIME32_DIFF(millis(),
                    lastTimeStamp[eBusInfo]) >= BUS_INFO_UPDATE_INTERVAL_SEC) {
      // get station infos
      modRegStartAddr = AUTO_CURR_STATION_MODREG;
      modRegNum = 9;
      retry = 0;
      if (res = this->SMBModMaster.readHoldingRegisters(modRegStartAddr,
                modRegNum)) {

        PRT("rd current station modreg error", res);

        if (++retry >= 2) {

          PRT("rd  current station modreg error time exceeded 2", retry);
          retryTotal += retry;

        }
      }




      // other infos
      /*
       * 	AUTO_CURR_STATION_MODREG,
	MANUAL_CURR_STATION_MODREG,
	APC_MODREG,
	MANUAL_APC_MODREG,
	BUS_GRP_NUM_MODREG,
	BUS_LINE_NUM_MODREG,
	BUS_NUM_MODREG,
	TOTAL_STATIONS_MODREG,
	DRIVER_ID_MODREG,
	DRIVER_NAME_MODREG, // PIXELS_PER_CHIN_CHAR*MAX_DRIVER_NAME
       */
      idx = 2;
      UPDATE_VALUE_DISPLAYBITMAP(this->tBusInfo.autoPassengerCount,
                                 (UC )(this->SMBModMaster.getResponseBuffer(idx) & 0xFF),
                                 this->tBusInfo.displayBitMap.autoPassengerCount);
      idx++;
      idx++; // skip manual APC
      UPDATE_VALUE_DISPLAYBITMAP(this->tBusInfo.busGrpNum,
                                 this->SMBModMaster.getResponseBuffer(idx),
                                 this->tBusInfo.displayBitMap.busGrpNum);
      idx++;
      UPDATE_VALUE_DISPLAYBITMAP(this->tBusInfo.busLineNum,
                                 this->SMBModMaster.getResponseBuffer(idx),
                                 this->tBusInfo.displayBitMap.busLineNum);
      idx++;
      UPDATE_VALUE_DISPLAYBITMAP(this->tBusInfo.busNum,
                                 this->SMBModMaster.getResponseBuffer(idx),
                                 this->tBusInfo.displayBitMap.busNum);
      idx++;
      this->tBusInfo.totalStations =
        (UC) (this->SMBModMaster.getResponseBuffer(idx) & 0xFF);
      idx++;
      UPDATE_VALUE_DISPLAYBITMAP(this->tBusInfo.driverId,
                                 this->SMBModMaster.getResponseBuffer(idx),
                                 this->tBusInfo.displayBitMap.driverId);

      // SPECIAL: current station change, so need to update current station font
      // for(idx = 0; idx < TIME_MODREG_NUM; idx++)
      idx = 0;

      if (this->tBusInfo.autoCurrStation
          != (UC) (this->SMBModMaster.getResponseBuffer(0) & 0xFF)) {
        this->tBusInfo.autoCurrStation =
          (UC) (this->SMBModMaster.getResponseBuffer(0) & 0xFF);
        // update manu from auto
        this->tBusInfo.manuCurrStation = this->tBusInfo.autoCurrStation;

        // update station fonts
        modRegStartAddr = CURR_STATION_FONT_START_MODREG;
        modRegNum = 64;
        US stationFontIdx = 0;
        US rdIdx = 0;
        retry = 0;

   #define STATION_FONT_RD_TIMES (XINXI_MENU_BYTES_PER_STATION/64/2)
        for (rdIdx = 0; rdIdx < STATION_FONT_RD_TIMES; rdIdx++) {

        	// request infos from modbus server

          if (res = this->SMBModMaster.readHoldingRegisters(modRegStartAddr, modRegNum)) {
            Serial.print(F("rd current station fonts modreg error: "));
            Serial.println(res);
            if (++retry >= 2) {
              Serial.print(F("rd current station fonts modreg error time exceeded 2: "));
              Serial.println(retry);

              //return;
            }
          }

          modRegStartAddr += modRegNum;

          // get & store resp infos
          for (idx = 0; idx < modRegNum; idx++) {
          	US resp = this->SMBModMaster.getResponseBuffer(idx);
          	// font_MSB -> reg_LSB, font_LSB -> reg_MSB (font_MSB->font_LSB: left -> right)
            this->tBusInfo.currentStation[stationFontIdx++] = (UC) (resp & 0xFF); // font_MSB
            this->tBusInfo.currentStation[stationFontIdx++] = (UC) ((resp >> 8) & 0xFF); // font_LSB

          }
        }

        // next station
        modRegStartAddr = NEXT_STATION_FONT_START_MODREG;
        modRegNum = 64;
        stationFontIdx = 0;
        rdIdx = 0;

    #define STATION_FONT_RD_TIMES (XINXI_MENU_BYTES_PER_STATION/64/2)
        for (rdIdx = 0; rdIdx < STATION_FONT_RD_TIMES; rdIdx++) {

        	// request infos from modbus server
          if (res = this->SMBModMaster.readHoldingRegisters(modRegStartAddr, modRegNum)) {
            Serial.print(F("rd next station fonts modreg error: "));
            Serial.println(res);
            if (++retry >= 2) {
              Serial.print(F("rd next station fonts modreg error time exceeded 2: "));
              Serial.println(retry);
              return;
            }
          }

          modRegStartAddr += modRegNum;

          // get & store resp infos
          for (idx = 0; idx < modRegNum; idx++) {
          	US resp = this->SMBModMaster.getResponseBuffer(idx);
          	// font_MSB -> reg_LSB, font_LSB -> reg_MSB (font_MSB->font_LSB: left -> right)
            this->tBusInfo.nextStation[stationFontIdx++] = (UC) (resp & 0xFF); // font_MSB
            this->tBusInfo.nextStation[stationFontIdx++] = (UC) ((resp >> 8) & 0xFF); // font_LSB
            //Serial.println(F("Current staion font: "));
            //Serial.println(resp, HEX);
          }
        }

        // update bitmap for display
        this->tBusInfo.displayBitMap.autoCurrStation = 0x1;
        this->tBusInfo.displayBitMap.currentStation = 0x1;
        this->tBusInfo.displayBitMap.nextStation = 0x1;

      }




      // FIXME: driver Id not used now
      // driver infos
      if (this->tBusInfo.driverId
          != (UC) (this->SMBModMaster.getResponseBuffer(idx++) & 0xFF)) {
        this->tBusInfo.driverId =
          (UC) (this->SMBModMaster.getResponseBuffer(idx++) & 0xFF);
#ifdef HAVE_DRV_NAME
        // update drive name
        modRegStartAddr = DRIVER_NAME_MODREG;
        modRegNum = 64;
#define DRIVER_NAME_FONT_RD_TIMES 5 // MAX_DRIVER_NAME_LEN/64/2=640/2/64=5
        for (idx = 0; idx < DRIVER_NAME_FONT_RD_TIMES; idx++) {
          if (res = this->SMBModMaster.readHoldingRegisters(
                      modRegStartAddr, modRegNum)) {
            Serial.print(F("rd timesync modreg error: "));
            Serial.println(res);
            if (++retry >= 2) {
              Serial.print(F("rd timesync modreg error time exceeded 2: "));
              Serial.println(retry);
              return;
            }
          }
          modRegStartAddr += modRegNum;
        }
#define MAX_DRIVER_NAME_LEN_WORD 320 // MAX_DRIVER_NAME_LEN/2
        for (idx = 0; idx < MAX_DRIVER_NAME_LEN_WORD;) {
        	US resp = this->SMBModMaster.getResponseBuffer(idx);
          this->tBusInfo.driverName[idx++] = (UC) (resp & 0xFF);
          this->tBusInfo.driverName[idx++] =
            (UC) ((resp >> 8) & 0xFF);
        }

#endif
      }


      // update time stamp
      lastTimeStamp[eBusInfo] = millis();
    }



    // comm infos
    retry = 0;
    if (TIME32_DIFF(millis(),
                    lastTimeStamp[eCommInfo]) >= COMM_INFO_UPDATE_INTERVAL_SEC) {
      // get infos
      modRegStartAddr = WEATHER_MODREG;
      modRegNum = 4;
      if (res = this->SMBModMaster.readHoldingRegisters(modRegStartAddr,
                modRegNum)) {
        Serial.print(F("rd comm infos modreg error: "));
        Serial.println(res);
        if (++retry >= 2) {
          Serial.print(F("rd comm infos modreg error time exceeded 2: "));
          Serial.println(retry);
        }
      }
      // for(idx = 0; idx < TIME_MODREG_NUM; idx++)
      idx = 0;
      UPDATE_VALUE_DISPLAYBITMAP(this->tCmnInfo.weather,
                                 (UC )(this->SMBModMaster.getResponseBuffer(idx) & 0xFF),
                                 this->tCmnInfo.displayBitMap.weather);
      idx++;
      UPDATE_VALUE_DISPLAYBITMAP(this->tCmnInfo.temperature,
                                 (UC )(this->SMBModMaster.getResponseBuffer(idx) & 0xFF),
                                 this->tCmnInfo.displayBitMap.temperature);
      idx++;
      UPDATE_VALUE_DISPLAYBITMAP(this->tCmnInfo.humidity,
                                 (UC )(this->SMBModMaster.getResponseBuffer(idx) & 0xFF),
                                 this->tCmnInfo.displayBitMap.humidity);
      idx++;
      UPDATE_VALUE_DISPLAYBITMAP(this->tCmnInfo.ultraViolet,
                                 (UC )(this->SMBModMaster.getResponseBuffer(idx) & 0xFF),
                                 this->tCmnInfo.displayBitMap.ultraViolet);

      // update time stamp
      lastTimeStamp[eCommInfo] = millis();
    }

  }
#endif

}

void modbusDB::get64BaoZhanFont() {
  //Serial.println(F("get64BaoZhanFont in"));
}

void modbusDB::parseModbusMsg() {
  //Serial.println(F("parseModbusMsg in"));
#ifdef  SELF_TEST
#else

	// send infos to BBB: manual current station (next station in BBB side), manual APC level
	this->refreshInfosToBBB();

  // get infos from BBB
  this->getInfosFromBBB();
  // TODO:get control infos from BBB
#endif
}


// VENDOR logo when startup
// logo: 128x80 ?
// vision: 128x80 ?


#define LOGO_LEN 4
#define LOGO_WIDTH 160 // (40*4)
#define LOGO_HEIGHT 64
#define VISION_LEN 7
#define VISION_WIDTH  280 // (40*7)
#define VISION_HEIGHT 40
#define Y_GAP 21

#define LINE_HEIGHT_UP_DOWN 3
#define X_POS_OFFSET 10

#define WAIT_CIRCLE_RADIUS 4
#define WAIT_CIRCLE_NUM 20
#define WAIT_CIRCLE_DISTANCE ((VISION_WIDTH + LOGO_WIDTH)/WAIT_CIRCLE_NUM)
#define CLR_GAP 2

#define Y_AJUST 5
#define VENDOR_LOGO_START_X 20
#define VENDOR_LOGO_START_Y (MIDDLE_Y - LINE_HEIGHT_UP_DOWN - LOGO_HEIGHT - Y_AJUST)
#define VENDOR_VISION_START_X 4
#define VENDOR_VISION_START_Y (MIDDLE_Y + LINE_HEIGHT_UP_DOWN + Y_AJUST + 3)

void SMBprint::printVendorLogo(UC flg) {

  String logoStr("0123");
  String visionStr("0123456");
  UC idx = 0;
  // from top to center: vendorLogo
  // from bottom to center: vendor vision
  static int x_pos = VENDOR_LOGO_START_X;
  static UC direction = 0;
  // static int x_pos_border = VENDOR_LOGO_START_X;
  static int x_pos_right_border = 0;
  static UC randomCnt = 0;
  static UC circleCnt = 0;
  int tmp = 0;


  if (!flg) {
	  Serial.println(F("printVendorLogo initial in"));

	  // border
	  idx = 4;
	  this->setColor(173, 255, 47);
	    this->drawRoundRect(idx, idx, MAX_X_WIDTH - idx, MAX_Y_HEIGHT - idx);
	    idx++;
		  this->setColor(173, 255, 47);
		  this->drawRoundRect(idx, idx, MAX_X_WIDTH - idx, MAX_Y_HEIGHT - idx);
		    idx++;
			  this->setColor(173, 255, 47);
			  this->drawRoundRect(idx, idx, MAX_X_WIDTH - idx, MAX_Y_HEIGHT - idx);
			    idx++;
				  this->setColor(173, 255, 47);
				  this->drawRoundRect(idx, idx, MAX_X_WIDTH - idx, MAX_Y_HEIGHT - idx);
				    idx++;
					  this->setColor(173, 255, 47);
					  this->drawRoundRect(idx, idx, MAX_X_WIDTH - idx, MAX_Y_HEIGHT - idx);
					    idx++;
						  this->setColor(173, 255, 47);
						  this->drawRoundRect(idx, idx, MAX_X_WIDTH - idx, MAX_Y_HEIGHT - idx);
						    idx++;
							  this->setColor(173, 255, 47);
							  this->drawRoundRect(idx, idx, MAX_X_WIDTH - idx, MAX_Y_HEIGHT - idx);
							    idx++;
								  this->setColor(173, 255, 47);
								  this->drawRoundRect(idx, idx, MAX_X_WIDTH - idx, MAX_Y_HEIGHT - idx);
								    idx++;
									  this->setColor(173, 255, 47);
									  this->drawRoundRect(idx, idx, MAX_X_WIDTH - idx, MAX_Y_HEIGHT - idx);
									    idx++;
										  this->setColor(173, 255, 47);
										  this->drawRoundRect(idx, idx, MAX_X_WIDTH - idx, MAX_Y_HEIGHT - idx);
    // logo
    this->setColor(255, 193, 37);
    this->setBackColor(0, 0, 0);
    // const char vendorName[] = "";
    idx = 0;
    this->setFont(vendorLogo);
    this->print(String(logoStr[idx++]), x_pos, VENDOR_LOGO_START_Y);
    delay(100);
    x_pos += this->getFontXsize();
    this->print(String(logoStr[idx++]), x_pos, VENDOR_LOGO_START_Y); // hang zhou hui jun...
    delay(100);
    x_pos += this->getFontXsize();
    this->print(String(logoStr[idx++]), x_pos, VENDOR_LOGO_START_Y); // hang zhou hui jun...
    delay(100);
    x_pos += this->getFontXsize();
    this->print(String(logoStr[idx++]), x_pos, VENDOR_LOGO_START_Y); // hang zhou hui jun...
    delay(100);


    // draw line: right to left: 11 = 21 - 2*5 pixel, then up/down: 5 pixels

    this->setColor(255, 255, 0);
    for (tmp = (VENDOR_LOGO_START_X + 2*X_POS_OFFSET); tmp < (LOGO_WIDTH + VISION_WIDTH); tmp += X_POS_OFFSET)
    {
      this->fillRect(tmp, MIDDLE_Y - LINE_HEIGHT_UP_DOWN,
                     tmp + X_POS_OFFSET, MIDDLE_Y + LINE_HEIGHT_UP_DOWN);
      delay(30);
    }
    Serial.print(F("printVendorLogo line x_pos right border:"));
    Serial.println(tmp);
    // vision
    x_pos = LOGO_LEN * 40; // start here
    this->setColor(255, 0, 0);
    this->setBackColor(0, 0, 0);
    // const char vendorName[] = "";
    this->setFont(vendorVision);
    for (idx = 0; idx < VISION_LEN;) {
      this->print(String(visionStr[idx++]), x_pos, VENDOR_VISION_START_Y);
      x_pos += this->getFontXsize();
      delay(100);
    }
    // x_pos_right_border = x_pos;
    // clr x_pos for next animation
    x_pos = VENDOR_LOGO_START_X - 3;
    direction = 0;
    //Serial.println(F("printVendorLogo initial out"));
  }
  else if(!(circleCnt++%5)){
    // wait circle: radius: (Y_GAP - 1)/2 - 2 = 8, color: green
	  //Serial.println(F("printVendorLogo wait in"));
    if (!direction) {
    	//Serial.println(F("printVendorLogo wait dir---0"));

    	// circle color
    	// Blue4	0 0 139
    	// Brown3	205 51 51
    	// LawnGreen	124 252 0
    	// DarkGoldenrod1	255 185 15
    	// BlueViolet	138 43 226
      this->setColor(138, 43, 226);
      x_pos += WAIT_CIRCLE_DISTANCE;

      // reach to right border
      if (x_pos > (VISION_WIDTH + LOGO_WIDTH + WAIT_CIRCLE_RADIUS)) {
        direction = 1; // reverse
        x_pos -= WAIT_CIRCLE_DISTANCE; // back
        //Serial.println(F("printVendorLogo wait dir---reverse:0->1"));
      }
      else
      {
          this->fillCircle(x_pos, MIDDLE_Y, WAIT_CIRCLE_RADIUS);
      }
    } else {
    	//Serial.println(F("printVendorLogo wait dir---1"));
      // make sure x_pos is correct
      if ((x_pos - WAIT_CIRCLE_DISTANCE) > 0) {
        // clear circles
          this->setColor(0, 0, 0);
          this->fillCircle(x_pos, MIDDLE_Y, WAIT_CIRCLE_RADIUS);

        // redraw line
        this->setColor(255, 255, 0);
        this->fillRect(x_pos + WAIT_CIRCLE_RADIUS,
                       MIDDLE_Y - LINE_HEIGHT_UP_DOWN,
                       x_pos - WAIT_CIRCLE_RADIUS,
                       MIDDLE_Y + LINE_HEIGHT_UP_DOWN);

        x_pos -= WAIT_CIRCLE_DISTANCE;
        // reach to left border
        if (x_pos < (VENDOR_LOGO_START_X)) {
          direction = 0; // reverse
          x_pos = VENDOR_LOGO_START_X; // back
          //Serial.println(F("printVendorLogo wait dir---reverse(reach left border):1->0"));
        }
      } else {
        // reach to left border
        direction = 0; // reverse
        x_pos = VENDOR_LOGO_START_X; // back
        //Serial.println(F("printVendorLogo wait dir---reverse(no circle):1->0"));
      }

    }
    Serial.println(x_pos);
    // Draw some random circles
      // for (int i=0; i<100; i++)
    int x, y, x2, y2, r;
    if((randomCnt++)%5)
    {
    this->setColor(random(255), random(255), random(255));
        x=48+random(390); // 40~460 - 32
        y=250+random(25); // 250~310 - 32
        r=random(32);
        this->drawCircle(x, y, r);

    // Draw some random rectangles
      this->setColor(random(255), random(255), random(255));
        x=320+random(140); // 320~460
        y=20+random(120); // 10~130
        x2=320+random(140); // 320~460
        y2=20+random(120); // 10~130
        this->drawRect(x, y, x2, y2);
    }
    else // clr random zone
    {
        this->setColor(0,0,0);
        this->fillRect(15,250 - 32,462,305);
        this->setColor(0,0,0);
        this->fillRect(318,18,462,142);
    }

    //Serial.println(F("printVendorLogo wait out"));
  }

}
void SMBprint::printBorder() {
  //Serial.println(F("printBorder in"));
  this->setColor(200, 200, 0);
  this->drawRoundRect(0, 1, MAX_X_WIDTH - 1, MAX_Y_HEIGHT - 1);
}

// top info
// start_x, start_y, width, height: 1, 1, 479, 40
#define TOP_START_X		8
#define TOP_START_Y		8
#define TOP_X_GAP 	5
#define TOP_WIDTH 		(MAX_X_WIDTH - 1)
#define TOP_HEIGHT		40
// font:  /* ��(0) ��(1) ��(2) ˾(3) ��(4) ��(5) ��(6) ��(7) ��(8)*/
void SMBprint::printTop() {

  US x_pos = TOP_START_X;
  US y_pos = TOP_START_Y;
  static UC topFirst = 1;
  String tmpStr;


  // make sure backColor
  SMBprt.setBackColor(0, 0, 0);
  //Serial.println(F("printTop in"));
  if (topFirst) {
    topFirst = 0;
    // border
    this->setColor(255, 255, 0);
    this->drawLine(1, TOP_HEIGHT, TOP_WIDTH, TOP_HEIGHT);

    /*
    // busGrp
    this->setColor(255, 193, 37); // GOLD1
    this->setBackColor(0,0,0);
    this->setFont(topFont); // TODO: topFont add ":"
    this->print(F("018"), x_pos, y_pos);

    x_pos += this->getFontXsize() * 3; // 3 character --- cz:
    this->setColor(255,255,255);
    this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
    this->print(String(this->pDB->tBusInfo.busGrpNum), x_pos, y_pos);*/

    // busNum
#ifdef  SELF_TEST
    this->pDB->tBusInfo.busNum = 410;
#endif
    tmpStr = String(this->pDB->tBusInfo.busNum);
    tmpStr = padStart(tmpStr, 3, ";");
    this->setColor(255,255,255);
    this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
    this->print(tmpStr, x_pos, y_pos);
#define GAP 1
    x_pos += this->getFontXsize() * (tmpStr.length() + GAP);
    this->setColor(255, 193, 37); // GOLD1
    this->setBackColor(0,0,0);
    this->setFont(topFont); //
    this->print(F("0"), x_pos, y_pos); // lu

    x_pos += this->getFontXsize() * 1; // 3 character --- cc:

#ifdef HAVE_DRV_NAME
    // driver: ID + driverName
    this->setColor(255,255,255);


#define DRV_NAME_WIDTH 32
#define DRV_NAME_HEIGHT 32

#ifdef SELF_TEST

    this->printFont(x_pos, y_pos, DRV_NAME_WIDTH,
    		DRV_NAME_HEIGHT, drvNameFont, 5);
#else
    this->printFont(x_pos, y_pos, DRV_NAME_WIDTH,
    		DRV_NAME_HEIGHT, SMBprt.pDB.tBusInfo.driverName, MAX_DRIVER_NAME);
#endif

#endif

#ifdef SELF_TEST
    this->pDB->tSysInfo.tDateTime.year = 2015;
    this->pDB->tSysInfo.tDateTime.month = 7;
    this->pDB->tSysInfo.tDateTime.day = 24;
    this->pDB->tSysInfo.tDateTime.hour = 16;
    this->pDB->tSysInfo.tDateTime.min = 24;
    this->pDB->tSysInfo.tDateTime.sec = 58;
#endif


    // datetime
#ifdef HAVE_DRV_NAME
    x_pos += TOP_X_GAP  + DRV_NAME_WIDTH * MAX_DRIVER_NAME;
#else
    x_pos += TOP_X_GAP;
#endif
    // year
    this->setColor(255,255,255);
    this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
    this->print(padStart(String(this->pDB->tSysInfo.tDateTime.year), 4, ";"), x_pos, y_pos);
    x_pos += this->getFontXsize() * 4; // 4 character --- cc:
    this->setColor(255, 193, 37); // GOLD1
    this->setBackColor(0,0,0);
    this->setFont(topFont); // TODO: topFont add ":"
    this->print(F("1"), x_pos, y_pos); // nian
    x_pos += this->getFontXsize() * 1; // 3 character --- cc:
#if 1
    // month
    this->setColor(255,255,255);
    this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
    this->print(padStart(String(this->pDB->tSysInfo.tDateTime.month), 2, ";"), x_pos, y_pos);
    x_pos += this->getFontXsize() * 2; // 2 character --- cc:
    this->setColor(255, 193, 37); // GOLD1
    this->setBackColor(0,0,0);
    this->setFont(topFont); // TODO: topFont add ":"
    this->print(F("2"), x_pos, y_pos); // nian
    x_pos += this->getFontXsize() * 1; // 1 character --- cc:

    // day
    this->setColor(255,255,255);
    this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
    this->print(padStart(String(this->pDB->tSysInfo.tDateTime.day), 2, ";"), x_pos, y_pos);
    x_pos += this->getFontXsize() * 2; // 2 character --- cc:
    this->setColor(255, 193, 37); // GOLD1
    this->setBackColor(0,0,0);
    this->setFont(topFont); // TODO: topFont add ":"
    this->print(F("3"), x_pos, y_pos); // nian
    x_pos += this->getFontXsize() * 1; // 1 character --- cc:


    // hour:min:sec
    this->setColor(255,255,255);
    this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
    this->print(padStart(String(this->pDB->tSysInfo.tDateTime.hour), 2, ";"), x_pos, y_pos);
    x_pos += this->getFontXsize() * 2; // 2 character --- cc:
    this->setColor(255, 193, 37); // GOLD1
    this->setBackColor(0,0,0);
    this->setFont(topFont); // TODO: topFont add ":"
    this->print(F("6"), x_pos, y_pos); // shi
    x_pos += this->getFontXsize() * 1; // 1 character --- cc:
    this->setColor(255,255,255);
    this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
    this->print(padStart(String(this->pDB->tSysInfo.tDateTime.min), 2, ";"), x_pos, y_pos);

    x_pos += this->getFontXsize() * 2; // 2 character --- cc:
    this->setColor(255, 193, 37); // GOLD1
    this->setBackColor(0,0,0);
    this->setFont(topFont); // TODO: topFont add ":"
    this->print(F("7"), x_pos, y_pos); // nian
#ifdef HAVE_SEC
    x_pos += this->getFontXsize() * 1; // 1 character --- cc:
    this->setColor(255,255,255);
    this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
    this->print(String(this->pDB->tSysInfo.tDateTime.sec), x_pos, y_pos);
    x_pos += this->getFontXsize() * 2; // 2 character --- cc:
#endif

#endif

  } else {

    // busNum
    if (this->pDB->tBusInfo.displayBitMap.busNum) {
      this->pDB->tBusInfo.displayBitMap.busNum = 0;
      // busNum
      	      tmpStr = padStart(String(this->pDB->tBusInfo.busNum), 3, ";");
      	      this->setColor(255,255,255);
      	      this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
      	      this->print(tmpStr, x_pos, y_pos);
      	  #define GAP 1
      	      x_pos += this->getFontXsize() * (tmpStr.length() + GAP);
      	      this->setColor(255, 193, 37); // GOLD1
      	      this->setBackColor(0,0,0);
      	      this->setFont(topFont); //
      	      this->print(F("0"), x_pos, y_pos); // lu-

      	      x_pos += this->getFontXsize() * 1; // 3 character --- cc:
    } else {
        // busNum
    			  tmpStr = padStart(String(this->pDB->tBusInfo.busNum), 3, ";");
        	      //this->setColor(255,255,255);
        	      this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
        	      //this->print(tmpStr, x_pos, y_pos);
        	  #define GAP 1
        	      x_pos += this->getFontXsize() * (tmpStr.length() + GAP);
        	      //this->setColor(255, 193, 37); // GOLD1
        	      //this->setBackColor(0,0,0);
        	      this->setFont(topFont); //
        	      //this->print(F("05"), x_pos, y_pos); // lu-

        	      x_pos += this->getFontXsize() * 1; // 3 character --- cc:
    }

#ifdef HAVE_DRV_NAME
    // driver: ID + driverName
    // x_pos += TOP_X_GAP + this->getFontXsize() * 4;
    if (this->pDB->tBusInfo.displayBitMap.driverId) {
      this->pDB->tBusInfo.displayBitMap.driverId = 0;
      // driver: ID + driverName
      this->setColor(255,255,255);


  #define DRV_NAME_WIDTH 32
  #define DRV_NAME_HEIGHT 32

  #ifdef SELF_TEST

      this->printFont(x_pos, y_pos, DRV_NAME_WIDTH,
      		DRV_NAME_HEIGHT, drvNameFont, MAX_DRIVER_NAME);
  #else
      this->printFont(x_pos, y_pos, DRV_NAME_WIDTH,
      		DRV_NAME_HEIGHT, SMBprt.pDB.tBusInfo.driverName, MAX_DRIVER_NAME);
  #endif
    } else {
      x_pos += DRV_NAME_WIDTH * MAX_DRIVER_NAME; // 3 character --- cc:
    }
#endif


    // datetime
    x_pos += TOP_X_GAP;
    // year
    if (this->pDB->tSysInfo.tDateTime.displayBitMap.year) {
      this->pDB->tSysInfo.tDateTime.displayBitMap.year = 0;

      this->setColor(255,255,255);
          this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
          this->print(padStart(String(this->pDB->tSysInfo.tDateTime.year), 4, ";"), x_pos, y_pos);
          x_pos += this->getFontXsize() * 4; // 4 character --- cc:
          this->setColor(255, 193, 37); // GOLD1
          this->setBackColor(0,0,0);
          this->setFont(topFont); // TODO: topFont add ":"
          this->print(F("1"), x_pos, y_pos); // nian
          x_pos += this->getFontXsize() * 1; // 3 character --- cc:

    } else {
    	//this->setColor(255,255,255);
    	    this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
    	    //this->print(String(this->pDB->tSysInfo.tDateTime.year), x_pos, y_pos);
    	    x_pos += this->getFontXsize() * 4; // 4 character --- cc:
    	    //this->setColor(255, 193, 37); // GOLD1
    	    //this->setBackColor(0,0,0);
    	    this->setFont(topFont); // TODO: topFont add ":"
    	    //this->print(F("5"), x_pos, y_pos); // nian
    	    x_pos += this->getFontXsize() * 1; // 3 character --- cc:
    }

    // month
    if (this->pDB->tSysInfo.tDateTime.displayBitMap.month) {
      this->pDB->tSysInfo.tDateTime.displayBitMap.month = 0;
      this->setColor(255,255,255);
         this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
         this->print(padStart(String(this->pDB->tSysInfo.tDateTime.month), 2, ";"), x_pos, y_pos);
         x_pos += this->getFontXsize() * 2; // 2 character --- cc:
         this->setColor(255, 193, 37); // GOLD1
         this->setBackColor(0,0,0);
         this->setFont(topFont); // TODO: topFont add ":"
         this->print(F("2"), x_pos, y_pos); // nian
         x_pos += this->getFontXsize() * 1; // 1 character --- cc:
    } else {
    	// this->setColor(255,255,255);
    	    this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
    	    //this->print(String(this->pDB->tSysInfo.tDateTime.month), x_pos, y_pos);
    	    x_pos += this->getFontXsize() * 2; // 2 character --- cc:
    	   // this->setColor(255, 193, 37); // GOLD1
    	    //this->setBackColor(0,0,0);
    	    this->setFont(topFont); // TODO: topFont add ":"
    	    //this->print(F("6"), x_pos, y_pos); // nian
    	    x_pos += this->getFontXsize() * 1; // 1 character --- cc:
    }

    // day
    if (this->pDB->tSysInfo.tDateTime.displayBitMap.day) {
      this->pDB->tSysInfo.tDateTime.displayBitMap.day = 0;

      this->setColor(255,255,255);
      this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
      this->print(padStart(String(this->pDB->tSysInfo.tDateTime.day), 2, ";"), x_pos, y_pos);
      x_pos += this->getFontXsize() * 2; // 2 character --- cc:
      this->setColor(255, 193, 37); // GOLD1
      this->setBackColor(0,0,0);
      this->setFont(topFont); // TODO: topFont add ":"
      this->print(F("3"), x_pos, y_pos); // nian
      x_pos += this->getFontXsize() * 1; // 1 character --- cc:

    } else {
        //this->setColor(255,255,255);
        this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
        //this->print(String(this->pDB->tSysInfo.tDateTime.day), x_pos, y_pos);
        x_pos += this->getFontXsize() * 2; // 2 character --- cc:
        //this->setColor(255, 193, 37); // GOLD1
        //this->setBackColor(0,0,0);
        this->setFont(topFont); // TODO: topFont add ":"
        //this->print(F("7"), x_pos, y_pos); // nian
        x_pos += this->getFontXsize() * 1; // 1 character --- cc:
    }


    // hour:min:sec
    if (this->pDB->tSysInfo.tDateTime.displayBitMap.hour) {
      this->pDB->tSysInfo.tDateTime.displayBitMap.hour = 0;
      this->setColor(255,255,255);
      this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
      this->print(padStart(String(this->pDB->tSysInfo.tDateTime.hour), 2, ";"), x_pos, y_pos);
      x_pos += this->getFontXsize() * 2; // 2 character --- cc:
      this->setColor(255, 193, 37); // GOLD1
      this->setBackColor(0,0,0);
      this->setFont(topFont); // TODO: topFont add ":"
      this->print(F("6"), x_pos, y_pos); // nian
      x_pos += this->getFontXsize() * 1; // 1 character --- cc:
    } else {
        //this->setColor(255,255,255);
        this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
        //this->print(String(this->pDB->tSysInfo.tDateTime.hour), x_pos, y_pos);
        x_pos += this->getFontXsize() * 2; // 2 character --- cc:
        //this->setColor(255, 193, 37); // GOLD1
        //this->setBackColor(0,0,0);
        this->setFont(topFont); // TODO: topFont add ":"
        //this->print(F("8"), x_pos, y_pos); // nian
        x_pos += this->getFontXsize() * 1; // 1 character --- cc:
    }

    if (this->pDB->tSysInfo.tDateTime.displayBitMap.min) {
    	//PRT("min", this->pDB->tSysInfo.tDateTime.min);
      this->pDB->tSysInfo.tDateTime.displayBitMap.min = 0;
      this->setColor(255,255,255);
          this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
          this->print(padStart(String(this->pDB->tSysInfo.tDateTime.min), 2, ";"), x_pos, y_pos);
          x_pos += this->getFontXsize() * 2; // 2 character --- cc:
          this->setColor(255, 193, 37); // GOLD1
          this->setBackColor(0,0,0);
          this->setFont(topFont); // TODO: topFont add ":"
          this->print(F("7"), x_pos, y_pos); // fen
          x_pos += this->getFontXsize() * 1; // 1 character --- cc:
    } else {
    	//this->setColor(255,255,255);
    	    this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
    	    //this->print(String(this->pDB->tSysInfo.tDateTime.min), x_pos, y_pos);
    	    x_pos += this->getFontXsize() * 2; // 2 character --- cc:
    	    //this->setColor(255, 193, 37); // GOLD1
    	    //this->setBackColor(0,0,0);
    	    this->setFont(topFont); // TODO: topFont add ":"
    	    //this->print(F("8"), x_pos, y_pos); // nian
    	    x_pos += this->getFontXsize() * 1; // 1 character --- cc:
    }
#ifdef HAVE_SEC
    if (this->pDB->tSysInfo.tDateTime.displayBitMap.sec) {
        this->setColor(255,255,255);
        this->setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
        this->print(String(this->pDB->tSysInfo.tDateTime.sec), x_pos, y_pos);
        x_pos += this->getFontXsize() * 2; // 2 character --- cc:


    }
#endif

  }

}

// VENDOR info
// start_x, start_y, width, height: 1, 280, 479, 40
#define BOTT_START_X 	2
#define BOTT_START_Y	280
#define BOTT_WIDTH 		(MAX_X_WIDTH - 1)
#define BOTT_HEIGHT		40
/*��(0) ��(1) ��(2) ��(3) ��(4) ��(5) ��(6) ��(7) ��(8) ˾(9) ��(10) ��(11) ��(12) Ʒ(13)*/
/*
 *
 * #define RED (255, 0, 0)
#define GREEN (0, 255, 0)
#define BLUE (0, 0, 255)
#define WHITE (255, 255, 255)
#define BLACK (0, 0, 0)
#define YELLOW (255, 255, 0)
#define LIGHT_YELLOW1	(255, 255, 224)
#define GOLD1 (255, 193, 37)
#define GOLD3 (205, 173, 0)
 */
void SMBprint::printBottom() {
  //Serial.println(F("printBottom in"));
  this->setColor(255, 193, 37);
  this->drawRoundRect(BOTT_START_X, BOTT_START_Y, BOTT_WIDTH - 2,
		  BOTT_START_Y + BOTT_HEIGHT - 2);
  this->setColor(205, 173, 0);
  this->drawRoundRect(BOTT_START_X + 1, BOTT_START_Y + 1, BOTT_WIDTH - 1,
		  BOTT_START_Y + BOTT_HEIGHT - 3);
  this->setColor(255, 193, 37);
  this->drawRoundRect(BOTT_START_X + 2, BOTT_START_Y + 2, BOTT_WIDTH,
		  BOTT_START_Y + BOTT_HEIGHT - 4);

  this->setColor(0, 0, 0);
  this->fillRect(BOTT_START_X + 3, BOTT_START_Y + 3, BOTT_WIDTH - 5,
		  BOTT_START_Y + BOTT_HEIGHT - 5);
  this->setColor(255, 193, 37);
  this->setBackColor(0, 0, 0);
  // const char vendorName[] = "";
  this->setFont(vendorName);
  this->print(F(" !\"#$%&'()*+,-"), CENTER, BOTT_START_Y + 5); // hang zhou hui jun...
}
void SMBprint::popMsg(enum errorNo en) {
  //Serial.println(F("popMsg in"));
#define MSG_BOX_WIDTH	((MAX_X_WIDTH/2) - 32)
#define MSG_BOX_HEIGHT	((MAX_Y_HEIGHT/2) - 32)

	// pop msg box
	if(eNA != en)
	{
		  //Serial.println(F("printBottom in"));
		  this->setColor(255, 255, 255); // white border
		  this->drawRoundRect(MIDDLE_X - MSG_BOX_WIDTH/2, MIDDLE_Y - MSG_BOX_HEIGHT/2, MIDDLE_X + MSG_BOX_WIDTH/2, MIDDLE_Y + MSG_BOX_HEIGHT/2);
		  this->setColor(255, 255, 255);
		  this->drawRoundRect(MIDDLE_X - MSG_BOX_WIDTH/2 + 1, MIDDLE_Y - MSG_BOX_HEIGHT/2 + 1, MIDDLE_X + MSG_BOX_WIDTH/2 - 1,
				  MIDDLE_Y + MSG_BOX_HEIGHT/2 - 1);
		  this->setColor(255, 255, 255);
		  this->drawRoundRect(MIDDLE_X - MSG_BOX_WIDTH/2 + 2, MIDDLE_Y - MSG_BOX_HEIGHT/2 + 2, MIDDLE_X + MSG_BOX_WIDTH/2 - 2,
				  MIDDLE_Y + MSG_BOX_HEIGHT/2 - 2);

		  this->setColor(255, 0, 0); // red box
		  this->fillRoundRect(MIDDLE_X - MSG_BOX_WIDTH/2 + 3, MIDDLE_Y - MSG_BOX_HEIGHT/2 + 3, MIDDLE_X + MSG_BOX_WIDTH/2 - 3,
				  MIDDLE_Y + MSG_BOX_HEIGHT/2 - 3);

	}

	// display msg
	if(eCommLinkError == en)
	{
		  this->setColor(255, 255, 0); // yellow font
		  this->setBackColor(255, 0, 0);
		  // const char vendorName[] = "";
		  this->setFont(errorFont);
		  US tmpYpos = MIDDLE_Y - MSG_BOX_HEIGHT/2 + this->getFontYsize() - 24;
		  this->print(F("012"), CENTER, tmpYpos);
		  this->print(F("3456"), CENTER, tmpYpos + this->getFontYsize() + 8);
	}
	else if(eBBBDead == en)
	{
		  this->setColor(255, 255, 0); // yellow font
		  this->setBackColor(255, 0, 0);
		  // const char vendorName[] = "";
		  this->setFont(errorFont);
		  US tmpYpos = MIDDLE_Y - MSG_BOX_HEIGHT/2 + this->getFontYsize() - 32;
		  this->print(F("012"), CENTER, tmpYpos);
		  this->print(F("7856"), CENTER, tmpYpos + this->getFontYsize() + 8);
	}


}

#define XINXI_START_X	65
#define XINXI_START_Y	50
#define XINXI_X_GAP		10
#define XINXI_Y_GAP		10
/*
<SPACE>!"#$%&'()*+,-./0123456789

 ��(0) ��(1) ��(2) վ(3) ��(4) һ(5) ӵ(6) ��(7) ��(8) ��(9) ��(10) ��(11) ��(12) ��(13) ��(14) ��(15)
 ��(16) ��(17) ��(18) ��(19) ��(20) ʪ(21) ��(22) ��(23) ��(24) ��(25)*/
void displayXinXi(Menu &xinXi, Menu &last) {



  US x_pos = XINXI_START_X;
  US y_pos = XINXI_START_Y;
  String asciiArray("0123456789:;<=>?@ABCDEFGHIJKLM");
  String tmpStr("");

  Serial.println(F("displayXinXi in"));

  if (Menu::REDRAW == xinXi.disFlg) {

	  // handle tab
	  // xinxi tab: enable
	  // 255 0 0
#define FONT_AJUST_Y 15
#define RECT_AJUST_X 2
#define RECT_AJUST_Y 2
	  SMBprt.setColor(255, 255, 255);
	  SMBprt.drawRect(TAB_START_X, TAB_START_Y, TAB_WIDTH, TAB_START_Y + TAB_HEIGHT + RECT_AJUST_Y);
	  //SMBprt.setColor(255, 0, 0);
	  //SMBprt.setBackColor(255, 0, 0);
	  SMBprt.setColor(0, 0, 255);
	  SMBprt.setBackColor(0, 0, 255);
	  SMBprt.fillRect(TAB_START_X + RECT_AJUST_X, TAB_START_Y, TAB_WIDTH - RECT_AJUST_X, TAB_START_Y + TAB_HEIGHT);
	  SMBprt.setColor(255, 255, 255);
	  SMBprt.setFont(tabFont);
	  SMBprt.print(F("0"), TAB_START_X + RECT_AJUST_X, TAB_START_Y + FONT_AJUST_Y);
	  SMBprt.print(F("1"), TAB_START_X + RECT_AJUST_X, TAB_START_Y + FONT_AJUST_Y + SMBprt.getFontXsize());

	  // shezhi tab: disable
	  // 0 191 255
	  SMBprt.setColor(255, 255, 255);
	  SMBprt.drawRect(TAB_START_X, TAB_START_Y + TAB_HEIGHT, TAB_WIDTH, TAB_START_Y + 2*TAB_HEIGHT + RECT_AJUST_Y + 5);
	  SMBprt.setColor(0, 191, 255);
	  SMBprt.setBackColor(0, 191, 255);
	  SMBprt.fillRect(TAB_START_X + RECT_AJUST_X, TAB_START_Y + TAB_HEIGHT, TAB_WIDTH - RECT_AJUST_X, TAB_START_Y + 2*TAB_HEIGHT + RECT_AJUST_Y + 3);
	  SMBprt.setColor(0, 0, 200);
	  SMBprt.setFont(tabFont);
	  SMBprt.print(F("2"), TAB_START_X + RECT_AJUST_X, TAB_START_Y + FONT_AJUST_Y + TAB_HEIGHT);
	  SMBprt.print(F("3"), TAB_START_X + RECT_AJUST_X, TAB_START_Y + FONT_AJUST_Y + TAB_HEIGHT + SMBprt.getFontXsize());



	  // clr refresh zone
	  SMBprt.setColor(0, 0, 0);
	  SMBprt.fillRect(TAB_WIDTH + RECT_AJUST_X, TAB_START_Y, MAX_X_WIDTH - XINXI_X_GAP, TAB_START_Y + 2*TAB_HEIGHT + RECT_AJUST_Y + 3);

    // current station
    SMBprt.setColor(255, 255, 255);
    SMBprt.setBackColor(0, 0, 0);
    SMBprt.setFont(xinXiFont);
    SMBprt.print(String(asciiArray[4]) + String(asciiArray[5]) + String(asciiArray[3]) + String(asciiArray[25]), x_pos, y_pos); // ��һվ

    x_pos += SMBprt.getFontXsize() * 4; // 3 character --- cz:
    SMBprt.setColor(255,255,255);

#ifdef SELF_TEST
    SMBprt.setFont(HanZi);
    SMBprt.print(F("01234567"), x_pos, y_pos);
#else
    SMBprt.printFont(x_pos, y_pos, XINXI_MENU_PIXELS_WIDTH_PER_CHIN_CHAR, XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR, /*HanZi*/ (uint8_t *)&(SMBprt.pDB->tBusInfo.currentStation[0]), XINXI_MENU_CHIN_CHAR_PER_STATION);
#endif


    // next station
    x_pos = XINXI_START_X;
    y_pos += XINXI_Y_GAP + XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR;
    SMBprt.setColor(255,255,255);
    SMBprt.setBackColor(0,0,0);
    SMBprt.setFont(xinXiFont); // TODO: topFont add ":"
    SMBprt.print(String(asciiArray[1]) + String(asciiArray[2]) + String(asciiArray[3]) + String(asciiArray[25]), x_pos, y_pos);

    x_pos += SMBprt.getFontXsize() * 4; // 3 character --- cc:
    SMBprt.setColor(255,255,255);
#ifdef SELF_TEST
    SMBprt.setFont(HanZi);
    SMBprt.print(F("01234567"), x_pos, y_pos);
#else
    SMBprt.printFont(x_pos, y_pos, XINXI_MENU_PIXELS_WIDTH_PER_CHIN_CHAR, XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR, SMBprt.pDB->tBusInfo.nextStation, XINXI_MENU_CHIN_CHAR_PER_STATION);
#endif

    // YJCD
    x_pos = XINXI_START_X;
    y_pos += XINXI_Y_GAP + XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR;
    SMBprt.setColor(255,255,255);
    SMBprt.setBackColor(0,0,0);
    SMBprt.setFont(xinXiFont); // TODO: topFont add ":"
    SMBprt.print(String(asciiArray[6]) + String(asciiArray[7]) + String(asciiArray[8]) + String(asciiArray[9]) + String(asciiArray[25]), x_pos, y_pos);

    x_pos += SMBprt.getFontXsize() * 5; // 3 character --- cc:

    SMBprt.setFont(xinXiFont);
#ifdef SELF_TEST
    SMBprt.pDB->tBusInfo.autoPassengerCount = 3;
#endif
#define QING 0
#define ZHONG 1
#define MAN 2
#define JI 3
    if ((QING == SMBprt.pDB->tBusInfo.autoPassengerCount)
        || ZHONG == SMBprt.pDB->tBusInfo.autoPassengerCount) {
      SMBprt.setColor(255,255,255);
      if (QING == SMBprt.pDB->tBusInfo.autoPassengerCount) {
        SMBprt.print(String(asciiArray[10]), x_pos, y_pos);
      } else {
        SMBprt.print(String(asciiArray[12]), x_pos, y_pos);
      }
    } else if (MAN == SMBprt.pDB->tBusInfo.autoPassengerCount) {
      SMBprt.setColor(255,255,0);
      SMBprt.print(String(asciiArray[11]), x_pos, y_pos);
    } else if (JI == SMBprt.pDB->tBusInfo.autoPassengerCount) {
      SMBprt.setColor(255,0,0);
      SMBprt.print(String(asciiArray[7]), x_pos, y_pos);
    }

    // wealth & ultra-violet
    x_pos = XINXI_START_X;
    y_pos += XINXI_Y_GAP + XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR;
    SMBprt.setColor(255,255,255);
    SMBprt.setBackColor(0,0,0);
    SMBprt.setFont(xinXiFont); // TODO: topFont add ":"
    SMBprt.print(String(asciiArray[13]) + String(asciiArray[14]) + String(asciiArray[25]), x_pos, y_pos);
    x_pos += SMBprt.getFontXsize() * 3; // 3 character --- cc:

#define SUNNING 0
#define CLOUDY 1
#define CLOUDY_MORE 2
#define RAINY 3

    if (SUNNING == SMBprt.pDB->tCmnInfo.weather) {

      SMBprt.print(String(asciiArray[15]), x_pos, y_pos);

    } else if (CLOUDY == SMBprt.pDB->tCmnInfo.weather) // duoyun
    {
      SMBprt.print(String(asciiArray[16]) + String(asciiArray[17]), x_pos, y_pos);
    } else if (CLOUDY_MORE == SMBprt.pDB->tCmnInfo.weather) {
      SMBprt.print(String(asciiArray[18]), x_pos, y_pos);
    } else if (RAINY == SMBprt.pDB->tCmnInfo.weather) //xiayu
    {
      SMBprt.print(String(asciiArray[19]), x_pos, y_pos);
    }
    x_pos += SMBprt.getFontXsize() * 2; // 3 character --- cc:
    SMBprt.setColor(255,255,255);
    SMBprt.print(String(asciiArray[22]) + String(asciiArray[23]) + String(asciiArray[24]) + String(asciiArray[25]), x_pos, y_pos);
    x_pos += SMBprt.getFontXsize() * 4; // 3 character --- cc:

#define WEAK 0
#define STRONG 1
#define ULTRA_STRONG 2

    if (WEAK == SMBprt.pDB->tCmnInfo.ultraViolet) {

      SMBprt.print(String(asciiArray[27]), x_pos, y_pos);

    } else if (STRONG == SMBprt.pDB->tCmnInfo.ultraViolet) // duoyun
    {
      SMBprt.print(String(asciiArray[26]), x_pos, y_pos);
    } else if (ULTRA_STRONG == SMBprt.pDB->tCmnInfo.ultraViolet) {
      SMBprt.print(String(asciiArray[28]) + String(asciiArray[26]), x_pos, y_pos);
    }

    // PRT("ultraViolet", SMBprt.pDB->tCmnInfo.ultraViolet);


    // temperature & humidity
    x_pos = XINXI_START_X;
    y_pos += XINXI_Y_GAP + XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR;
    SMBprt.setColor(255,255,255);
    SMBprt.setBackColor(0,0,0);
    SMBprt.setFont(xinXiFont); // TODO: topFont add ":"
    SMBprt.print(String(asciiArray[20]) + String(asciiArray[9]) + String(asciiArray[25]), x_pos, y_pos);
    x_pos += SMBprt.getFontXsize() * 3; // 3 character --- cc:
    SMBprt.setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
#define AJUST_NUMBER_FONT 5
    SMBprt.print(String(SMBprt.pDB->tCmnInfo.temperature), x_pos, y_pos + AJUST_NUMBER_FONT);
    x_pos += SMBprt.getFontXsize() * 2.5; // 2.5 character --- cc:
    SMBprt.setFont(xinXiFont); // TODO: thinNumber should be indexed by ASCII char
    SMBprt.print(String(asciiArray[29]), x_pos, y_pos);

    // humidity
#define HUMIDITY_AJUST_X 10
    x_pos += SMBprt.getFontXsize() * 1 + XINXI_X_GAP + HUMIDITY_AJUST_X; // 3 character --- cc:
    SMBprt.setFont(xinXiFont); // TODO: thinNumber should be indexed by ASCII char
    SMBprt.print(String(asciiArray[21]) + String(asciiArray[9]) + String(asciiArray[25]), x_pos, y_pos);
    x_pos += SMBprt.getFontXsize() * 3; // 3 character --- cc:
    SMBprt.setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
    SMBprt.print(String(SMBprt.pDB->tCmnInfo.humidity), x_pos, y_pos + AJUST_NUMBER_FONT);

    // switch state to REFRESH
    xinXi.disFlg = Menu::REFRESH;

  }
  else // else if(Menu::REFRESH == xinXi.disFlg)
  {
    // if just need to refresh
    /**
     * related DB data: current station, next station (*FIXME: here, current station will be displayed as prev station,
     * next station will be the next station which will be arrived); YJCD; wealth, temperature, humidity, ultra-violet
     */
    // chk bitmap related DBdata
    // stations
	    x_pos = XINXI_START_X;
    if (SMBprt.pDB->tBusInfo.displayBitMap.autoCurrStation) {
    	Serial.println(F("auto curr station updated!"));
      // clear
      SMBprt.pDB->tBusInfo.displayBitMap.autoCurrStation = 0;
      // refresh
      SMBprt.setColor(255, 255, 255);
      SMBprt.setBackColor(0, 0, 0);
      SMBprt.setFont(xinXiFont);
      SMBprt.print(String(asciiArray[4]) + String(asciiArray[5]) + String(asciiArray[3]) + String(asciiArray[25]), x_pos, y_pos); // ��һվ

      x_pos += SMBprt.getFontXsize() * 4; // 3 character --- cz:
      SMBprt.setColor(255,255,255);

  #ifdef SELF_TEST
      SMBprt.setFont(HanZi);
      SMBprt.print(F("01234567"), x_pos, y_pos);
  #else
      SMBprt.printFont(x_pos, y_pos, XINXI_MENU_PIXELS_WIDTH_PER_CHIN_CHAR, XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR, SMBprt.pDB->tBusInfo.currentStation, XINXI_MENU_CHIN_CHAR_PER_STATION);
  #endif

      // next station
      x_pos = XINXI_START_X;
      y_pos += XINXI_Y_GAP + XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR;
      SMBprt.setColor(255,255,255);
      SMBprt.setBackColor(0,0,0);
      SMBprt.setFont(xinXiFont); // TODO: topFont add ":"
      SMBprt.print(String(asciiArray[1]) + String(asciiArray[2]) + String(asciiArray[3]) + String(asciiArray[25]), x_pos, y_pos);

      x_pos += SMBprt.getFontXsize() * 4; // 3 character --- cc:
      SMBprt.setColor(255,255,255);
  #ifdef SELF_TEST
      SMBprt.setFont(HanZi);
      SMBprt.print(F("01234567"), x_pos, y_pos);
  #else
      SMBprt.printFont(x_pos, y_pos, XINXI_MENU_PIXELS_WIDTH_PER_CHIN_CHAR, XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR, SMBprt.pDB->tBusInfo.nextStation, XINXI_MENU_CHIN_CHAR_PER_STATION);
  #endif

    } else {

        // next station
        y_pos += XINXI_Y_GAP + XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR;


    }

    // YJCD
    x_pos = XINXI_START_X;
    if (SMBprt.pDB->tBusInfo.displayBitMap.autoPassengerCount) {
    	Serial.println(F("autoPassengerCount updated!"));
      // clear
      SMBprt.pDB->tBusInfo.displayBitMap.autoPassengerCount = 0;
      // refresh
      // YJCD
      y_pos += XINXI_Y_GAP + XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR;
      SMBprt.setColor(255,255,255);
      SMBprt.setBackColor(0,0,0);
      SMBprt.setFont(xinXiFont); // TODO: topFont add ":"
      SMBprt.print(String(asciiArray[6]) + String(asciiArray[7]) + String(asciiArray[8]) + String(asciiArray[9]) + String(asciiArray[25]), x_pos, y_pos);

      x_pos += SMBprt.getFontXsize() * 5; // 3 character --- cc:

      SMBprt.setFont(xinXiFont);
  #ifdef SELF_TEST
      SMBprt.pDB->tBusInfo.autoPassengerCount = 3;
  #endif
  #define QING 0
  #define ZHONG 1
  #define MAN 2
  #define JI 3
      if ((QING == SMBprt.pDB->tBusInfo.autoPassengerCount)
          || ZHONG == SMBprt.pDB->tBusInfo.autoPassengerCount) {
        SMBprt.setColor(255,255,255);
        if (QING == SMBprt.pDB->tBusInfo.autoPassengerCount) {
          SMBprt.print(String(asciiArray[10]), x_pos, y_pos);
        } else {
          SMBprt.print(String(asciiArray[12]), x_pos, y_pos);
        }
      } else if (MAN == SMBprt.pDB->tBusInfo.autoPassengerCount) {
        SMBprt.setColor(255,255,0);
        SMBprt.print(String(asciiArray[11]), x_pos, y_pos);
      } else if (JI == SMBprt.pDB->tBusInfo.autoPassengerCount) {
        SMBprt.setColor(255,0,0);
        SMBprt.print(String(asciiArray[7]), x_pos, y_pos);
      }


    } else {
      // YJCD
      y_pos += XINXI_Y_GAP + XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR;

    }

    // wealth
    x_pos = XINXI_START_X;
    if (SMBprt.pDB->tCmnInfo.displayBitMap.weather) {
    	Serial.println(F("weather updated!"));
      // clear
      SMBprt.pDB->tCmnInfo.displayBitMap.weather = 0;
      // refresh
      // wealth
      x_pos = XINXI_START_X;
          y_pos += XINXI_Y_GAP + XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR;
          SMBprt.setColor(255,255,255);
          SMBprt.setBackColor(0,0,0);
          SMBprt.setFont(xinXiFont); // TODO: topFont add ":"
          SMBprt.print(String(asciiArray[13]) + String(asciiArray[14]) + String(asciiArray[25]), x_pos, y_pos);
          x_pos += SMBprt.getFontXsize() * 3; // 3 character --- cc:

      #define SUNNING 0
      #define CLOUDY 1
      #define CLOUDY_MORE 2
      #define RAINY 3

          if (SUNNING == SMBprt.pDB->tCmnInfo.weather) {

            SMBprt.print(String(asciiArray[15]), x_pos, y_pos);

          } else if (CLOUDY == SMBprt.pDB->tCmnInfo.weather) // duoyun
          {
            SMBprt.print(String(asciiArray[16]) + String(asciiArray[17]), x_pos, y_pos);
          } else if (CLOUDY == SMBprt.pDB->tCmnInfo.weather) {
            SMBprt.print(String(asciiArray[18]), x_pos, y_pos);
          } else if (CLOUDY == SMBprt.pDB->tCmnInfo.weather) //xiayu
          {
            SMBprt.print(String(asciiArray[19]), x_pos, y_pos);
          }


    } else {
      // wealth & ultra-violet
      y_pos += XINXI_Y_GAP + XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR;
      x_pos += SMBprt.getFontXsize() * 3; // 3 character --- cc:

    }

    // ultra-violet
    if (SMBprt.pDB->tCmnInfo.displayBitMap.ultraViolet) {
    	Serial.println(F("ultraViolet updated!"));
      // clear
      SMBprt.pDB->tCmnInfo.displayBitMap.ultraViolet = 0;
      // refresh
      x_pos += SMBprt.getFontXsize() * 2; // 3 character --- cc:
      SMBprt.setColor(255,255,255);
      SMBprt.print(String(asciiArray[22]) + String(asciiArray[23]) + String(asciiArray[24]) + String(asciiArray[25]), x_pos, y_pos);
      x_pos += SMBprt.getFontXsize() * 4; // 3 character --- cc:

#define WEAK 0
#define STRONG 1
#define ULTRA_STRONG 2

    if (WEAK == SMBprt.pDB->tCmnInfo.ultraViolet) {

      SMBprt.print(String(asciiArray[27]), x_pos, y_pos);

    } else if (STRONG == SMBprt.pDB->tCmnInfo.ultraViolet) // duoyun
    {
      SMBprt.print(String(asciiArray[26]), x_pos, y_pos);
    } else if (ULTRA_STRONG == SMBprt.pDB->tCmnInfo.ultraViolet) {
      SMBprt.print(String(asciiArray[28]) + String(asciiArray[26]), x_pos, y_pos);
    }


    } else {
      x_pos += SMBprt.getFontXsize() * 2; // 3 character --- cc:
      x_pos += SMBprt.getFontXsize() * 4; // 3 character --- cc:

    }

    // temperature
    x_pos = XINXI_START_X;
    if (SMBprt.pDB->tCmnInfo.displayBitMap.temperature) {
    	Serial.println(F("temperature updated!"));
      // clear
      SMBprt.pDB->tCmnInfo.displayBitMap.temperature = 0;
      // refresh
      // temperature
      x_pos = XINXI_START_X;
        y_pos += XINXI_Y_GAP + XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR;
        SMBprt.setColor(255,255,255);
        SMBprt.setBackColor(0,0,0);
        SMBprt.setFont(xinXiFont); // TODO: topFont add ":"
        SMBprt.print(String(asciiArray[20]) + String(asciiArray[9]) + String(asciiArray[25]), x_pos, y_pos);
        x_pos += SMBprt.getFontXsize() * 3; // 3 character --- cc:
        SMBprt.setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
#define AJUST_NUMBER_FONT 5
        SMBprt.print(String(SMBprt.pDB->tCmnInfo.temperature), x_pos, y_pos + AJUST_NUMBER_FONT);
        x_pos += SMBprt.getFontXsize() * 2; // 3 character --- cc:
        SMBprt.setFont(xinXiFont); // TODO: thinNumber should be indexed by ASCII char
        SMBprt.print(String(asciiArray[29]), x_pos, y_pos);

    } else {
      // temperature
      y_pos += XINXI_Y_GAP + XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR;
      SMBprt.setFont(xinXiFont);
      x_pos += SMBprt.getFontXsize() * 3; // 3 character --- cc:
      SMBprt.setFont(thinNumber);
      x_pos += SMBprt.getFontXsize() * 3 + XINXI_X_GAP; // 3 character --- cc:

    }
    // humidity
    if (SMBprt.pDB->tCmnInfo.displayBitMap.humidity) {
    	Serial.println(F("humidity updated!"));
      // clear
      SMBprt.pDB->tCmnInfo.displayBitMap.humidity = 0;
      // refresh
      // humidity
#define HUMIDITY_AJUST_X 10
      x_pos += SMBprt.getFontXsize() * 1 + XINXI_X_GAP + HUMIDITY_AJUST_X; // 3 character --- cc:
        SMBprt.setFont(xinXiFont); // TODO: thinNumber should be indexed by ASCII char
        SMBprt.print(String(asciiArray[21]) + String(asciiArray[9]) + String(asciiArray[25]), x_pos, y_pos);
        x_pos += SMBprt.getFontXsize() * 3; // 3 character --- cc:
        SMBprt.setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
        SMBprt.print(String(SMBprt.pDB->tCmnInfo.humidity), x_pos, y_pos + AJUST_NUMBER_FONT);
    } else {
      // humidity
        x_pos += SMBprt.getFontXsize() * 1 + XINXI_X_GAP; // 3 character --- cc:
        SMBprt.setFont(xinXiFont); // TODO: thinNumber should be indexed by ASCII char
        // SMBprt.print(F("sd:"), x_pos, y_pos);
        //SMBprt.print(String(asciiArray[21] + asciiArray[9] + asciiArray[25]), x_pos, y_pos);
        x_pos += SMBprt.getFontXsize() * 3; // 3 character --- cc:
        SMBprt.setFont(thinNumber); // TODO: thinNumber should be indexed by ASCII char
        //SMBprt.print(String(SMBprt.pDB->tCmnInfo.humidity), x_pos, y_pos);

    }

  }


}

/*void SMBprint::displayBaoZhan() {
 Serial.println(F("displayBaoZhan in"));
 }*/
#define SHEZHI_START_X	80
#define SHEZHI_START_Y	50
#define SHEZHI_X_GAP		10
#define SHEZHI_Y_GAP		10
/* ��(0) ��(1) ��(2) ͨ(3) ��(4) ��(5) ��(6) ��(7) ��(8)*/
void displaySheZhi(Menu &sheZhi, Menu &last) {
#if 1
  US x_pos = XINXI_START_X;
  US y_pos = XINXI_START_Y;

  //Serial.println(F("displaySheZhi in"));

  if (Menu::REDRAW == sheZhi.disFlg) {

	  // handle tab
	  // xinxi tab: disable
	  // 0 191 255
#define FONT_AJUST_Y 15
#define RECT_AJUST_X 2
#define RECT_AJUST_Y 2
	  SMBprt.setColor(255, 255, 255);
	  SMBprt.drawRect(TAB_START_X, TAB_START_Y, TAB_WIDTH, TAB_START_Y + TAB_HEIGHT + RECT_AJUST_Y);
	  SMBprt.setColor(0, 191, 255);
	  SMBprt.setBackColor(0, 191, 255);
	  SMBprt.fillRect(TAB_START_X + RECT_AJUST_X, TAB_START_Y, TAB_WIDTH - RECT_AJUST_X, TAB_START_Y + TAB_HEIGHT);
	  SMBprt.setColor(0, 0, 200);
	  SMBprt.setFont(tabFont);
	  SMBprt.print(F("0"), TAB_START_X + RECT_AJUST_X, TAB_START_Y + FONT_AJUST_Y);
	  SMBprt.print(F("1"), TAB_START_X + RECT_AJUST_X, TAB_START_Y + FONT_AJUST_Y + SMBprt.getFontXsize());

	  // shezhi tab: enable
	  // 255 0 0
	  SMBprt.setColor(255, 255, 255);
	  SMBprt.drawRect(TAB_START_X, TAB_START_Y + TAB_HEIGHT, TAB_WIDTH, TAB_START_Y + 2*TAB_HEIGHT + RECT_AJUST_Y + 5);
	  SMBprt.setColor(0, 0, 255);
	  SMBprt.setBackColor(0, 0, 255);
	  SMBprt.fillRect(TAB_START_X + RECT_AJUST_X, TAB_START_Y + TAB_HEIGHT, TAB_WIDTH - RECT_AJUST_X, TAB_START_Y + 2*TAB_HEIGHT + RECT_AJUST_Y + 3);
	  SMBprt.setColor(255, 255, 255);
	  SMBprt.setFont(tabFont);
	  SMBprt.print(F("2"), TAB_START_X + RECT_AJUST_X, TAB_START_Y + FONT_AJUST_Y + TAB_HEIGHT);
	  SMBprt.print(F("3"), TAB_START_X + RECT_AJUST_X, TAB_START_Y + FONT_AJUST_Y + TAB_HEIGHT + SMBprt.getFontXsize());

	  // clr refresh zone
	  SMBprt.setColor(0, 0, 0);
	  SMBprt.fillRect(TAB_WIDTH + RECT_AJUST_X, TAB_START_Y, MAX_X_WIDTH - SHEZHI_X_GAP, TAB_START_Y + 2*TAB_HEIGHT + RECT_AJUST_Y + 3);

    // device setting
	SMBprt.setColor(255, 255, 255);
    SMBprt.setBackColor(0,0,0);
    SMBprt.setFont(sheZhiFont); // TODO: topFont add ":"
    // SMBprt.print(F("sbsz"), x_pos, y_pos); //
    SMBprt.print(F("0102"), x_pos, y_pos); //

    y_pos += SHEZHI_Y_GAP + XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR;

    // SMBprt.print(F("txsz"), x_pos, y_pos);
    SMBprt.print(F("3402"), x_pos, y_pos);
    y_pos += SHEZHI_Y_GAP + XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR;

    // SMBprt.print(F("mmsz"), x_pos, y_pos);
    SMBprt.print(F("5602"), x_pos, y_pos);
    y_pos += SHEZHI_Y_GAP + XINXI_MENU_PIXELS_HEIGHT_PER_CHIN_CHAR;

    // SMBprt.print(F("gy"), x_pos, y_pos);
    SMBprt.print(F("78"), x_pos, y_pos);


    // switch state to REFRESH
    sheZhi.disFlg = Menu::REFRESH;
  }
  else
  {
	  // do sheZhi
  }
#endif
}


// handler keyprocess func

void SMBKeypad::procKeyXinXi(Menu &xinXi, char *key) {
  //Serial.println(F("procKeyXinXi in"));
}

//void procKeyBaoZhan(char *key) {
//	Serial.println(F("procKeyBaoZhan in"));
//}

void SMBKeypad::procKeySheZhi(Menu &sheZhi, char *key) {
  //Serial.println(F("procKeySheZhi in"));
}
