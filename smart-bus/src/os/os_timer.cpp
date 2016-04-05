/*
 * os_timer.cpp
 *
 *  Created on: 07-29-2015
 *      Author: yanjie 
 */

#include "os_timer.h"


OsTimer::OsTimer(U32 _startMs, U32 _periodMs, pFunc _func, void *_that) {
    this->ts.it_value.tv_sec = _startMs / 1000;
    this->ts.it_value.tv_nsec = (_startMs%1000) * 1000000;
    this->ts.it_interval.tv_sec = _periodMs / 1000;
    this->ts.it_interval.tv_nsec = (_periodMs%1000) * 1000000;
    this->sev.sigev_notify = SIGEV_THREAD;
    this->sev.sigev_value.sival_ptr = _that; // for compatible with static function member in that class (invoke OsTimer class)
    this->sev.sigev_notify_function = _func;
    this->sev.sigev_notify_attributes = NULL;
    this->create();
}

void OsTimer::init(U32 _startMs, U32 _periodMs, pFunc _func, void *_that) {
    this->ts.it_value.tv_sec = _startMs / 1000;
    this->ts.it_value.tv_nsec = (_startMs%1000) * 1000000;
    this->ts.it_interval.tv_sec = _periodMs / 1000;
    this->ts.it_interval.tv_nsec = (_periodMs%1000) * 1000000;
    this->sev.sigev_notify = SIGEV_THREAD;
    this->sev.sigev_value.sival_ptr = _that;
    this->sev.sigev_notify_function = _func;
    this->sev.sigev_notify_attributes = NULL;

}

eRet OsTimer::create() {
    int ret = 0;
    ret = timer_create(CLOCK_REALTIME, &this->sev, &this->timerId);
    ASSERT(ret != -1);
    return eOK;
}

eRet OsTimer::start() {
    int ret = 0;
    ret = timer_settime(this->timerId, 0, &this->ts, 0);
    ASSERT(ret != -1);
    return eOK;
}

eRet OsTimer::destroy() {
    int ret = 0;
    ret = timer_delete(this->timerId);
    ASSERT(ret != -1);
    return eOK;
}

