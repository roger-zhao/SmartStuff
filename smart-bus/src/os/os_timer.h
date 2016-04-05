/*
 * os_timer.h
 *
 *  Created on: 07-29-2015
 *      Author: yanjie 
 */

#ifndef OS_TIMER_H
#define OS_TIMER_H

#include <signal.h>
#include <time.h>
#include <sys/time.h>

#include "common.h"
#include "base_class.h"

typedef void (*pFunc)(union sigval sv);

class OsTimer: public SMBase
{
    public:
	    OsTimer() {};
	    OsTimer(U32 _startMs, U32 _periodMs, pFunc _func, void *_that);
	    ~OsTimer() {};
        void init(U32 _startMs, U32 _periodMs, pFunc _func, void *_that);
        eRet create();
        eRet start();
        eRet destroy();
        static ULL getMs() 
        {
            int ret = 0;
            struct timeval tv;
            ret = gettimeofday(&tv, NULL);
            ASSERT(ret != -1);
            return ((ULL)tv.tv_sec*1000 + (ULL)tv.tv_usec/1000);
        }
    private:
        timer_t timerId;
        sigevent sev;
        itimerspec ts;

};

#endif
