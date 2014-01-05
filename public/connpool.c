/*
 *file name : connpool.c
 *des : 
 *date : 2013-11-28
 *author : liwq (286570361)
 *notes :
 *       2013-11-28   liwq      create files
 */

/**********************************************************/
#include "connpool.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

/*
 *初始化连接池
 */
static typePools *init_pools(int size)
{
  typePools *p = malloc(sizeof(typePools));
  assert(p);

  p->used = 0;
  p->size = size;
  p->val = malloc(sizeof(MYSQL*) * size);
  assert(p->val);

  p->seq = malloc(sizeof(int) * size);
  assert(p->seq);

  int i = 0;
  for (; i<size; ++i)
  {
    p->val[i] = NULL;
    p->seq[i] = 0;
  }

  pthread_mutex_init(&p->lock, NULL);

  return p;
}

/*
 *添加链接到连接池
 */
static int add_pools(typePools *p, MYSQL *pDb)
{
  pthread_mutex_lock(&p->lock);
  if (p->size == p->used)
  {
    pthread_mutex_unlock(&p->lock);
    return 0;
  }

  #ifdef __DEBUG__
    printf("add_pools dbhandle:%p\n", pDb);
  #endif

  int i = 0;
  for (; i<p->size; ++i)
  {
    if (p->seq[i] == 0)
    {
      p->seq[i] = 1;
      break;
    }
  }
  p->val[i] = pDb;
  ++p->used;
  pthread_mutex_unlock(&p->lock);
  return 1;
}

/*
 *从连接池取一个链接使用
 */
static MYSQL *get_Conn(typePools *p)
{
  pthread_mutex_lock(&p->lock);
  if (p->used == 0) 
  {
    pthread_mutex_unlock(&p->lock);
    return NULL;
  }

  #ifdef __DEBUG__
    printf("get a Conn\n");
  #endif

  int i = 0;
  for (; i<p->size; ++i)
  {
    if (p->seq[i] == 1)
    {
      p->seq[i] = 0;
      break;
    }
  }
  --p->used;
  pthread_mutex_unlock(&p->lock);
  return p->val[i];
}

static int destory_pools(typePools *p)
{
  free(p->val);
  free(p->seq);
  free(p);
  return 0;
}

/*
 *初始化MYSQL链接池
 */
int initPool(typeConnpool *pl, int maxSize,\
             char *pUser, char *pPasswd,   \
             char *pHost, int port, char *pDb)
{
  assert(pl);

  pl->maxSize = maxSize;
  pl->pUser = strdup(pUser);
  pl->pPswd = strdup(pPasswd);
  pl->pHost = strdup(pHost);
  pl->port = port;
  pl->pDb = strdup(pDb);
  //pthread_mutex_init(&pl->lock, NULL);
  pl->pPools = init_pools(pl->maxSize);
  return 0;
}

static MYSQL *connect(typeConnpool *pl)
{
  MYSQL *pDb;
  pDb = mysql_init(NULL);

  #ifdef __DEBUG__
    printf("connect-mysql_init\n");
  #endif

  #if MYSQL_VERSION_ID >= 50013
    my_bool reconnect = 1;
    mysql_options(pDb, MYSQL_OPT_RECONNECT, &reconnect);
  #endif
  #ifdef __DEBUG__
    printf("host:%s  user:%s\n",pl->pHost, pl->pUser);
  #endif
  if (mysql_real_connect(pDb, pl->pHost, pl->pUser, pl->pPswd, pl->pDb, pl->port, NULL, 0))
  {
    return pDb;
  }
  perror(mysql_error(pDb));
  return NULL;
}

/*
 *获取一个链接
 */
MYSQL *getConnDb(typeConnpool *pl)
{
  //pthread_mutex_lock(&pl->lock);
  assert(pl);
  
  MYSQL *pDb;
  pDb = get_Conn(pl->pPools);

  //没取到链接，创建一个链接
  if (!pDb) 
  {
    pDb = connect(pl);
  }

  if (0 != mysql_ping(pDb))
  {
    perror(mysql_error(pDb));
    mysql_close(pDb);
    perror("mysql conn err");
    //pthread_mutex_unlock(&pl->lock);
    return NULL;
  }
  //pthread_mutex_unlock(&pl->lock);
  return pDb;
}

/*
 *回收链接
 */
int recycleDb(typeConnpool *pl, MYSQL *pDb)
{
  //回收链接失败，销毁链接
  //pthread_mutex_lock(&pl->lock);
  if (0 == add_pools(pl->pPools, pDb))
  {
    mysql_close(pDb);
    //pthread_mutex_unlock(&pl->lock);
    return 0;
  }
  //pthread_mutex_unlock(&pl->lock);
  return 1;
}

/*
 *销毁链接
 */
int desPool(typeConnpool *pl)
{
  MYSQL *db = NULL;
  while(pl->maxSize--)
  {
    db = get_Conn(pl->pPools);
    if (!db) break;
    mysql_close(db);
  }
  destory_pools(pl->pPools);
  pl->maxSize = 0;

  if (pl->pUser) free(pl->pUser);
  if (pl->pPswd) free(pl->pPswd);
  if (pl->pHost) free(pl->pHost);
  if (pl->pDb)   free(pl->pDb);

  return 0;
}
