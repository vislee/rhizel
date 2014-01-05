/*
 *file name : conf.c
 *des : 
 *date : 2013-10-21
 *author : liwq (286570361)
 *notes :
 *       2013-10-21   liwq      create files
 */

/**********************************************************/
#include "conf.h"

static char * trim(char *s)
{
  char *ptmp = s;
  int len = strlen(s);
  int tmpLen = len;
  while (len)
  {
    if (isspace(*(ptmp+len-1)))
    {
      *(ptmp+len-1) = '\0';
      --len;
    }
    else 
    {
      break;
    }
  }
  while(isspace(*ptmp))
  {
    ++ptmp;
  }
  if (tmpLen > strlen(ptmp))
  {
    memmove(s, ptmp, strlen(ptmp)+1);
  }
  return s;
}
static bool is_comment(char **line)
{
  if (*line[0] == '#' || *line[0] == '\0' || strncmp(*line, "//", 2) == 0)
    return true;

  return false;
}

static ssize_t _readline(char **lineptr, size_t *n, FILE *stream)
{
    ssize_t len = getline(lineptr, n, stream);
    if (len == -1)
        return -1;

    char  *_line = NULL;
    size_t _n    = 0;
    trim(*lineptr);
    len = strlen(*lineptr);
    while (len >= 2 && (*lineptr)[len-1] == '\\')
    {
        (*lineptr)[len-1] = 0;
        trim(*lineptr);
        len = strlen(*lineptr);

        if (getline(&_line, &_n, stream) == -1)
        {
            free(_line);
            return 0;
        }

        char *next_line = _line;
        trim(next_line);

        ssize_t next_len = strlen(next_line);
        ssize_t need_len = len - 1 + next_len + 1;

        if (*n < need_len)
        {
            while (*n < need_len)
                *n *= 2;

            *lineptr = realloc(*lineptr, *n);
            if (*lineptr == NULL)
            {
                free(_line);
                return -1;
            }
        }
        
        strcat(*lineptr, next_line);
        len = strlen(*lineptr);
    }

    if (_line)
        free(_line);

    return len;
}

void cfg_free(cfg_st *handler)
{
    struct cfg_section *sec_curr = handler;
    struct cfg_section *next = NULL;

    while (sec_curr)
    {
        next = sec_curr->next;

        struct cfg_obj *obj_curr = sec_curr->obj;
        struct cfg_obj *arg_next = NULL;

        while (obj_curr)
        {
            arg_next = obj_curr->next;

            free(obj_curr->name);
            free(obj_curr->value);
            free(obj_curr);

            obj_curr = arg_next;
        }

        free(sec_curr->name);
        free(sec_curr);

        sec_curr = next;
    }

    return;
}

static void ini_print(cfg_st *handler)
{
  struct cfg_section *sec_curr = handler;

  while (sec_curr)
  {
      if (sec_curr->name == NULL)
      {
        sec_curr = sec_curr->next;
        continue;
      }

      printf("[%s]\n", sec_curr->name);

      struct cfg_obj *obj = sec_curr->obj;

      while (obj)
      {
          if (obj->name == NULL || obj->value == NULL)
              continue;

          printf("    %-20s = %s\n", obj->name, obj->value);
          obj = obj->next;
      }

      sec_curr = sec_curr->next;
  }
  return;
}

static struct cfg_section *create_section(struct cfg_section *sec_head, char *name)
{
    struct cfg_section *p = calloc(1, sizeof(struct cfg_section));

    if (p == NULL)
    {
        cfg_free(sec_head);

        return NULL;
    }

    if ((p->name = strdup(name)) == NULL)
    {
        cfg_free(sec_head);

        return NULL;
    }

    return p;
}

static struct cfg_section *find_section(struct cfg_section *sec_head, char *name)
{
    struct cfg_section *sec_curr = sec_head;

    while (sec_curr)
    {
        if (sec_curr->name && strcmp(sec_curr->name, name) == 0)
            return sec_curr;

        sec_curr = sec_curr->next;
    }

    return NULL;
}

static struct cfg_obj *create_obj(struct cfg_section *sec_head, char *name, char *value)
{
    struct cfg_obj *p = calloc(1, sizeof(struct cfg_obj));

    if (p == NULL)
    {
        cfg_free(sec_head);

        return NULL;
    }

    if ((p->name = strdup(name)) == NULL)
    {
        cfg_free(sec_head);

        return NULL;
    }

    if ((p->value = strdup(value)) == NULL)
    {
        cfg_free(sec_head);

        return NULL;
    }

    return p;
}

static struct cfg_obj *find_obj(struct cfg_section *sec_curr, char *name)
{
    struct cfg_obj *obj = sec_curr->obj;

    while (obj)
    {
        if (obj->name && strcmp(obj->name, name) == 0)
            return obj;

        obj = obj->next;
    }

    return NULL;
}

cfg_st *cfg_load(char *path)
{
    FILE *fp = fopen(path, "r");
    if (NULL == fp)
    {
      return NULL;
    }

    struct cfg_section *sec_head = NULL;
    struct cfg_section *sec_prev = NULL;
    struct cfg_section *sec_curr = NULL;

    struct cfg_obj *obj_curr = NULL;
    struct cfg_obj *obj_prev = NULL;

    char *line = NULL;
    size_t   n  = 0;
    ssize_t len = 0;

    while ((len = _readline(&line, &n, fp)) != -1)
    {
        char *s = line;
        #ifdef __DEBUG__
          printf("1_readline:%s\n",s);
        #endif
        if (is_comment(&s))
            continue;
        len = strlen(s);

        if (len>2 && s[0]=='[' && s[len-1]==']')
        {
            char *name = s + 1;
            *(name+len-1-1) = '\0';
            trim(name);

            if ((sec_curr = find_section(sec_head, name)) == NULL)
            {
                if ((sec_curr = create_section(sec_head, name)) == NULL)
                {
                    free(line);
                    return NULL;
                }

                if (sec_head == NULL)
                    sec_head = sec_curr;
                if (sec_prev != NULL)
                    sec_prev->next = sec_curr;

                sec_prev = sec_curr;
                obj_prev = NULL;
            }
            else
            {
                obj_prev = sec_curr->obj;
                while (obj_prev->next != NULL)
                    obj_prev = obj_prev->next;
            }
            continue;
        }

        char *delimiter = strchr(s, '=');
        if (delimiter == NULL)
            continue;
        *delimiter = '\0';

        char *name = s;
        trim(name);

        char *value = delimiter + 1;
        delimiter = strchr(value, '#');
        if (NULL != delimiter)
        {
          *delimiter = '\0';
        }
        trim(value);

        if (sec_curr == NULL)
        {
            if ((sec_curr = create_section(sec_head, "global")) == NULL)
            {
                free(line);

                return NULL;
            }

            if (sec_head == NULL)
                sec_head = sec_curr;
            sec_prev = sec_curr;
            obj_prev = NULL;
        }

        if ((obj_curr = find_obj(sec_curr, name)) == NULL)
        {
            obj_curr = create_obj(sec_head, name, value);
            if (obj_curr == NULL)
            {
                free(line);

                return NULL;
            }

            if (obj_prev)
                obj_prev->next = obj_curr;
            if (sec_curr->obj == NULL)
                sec_curr->obj = obj_curr;

            obj_prev = obj_curr;
        }
        else
        {
            char *old_value = obj_curr->value;

            if ((obj_curr->value = strdup(value)) == NULL)
            {
                cfg_free(sec_head);

                free(line);

                return NULL;
            }

            free(old_value);
        }
    }

    free(line);
    fclose(fp);

    if (sec_head == NULL)
    {
        if ((sec_head = calloc(1, sizeof(struct cfg_section))) == NULL)
            return NULL;
    }
    #ifdef __DEBUG__
      ini_print(sec_head);
    #endif
    
    return sec_head;
}

int cfg_read_str(cfg_st *handler,
        char *section, char *name, char **value, char *default_value)
{
    if (!handler || !name || !value)
        return -1;

    if (section == NULL || *section == 0)
        section = "global";

    struct cfg_section *sec_curr = handler;

    while (sec_curr)
    {
        if (sec_curr->name && strcmp(section, sec_curr->name) == 0)
            break;

        sec_curr = sec_curr->next;
    }

    if (sec_curr)
    {
        struct cfg_obj *obj = sec_curr->obj;

        while (obj)
        {
            if (obj->name && obj->value && strcmp(obj->name, name) == 0)
            {
                #ifdef __DEBUG__
                  printf("name:%s val is:%s\n",obj->name,obj->value);
                #endif
                *value = strdup(obj->value);
                if (*value == NULL)
                    return -1;

                return 0;
            }

            obj = obj->next;
        }
    }

    if (default_value)
    {
        *value = strdup(default_value);
        if (*value == NULL)
            return -1;
    }
    else
    {
        *value = NULL;
    }

    return 1;
}



int cfg_read_strn(cfg_st *handler,
        char *section, char *name, char *value, size_t n, char *default_value)
{
    char *s = NULL;
    int ret = cfg_read_str(handler, section, name, &s, default_value);
    if (ret < 0)
        return ret;

    memset(value, 0, n);

    if (s)
    {
        strncpy(value, s, n);
        free(s);
    }

    return ret;
}

static int ini_read_num(cfg_st *handler,
        char *section, char *name, void *value, bool is_unsigned)
{
    char *s = NULL;
    int ret = cfg_read_str(handler, section, name, &s, NULL);
    if (ret == 0)
    {
        if (is_unsigned)
            *(unsigned long long int*)value = strtoull(s, NULL, 0);
        else
            *(long long int *)value = strtoll(s, NULL, 0);

        free(s);
    }

    return ret;
}

# define INI_READ_SIGNED(type) do { \
    long long int v; \
    int ret = ini_read_num(handler, section, name, &v, false); \
    if (ret == 0) { \
        *value = (type)v; \
    } \
    else if (ret > 0) { \
        *value = default_value; \
    } \
    return ret; \
} while (0)

# define INI_READ_UNSIGNED(type) do { \
    unsigned long long int v; \
    int ret = ini_read_num(handler, section, name, &v, true); \
    if (ret == 0) { \
        *value = (type)v; \
    } \
    else if (ret > 0) { \
        *value = default_value; \
    } \
    return ret; \
} while (0)

int cfg_read_int(cfg_st *handler,
        char *section, char *name, int *value, int default_value)
{
    INI_READ_SIGNED(int);
}


int cfg_read_float(cfg_st *handler,
        char *section, char *name, float *value, float default_value)
{
    char *s = NULL;
    int ret = cfg_read_str(handler, section, name, &s, NULL);
    if (ret == 0)
    {
        *value = strtof(s, NULL);

        free(s);
    }
    else if (ret > 0)
    {
        *value = default_value;
    }

    return ret;
}

int cfg_read_double(cfg_st *handler,
        char *section, char *name, double *value, double default_value)
{
    char *s = NULL;
    int ret = cfg_read_str(handler, section, name, &s, NULL);
    if (ret == 0)
    {
        *value = strtod(s, NULL);

        free(s);
    }
    else if (ret > 0)
    {
        *value = default_value;
    }

    return ret;
}

int cfg_read_ipv4_addr(cfg_st *handler,
        char *section, char *name, struct sockaddr_in *addr, char *default_value)
{
    char *s = NULL;
    int ret = cfg_read_str(handler, section, name, &s, default_value);
    if (ret < 0)
        return ret;

    memset(addr, 0, sizeof(struct sockaddr_in));

    if (s)
    {
        char *ip = strtok(s, ": \t");
        if (ip == NULL)
        {
            free(s);

            return -1;
        }

        char *port = strtok(NULL, ": \t");
        if (port == NULL)
        {
            free(s);

            return -1;
        }

        addr->sin_family = AF_INET;
        if (inet_aton(ip, &addr->sin_addr) == 0)
        {
            free(s);

            return -1;
        }

        addr->sin_port = htons((uint16_t)atoi(port));

        free(s);
    }

    return ret;
}

int cfg_read_bool(cfg_st *handler,
        char *section, char *name, bool *value, bool default_value)
{
    char *s = NULL;
    int ret = cfg_read_str(handler, section, name, &s, NULL);
    if (ret == 0)
    {
        int i;
        for (i = 0; s[i]; ++i)
            s[i] = tolower(s[i]);

        if (strcmp(s, "true") == 0)
            *value = true;
        else if (strcmp(s, "false") == 0)
            *value = false;
        else
            *value = default_value;

        free(s);
    }
    else if (ret > 0)
    {
        *value = default_value;
    }

    return ret;
}