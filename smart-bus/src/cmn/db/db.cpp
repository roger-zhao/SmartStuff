#include "db.h"


using namespace std;





int getVoiceDecode(char* src,char* codebuf)
{
    const char *d = ";";
    char *p;
    p = strtok(src, d);
    unsigned int a = 0;
    memset(codebuf, 0, STATION_MAX_NUM);
    int i = 0;
    while (p)
    {
        // printf("%s\n",p);
        sscanf(p, "%2x", &a);
        codebuf[i] = a & 0xFF;
        i++;
        if (i >= STATION_MAX_NUM)
            break;
        p = strtok(NULL, d);
    }

    return 0;
}

int getFrontPanelDecode(char* src,char* codebuf)
{
    const char *d = ";";
    char *p;
    p = strtok(src, d);
    unsigned int a = 0;
    memset(codebuf, 0, FRONT_PANEL_CODE_NUM_MAX);
    int i = 0;
    while (p)
    {
        // printf("%s\n",p);
        sscanf(p, "%2x", &a);
        codebuf[i] = a & 0xFF;
        i++;
//      printf("i %d val %x\n",i,codebuf[i]);
        if (i >= FRONT_PANEL_CODE_NUM_MAX)
            break;
        p = strtok(NULL, d);
    }

    return 0;
}

int getID(xmlNodePtr cur)
{
    int id = -2;
    xmlAttrPtr attrPtr = cur->properties;
    if (attrPtr != NULL && !xmlStrcmp(attrPtr->name, BAD_CAST "id"))
    {
        xmlChar* szAttr = xmlGetProp(cur,BAD_CAST "id");

        sscanf((char*) szAttr, "%d", &id);
        xmlFree( szAttr);
    }
    return id;
}


void parseStation(xmlDocPtr doc, xmlNodePtr cur,StationDB* stationInfo)
{
    xmlChar *key;
    cur = cur->xmlChildrenNode;
    while (cur != NULL)
    {
        if ((!xmlStrcmp(cur->name, (const xmlChar *) "Name")))
        {
            key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
            //printf("Name: %s\n", key);
            strcpy(stationInfo->stationName,(char*)key);
            //sscanf((char*)key,"%s",stationInfo->stationName);
            xmlFree(key);
        }
        else if ((!xmlStrcmp(cur->name, (const xmlChar *) "VoiceDecode")))
        {
            key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
            getVoiceDecode((char*) key,stationInfo->NameCode);
            //printf("VoiceDecode: %s\n", key);
            xmlFree(key);
        }
        else if ((!xmlStrcmp(cur->name, (const xmlChar *) "DisplayDecode")))
        {
            key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
            getFrontPanelDecode((char*) key,stationInfo->frontPanelCode);
            //printf("--------VoiceDecode: %s\n", key);
            xmlFree(key);
        }
        cur = cur->next;
    }
    return;
}

void parseDoc(const char *docname,RouteDB* routeInfo)
{

    xmlDocPtr doc;
    xmlNodePtr cur;

    doc = xmlParseFile(docname);

    if (doc == NULL)
    {
        fprintf(stderr, "Document not parsed successfully. \n");
        return;
    }

    cur = xmlDocGetRootElement(doc);

    if (cur == NULL)
    {
        fprintf(stderr, "empty document\n");
        xmlFreeDoc(doc);
        return;
    }

    if (xmlStrcmp(cur->name, (const xmlChar *) "Route"))
    {
        fprintf(stderr, "document of the wrong type, root node != story");
        xmlFreeDoc(doc);
        return;
    }
    routeInfo->route_id = getID(cur);
    //cout<<"route_id = "<<routeInfo.route_id<<endl;
    cur = cur->xmlChildrenNode;
    int index  = 0;
    while (cur != NULL)
    {
        if ((!xmlStrcmp(cur->name, (const xmlChar *) "station")))
        {
            routeInfo->station[index].stationId = getID(cur);
            //cout<<"station id = "<<routeInfo.station[index].stationId<<endl;
            parseStation(doc, cur,&(routeInfo->station[index]));
            index ++;
            if(index >= STATION_MAX_NUM)
                break;
        }

        cur = cur->next;
    }
    xmlFreeDoc(doc);
    return;
}
