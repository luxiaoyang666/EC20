/*********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  comport.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(09/07/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "09/07/20 09:04:41"
 *
 ********************************************************************************/
#include "sms.h"


/************************************************************************************ 
 *
 *     Function:  int send_at_cmd(ComportAttr *comport,char *atcmd,char *expected_recv,char *rmsg,int msgsize,int timeout)
 *
 *    Parameter:  ComportAttr *comport   -   Serial port for communication
 *              
 *                char *arcmt            -   Commands to be sent to the serial port
 *
 *                char *expected_recv    -   Expected information
 *
 *                char *rmsg             -   Used to save the received information, can be NULL 
 *
 *                int msgsize            -   The size of rmsg  
 *
 *                int timeout            -   If this option is not 0, the program will call select to block read
 *
 *  Description:  Send the command in "atcmd" to the serial port, read the serial port, if you read the common-
 *                value with the lieutenant colonel expected_recv, the function returns 0, otherwise  returns -1
 *
 * Return Value:  0                      -   AT command sent successfully and received the expected value
 *                
 *                negative number        -   AT command failed to send or did not receive the expected value
 *
 ************************************************************************************/
int send_at_cmd(ComportAttr *comport,char *atcmd,char *expected_recv,char *rmsg,int msgsize,int timeout)
{
    char   temp_msg[128] = {0};

    if(!atcmd || !expected_recv)
    {
        printf("Unable to send AT commond,Invalid parameter.\n");
        return -1;
    }

    if(comport_send(comport,atcmd,strlen(atcmd)) < 0)
    {
        printf("Send AT commond failed:%s\n",strerror(errno));
        return -2;
    }

    usleep(10000);

    /* If rmsg is passed in, use rmsg */
    if(rmsg)
    {
        if(comport_recv(comport,rmsg,msgsize,timeout) <= 0)
        {
            printf("Recving message failed:%s\n",strerror(errno));
            return -3;
        }

        /* Search expectations */
        if(!strstr(rmsg,expected_recv))
        {
            printf("Can't find what you expect to receive,\'expected_recv\' is not in \'rmsg\'\n");
            return -4;
        }
    }

    /* Use a temporary stack array, invalid after returning */
    else 
    {
        if(comport_recv(comport,temp_msg,128,timeout) <= 0)
        {
            printf("Recving message failed:%s\n",strerror(errno));
            return -3;
        }

        if(!strstr(temp_msg,expected_recv))
        {   
            printf("Can't find what you expect to receive,\'expected_recv\' is not in \'rmsg\'\n");
            return -4; 
        } 
    }

    return 0;
}

/* Check if the serial port can communicate */
int check_comport_ready(ComportAttr *comport)
{         
    int    retval;
    
    retval = send_at_cmd(comport,"AT\r","OK",NULL,0,2);
    
    if(retval != 0)
    {
        printf("Serial port cannot be used\n");
        return retval;
    }
   
    return retval;
}  

/* Check if the module recognizes the SIM card */
int check_if_there_is_sim(ComportAttr *comport)
{           
    int    retval;
            
    retval = send_at_cmd(comport,"AT+CPIN?\r","READY",NULL,0,2);
            
    if(retval != 0)
    {       
        printf("No SIM detected\n");
        return retval;
    }       
            
    return retval;
}           
            
/* Check the SIM card registration," 0,1 " , " 0,3 "  means available */
int check_sim_login(ComportAttr *comport)
{
    int    retval1;
    int    retval2;

    retval1 = send_at_cmd(comport,"AT+CREG?\r","0,1",NULL,0,2);
    retval2 = send_at_cmd(comport,"AT+CREG?\r","0,3",NULL,0,2);

    if(retval1 && retval2)
    {
        printf("SIM is not registered\n");
        return -1;
    }

    return 0;
}

/* Check the SIM card signal strength ,The signal strength is too low or 99 is unavailable */
int check_sim_signal(ComportAttr *comport)
{
    int    i;
    int    retval;
    int    signal_strength;
    char   str_signal[10] = {0};
    char   msg[128] = {0};

    retval = send_at_cmd(comport,"AT+CSQ\r","+CSQ",msg,sizeof(msg),2);

/* IF Sucess:
 *
 * AT+CSQ
 *
 * +CSQ: 9,99
 *
 * OK
 *
 * * * * * * * 
 * AT+CSQ
 *
 * +CSQ: 23,99
 *
 * OK
 *
 * */

    if(retval != 0)
    {
        printf("Can not check signal\n");
        return -1;
    }

    for(i = 0; i < sizeof(msg); i++)
    {
        if(msg[i] == ',')  //Locate ' , '
        {
            if(msg[i-2] == ' ')  
                strncpy(str_signal,&msg[i-1],1);  //The signal strength is in single digits, for example: 8,99
            else 
                strncpy(str_signal,&msg[i-2],2);  //The signal strength is ten digits, for example: 28,99
        }
    }

    signal_strength = atoi(str_signal);

    if(signal_strength < 7 || signal_strength == 99)  //Signal is too low or no signal
    {
        printf("Signal Strengh is too low or no signal\n");
        return -2;
    }

    return 0;
}


/* Call 'check series' of functions, return 0 if all pass, otherwise return -1,-2.... */
int check_sim_allready(ComportAttr *comport)
{

    if(check_comport_ready(comport) < 0)
        return -1;
    printf("Serial port ready!\n");


    if(check_if_there_is_sim(comport) < 0)
        return -2;
    printf("SIM card can be detected!\n");


    if(check_sim_login(comport) < 0)
        return -3;
    printf("SIM card is registered!\n");


    if(check_sim_signal(comport) < 0)
        return -4;
    printf("Signal strength meets requirements!\n");

    printf("\nSIM cards are all ready!\n");

    return 0;
}



/* Obtain the SMS center number of the SIM card
 * Different operators and different regions will result in different SMS center numbers
 * China Telecom can not use.
 * */
int get_sms_center_number(ComportAttr *comport,char *center_number)
{
    int      retval;
    char    *ptr;
    char     rbuf[128] = {0};

    retval = send_at_cmd(comport,"AT+CSCA?\r","CSCA",rbuf,sizeof(rbuf),2);

/* IF Success:
 *
 * AT+CSCA?
 *
 * +CSCA: "+8613010788500",145
 *
 * OK
 *
 * */

    if(retval < 0)
    {
        printf("Can not receive CSCA\n");
        return -1;
    }

    ptr = strstr(rbuf,"CSCA");
    ptr += 7;

    strncpy(center_number,ptr,CENTER_NUM_LEN);

    return 0;
}


/* Send SMS in TEXT format */
int text_sms_send(ComportAttr *comport,char *sms_buf,char *phone_number)
{
    int    retval;
    char   sbuf[30] = {0};

    if(!sms_buf || !phone_number)
    {
        printf("Invalid parameter\n");
        return -1;
    }

    retval = send_at_cmd(comport,"AT+CMGF=1\r","OK",NULL,0,2);  //"AT+CMGF=1" means TEXT SMS
    if(retval < 0)
    {
        printf("Send \"AT+CMGF=1\" failed or can not receive \"OK\"\n");
        return -2;
    }

    /* Phone Number */
    sprintf(sbuf,"AT+CMGS=\"%s\"\r",phone_number);

    /* Expect to receive ' > ' */
    retval = send_at_cmd(comport,sbuf,">",NULL,0,2);
    if(retval < 0)
    {
        printf("Send AT+CMGS failed or can not receive \'>\'\n");
        return -3;
    }

    strcat(sms_buf,"\x1a");

    /* Send SMS,'\x1a'is the end of SMS sending Peugeot */
    retval = send_at_cmd(comport,sms_buf,"OK",NULL,0,10);
    if(retval < 0)
    {
        printf("SMS send failed\n");
        return -4;
    }

    printf("SMS sent successfully!\n");

    return 0;
}


/* Send PDU SMS , Will call the PDU encoded function */
int pdu_sms_send(ComportAttr *comport,char *sms_buf,char *phone_number)
{      
    int     value_cmgs;
    char    pdu_buf[512] = {0};
    char    sbuf[32] = {0};
    char    center_number[128] = {0};
                  
    if(!sms_buf || !phone_number)
    {             
        printf("Invalid parameter\n");
        return -1; 
    }

    if(get_sms_center_number(comport,center_number) < 0)  //Obtain SMS Center Number
    { 
        printf("Can not get SMS center number\n");
        return -2;
    } 
 
    if(pdu_encod(sms_buf,center_number,phone_number,pdu_buf,&value_cmgs) < 0)  //PDU encoding
    { 
        printf("Failed to construct PDU code\n");
        return -3;
    } 
      
    if(send_at_cmd(comport,"AT+CMGF=0\r","OK",NULL,0,2) < 0)  //"AT+CMGF=0" means PDU SMS
    {                    
        printf("Send \"AT+CMGF=0\" failed\n");
        return -4;       
    }                    
     
    /* Splice the processed phone number with the processed SMS into a string
     * value_cmgs represents one-half the length of the string, in decimal.
     * */
    sprintf(sbuf,"AT+CMGS=%d\r",value_cmgs);

    /*  Expect to receive ' > ' */ 
    if(send_at_cmd(comport,sbuf,">",NULL,0,2) < 0)  //Most of the failures are due to PDU encoding failure
    {
        printf("Send \"AT+CMGS=%d\"failed,or can not receive \'>\'\n",value_cmgs);
        return -5;
    }

    strcat(pdu_buf,"\x1a");

    /* If it can't receive OK within 10s,return */
    if(send_at_cmd(comport,pdu_buf,"OK",NULL,0,10) < 0)
    {
        printf("PDU SMS send failed\n");
        return -6;
    }

    printf("SMS sent successfully!\n");

    return 0;
}





