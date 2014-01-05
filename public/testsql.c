#include "connpool.h"
#include <stdio.h>
#include <string.h>
#include <mysql.h>


int main(void)
{
  char insersql[1024] = {0};
  strcpy(insersql, "insert into vince_lee_test_acct(acct_id, acct_name, acct_amount, user_id, times) values(@n:=ifnull(@n,0)+1, 'zhouzhou', 3000, 1111, now())");
  typeConnpool pools;
  MYSQL_RES *results;
  MYSQL_ROW record;
  initPool(&pools, 3, "sbilling_test", "sbilling_test", "221.123.166.214", 3306, "sbilling_test");
  MYSQL *db1 = getConnDb(&pools);
  mysql_query(db1, "select User_ID from vince_Product_User limit 3");
  results = mysql_store_result(db1);
  recycleDb(&pools, db1);
  while((record = mysql_fetch_row(results))) {
      printf("USER_ID:%s\n", (char*)record[0]);
   }
  mysql_free_result(results);

  MYSQL *db2 = getConnDb(&pools);

  mysql_query(db2, "select now()");
  results = mysql_store_result(db2);
  while((record = mysql_fetch_row(results))) {
      printf("TIME:%s\n", (char*)record[0]);
  }

  MYSQL *db3 = getConnDb(&pools);
  int ret = mysql_query(db3, insersql);
  if (ret)
  {
    perror(mysql_error(db3));
  }
  else
  {
    printf("insert into ok\n");
  }
  recycleDb(&pools, db2);
  recycleDb(&pools, db3);
  desPool(&pools);
  return 0;
}
