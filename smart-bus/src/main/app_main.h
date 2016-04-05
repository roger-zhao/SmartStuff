/*
 * main_loop.h
 *
 *  Created on: 
 *      Author: yanjie 
 */

#ifndef APP_MAIN_H 
#define APP_MAIN_H 

#include "common.h"
#include "base_class.h"

class MainLoop: public APP 
{
    public:
    	MainLoop() {};
    	~MainLoop() {};
        void init(void);
        void run(void);
        void stop(void);

        char firstFlag;
};

#endif 
