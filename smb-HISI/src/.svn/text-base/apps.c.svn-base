#include "apps.h"


static char *conf_file_name = "daemon.conf";
static int delay = 1;
extern char *app_name;

const int MAGIC_NUM_CONF_FILE = 0xdeadbeef;

#define MAX_LEN_LINE (MAX_PROC_NAME_LEN + 4*MAX_ARGC_PROC)

int cleanStr(CHAR* str, CHAR* dest)
{
    UCHAR len = 0;
    if((NULL == dest) || (NULL == str))
    {
        syslog(LOG_ERR, "dest or str null!");
        DBG(DBG_ERROR, "dest or str null!");
        return -1;
    }
    len = strlen(str);
    if((MAX_LEN_LINE < len) || (len < 2))
    {
        syslog(LOG_ERR, "line: %d > max(%d)!", len, MAX_LEN_LINE);
        DBG(DBG_ERROR, "line: %d > max(%d)!", len, MAX_LEN_LINE);
        return -1;
    }

    strncpy(dest, str, len - 2);
    dest[len - 2] = '\0';
    return 0;
}


int parse_conf_file(CHAR* conFileName, tProcInfo* ptProcInfo)
{
	UINT32 len = 0;
    FILE* fp = NULL;
    CHAR pattern = -1;
    INT32 headMagic = 0;
    CHAR err = 0;
    UCHAR procIdx = 0;
    CHAR* line = NULL;
    CHAR* tmp = NULL;  
    CHAR lineStr[MAX_LEN_LINE];

    CHAR argcTmp = 0;
    UCHAR i = 0;

    // test
	fp = fopen(conFileName, "r");

	if(fp == NULL) {
		syslog(LOG_ERR, "Can not open config file: %s, error: %s",
				conf_file_name, strerror(errno));
		DBG(DBG_INFO, "Can not open config file: %s", conf_file_name);
        goto FAILED;
	}

    // while(NULL != fgets(line, (MAX_PROC_NAME_LEN + 4*MAX_ARGC_PROC), pFd))
    while(-1 != (err = getline(&line, &len, fp)))
    {
        DBG(DBG_INFO, "line: %s", line);
        // EOF
        // FIXME
        if(2 == strlen(line))
        {
            break;
        }

        if(-1 == cleanStr(line, lineStr))
        {
            syslog(LOG_ERR, "clean str wrong!");
            DBG(DBG_ERROR, "clean str wrong!");
            break;
            // goto FAILED;
        }

        if(strstr(line, "# MAGIC NUMBER"))
        {
            pattern = eHead; 
            continue;
        }
        else if(strstr(line, "# PROCs:"))
        {
            pattern = eProcInfo; 
            continue;
        }

        switch(pattern)
        {
            case eHead:
                // DBG(DBG_INFO, "eHead");
                if(-1 != sscanf(line, "%x", &headMagic))
                {
                    if(0xdeadbeef != headMagic)
                    {
                        syslog(LOG_ERR, "Conf file head is wrong: 0x%08x", headMagic);
                        DBG(DBG_ERROR, "Conf file head is wrong: 0x%08x", headMagic);
                        goto FAILED;
                    }
                }
                else
                {
                    syslog(LOG_ERR, "Conf file no head magic num");
                    DBG(DBG_ERROR, "Conf file no head magic num");
                    goto FAILED;
                }
                break;
            case eProcInfo:
                // DBG(DBG_INFO, "eProcInfo");
                // get proc cmd name
                argcTmp = 0;
                tmp = strtok(lineStr, " ");  
                if(NULL == tmp)
                {
                    continue;
                }

                if(-1 == (sscanf(tmp, "%s", ptProcInfo->proc[procIdx].cProcCmd)))
                {
                    syslog(LOG_ERR, "get cmd name wrong!");
                    DBG(DBG_ERROR, "get cmd name line wrong!");
                    goto FAILED;
                }
                tmp = strtok(NULL, " ");  
                while(tmp != NULL)  
                {     
                    if(-1 == sscanf(tmp, "%d", &(ptProcInfo->proc[procIdx].iProcArgv[argcTmp])))
                    {
                        syslog(LOG_ERR, "get proc args wrong!");
                        DBG(DBG_ERROR, "get proc args wrong!");
                        goto FAILED;
                    }
                    argcTmp++;
                    DBG(DBG_INFO, "parse line argv[%d]: %d!\n", argcTmp, ptProcInfo->proc[procIdx].iProcArgv[argcTmp]);
                    tmp = strtok(NULL, " ");  
                }  

                ptProcInfo->proc[procIdx].ucProcIdx = procIdx;         
                // store proc info: argcTmp - 1 (for cmd name)
                ptProcInfo->proc[procIdx].ucProcArgc = argcTmp;         
                DBG(DBG_INFO, "argc[%d]: %d\n", procIdx, argcTmp - 1);
                procIdx++;
                break;
            default:
                break;
        } // switch(pattern)
    }
    ptProcInfo->ucProcNum = procIdx;
    free(tmp);
    free(line);
	fclose(fp);

    return 0;
FAILED:
    return -1;
}

/**
 * \brief Read configuration from config file
 */
int read_conf_file(tProcInfo* ptProcInfo)
{
	FILE *conf_file = NULL;
	int ret = EXIT_SUCCESS;
    int iProc = 0;
    int iArgv = 0;


	if (conf_file_name == NULL) 
    {
		syslog(LOG_ERR, "config file not existed");
		DBG(DBG_INFO, "config file not existed");
        return EXIT_FAILURE;
    }

    // test
	conf_file = fopen(conf_file_name, "r");

	if(conf_file == NULL) {
		syslog(LOG_ERR, "Can not open config file: %s, error: %s",
				conf_file_name, strerror(errno));
		DBG(DBG_INFO, "Can not open config file: %s", conf_file_name);
		return EXIT_FAILURE;
	}

	fclose(conf_file);

    if(-1 == (ret = parse_conf_file(conf_file_name, ptProcInfo)))
    {
		syslog(LOG_ERR, "Parse conf_file error: %d", ret);
		DBG(DBG_INFO, "Parse conf_file error: %d", ret);
		return EXIT_FAILURE;
    }

#if 1
    printf("=====================================ProcInfo=========================================\n");
    for(iProc = 0; iProc < ptProcInfo->ucProcNum; iProc++)
    {
        printf("proc-%d: %s\n", ptProcInfo->proc[iProc].ucProcIdx, ptProcInfo->proc[iProc].cProcCmd);
        for(iArgv = 0; iArgv < ptProcInfo->proc[iProc].ucProcArgc; iArgv++)
        {
            printf("    argv[%d]: %d\n", iArgv, ptProcInfo->proc[iProc].iProcArgv[iArgv]);
        }

    }
    printf("==============================================================================\n");
#endif

	return ret;
}

INT32 exeCmd(CHAR* cmd, CHAR* args)
{
    UCHAR ret = EXIT_SUCCESS;
    INT32 pid = 0;
    UCHAR j = 0;


    if(NULL == cmd)
    {
		syslog(LOG_ERR, "cmd null!");
		DBG(DBG_INFO, "cmd null!");
        return EXIT_FAILURE;
    }
    pid = fork();
    if(0 == pid) // subprocess
    {
        DBG(DBG_INFO, "execmd: %s, args: %s\n", cmd, *args);
        DBG(DBG_INFO, "execmd: %s, args1: %s\n", cmd, *(args++));
        execvp(cmd, args);
        _exit(0);
    }
    else if(pid) // father
    {
        return pid;
    }
    else if(-1 == pid) // fork err
    {
        return EXIT_FAILURE;
    }
}

UCHAR startProcs(tProcInfo* ptProcInfo)
{
    UCHAR ret = EXIT_SUCCESS;
    CHAR cmdLine[MAX_PROC_NAME_LEN];
    CHAR argv[MAX_ARGC_PROC][MAX_LEN_LINE]= {"", };
    CHAR* args[MAX_ARGC_PROC];
    INT32 iProc = 0;
    INT32 iArgv = 0;
    CHAR cArgv[10];


    if(NULL == ptProcInfo)
    {
		syslog(LOG_ERR, "proc info is null");
		DBG(DBG_INFO, "proc info is null");
		return EXIT_FAILURE;
    }

    for(iProc = 0; iProc < ptProcInfo->ucProcNum; iProc++)
    {
        // generate cmd line
        memset(argv, 0, sizeof(argv));
        DBG(DBG_INFO, "startProcs-name: <%s>\n", ptProcInfo->proc[iProc].cProcCmd);
        strcpy(cmdLine, ptProcInfo->proc[iProc].cProcCmd);
        for(iArgv = 0; iArgv < ptProcInfo->proc[iProc].ucProcArgc; iArgv++)
        {
            sprintf(argv[iArgv], "%d", ptProcInfo->proc[iProc].iProcArgv[iArgv]);
            DBG(DBG_INFO, "startProcs-cargv: <%s>\n", argv[iArgv]);
            args[iArgv] = argv[iArgv];
        }
        args[iArgv] = NULL;
        DBG(DBG_INFO, "startProcs-argv: <%s>\n", argv);
        DBG(DBG_INFO, "startProcs-cmd: <%s>\n", cmdLine);

        // execute cmd line
        if(-1 == ( ptProcInfo->proc[iProc].procPID = exeCmd(cmdLine, args)))
        {
		    syslog(LOG_ERR, "exeCmd err: %s!", cmdLine);
		    DBG(DBG_INFO, "exeCmd err: %s!", cmdLine);
		    return EXIT_FAILURE;
        }
        else
        {
            DBG(DBG_INFO, "exe: pid %d\n", ptProcInfo->proc[iProc].procPID);
        }
    }

    
    DBG(DBG_INFO, "%s in", __FUNCTION__);

    return ret;
}

