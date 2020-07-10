/********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  ComportInit.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(10/07/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "10/07/20 16:21:50"
 *                 
 ********************************************************************************/
#ifndef  _COMPORTINIT_H_
#define  _COMPORTINIT_H_

#include "ComportOpen.h"
#include <string.h>


/* 串口属性的初始化，因为内容较多，单独列出 */
int comport_init(MyAttr *attr);

#endif   /* ----- #ifndef _COMPORTINIT_H_  ----- */
