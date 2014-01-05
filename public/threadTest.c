#include <stdio.h>
#include <Python.h>

#include "thrdpool.h"

void proc(void *data)
{
  printf("========[%s]==========\n", (void*)data);
}
void* procNext(void *data)
{
  char *p = (char*)data;
  //printf("=========[%s]===========\n", p);
  PyObject *pName, *pModule, *pDict, *pFunc;
  
  PyGILState_STATE  state = PyGILState_Ensure();
  PyRun_SimpleString("import sys");
  //PyRun_SimpleString("print('------hello python------')");

  //--------------------------
  char libPath[128] = {0};
  sprintf(libPath, "sys.path.append('%s')", "/sbilling/work/vince/sbill/lib/");

  PyRun_SimpleString(libPath);
  pName = PyString_FromString("prt");
  pModule = PyImport_Import(pName);
  if (!pModule)
  {
    perror("PyImport_Import");
    goto exit_flag;
  }
  pDict = PyModule_GetDict(pModule);
  if (!pDict)
  {
    goto exit_flag;
  }
  pFunc = PyDict_GetItemString(pDict, "prt");
  if (!pFunc || !PyCallable_Check(pFunc))
  {
    perror("PyDict_GetItemString");
    goto exit_flag;
  }
  
  PyObject *pyParams = PyTuple_New(1);
  PyObject *pyValue = PyString_FromString(p);
  PyTuple_SetItem(pyParams, 0, pyValue);
  PyEval_CallObject(pFunc, pyParams);
  //--------------------------

  // Clean up
  exit_flag:
  //if (pyValue)
  //{
  //  Py_DECREF(pyValue);
  //}
  if (pyParams)
  {
    Py_DECREF(pyParams);
  }
  //if (pFunc)
  //{
  //  Py_DECREF(pFunc);
  //}
  if (pModule)
  {
    Py_DECREF(pModule);
  }
  if (pName)
  {
    Py_DECREF(pName);
  }

  //if (pDict)
  //{
  //  Py_DECREF(pDict);
  //}

  PyGILState_Release(state);
}

int main()
{
  printf("========main begin=========\n");
  tpool_t *myPools = NULL;
  tpool_create(&myPools, 2);
  sleep(1);
  Py_Initialize();
  PyEval_InitThreads();
  PyEval_ReleaseThread(PyThreadState_Get());
  int i = 20;
  printf("tpool_add_work begin ...");
  while (i--)
  {
    //printf("\n[%d]\n",i);
    char aa[10] = {0};
    sprintf(aa, "%d", i);
    tpool_add_work(myPools, procNext, (void*)aa, 10);
  }
  printf("end1\n");
  tpool_destroy(myPools, 1);
  sleep(5);
  PyGILState_Ensure();
  Py_Finalize();    
  printf("end2\n");
  return 0;
}
