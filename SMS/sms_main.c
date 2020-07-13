/*********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  comport.c
 *    Description:  This file Send TEXT or PDU SMS
 *                 
 *        Version:  1.0.0(04/07/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "04/07/20 03:02:28"
 *                 
 ********************************************************************************/
#include "comport.h"
#include "sms.h"
#include <getopt.h>
#include <signal.h>
#include "PDU.h"

void help_information();
void List_Sms_type();
int identify_sms_type(const char *sms_buf,int buf_len);


int  g_stop = 0;

void signal_capture(int signal_num)
{
    if(SIGINT == signal_num)
        g_stop = 1;
}

int main(int argc, char *argv[])
{
    int                  i;
    int                  ch;
    int                  chose;
    int                  retval;
    int                  sms_type;
    char                 sms_buf[256] = {0};
    char                 phone_number[256] = {0};
    ComportAttr         *comport;

    struct option    options[] = {
        {"help",no_argument,NULL,'h'},
        {"baudrate",required_argument,NULL,'b'},
        {"databits",required_argument,NULL,'d'},
        {"parity",required_argument,NULL,'p'},
        {"stopbits",required_argument,NULL,'s'},
        {"name",required_argument,NULL,'n'},
        {NULL,0,NULL,0}
    };

    comport = (ComportAttr *)malloc(sizeof(ComportAttr));
    memset(comport,0,sizeof(ComportAttr));
    
    /* Get command line parameters */
    while((ch = getopt_long(argc,argv,"hb:d:p:s:n:",options,NULL)) != -1)
    {
        switch(ch)
        {
            case 'h':
                help_information();
                return 0;

            case 'b':
                comport->BaudRate = atoi(optarg);
                break;

            case 'd':
                comport->DataBits = atoi(optarg);
                break;

            case 'p':
                comport->Parity = optarg[0];
                break;

            case 's':
                comport->StopBits = atoi(optarg);
                break;

            case 'n':
                strncpy(comport->SerialName,optarg,SERIALNAME_LEN);
                break;
                
        }
    }


    /* 没有传入串口名 */
    if(strlen(comport->SerialName) == 0)
    {
        help_information();
        return 0;
    }

    if(comport_open(comport) != 0)
    {
        printf("Open %s failed!\n",comport->SerialName);
        return -1;
    }

    retval = comport_init(comport);
    if(retval < 0)
        goto cleanup;

    signal(SIGINT,signal_capture);

    if(check_sim_allready < 0)
    {
        printf("SIM card is not available, please check\n");
        goto cleanup;
    }

    else 
        printf("SIM card is allready!\n");

    printf("\nThis program supports TEXT SMS and PDU SMS.\nSMS format is automatically determined.\nWhen entering the phone number, please pay attention to input \"+86\"!\nStart texting....\n");

    while(!g_stop)
    {
        bzero(phone_number,sizeof(phone_number));
        bzero(sms_buf,sizeof(sms_buf));

        printf("\n\t<1>  Send message\t<other>   Quit\n");
        scanf("%d",&chose);
        getchar();  //Refresh LF 

        if( 1 != chose )
            goto cleanup;

        printf("Enter phone number:  ");
        scanf("%s",phone_number);
        getchar();

        printf("\nEnter SMS:  ");
        fgets(sms_buf,sizeof(sms_buf),stdin);

        /* Delete the LF brought by fgets() */
        for(i = 0; i < sizeof(sms_buf); i++)
        {
            if(sms_buf[i] == 0x0a)
            {
                sms_buf[i] = 0;
                break;
            }
        }

        printf("\n");

        /* Identify whether the text message format is TEXT or PDU */
        sms_type = identify_sms_type(sms_buf,strlen(sms_buf));

        /* PDU */
        if(0 == sms_type)
        {
            if(pdu_sms_send(comport,sms_buf,phone_number) < 0)
            {
                printf("PDU SMS Sent failed\n");
                continue;
            }
        }

        /* TEXT */
        else if(1 == sms_type)
        {
            if(text_sms_send(comport,sms_buf,phone_number) < 0)
            {
                printf("TEXT SMS Sent failed\n");
                continue;
            }
        }

        else 
            continue;

    }


cleanup:
    comport_close(comport);

    return 0;
}

void help_information()
{
    printf("SMS Sending System v1.0.2(Mon 13 Jul 05:08:24 BST 2020)\n");
    printf("\nUsage:./sms [-b BaudRate] [-d Databits] [-p Parity] [-s StopBits] [-n SerialName]\n\n");
    printf("\t-b   Set BaudRate\n");
    printf("\t-d   Set Databits\n");
    printf("\t-p   Set Parity,0 for no parity,1 for Odd parity,2 for Evev parity\n");
    printf("\t-s   Set StopBits\n");
    printf("\t-n   Set the name of the serial port you want to use\n");
    printf("\nCtrl + c to exit the program\n");
    printf("\n\tIf you do not specify parameters,The default parameters of the program are as follows:\n\n");
    printf("\tBaudRate: 1115200\n\tDatabits: 8bits\n\tParity:   no parity\n\tStopBits: 1bits\n");
}

int identify_sms_type(const char *sms_buf,int buf_len)
{
    int     i;

    for(i = 0; i < buf_len; i++)
    {
        if( (int)sms_buf[i] > 0x7F)
            return 0;
    }

    return 1;
}
