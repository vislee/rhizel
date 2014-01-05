/*
 *file name : file.h
 *des : 
 *date : 2013-10-21
 *author : liwq (286570361)
 *notes :
 *       2013-10-21   liwq      create files
 */

/**********************************************************/
#ifndef __FILE_H__
#define __FILE_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#define MAXLEN 1024
#define PATHLEN 512
#define FILELEN 128

char *mkFlNm(char *fullFileName, char *dirName, char *fileName);

FILE * openFile(char *fileDir, char *fileName, char *module);

int closeFile(FILE *fp);

void clear(FILE *fp);

void clearFile(char *dirName, char *fileName);

int getnRcd(void *record, size_t size, size_t nmemb, FILE *fp);

size_t getRcd(char *record, size_t size, FILE *fp);

size_t putnRcd(void *ptr, size_t size, size_t nmemb, FILE *fp);

size_t putRcd(char *s, FILE *fp);

long getFlLth(char *dirName, char *fileName);

int reNm(char *oldFileName, char *newFileName, char *dirName);

int isHaveFile(char *file);
#endif //__FILE_H__
