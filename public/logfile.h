/*
 *file name : logfile.h
 *des : 
 *date : 2013-10-23
 *author : liwq (286570361)
 *notes :
 *       2013-10-23   liwq      create files
 */

/**********************************************************/
#ifndef __LOGFILE_H__
#define __LOGFILE_H__

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdarg.h>

#define STRLEN  1024
#define NAMELEN  512

#define DEBUG 1
#define INFO 2
#define WARN 3
#define ERROR 4

typedef struct{
  FILE * fpLog;
  char lastDate[8+1];
  char logPath[128+1];
  char programName[32+1];
  char procID[16+1];
  int logLevel;
}typeLogFile;

//初始化日志
int initLog(typeLogFile *pLogFile, const char *logPath, const char *programName, \
            const char *sysDate, const char *procID, int logLevel);
//写日志
int wrtLog(typeLogFile *pLogFile, const char *file, long line, \
           const char *func, int logLevel, char * fmt, ...);
//关闭日志
int closeLog(typeLogFile *pLogFile);

#define LOG_DEBUG(pLogFile, ...) \
        wrtLog(pLogFile, __FILE__, __LINE__, __func__, DEBUG, __VA_ARGS__)

#define LOG_INFO(pLogFile, ...) \
        wrtLog(pLogFile, __FILE__, __LINE__, __func__, INFO, __VA_ARGS__)

#define LOG_WARN(pLogFile, ...) \
        wrtLog(pLogFile, __FILE__, __LINE__, __func__, WARN, __VA_ARGS__)

#define LOG_ERROR(pLogFile, ...) \
        wrtLog(pLogFile, __FILE__, __LINE__, __func__, ERROR, __VA_ARGS__)

#endif  //__LOGFILE_H__
