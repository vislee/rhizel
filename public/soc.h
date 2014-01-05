/*
 *file name : soc.h
 *des : 
 *date : 2013-10-21
 *author : liwq (vince_lee@bizconf.cn)
 *notes :
 *       2013-10-21   liwq      create files
 */

/**********************************************************/
#ifndef __SOC_H__
#define __SOC_H__
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <unistd.h>
#include <string.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#define MSGMAX 1024

#define CONNECT_ERR -99
/*********
**get tcp connect handle
**
***************************************/
int getTcpConSoc(char *ip, int port);

/*********
**close handle
***************************************/
void clsSoc(int soc);


/*********
**send string
***************************************/
int sendCmdStr(int soc, char *sdCmd);

/*********
**recv string
***************************************/
int recvInfoStr(int soc, char *recBuf, int iDftRdCt);

#endif //__SOC_H__
