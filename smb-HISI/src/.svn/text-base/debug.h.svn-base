#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef BOARD
#define DEBUG_MSG 1 
#endif

// specify DBG level
#define DBG_NULL 0x0
#define DBG_FATAL 0x2
#define DBG_ERROR (DBG_FATAL + 0x1)
#define DBG_WARN  (DBG_ERROR + 0x1)
#define DBG_INFO  (DBG_WARN + 0x1) 

#ifndef DBG_LVL
#define DBG_LVL DBG_WARN
#endif

#ifndef DBGK_LVL
#define DBGK_LVL DBG_NULL
#endif


/**
 * Debug macro
 * Params:
 *   @param level - level of details of debug message.
 *   @param format - Formatting string for the debug message
 *   @param ... -
 */
#ifdef BOARD
#define PRT(format,...) do { \
                            if(DEBUG_MSG) printf(format, ##__VA_ARGS__);\
                        }while(0)
#define PRTK(format,...) do { \
                            if(DEBUG_MSG) printk(format, ##__VA_ARGS__);\
                        }while(0)
#elif defined(HOST_PC)
#define PRT(format,...) printf(format, ##__VA_ARGS__);
#define PRTK(format,...) printk(format, ##__VA_ARGS__);
#endif

#define DBG(level,format,...) {if(level<=DBG_LVL){PRT( "%s:%i:"#format"\n",__FILE__,__LINE__,##__VA_ARGS__);}}
#define DBGK(level,format,...) {if(level<=DBGK_LVL){PRTK( "%s:%i:"#format"\n",__FILE__,__LINE__,##__VA_ARGS__);}}

#endif /* DEBUG_H_ */
