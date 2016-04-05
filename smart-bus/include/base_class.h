#ifndef _BASE_CLASS_H
#define _BASE_CLASS_H


#include <common.h>

using namespace std;

// define base class
class SMBase
{
    public:
        static string sClassName;

	    SMBase() {};
	    virtual ~SMBase() {};
};


// define app base class
class APP: public SMBase 
{
    public:
        static string sAppName;
        SmartBusAppState m_state;

	    APP() {};
	    virtual ~APP() {};

        virtual void init() {};
        virtual void run(){/*enable adpter*/}
        virtual void stop(){/* stop adapter*/}
        virtual SmartBusAppState getAppStatus(){return m_state;}
        virtual void updateState(SmartBusAppState status) {m_state = status;}
        virtual void createTimer(int seconds){};
        virtual void executecmd(string cmd){};// cal system to execute cmd like shell script
        virtual void formAdpCmd(string cmd){};
};

#endif
