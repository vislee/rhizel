/*
 *file name : connpool.h
 *des : mysqlÊý¾Ý¿âÁ´½Ó³Ø
 *date : 2013-11-28
 *author : liwq (286570361)
 *notes :
 *       2013-11-28   liwq      create files
 */

/**********************************************************/
#ifndef __CONN_POOL_H__
#define __CONN_POOL_H__

#include <stdlib.h>
#include <mysql.h>
#include <pthread.h>

typedef struct
{
  MYSQL **val;
  int size;
  int used;
  int *seq;
  pthread_mutex_t lock;
} typePools;

typedef struct 
{
  //pthread_mutex_t lock;
  int maxSize;
  char *pPswd;
  char *pUser;
  char *pHost;
  int port;
  char *pDb;
  typePools *pPools;
} typeConnpool;

int initPool(typeConnpool *pl, int maxSize,\
             char *pUser, char *pPasswd,   \
             char *pHost, int port, char *pDb);

MYSQL *getConnDb(typeConnpool *pl);

int recycleDb(typeConnpool *pl, MYSQL *pDb);

int desPool(typeConnpool *pl);


#endif //__CONN_POOL_H__
