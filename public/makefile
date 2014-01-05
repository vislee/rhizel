
#author:liwq (286570361)

SQLINCLUDE=-I/usr/include/mysql/
SQLLIB=-L/usr/lib64/mysql/ -lmysqlclient
PYINCLUDE=-I/usr/include/python2.6/
PYLIB=-L/usr/lib/python2.6/ -lpython2.6

INCLUDE=-I. -pthread ${SQLINCLUDE} ${PYINCLUDE} 
LZIBS=-L. -lm -lnsl -lpthread ${SQLLIB} ${PYLIB}
#编译选项
DEBUG=${CFLAG}
ifeq ($(DEBUG),-D__DEBUG__)
  GPROF=-pg
endif
##
CC=gcc ${GPROF}


COMPILE=${CC} ${DEBUG} ${INCLUDE} -g -Wall  -c 
LINK=${CC} ${DEBUG} -g -Wall ${LZIBS}

#目标文件,添加新文件需要补充该目标
OBJECT=public.o \
       conf.o \
       soc.o \
       file.o \
       logfile.o \
       billFile.o \
       thrdpool.o \
       connpool.o
all:$(OBJECT)

%.o: %.c
	$(COMPILE) $< -o $@ 


move:
	cp $(OBJECT) ../object/.

clean:
	-@rm -rf *.o core*


threadTest:
threadTest:threadTest.o thrdpool.o
	${LINK} -o threadTest threadTest.o thrdpool.o

testsql:
testsql: testsql.o connpool.o
	${LINK} -o testsql testsql.o connpool.o

tConn:
tConn: tConn.o connpool.o thrdpool.o
	${LINK} -o tConn tConn.o connpool.o thrdpool.o

.PHONY:all move clean threadTest testsql tConn
