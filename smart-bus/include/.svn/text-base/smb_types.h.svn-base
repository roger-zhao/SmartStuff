/*
********************************************************************************
*  
* (c) Copyright 2015 smb-group
*
* Filename : smb_types.h
* Programmer(s): Zhao Yanjie
* Created : 2015/02/
* Modified :
* Description : Define of data type.
********************************************************************************
*/

#ifndef _SMB_TYPES_H
#define _SMB_TYPES_H

#ifndef NULL
#define NULL		((void *) 0)
#endif



// basic types
typedef	unsigned char	U8;		// 8-bit value
typedef	         char	S8;		// 8-bit value
typedef	         char	CHAR;		// 8-bit value
typedef	unsigned short	USHORT;		// 16-bit value
typedef	unsigned short	U16;		// 16-bit value
typedef	         short	S16;		// 16-bit value
typedef	         int	INT32;		
typedef	         int	S32;		
typedef	unsigned int	UINT32;		
typedef	unsigned int	U32;		
typedef	unsigned long	UL;		
typedef	unsigned long long	ULL;		


// self-define enumerate
// return val
typedef enum {
    eOK = 0x0, 
    eFail = ~(0x0),
} eRet;

// copy from GPS comm.h
#define GPS_RECORD_SIZE_DEFAUT 1000 /*save 1000 records even if communication disconnect*/
#define GPS_RECTAREA_SIZE 30 /*define 30 rectangle area to report station auto*/

typedef enum
{
    ADP_IDLE      =  0,
    ADP_READY,
    ADP_RUNNING,
    ADP_WORKING,
    ADP_SLEEP,
    ADP_UnKnown
} SmartBusAdpState;

typedef enum
{
	APP_DISABLE = 0,
    APP_ENABLE,
    APP_RUNNING,
    APP_WORKING,
    APP_NOTLOAD,
    APP_LOADING,
    APP_LOADED,
    APP_UnKnown
} SmartBusAppState;

typedef enum
{
    DEV_ENABLE      =  0,
    DEV_DISABLE,
    DEV_SLEEP,
    DEV_WAKEUP,
    DEV_RESET,
    DEV_UPDATE,
    DEV_UnKnown
} OperateDev;

#endif		// _SMB_TYPES_H_

