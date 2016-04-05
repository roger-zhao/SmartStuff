/*
 * app_dvr.cpp
 *
 *  Created on: Jun 1, 2015
 *      Author: ZhaoYJ 
 */
#include "app_dvr.h"

DvrApp::DvrApp()
{
    this->pcAdp = new DvrAdp();
}

DvrApp::~DvrApp()
{
    delete this->pcAdp; 
}

// connect with HISI board
void DvrApp::init()
{
    DBG(DBG_INFO, "enter DvrApp init!");
    eRet ret = eOK;
    ret = this->pcAdp->dataClientInit();
    ASSERT(eOK == ret);
}

// get infos from HISI
void DvrApp::run()
{
    eRet ret = eFail;
    U8 always = 1;
    U16 Dvrlogin = 0;
    U16 loginRegVal = 0;

    time_t now;
    struct tm *timeNow;
    U16 year;
    union uts_monthDay uMd;
    union uts_hourMin uHm;
    union uts_secMs uSm;

    U16 APC_in = 0;
    U16 APC_out = 0;
    U16 memUse = 0;
    U16 cpuLoad = 0;
    U32 timeInterval = 0;
    DVR_dateTime dt;

    DBG(DBG_INFO, "enter DvrApp run!");
    while(always)    
    {
        // data client 
        ret = this->pcAdp->dataClientRun();
        if(eFail == ret)
        {
            DBG(DBG_ERROR, "DVR board seems dead!");
            // notify DVR state
            this->setDvrState(-1);
        }
        else
        {
            this->setDvrState(1);
        }

        if(!Dvrlogin)
        {
            // Dvr login
            ret = this->pcAdp->getVal(LOGIN_MODREG_DVR, &loginRegVal);
            ASSERT(eOK == ret);
            
            if(LOGIN_REQ == loginRegVal)
            {
                ret = this->pcAdp->setVal(LOGIN_MODREG_DVR, LOGIN_RES);
                ASSERT(eOK == ret);
                // finish HMI login 
                Dvrlogin = 1;
            }
            else if(LOGIN_RES == loginRegVal)
            {
                // already login: mightbe after login, BBB power off, but DVR is running still
                Dvrlogin = 1;
            }
        }
        else
        {
            // re-login?
            // DVR login
            ret = this->pcAdp->getVal(LOGIN_MODREG_DVR, &loginRegVal);
            ASSERT(eOK == ret);
            
            if(LOGIN_REQ == loginRegVal)
            {
                ret = this->pcAdp->setVal(LOGIN_MODREG_DVR, LOGIN_RES);
                ASSERT(eOK == ret);
                // finish HMI login 
                Dvrlogin++;
                DBG(DBG_WARN, "DVR board re-login: %d!", Dvrlogin);
            }

            // data-exchange with DVR board:
            // time-sync
            // if((OsTimer::getMs() - timeInterval) > 500)
            {
                // timeInterval = OsTimer::getMs();
                time(&now);
                timeNow = localtime(&now);
                year = timeNow->tm_year;
                uMd.md.mon = timeNow->tm_mon;
                uMd.md.day = timeNow->tm_mday;
                uHm.hm.hour = timeNow->tm_hour;
                uHm.hm.min = timeNow->tm_min;
                uSm.sm.sec = timeNow->tm_sec;
                dt.year = (U16)year;
                dt.monDay = (U16)uMd.val;
                dt.hourMin = (U16)uHm.val;
                dt.secMs = (U16)uSm.val;
                // refresh to DVR board
                ret = this->pcAdp->setVals(TIME_SYNC_YEAR_MODREG_DVR, (U16 *)&dt, 4);
                ASSERT(eOK == ret);

                // get APC value (might be not used)
                ret = this->pcAdp->getVal(RPT_INFO_APC_CNT_IN_MODREG_DVR, &APC_in);
                ASSERT(eOK == ret);
                ret = this->pcAdp->getVal(RPT_INFO_APC_CNT_OUT_MODREG_DVR, &APC_out);
                ASSERT(eOK == ret);
                // update to main_loop
                this->setAPCnt((APC_in > APC_out)?(APC_in - APC_out):0);

                // get mem & cpu(use 5min load)
                ret = this->pcAdp->getVal(RPT_INFO_CPUINFO_LOAD5MIN_MODREG_DVR, &cpuLoad);
                ASSERT(eOK == ret);
                ret = this->pcAdp->getVal(RPT_INFO_CPUINFO_MEM_MODREG_DVR, &memUse);
                ASSERT(eOK == ret);
                // update to main_loop
                this->setCpuLoad(cpuLoad);
                this->setMemUse(memUse);
            }

            // delay 500 ms
            usleep(500*1000);

        }
    }
}

void DvrApp::stop()
{
    eRet ret = eOK;
    // delete dataclient
    ret = this->pcAdp->dataClientStop();
    ASSERT(eOK == ret);
}
