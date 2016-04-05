#ifndef DB_H
#define DB_H
#include <iostream>
#include <sstream>
#include <unistd.h>
//#include "SPIDevice.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>


#define STATION_MAX_NUM                               20
#define FRONT_PANEL_CODE_NUM_MAX                      128*8      //8*32*32/8 // 8 chars for each station


typedef struct
{
    int stationId;
    char stationName[50];
    char NameCode[50];
    char frontPanelCode[FRONT_PANEL_CODE_NUM_MAX];
    double latitude;
    double longitude;
} StationDB;
typedef struct
{
    int route_id;
    StationDB station[STATION_MAX_NUM];
} RouteDB;

void parseDoc(const char *docname,RouteDB* routeInfo);


#endif
