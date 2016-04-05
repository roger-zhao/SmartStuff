/*
 * main_loop.cpp
 *
 *  Created on:
 *      Author: yanjie
 */

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
//using for fifo
#include<sys/types.h>
#include<sys/stat.h>
//select
#include <sys/select.h>

#include "common.h"

#include "../../os/os_task.h"
#include "../../os/os_ipc.h"
#include "../../app/gps/app_gps.h"
#include "omc.h"





extern cGPSApp *pcGpsApp;
extern int msgSendFifo;
extern pthread_mutex_t counter_lock;
void MsgSend::init()
{
    DBG(DBG_INFO, "enter MsgSend init!");

}

void MsgSend::run()
{
    DBG(DBG_INFO, "enter MsgSend run!");
    fd_set input_fd; /*FD SET to be passed to select call */
    int retSelect; /* Return value of select call*/
    struct timeval timeout; /* Timeout to be passed to select call */
    int readRet = 0;
    char readbuf[600] = { 0 };
    GPS_INFO * pGpsObj = NULL;
    MessageInfo* pMsgObj = NULL;

    while (1)
    {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        /*
         ** Assigning the FD to be supplied to select call
         */
        FD_ZERO(&input_fd);
        FD_SET(msgSendFifo, &input_fd);

        /*
         ** Select call to block on the assigned FDs
         */
        retSelect = select(msgSendFifo + 1, &input_fd, NULL, NULL, &timeout);
        if (retSelect == -1)
            printf("select error\n");
        if (FD_ISSET(msgSendFifo, &input_fd))
        {
            readRet = read(msgSendFifo, (void*) readbuf, sizeof(readbuf));
            if (readRet > 0)
            {
                //printf("read sucess:%s\n", readbuf);
                pMsgObj = (MessageInfo*) readbuf;
                //printf("msgid %d\n", pMsgObj->msgid);
                switch (pMsgObj->msgid)
                {
                case MSG_GPS_ID:
                    pGpsObj = (GPS_INFO*) pMsgObj->message;
                    printf("sec %d\n", pGpsObj->D.second);
                    break;
                default:
                    break;
                }
            }
            else
            {
                printf("read fifo failed \n");
            }
        }
    }
}

void MsgSend::stop()
{
    DBG(DBG_INFO, "enter MsgSend stop!");
}
