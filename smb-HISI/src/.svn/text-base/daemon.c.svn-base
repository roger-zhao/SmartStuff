/*
 *
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * ***** END GPL LICENSE BLOCK *****
 *
 * Contributor(s): Jiri Hnidek <jiri.hnidek@tul.cz>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "smb_types.h"
#include "test.h"
#include "inter_cpu.h"
#include "apps.h"
#include "sys.h"
#include "info.h"
#include "debug.h"

static int running = 0;
static int counter = 0;
char *app_name = NULL;
static int pid_fd = -1;
static char *pid_file_name = NULL;
static FILE *log_stream;
static UCHAR firstRun = 1;
static UCHAR reRun = 0;

#define DEF_PROC_NUM 3
#define DEF_PID 6789 

#define DEF_IP "192.68.2.20" 
#define DEF_MODBUS_PORT 1502 

tProc defProc[DEF_PROC_NUM] = {
    0, DEF_PID, eNoManage,"./live555MediaServer", 0, {0, },
    1, DEF_PID, eNoManage, "./sample_venc", 1, {0, },
    2, DEF_PID, eNoManage, "./smb_APC", 0, {0, },
};

// DB
tProcInfo procInfo;
tInterCPUInfo interCPUInfo;
tSysInfo sysInfo;



// system init(DB init, ...);
UCHAR sysInit()
{
    UCHAR ret = EXIT_SUCCESS;
    UCHAR procIdx = 0;
    CHAR ipAddr[128];

    DBG(DBG_INFO, "sysinit in");
    // init procInfo when no conf file
    if(0 == procInfo.ucProcNum)
    {
        procInfo.ucProcNum = DEF_PROC_NUM;

        for(; procIdx < procInfo.ucProcNum; procIdx++)
        {
            procInfo.proc[procIdx] = defProc[procIdx];
        }
    }

    // create shm
    if(EXIT_FAILURE == createShms())
    {
        DBG(DBG_ERROR, "err: create Shm failed ");
        return EXIT_FAILURE;
    }

    // init interCPUInfo
    interCPUInfo.ullRxCnt = 0;
    interCPUInfo.ullTxCnt = 1;
    // Tx: login
    g_loginOK = 0;
    interCPUInfo.tx[0].regAddr = LOGIN_MODREG;
    interCPUInfo.tx[0].regVal = LOGIN_REQ;
    interCPUInfo.tx[0].eFlag = eWrite;
    // modbus conn 
    if(0 == getLocalIp(ipAddr))
    {
        DBG(DBG_INFO, "IP: %s", ipAddr);
        printf("IP: %s\n", ipAddr);
    }
    else
    {
        strcpy(ipAddr, DEF_IP);
    }

    ctx = modbus_new_tcp(ipAddr, DEF_MODBUS_PORT);


    // init sysInfo 
    sysInfo.ucCPULoad = 0;
    sysInfo.ucCPUMemUse = 0;
    sysInfo.ucCPUDiskUse = 0;

#if 0

    for(procIdx = 0; procIdx < procInfo.ucProcNum; procIdx++)
    {
        procInfo.proc[procIdx] = defProc[procIdx];
        printf("proc: %d, \"%s\", argc: %d, argv: %s\n",
                procInfo.proc[procIdx].ucProcIdx, procInfo.proc[procIdx].cProcCmd,
                procInfo.proc[procIdx].ucProcArgc, procInfo.proc[procIdx].cProcArgv
                );
    }
#endif

    DBG(DBG_INFO, "sysinit out");

    return ret;
}

// according to modbus msg to manage procs
UCHAR manageProcsViaCmd(tProcInfo* ptProcInfo, tInterCPUInfo* ptInterCPUInfo)
{
    UCHAR ret = EXIT_SUCCESS;
    UCHAR msgIdx = 0;
    ULL rxCnt = ptInterCPUInfo->ullRxCnt;
    U16 reg = 0xffff;
    U16 val = 0x0;
    U16 stat = 0;
    CHAR* date = "06-11";
    U16 month = 0;
    U16 day = 0;
    struct tm time;
    struct timeval tv;
    time_t setTime;
    CHAR APCModTmp[8] = "0";
    CHAR APCMod[128] = "0 0 0 0 0 0";
    UCHAR IPp1 = 0;
    UCHAR IPp2 = 0;
    CHAR IPstrP1[8];
    CHAR IP[128];

    DBG(DBG_INFO, "%s in", __FUNCTION__);

    for(msgIdx = 0; msgIdx < rxCnt; msgIdx++)
    {
        if(eWrite == ptInterCPUInfo->rx[msgIdx].eFlag)
        {
            // clear
            ptInterCPUInfo->rx[msgIdx].eFlag = eNA;
            // get info
            reg = ptInterCPUInfo->rx[msgIdx].regAddr;
            val = ptInterCPUInfo->rx[msgIdx].regVal;

            // parse info
            switch(reg)
            {
                // time sync: 
                case TIME_SYNC_YEAR_MODREG:
                    time.tm_year = val;
                    break;
                case TIME_SYNC_MON_DAY_MODREG:
                    // 4:5 of val
                    time.tm_mon = (val >> 5) & (0xF); 
                    time.tm_mday = val & (0x1F); 
                    break;
                case TIME_SYNC_HOUR_MIN_MODREG:
                    // 5:8
                    time.tm_hour = (val >> 8) & (0x1F); 
                    time.tm_min = val & (0xFF); 
                    break;
                case TIME_SYNC_SEC_MS_MODREG:
                    // 8:8 (ms accuracy: 1000/64 = 15.625ms)
                    // ms: >500ms, + 1s
                    time.tm_sec = (val >> 8) & 0xFF;
                    // set systime
                    setTime = mktime(&time);
                    tv.tv_sec = setTime;
                    tv.tv_usec = 0;
                    // printf("Set system datatime now!\n");
#if 1
                    if(settimeofday(&tv, NULL) < 0)
                    {
                        printf("Set system datatime error: %d!\n", errno);
                        DBG(DBG_ERROR, "Set system datatime error: %d!", errno);
                    }
#endif
                    break;

                // rpt 
                // case RPT_INFO_CPUINFO_LOAD_MODREG:
                // case RPT_INFO_CPUINFO_MEM_MODREG:
                // case RPT_INFO_CPUINFO_DISK_MODREG:

                // proc manage
                case MANAGE_DEF_PROCS_MODREG:

                // cmd 
                case CMD_VENC_MODREG:
                case CMD_VENC_CODEC_MODREG:
                case CMD_VPSS_MODREG:

                // APC parameters
                // BBB cmd APC to counting passenger
                // BBB cmd(0x1234) ->APC: APC counting start
                // BBB cmd(0x6789) ->APC: APC counting stop
                case CMD_APC_DO_COUNTING_MODREG:
                    printf("get cmd of APC in daemon: 0x%04x\n", val);
                    if(EXIT_FAILURE == sendCmd2APC(eCmdDoCounting, val))
                    {
                        DBG(DBG_ERROR, "err: notify APC to counting: %d", val);
                        return EXIT_FAILURE;
                    }
                    break;
                case CMD_APC_CODEC_MODREG:
                case CMD_APC_R_MAXMOD_MODREG:
                case CMD_APC_R_MINMOD_MODREG:
                case CMD_APC_G_MAXMOD_MODREG:
                case CMD_APC_G_MINMOD_MODREG:
                case CMD_APC_B_MAXMOD_MODREG:
                case CMD_APC_B_MINMOD_MODREG:

                // IP
                case CMD_SYS_IP1_MODREG:
                    // high byte
                    IPp1 = val >> 8;
                    // low byte
                    IPp2 = val & 0xFF;
                    break;
                case CMD_SYS_IP2_MODREG:
                    // restore IP1
                    sprintf(IP, "%d.%d.", IPp1, IPp2);

                    // high byte
                    IPp1 = val >> 8;
                    sprintf(IPstrP1, "%d.", IPp1);
                    strcat(IP, IPstrP1);
                    // low byte
                    IPp2 = val & 0xFF;
                    sprintf(IPstrP1, "%d", IPp2);
                    strcat(IP, IPstrP1);
                    // printf("new IP: <%s>\n", IP);
                    DBG(DBG_INFO, "new IP: %s", IP);
#if 1
                    if(EXIT_FAILURE == setLocalIP(IP))
                    {
                        DBG(DBG_ERROR, "err: set IP failed ");
                        return EXIT_FAILURE;
                    }
#endif
                    // printf("Set system IP now!\n");
                    break;
                default:
                    break;
            }

        }

    }


    return ret;
}
/**
 * \brief Callback function for handling signals.
 * \param	sig	identifier of signal
 */
void handle_signal(int sig)
{
	if((SIGTERM == sig) || (sig == SIGINT))
    {
		printf("Debug: stopping daemon ...\n");
		/* Unlock and close lockfile */
		if(pid_fd != -1) {
			lockf(pid_fd, F_ULOCK, 0);
			close(pid_fd);
		}
		/* Try to delete lockfile */
		if(pid_file_name != NULL) {
			unlink(pid_file_name);
		}
		running = 0;
		/* Reset signal handling to default behavior */
		signal(SIGINT, SIG_DFL);
        // kill user process
        INT32 iProc = 0;
        for(iProc = 0; iProc < procInfo.ucProcNum; iProc++)
        {
            printf("kill: pid %d\n", procInfo.proc[iProc].procPID);
            kill(procInfo.proc[iProc].procPID, SIGTERM);
        }

        // free modbus server
        stopModbusServer();

        // delete shm
	    if(EXIT_FAILURE == destroyShms())
        {
	    	syslog(LOG_ERR, "destory shms failed");
	    	DBG(DBG_ERROR, "destory shms failed");
        }
	} else if(sig == SIGHUP) {
		printf("Debug: reloading daemon config file ...\n");
		// read_conf_file(1);
	} else if(sig == SIGCHLD) {
		printf("Debug: received SIGCHLD signal\n");
	}
    _exit(0);
}

/**
 * \brief This function will daemonize this app
 */
static void daemonize()
{
	pid_t pid = 0;
	int fd;

	/* Fork off the parent process */
	pid = fork();

	/* An error occurred */
	if(pid < 0) {
		exit(EXIT_FAILURE);
	}

	/* Success: Let the parent terminate */
	if(pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* On success: The child process becomes session leader */
	if(setsid() < 0) {
		exit(EXIT_FAILURE);
	}

	/* Ignore signal sent from child to parent process */
	signal(SIGCHLD, SIG_IGN);

	/* Fork off for the second time*/
	pid = fork();

	/* An error occurred */
	if(pid < 0) {
		exit(EXIT_FAILURE);
	}

	/* Success: Let the parent terminate */
	if(pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* Set new file permissions */
	umask(0);

	/* Change the working directory to the root directory */
	/* or another appropriated directory */
	chdir("/");

	/* Close all open file descriptors */
	for(fd = sysconf(_SC_OPEN_MAX); fd > 0; fd--)
	{
		close(fd);
	}

	/* Reopen stdin (fd = 0), stdout (fd = 1), stderr (fd = 2) */
	// stdin = fopen("/dev/null", "r");
	// stdout = fopen("/dev/null", "w+");
	// stderr = fopen("/dev/null", "w+");

	/* Try to write PID of daemon to lockfile */
	if(pid_file_name != NULL)
	{
		char str[256];
		pid_fd = open(pid_file_name, O_RDWR|O_CREAT, 0640);
		if(pid_fd < 0)
		{
			/* Can't open lockfile */
			exit(EXIT_FAILURE);
		}
		if(lockf(pid_fd, F_TLOCK, 0) < 0)
		{
			/* Can't lock file */
			exit(EXIT_FAILURE);
		}
		/* Get current PID */
		sprintf(str, "%d\n", getpid());
		/* Write PID to lockfile */
		write(pid_fd, str, strlen(str));
	}
}

/**
 * \brief Print help for this application
 */
void print_help(void)
{
	printf("\n Usage: %s [OPTIONS]\n\n", app_name);
	printf("  Options:\n");
	printf("   -h --help                 Print this help\n");
	printf("   -c --conf_file filename   Read configuration from the file\n");
	printf("   -t --test_conf filename   Test configuration file\n");
	printf("   -l --log_file  filename   Write logs to the file\n");
	printf("   -d --daemon               Daemonize this application\n");
	printf("   -p --pid_file  filename   PID file used by daemonized app\n");
	printf("\n");
}

/* Main function */
int main(int argc, char *argv[])
{
	static struct option long_options[] = {
		{"conf_file", required_argument, 0, 'c'},
		{"test_conf", required_argument, 0, 't'},
		{"log_file", required_argument, 0, 'l'},
		{"help", no_argument, 0, 'h'},
		{"daemon", no_argument, 0, 'd'},
		{"pid_file", required_argument, 0, 'p'},
		{NULL, 0, 0, 0}
	};
	int value, option_index = 0, ret;
	char *log_file_name = NULL;
	int start_daemonized = 0;

	app_name = argv[0];

	/* Try to process all command line arguments */
	while( (value = getopt_long(argc, argv, "c:l:t:p:dh", long_options, &option_index)) != -1) {
		switch(value) {
			case 'c':
				// conf_file_name = strdup(optarg);
				break;
			case 'l':
				log_file_name = strdup(optarg);
				break;
			case 'p':
				pid_file_name = strdup(optarg);
				break;
			case 't':
				// return test_conf_file(optarg);
                break;
			case 'd':
				start_daemonized = 1;
				break;
			case 'h':
				print_help();
				return EXIT_SUCCESS;
			case '?':
				print_help();
				return EXIT_FAILURE;
			default:
				break;
		}
	}

	/* When daemonizing is requested at command line. */
	if(start_daemonized == 1) {
		/* It is also possible to use glibc function deamon()
		 * at this point, but it is useful to customize your daemon. */
		daemonize();
	}

	/* Open system log and write message to it */
	openlog(argv[0], LOG_PID|LOG_CONS, LOG_DAEMON);
	syslog(LOG_INFO, "Started %s", app_name);

	/* Daemon will handle two signals */
	signal(SIGINT, handle_signal);
	signal(SIGHUP, handle_signal);
	signal(SIGTERM, handle_signal);

	/* Try to open log file to this daemon */
	if(log_file_name != NULL) {
		log_stream = fopen(log_file_name, "a+");
		if (log_stream == NULL)
		{
			syslog(LOG_ERR, "Can not open log file: %s, error: %s",
				log_file_name, strerror(errno));
			log_stream = stdout;
		}
	} else {
		// log_stream = stdout;
	}

	/* Read configuration from config file */
	if(EXIT_FAILURE == read_conf_file(&procInfo))
    {
		syslog(LOG_ERR, "read conf file failed");
		DBG(DBG_WARN, "read conf file failed");
    }

	/* This global variable can be changed in function handling signal */
	running = 1;


	/* Never ending loop of server */
	while(running == 1) {
		/* Debug print */
#if 0
		ret = fprintf(log_stream, "Debug: %d\n", counter++);
		if(ret < 0) {
			syslog(LOG_ERR, "Can not write to log stream: %s, error: %s",
				(log_stream == stdout) ? "stdout" : log_file_name, strerror(errno));
			break;
		}
		ret = fflush(log_stream);
		if(ret != 0) {
			syslog(LOG_ERR, "Can not fflush() log stream: %s, error: %s",
				(log_stream == stdout) ? "stdout" : log_file_name, strerror(errno));
			break;
		}
#endif

		/* TODO: dome something useful here */
        /*step 0: startup 
         *  a, system init(DB initi, ...), [IP, NFS, SSH] move to init script, [Modbus service] move to intercpu;
         *  b, start all tasks from configure file: live555Server, sample_venc, vpss_attr, APC with default args(first time); 
         *  c, sync with BBB(login) via modbus
         * */
        if(reRun || firstRun)
        {

            printf("first run\n");
            if(EXIT_SUCCESS != sysInit())
            {
	        	syslog(LOG_ERR, "Can not init system!");
            }

            if(EXIT_SUCCESS != startProcs(&procInfo))
            {
	        	syslog(LOG_ERR, "Can not start Procs!");
            }

            if(EXIT_SUCCESS != startInterCPUComm())
            {
	        	syslog(LOG_ERR, "Can not login to BBB!");
	        	DBG(DBG_ERROR, "Can not login to BBB!");
            }
            else
            {
	        	DBG(DBG_ERROR, "login to BBB done!");
            }

            reRun = 0;
            firstRun = 0;
        }

        /*step 1: handle with inter-cpu comms 
         *  a, Rx: get infos(time-sync, ...) and req(request of rpt-infos, manage cmds)
         *  b, handel cmds
         *  c, Tx: resp rpt-infos(such as get APC fifo info: counter)
         * */
        while(LOGINOK != g_loginOK)
        {
	    	// syslog(LOG_ERR, "not login yet!");
            DBG(DBG_WARN, "login: %d, wait login...", g_loginOK);
            // printf("login: %d, wait login...\n", g_loginOK);
            sleep(1);
        }


        /*handle with cmd from BBB */
        if(EXIT_SUCCESS != manageProcsViaCmd(&procInfo, &interCPUInfo))
        {
	    	syslog(LOG_ERR, "Can not manageProcsViaCmd!");
        }

        /*step 3: statistic CPU infos: load, mem, disk
        * monitor processes (SW WTD, sys infos)
         * */
        if(EXIT_SUCCESS != statSysInfo(&sysInfo))
        {
	    	syslog(LOG_ERR, "Can not statSysInfo!");
        }

        if(EXIT_SUCCESS != checkWTD())
        {
	    	syslog(LOG_ERR, "Can not checkWTD4Procs!");
        }

		/* Real server should use select() or poll() for waiting at
		 * asynchronous event. Note: sleep() is interrupted, when
		 * signal is received. */
		// sleep(delay);
        test();

	}

	/* Close log file, when it is used. */
	if (log_stream != stdout)
	{
		// fclose(log_stream);
	}

	/* Write system log and close it. */
	syslog(LOG_INFO, "Stopped %s", app_name);
	closelog();

	/* Free allocated memory */
	if(log_file_name != NULL) free(log_file_name);
	if(pid_file_name != NULL) free(pid_file_name);

	return EXIT_SUCCESS;
}
