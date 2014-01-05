/*
 *file name : conf.h
 *des : 
 *date : 2013-10-21
 *author : liwq (286570361)
 *notes :
 *       2013-10-21   liwq      create files
 */

/**********************************************************/

#ifndef __CONF_H__
#define __CONF_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <arpa/inet.h>

struct cfg_obj
{
    char  *name;
    char  *value;
    struct cfg_obj *next;
};

struct cfg_section
{
    char  *name;
    struct cfg_obj *obj;
    struct cfg_section *next;
};

typedef struct cfg_section cfg_st;



cfg_st *cfg_load(char *path);


int cfg_read_str(cfg_st *cfgId,
        char *section, char *name, char **value, char *default_value);


int cfg_read_strn(cfg_st *cfgId,
        char *section, char *name, char *value, size_t n, char *default_value);


int cfg_read_int(cfg_st *cfgId,
        char *section, char *name, int *value, int default_value);


int cfg_read_float(cfg_st *cfgId,
        char *section, char *name, float *value, float default_value);

int cfg_read_double(cfg_st *cfgId,
        char *section, char *name, double *value, double default_value);


int cfg_read_ipv4_addr(cfg_st *cfgId,
        char *section, char *name, struct sockaddr_in *addr, char *default_value);


int cfg_read_bool(cfg_st *cfgId,
        char *section, char *name, bool *value, bool default_value);

void cfg_free(cfg_st *cfgId);

#endif //__CONF_H__

