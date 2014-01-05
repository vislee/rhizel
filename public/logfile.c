/*
 *file name : logfile.c
 *des : 
 *date : 2013-10-23
 *author : liwq (286570361)
 *notes :
 *       2013-10-23   liwq      create files
 */

/**********************************************************/
#include "logfile.h"
#include "file.h"
#include "public.h"

static char * mkLogNm(char *fileName, typeLogFile *pLogFile)
{
  sprintf(fileName, "%s_%8.8s_%s.log", pLogFile->programName, \
          pLogFile->lastDate, pLogFile->procID);

  return fileName;
}

static FILE *openLog(typeLogFile *pLogFile)
{
  char logName[NAMELEN] = {0};
  mkLogNm(logName, pLogFile);
  return pLogFile->fpLog = openFile(pLogFile->logPath, logName, "a+");
}

int initLog(typeLogFile *pLogFile, const char *logPath, const char *programName, \
            const char *sysDate, const char *procID, int logLevel)
{
  memset(pLogFile, 0x00, sizeof(typeLogFile));
    
  strcpy(pLogFile->logPath, logPath);
  strcpy(pLogFile->programName, programName);
  strcpy(pLogFile->procID, procID);
  strncpy(pLogFile->lastDate, sysDate, 8);
  pLogFile->lastDate[8] = 0;
  pLogFile->logLevel = logLevel;

  pLogFile->fpLog = openLog(pLogFile);
  if (!pLogFile->fpLog)
  {
    perror("initLog open err!");
    return -1;
  }

  return 0;
}

static int getLogLevel(char *levelInfo, int logLevel)
{
  switch(logLevel)
  {
    case DEBUG:
      strcpy(levelInfo, "DEBUG");
      break;
    case INFO:
      strcpy(levelInfo, "INFO");
      break;
    case WARN:
      strcpy(levelInfo, "WARN");
      break;
    case ERROR:
      strcpy(levelInfo, "ERROR");
      break;
    default:
      perror("error log level!");
  }
  return 0;
}

int wrtLog(typeLogFile *pLogFile, const char *file, long line, \
           const char *func, int logLevel, char * fmt, ...)
{
  va_list args;
  char logBuf[STRLEN+2*NAMELEN];
  char logStr[2*NAMELEN];
  char curDate[14+1];
  char logLevelInfo[10];

  if (pLogFile->logLevel > logLevel)
  {
    return 0;
  }

  getSysDate(curDate);
  
  if (strncmp(curDate,pLogFile->lastDate,8) != 0 )
  {
    fclose(pLogFile->fpLog);
        
    strncpy(pLogFile->lastDate, curDate, 8);
    pLogFile->lastDate[8]='\0';

    if(openLog(pLogFile) == NULL)
    {
      perror("wrtLog:Can't open log file!");
      return -1;
    }
  }

  memset(logLevelInfo, 0x00, sizeof(logLevelInfo));
  getLogLevel(logLevelInfo, logLevel);

  memset(logBuf,0,sizeof(logBuf));
  sprintf(logBuf,"[%8.8s %2.2s:%2.2s:%2.2s]",curDate, curDate+8, curDate+10, curDate+12);
  if (pLogFile->logLevel <= DEBUG)
  {
    sprintf(logBuf, "%s [%s:%s:%ld]", logBuf, file, func, line);
  }
  sprintf(logBuf, "%s [%9.9s]: ", logBuf, logLevelInfo);
  
  memset(logStr, 0x00, sizeof(logStr));
  va_start(args,fmt);
  vsprintf(logStr, fmt, args); 
  va_end(args);

  strcat(logBuf, logStr);
  strcat(logBuf, "\n");
  putRcd(logBuf,pLogFile->fpLog);
  fflush(pLogFile->fpLog);

  return 0;
}

int closeLog(typeLogFile *pLogFile)
{
  if (0 != closeFile(pLogFile->fpLog))
  {
    perror("close log file err!");
    return -1;
  }
  memset(pLogFile, 0x00, sizeof(typeLogFile));
  return 0;
}
