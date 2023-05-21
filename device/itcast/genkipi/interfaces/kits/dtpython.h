/*
  =======================================================================================
  File: 
        dtpython.h

  Description: 
        This header file is for the interface of DTPython project which is an experiment to make HOS board 
        to support Python. DTPython is made from MicroPython, but is NOT MicroPython anymore,
        because the basic purpose of DTPython is to supply another option to develop HOS device application.

  Author:
       Delphi Tang (唐佐林)
       http://www.dt4sw.com/

  Revision History:
       V0.0.1 (Initial Version)
  =======================================================================================

*/


#ifndef DTPYTHON_H
#define DTPYTHON_H

/*
  Description: 
      To initialize Python enviroment.

  Parameter:
      None

  Return Value:
      None
*/
void DTPython_Init(void);

/*
  Description: 
      To run a python file.

  Parameter:
      file -- python file name, such as "test.py"

  Return Value:
      None
*/
void DTPython_RunFile(const char* file);

/*
  Description: 
      To run python code statements.

  Parameter:
      statement -- python statements, such as "a = 1 + 2"

  Return Value:
      None
*/
void DTPython_RunCode(const char* statement);

/*
  Description: 
      To clean up Python enviroment.

  Parameter:
      None

  Return Value:
      None
*/
void DTPython_Deinit(void);

#endif