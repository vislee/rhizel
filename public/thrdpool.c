/*
 *file name : thrdpool.c
 *des : 
 *date : 2013-11-20
 *author : liwq (286570361)
 *notes :
 *       2013-10-21   liwq      create files
 */

/**********************************************************/
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
 
#include "thrdpool.h"

/* �������̺߳���, ������������ȡ��������ִ�� */
static void* 
thread_routine(void *arg)
{
    tpool_t *tpool = (tpool_t *)arg;
    tpool_work_t *work;
    
    while(1) {
        pthread_mutex_lock(&tpool->queue_lock);
        while(!tpool->queue_head && !tpool->shutdown) 
        {
            #ifdef __DEBUG__
              printf("[%lu]i sleep\n", pthread_self());
            #endif
            pthread_cond_wait(&tpool->queue_ready, &tpool->queue_lock);
        }
        if (tpool->shutdown) {
            #ifdef __DEBUG__
              printf("[%lu]i quit\n", pthread_self());
            #endif
            pthread_mutex_unlock(&tpool->queue_lock);
            pthread_exit(NULL);
        }
        #ifdef __DEBUG__
          printf("[%lu]i awake\n", pthread_self ());
        #endif
        work = tpool->queue_head;
        tpool->queue_head = tpool->queue_head->next;
        
        if (!tpool->queue_head)
        {
          pthread_cond_signal(&tpool->queue_empty);
        }
        pthread_mutex_unlock(&tpool->queue_lock);

        work->routine(work->arg);
        free(work->arg);
        free(work);
    }
    
    return NULL;   
}
 
/*
 * �����̳߳� 
 */
int
tpool_create(tpool_t **ppTpool, int max_thr_num)
{
    int i;
    tpool_t *tpool = NULL;
    tpool = calloc(1, sizeof(tpool_t));
    if (!tpool) {
        printf("%s: calloc failed\n", __FUNCTION__);
        exit(1);
    }
    
    /* ��ʼ�� */
    tpool->max_thr_num = max_thr_num;
    tpool->shutdown = 0;
    tpool->list_close = 0;
    tpool->queue_head = NULL;
    if (pthread_mutex_init(&tpool->queue_lock, NULL) !=0) {
        printf("%s: pthread_mutex_init failed, errno:%d, error:%s\n",
            __FUNCTION__, errno, strerror(errno));
        exit(1);
    }
    if (pthread_cond_init(&tpool->queue_ready, NULL) !=0 ) {
        printf("%s: pthread_cond_init failed, errno:%d, error:%s\n", 
            __FUNCTION__, errno, strerror(errno));
        exit(1);
    }
    
    if (pthread_cond_init(&tpool->queue_empty, NULL) !=0 ) {
        printf("%s: pthread_cond_init2 failed, errno:%d, error:%s\n", 
            __FUNCTION__, errno, strerror(errno));
        exit(1);
    }
    /* �����������߳� */
    tpool->thr_id = calloc(max_thr_num, sizeof(pthread_t));
    if (!tpool->thr_id) {
        printf("%s: calloc failed\n", __FUNCTION__);
        exit(1);
    }
    for (i = 0; i < max_thr_num; ++i) {
        if (pthread_create(&tpool->thr_id[i], NULL, thread_routine, (void*)tpool) != 0){
            printf("%s:pthread_create failed, errno:%d, error:%s\n", __FUNCTION__, 
                errno, strerror(errno));
            exit(1);
        }
        
    }    
    *ppTpool = tpool;
    return 0;
}
 
/* �����̳߳� */
void
tpool_destroy(tpool_t *tpool,int flag)
{
    int i;
    tpool_work_t *member;
 
    if (tpool->list_close) {
        return;
    }
    
    pthread_mutex_lock(&tpool->queue_lock);
    tpool->list_close =1;
    if (flag)
    {
      if (tpool->queue_head)
      {
        pthread_cond_wait(&tpool->queue_empty, &tpool->queue_lock);
      }
    }
    tpool->shutdown = 1;
    /* ֪ͨ�������ڵȴ����߳� */
    
    pthread_cond_broadcast(&tpool->queue_ready);
    pthread_mutex_unlock(&tpool->queue_lock);
    for (i = 0; i < tpool->max_thr_num; ++i) {
        pthread_join(tpool->thr_id[i], NULL);
    }
    free(tpool->thr_id);
 
    while(tpool->queue_head) {
        member = tpool->queue_head;
        tpool->queue_head = tpool->queue_head->next;
        free(member->arg);
        free(member);
    }
 
    pthread_mutex_destroy(&tpool->queue_lock);    
    pthread_cond_destroy(&tpool->queue_ready);
 
    free(tpool);    
}
 
/* ���̳߳��������� */
int
tpool_add_work(tpool_t *tpool, void*(*routine)(void*), void *arg, int len)
{
    tpool_work_t *work, *member;
    
    if (!routine){
        printf("%s:Invalid argument\n", __FUNCTION__);
        return -1;
    }
    
    work = malloc(sizeof(tpool_work_t));
    if (!work) {
        printf("%s:malloc failed\n", __FUNCTION__);
        return -1;
    }
    work->routine = routine;
    //work->arg = arg;
    work->arg = (void*)malloc(len);
    if (!work->arg) return 0;
    //memset(work->arg, 0x00, 100);
    memcpy(work->arg, arg, len);
    work->next = NULL;
 
    pthread_mutex_lock(&tpool->queue_lock);
    if (tpool->list_close)
    {
      pthread_mutex_unlock(&tpool->queue_lock);
      free(work->arg);
      free(work);
      return 0;
    }
    member = tpool->queue_head;
    if (!member) {
        tpool->queue_head = work;
    } else {
        while(member->next) {
            member = member->next;
        }
        member->next = work;
    }
    /* ֪ͨ�������̣߳������������� */
    pthread_cond_signal(&tpool->queue_ready);
    pthread_mutex_unlock(&tpool->queue_lock);
 
    return 0;    
}
