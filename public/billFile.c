/*
 *file name : billFile.c
 *des : 
 *date : 2013-11-01
 *author : liwq (286570361)
 *notes :
 *       2013-10-23   liwq      create files
 */

/**********************************************************/
#include "billFile.h"


static char * mkBillNm(char *fileName, typeBillFile *pBillFile)
{
  if (isDate(pBillFile->sysDate))
  {
    sprintf(fileName, "%s_%8.8s.txt", pBillFile->fileNameHead, \
            pBillFile->sysDate);
  }
  else
  {
    sprintf(fileName, "%s_%8.8s.txt", pBillFile->fileNameHead, \
            pBillFile->current);
  }

  return fileName;
}

FILE * openBillFile(typeBillFile *pBillFile)
{
  char billFileName[FILELEN] = {0};
  mkBillNm(billFileName, pBillFile);
  FILE *fp = openFile(pBillFile->filePath, billFileName, "a+");
  pBillFile->pFile = fp;
  strcpy(pBillFile->openFileNm, billFileName);
  return fp;
}

int closeBillFile(typeBillFile *pBillFile)
{
  if (0 != closeFile(pBillFile->pFile))
  {
    perror("close bill file err!");
    return -1;
  }
  pBillFile->pFile = NULL;
  return 0;
}

int wrtBill(typeBillFile *pBillFile, int count, char *pBill)
{
  char billFileName[FILELEN] = {0};
  mkBillNm(billFileName, pBillFile);
  if (0 != strcmp(billFileName, pBillFile->openFileNm))
  {
    closeBillFile(pBillFile);
    if (openBillFile(pBillFile) == NULL)
    {
      perror("openBillFile return null");
      return -1;
    }
    
  }

  if (!pBillFile->pFile)
  {
    if (openBillFile(pBillFile) == NULL)
    {
      perror("openBillFile return null");
      return -1;
    }
  }

  char billContent[MAXLEN] = {0};
  sprintf(billContent, "<%6.6d>:%s\n", count, pBill);
  #ifdef __DEBUG__
    printf("write:===%p ---- %s", pBillFile->pFile, billContent);
  #endif //__DEBUG__
  int len = putRcd(billContent, pBillFile->pFile);
  fflush(pBillFile->pFile);
  return len;
}

int getLastCount(typeBillFile *pBillFile)
{
  char billFileName[FILELEN] = {0};
  mkBillNm(billFileName, pBillFile);
  char fullFileName[PATHLEN] = {0};
  mkFlNm(fullFileName, pBillFile->filePath, billFileName);
  if (!isHaveFile(fullFileName))
  {
    return 0;
  }
  if (!pBillFile->pFile)
  {
    if (openBillFile(pBillFile) == NULL)
    {
      perror("openBillFile return null");
      return -1;
    }
  }
  fseek(pBillFile->pFile, 0L, SEEK_END);
  if(ftell(pBillFile->pFile) > MAXLEN)
  {
    fseek(pBillFile->pFile, -MAXLEN, SEEK_END);
  }
  else
  {
    fseek(pBillFile->pFile, -ftell(pBillFile->pFile), SEEK_END);
  }
  char billContentTmp[MAXLEN+1] = {0};
  char countTmp[10] = {0};
  getnRcd(billContentTmp, MAXLEN, 1, pBillFile->pFile);
  
  #ifdef __DEBUG__
    printf("read content:%s \n", billContentTmp);
  #endif //__DEBUG__
  
  char *pTmp1 = strrchr(billContentTmp, '<');
  if (pTmp1 && strlen(pTmp1)<7)
  {
    *pTmp1 = 0;
    pTmp1 = strrchr(billContentTmp, '<');
  }

  fseek(pBillFile->pFile, 0L, SEEK_END);

  #ifdef __DEBUG__
    printf("the search substr:%s\n", pTmp1);
  #endif //__DEBUG__

  if (!pTmp1) return 0;

  sscanf(pTmp1, "<%[^>]>%*s", countTmp);
  trim(countTmp);
  if (isNumber(countTmp))
  {
    return atoi(countTmp);
  }
  return -1;
}
