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
#include "PDU.h"

void help_information();
void List_Sms_type();

int main(int argc, char *argv[])
{
    int              i;
    int              ch;
    int              retval;
    int              sms_type;
    char             center_num[30] = {0};
    char             sms_buf[246] = {0};
    char             phone_number[20] = {0};
    Serial_attr      attr;

    struct option    options[] = {
        {"help",no_argument,NULL,'h'},
        {"baudrate",required_argument,NULL,'b'},
        {"databits",required_argument,NULL,'d'},
        {"parity",required_argument,NULL,'p'},
        {"stopbits",required_argument,NULL,'s'},
        {"name",required_argument,NULL,'n'},
        {NULL,0,NULL,0}
    };

   // attr = (Serial_attr*)malloc(sizeof(Serial_attr));
    memset(&attr,0,sizeof(Serial_attr));
    
    while((ch = getopt_long(argc,argv,"hb:d:p:s:n:",options,NULL)) != -1)
    {
        switch(ch)
        {
            case 'h':
                help_information();
                return 0;

            case 'b':
                attr.BaudRate = atoi(optarg);
                break;

            case 'd':
                attr.DataBits = atoi(optarg);
                break;

            case 'p':
                attr.Parity = optarg[0];
                break;

            case 's':
                attr.StopBits = atoi(optarg);
                break;

            case 'n':
                strncpy(attr.SerialName,optarg,SERIALNAME_LEN);
                break;
                
        }
    }


    /* 没有传入串口名 */
    if(strlen(attr.SerialName) == 0)
    {
        printf("Parameter warning:\n");
        printf("\tAt least need to enter the serial port name,You can specify the serial port name with -n.\n");
        return 0;
    }

    if(comport_open(&attr) != 0)
    {
        printf("Open %s failed!\n",attr.SerialName);
        return -1;
    }

    retval = comport_init(&attr);
    if(retval < 0)
        goto cleanup;

    while(1)
    {
        memset(phone_number,0,sizeof(phone_number));
        memset(center_num,0,sizeof(center_num));
        memset(sms_buf,0,sizeof(sms_buf));

        List_Sms_type();
        printf("\nYou choose:  ");
        scanf("%d",&sms_type);
        getchar();

        /*  TEXT  */
        if(1 == sms_type)
        {
            printf("请输入号码： ");
            fgets(phone_number,sizeof(phone_number),stdin);
            if(strstr(phone_number,"again"))
            {
                continue;
            }
            for(i = 0; i < sizeof(phone_number);i++)
            {
                if(phone_number[i] == '\n')  //消除fgets()带来的换行
                    phone_number[i] = '\0';
            }
            printf("请输入短信： ");
            fgets(sms_buf,sizeof(sms_buf),stdin);
            if(strstr(sms_buf,"again"))
            {    
                continue;
            } 
            for(i = 0; i < sizeof(sms_buf);i++)
            {
                if(sms_buf[i] == '\n')  //消除fgets()带来的换行
                    sms_buf[i] = '\0';
            }

            if(text_sms(&attr,sms_buf,phone_number) < 0)
            {
                printf("TEXT SMS send failure,make sure your inputments.\n");
                continue;
            }
        }

        /* PDU */
        else if(2 == sms_type)
        {
            printf("请输入短信中心号(包含+86):  ");
            scanf("%s",center_num);
            getchar();

            printf("请输入收件人号码(包含+86):  ");
            scanf("%s",phone_number);
            getchar();

            printf("请输入短信:  ");
            fgets(sms_buf,sizeof(sms_buf),stdin);
            if(strstr(sms_buf,"again"))
            {
                continue;
            }
            for(i = 0;i < sizeof(sms_buf);i++)
            {
                if(sms_buf[i] == '\n')
                    sms_buf[i] = '\0';
            }
            if(pdu_sms(attr,sms_buf,center_num,phone_number) != 0)
            {
                printf("PDU SMS send failure,make sure your inputments.\n");
                continue;
            }

        }

        else 
            break;
    }

cleanup:
    comport_close(&attr);

    return 0;
}

void help_information()
{
    printf("\t-b   Set BaudRate\n");
    printf("\t-d   Set Databits\n");
    printf("\t-p   Set Parity,0 for no parity,1 for Odd parity,2 for Evev parity\n");
    printf("\t-s   Set StopBits\n");
    printf("\t-n   Set the name of the serial port you want to use\n");
    printf("\t     Ctrl + c to exit the program\n");
    printf("\n\tIf you do not specify parameters,The default parameters of the program are as follows:\n");
    printf("\tBaudRate: 1115200\n\tDatabits: 8bits\n\tParity:   no parity\n\tStopBits: 1bits\n");
}

void List_Sms_type()
{
    printf("\n\n");
    printf("***********************************************************\n");
    printf("*\t\t\t\t\t\t\t  *\n");
    printf("*       Please select the type of SMS to be sent          *\n");
    printf("*   If you make a mistake, enter \"again\" and re-enter     *\n");
    printf("*\t\t\t\t\t\t\t  *\n");
    printf("*\t\t\t\t\t\t\t  *\n");
    printf("*  <1> TEXT          <2> PDU          <others> QUIT       *\n");
    printf("*\t\t\t\t\t\t\t  *\n");
    printf("***********************************************************\n");
}
