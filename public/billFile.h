/*
 *file name : billFile.h
 *des : 
 *date : 2013-11-01
 *author : liwq (286570361)
 *notes :
 *       2013-10-23   liwq      create files
 */

/**********************************************************/

#ifndef __BILL_FILE__H_
#define __BILL_FILE__H_


#include "public.h"
#include "file.h"

typedef struct _SBILL_FILE
{
  char filePath[PATHLEN];
  char fileNameHead[FILELEN];
  char openFileNm[FILELEN];
  char sysDate[8+1];
  char current[8+1];
  long record;
  FILE *pFile;
}typeBillFile;

FILE * openBillFile(typeBillFile *pBillFile);

int closeBillFile(typeBillFile *pBillFile);

int wrtBill(typeBillFile *pBillFile, int count, char *pBill);

int getLastCount(typeBillFile *pBillFile);

#endif //__BILL_FILE__H_
