/*
 *file name : file.c
 *des : 
 *date : 2013-10-21
 *author : liwq (286570361)
 *notes :
 *       2013-10-21   liwq      create files
 */

/**********************************************************/
#include "file.h"
#include <string.h>

char *mkFlNm(char *fullFileName, char *dirName, char *fileName)
{
  int dirLen;

  strcpy(fullFileName, dirName);
  dirLen = strlen(dirName);

  if(dirName[dirLen-1]=='/')
  {
    strcpy(fullFileName+dirLen, fileName);
  }
  else
  {
    dirName[dirLen]='/';
    strcpy(fullFileName+dirLen+1, fileName);
  }
  return fullFileName;
}

FILE *openFile(char *dirName, char *fileName, char *mode)
{
  char fullName[MAXLEN] = {0};
  mkFlNm(fullName, dirName, fileName);
  FILE *fp = NULL;
  fp = fopen(fullName, mode);
  if (NULL == fp)
  {
    perror("open file");
    return NULL;
  }
  return fp;
}

int closeFile(FILE *fp)
{
  if (fp != NULL)
  {
    fflush(fp);
    if (fclose(fp) < 0 )
    {
      return -1;
    }
    fp = NULL;
    return 0;
  }
  return 0;
}

void clear(FILE *fp)
{
  ftruncate(fileno(fp), 0L);
  fseek(fp, 0L, SEEK_SET);
}

void clearFile(char *dirName, char *fileName)
{
  FILE *fp = NULL;
  fp = openFile(dirName, fileName, "w");
  clear(fp);
  closeFile(fp);
}

int getnRcd(void *record, size_t size, size_t nmemb, FILE *fp)
{
  if(feof(fp))
    return 0;
  else
    return fread(record, size, nmemb, fp);
}

size_t getRcd(char *record, size_t size, FILE *fp)
{
  char *tmp = NULL;
  if(feof(fp))
    return -1;
  else
  {
    tmp = fgets(record, size, fp);
    if(tmp == NULL)
      return -2;
    return 0;
  }
}

size_t putnRcd(void *ptr, size_t size, size_t nmemb, FILE *fp)
{
	return fwrite(ptr, size, nmemb, fp);
}

size_t putRcd(char *s, FILE *fp)
{
	return fputs(s, fp);
}

long getFlLth(char *dirName, char *fileName)
{
  char fullName[MAXLEN] = {0};
  mkFlNm(fullName, dirName, fileName);
  struct stat buf;
  stat(fullName, &buf);
  return buf.st_size;
}

int reNm(char *oldFileName, char *newFileName, char *dirName)
{
  char oldFullFile[MAXLEN];
  char newFullFile[MAXLEN];

  mkFlNm(oldFullFile, dirName, oldFileName);
  mkFlNm(newFullFile, dirName, newFileName);

  return rename(oldFullFile, newFullFile);
}

int isHaveFile(char *file)
{
  if (access(file, F_OK) < 0)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}
