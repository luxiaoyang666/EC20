/********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  comport.h
 *    Description:  This head file of sms.c
 *
 *        Version:  1.0.0(09/07/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "09/07/20 08:58:33"
 *                 
 ********************************************************************************/
#ifndef  _SMS_H_
#define  _SMS_H_

#include "comport.h"
#include "PDU.h"

#define CENTER_NUM_LEN 14




/* Send AT Commond(comport_send plus) */
int send_at_cmd(ComportAttr *comport,char *atcmd,char *expected_recv,char *rmsg,int msgsize,int timeout);

/* Check if the serial port can communicate */
int check_comport_ready(ComportAttr *comport);

/* Check if the module can recognize the SIM card */
int check_if_there_is_sim(ComportAttr *comport);

/* Check the SIM card registration */
int check_sim_login(ComportAttr *comport);

/* Check the SIM card signal strength */
int check_sim_signal(ComportAttr *comport);

/* Use the above function to check whether the SIM card is ready to be used */
int check_sim_allready(ComportAttr *comport);

/* Get SMS Center Number of the SIM Card */
int get_sms_center_number(ComportAttr *comport,char *center_number);

/* Send TEXT Message */
int text_sms_send(ComportAttr *comport,char *sms_buf,char *phone_number);

/* Send PDU message */
int pdu_sms_send(ComportAttr *comport,char *sms_buf,char *phone_number);


#endif   /*   ----- #ifndef _COMPORT_H_  ----- */

