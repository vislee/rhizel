/*
 *file name : soc.c
 *des : 
 *date : 2013-10-21
 *author : liwq (vince_lee@bizconf.cn)
 *notes :
 *       2013-10-21   liwq      create files
 */

/**********************************************************/

#include "soc.h"


int getTcpConSoc(char *serverIp, int port)
{
  struct sockaddr_in client_addr;
  bzero(&client_addr, sizeof(client_addr));
  client_addr.sin_family = AF_INET; 
  client_addr.sin_port = htons(port);
  client_addr.sin_addr.s_addr = inet_addr(serverIp);

  int soc = -1;
  if((soc = socket(AF_INET,SOCK_STREAM,0)) < 0)
  {
    perror("create\n");
    return soc;
  }

  if(connect(soc,(struct sockaddr *)&client_addr,sizeof(client_addr)) < 0)
  {
    perror("connect\n");
    close(soc);
    return -1;
  }

  return soc;
}

void clsSoc(int soc)
{
  if (soc > 0)
  {
    close(soc);
  }
}

int sendCmdStr(int soc, char *sdCmd)
{
  if(send(soc,sdCmd,strlen(sdCmd)+1,0) < 0)
  {
    perror("send");
    return CONNECT_ERR;
  }
  return 0;
}


int recvInfoStr(int soc, char *recBuf, int iDftRdCt)
{
  struct timeval timeout;
  timeout.tv_sec = 80;
  timeout.tv_usec = 50;
  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(soc,&read_fds);

  int iLen = 0;
  while(iDftRdCt--)
  {
    int iSelRet = select(soc+1,&read_fds,NULL,NULL,&timeout);
    if (-1 == iSelRet)
    {
      perror("select.");
      return -1;
    }
    else if (0 == iSelRet)
    {
      sendCmdStr(soc, "LT");
      #ifdef __DEBUG__
        printf("recv timeout ,i will send LT\n");
      #endif
      continue;
    }
    else
    {
      if (FD_ISSET(soc, &read_fds))
      {
        char strRecvInfo[MSGMAX] = {0};
        int iRecRet = recv(soc,strRecvInfo,sizeof(strRecvInfo),0); 
        if(iRecRet < 0)
        {
          perror("recv.");
          return -1;
        }
        if (iRecRet == 0)
        {
          perror("recv connect err");
          return CONNECT_ERR;
        }
        char strJmInfo[MSGMAX] = {0};
        iRecRet -= 6;
        if (0 == strncmp("LT",strJmInfo, 2))
        {
          #ifdef __DEBUG__
            printf("recv a LT cmd!!!\n");
          #endif //__DEBUG__
          sendCmdStr(soc, "LT");
          continue;
        }
        #ifdef __DEBUG__
          printf("every recv[%d][%s]\n",iRecRet,strRecvInfo);
        #endif //__DEBUG__
        strncpy(recBuf+iLen, strJmInfo, iRecRet);
        iLen += iRecRet;
        timeout.tv_sec /= 2;
      }
    }
  }
  #ifdef __DEBUG__
    printf("buf is:%s,len is %d\n",recBuf, iLen);
  #endif //__DEBUG__
  return iLen;
}

