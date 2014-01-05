/*
 *file name : public.h
 *des : 
 *date : 2013-10-21
 *author : liwq (286570361)
 *notes :
 *       2013-10-21   liwq      create files
 *       2013-10-29   liwq      add function:getsubstr,getstrnum,strreplace
 *       2013-11-04   liwq      add function:strrstr
 */

/**********************************************************/
#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

#define STRLEN 1024
/*********
*守护进程方式
*
***************************************/
void daemonInit(void);

/**********
*判断字符串是否都为数字
*
***************************************/
int isNumber(char * sNumber);

/*********
*剔除字符串左边的空格制表符等
*
***************************************/
char* ltrim(char *s);

/*********
*剔除字符串右边的空格制表符等
***************************************/
char* rtrim(char *s);

/*********
*剔除字符串左右的空格制表符等
***************************************/
char* trim(char *s);

/*********
*取系统时间，格式：YYYYMMDDHH24MISS
***************************************/
char* getSysDate(char * sysDate);

/*********
*是否为合法日期YYYYMMDD
***************************************/
int isDate(char * sDate);

//判断是否为合法时间hh24miss
int isTime(char * sTime);

//判断是否为合法日期串yyyymmddhh24miss
int isDatetime(const char * sDatetime);

//14位字符串时间从1970年1月1日00时00分00秒所经过的秒数
long time2sec(const char *src);

//14位字符串秒数差值
long diffsec(const char *src1, const char *src2);

//字母转换为大写
char* strupr(char * lwrstr);

//字母转换为小写
char* strlwr(char * lwrstr);

//简单不对称加密解密
char* cryptStr(char * oStr,char * sStr);

//分离文件全路径的路径名和文件名
int getExePathAndName( const char *in_ptrArgv0,char *out_exename, char *out_exepath);

//获得子串，sp是分隔子符，n 表示第n个子串(从1开始计数)，substr存放获得的子串
void getsubstr(char *str, char sp, int n, char *substr);

//返回目标字符串中子串的个数
int getstrnum(char *dest ,char *searchstr);

//替换目标字符串中的子串
int strreplace(char *dest,char *searchstr,char *replacestr);

//返回目标字符串中最后一个子串的指针
char *strrstr(const char *str1,const char *str2);

#endif /*__PUBLIC_H__*/
