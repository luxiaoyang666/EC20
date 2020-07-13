/*********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  PDU.c
 *
 *    Description:  This file contains functions such as processing phone numbers, 
 *                  encoding conversion, etc. and implementing the final PDU format encoding
 *                 
 *        Version:  1.0.0(13/07/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "13/07/20 02:11:51"
 *                 
 ********************************************************************************/
#include "PDU.h"

int Processing_phone_number(char *phone_number)
{
    int    i;
    char   temp;
    char   head[64] = "11000D91";
    char   tail[64] = "000800";

/* 
 *  1100: fixed
 *  0D: the length of the mobile phone number, not counting the + sign, expressed in hexadecimal 
 *  91: sent to the mobile phone
 *
 * */

    if(!phone_number)
    {
        printf("Invalid input\n");
        return -1;
    }

    /* Drop the ‘+’ sign, add ‘F’, and swap the parity bits */
    strcat(phone_number,"F");
    strcpy(phone_number,&phone_number[1]);
    for(i = 0; i < 14;i+=2)
    {
        temp = phone_number[i];
        phone_number[i] = phone_number[i+1];
        phone_number[i+1] = temp;
    }

    /* Add header,tail */
    strcat(head,phone_number);
    strcat(head,tail);
    strcpy(phone_number,head);

    return 0;
}


int Processing_center_number(char *center_number)
{
    int i;
    char temp;
    char head[20] = "0891";  //08: length   91: globalization

    if(!center_number)
    {
        printf("Invalid input\n");
        return -1;
    }  
     

    /*  Drop the ‘+’ sign, add ‘F’, and swap the parity bits */ 
    strcpy(center_number,&center_number[1]);

    strcat(center_number,"F");

    for(i = 0; i < 14;i+=2)  
    {
        temp = center_number[i];
        center_number[i] = center_number[i+1];
        center_number[i+1] = temp;
    }


    /* Add header */
    strcat(head,center_number);
    strcpy(center_number,head);

    return -1;
}   

/* UTF-8 -> Unicode , LSB */
int utf8_to_unicode(char* utf8_buf,char* unic_buf)
{
    if(!utf8_buf)
    {
        printf("Invalid parameter\n");
        return -1;
    }
    char *temp = unic_buf;

    char b1,b2,b3,b4;  //b1: high data bit   b4: low data bits

    while(*utf8_buf)
    {
        if(*utf8_buf > 0x00 && *utf8_buf <= 0x7E)  //Single byte
        {
            *temp = 0;
            temp++;
            *temp = *utf8_buf;
            temp++;
            utf8_buf++;  //Next unprocessed character
        }

        else if(((*utf8_buf) & 0xE0) == 0xC0)  //Double bytes
        {
            b1 = *utf8_buf;
            b2 = *(utf8_buf+1);

            if((b2 & 0xC0) != 0x80)  //Check the legality of characters,Double bytes of UTF-8: 110xxxxx 10xxxxxx
                return -1;

            *temp = (b1 >> 2) & 0x07;
            temp++;
            *temp = (b1 << 6) + (b2 & 0x3F);
            temp++;
            utf8_buf+=2;
        }

        else if(((*utf8_buf) & 0xF0) == 0xE0)  //Three bytes
        {
            b1 = *utf8_buf;
            b2 = *(utf8_buf+1);
            b3 = *(utf8_buf+2);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )  //Check the legality of characters,1110xxxx 10xxxxxx 10xxxxxx
                return -1;

            *temp = (b1 << 4) + ((b2 >> 2) & 0x0F);
            temp++;
            *temp = (b2 << 6) + (b3 & 0x3F);
            temp++;
            utf8_buf+=3;
        }

        else if(*utf8_buf >= 0xF0 && *utf8_buf < 0xF8) //Four bytes
        {
            b1 = *utf8_buf;
            b2 = *(utf8_buf+1);
            b3 = *(utf8_buf+2);
            b4 = *(utf8_buf+3);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) || ((b4 & 0xC0) != 0x80) )  //11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                return -1;
            *temp = ((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03);  
             temp++;
            *temp = (b2 << 4) + ((b3 >> 2) & 0x0F); 
             temp++;
            *temp = (b3 << 6) + (b4 & 0x3F);

             temp++;
             utf8_buf+=4;
        }

        else
            return -1;

    }

    /* Add FFFE at the end */
    *temp = 0xFF;
    temp++;
    *temp = 0xFE;

    return 0;
}


/* Hex -> Str */
int Hex2Str( const char *sSrc,  char *sDest, int nSrcLen )  
{  
    int              i;
    char             szTmp[3];
    if(!sSrc || !sDest || nSrcLen <= 0)
    {
        printf("Unable to transcode Hex to String,Invalid parameter.\n");
        return -1;
    }
  
    for( i = 0; i < nSrcLen; i++ )  
    { 
        if(sSrc[i] != 0xFF && sSrc[i+1] != 0xFE)  //0xFF 0xFE is the end of Unicode
        {
            sprintf( szTmp, "%02X", (unsigned char) sSrc[i] );  
            memcpy( &sDest[i * 2], szTmp, 2 );  
        } 
        else 
            break;
    }
    return 0;
}


/************************************************************************************ 
 *
 *     Function:  int pdu_encod(char *sms_buf,char *center_number,char *phone_number,char *pdu_buf,int *val_cmgs)
 *
 *    Parameter:  char *sms_buf          -    Unprocessed SMS
 *              
 *                char *center_number    -    The SMS Center Number of SIM Card
 *
 *                char *phone_number     -    Recipient's Mobile Number
 *
 *                char *pdu_buf          -    Used to save the PDU code after completion
 *
 *                int  *val_cmgs         -    After splicing the processed phone number and the processed SMS,
 *
 *                                            the length of the string is half, which is a decimal number
 *
 *
 *  Description:  Use the SMS center number, recipient number, and SMS to encode the PDU,then save it in the fourth parameter,
 *                and also record the value required by cmgs
 *               
 * Return Value:  0                      -    PDU encoding success
 *                
 *                negative number        -    Failure
 *
 ************************************************************************************/
int pdu_encod(char *sms_buf,char *center_number,char *phone_number,char *pdu_buf,int *val_cmgs)
{
    char    temp[512] = {0};
    char    str_unicode[256] = {0};
    char    unicode_buf[256] = {0};

    if(!center_number || !phone_number)
    {
        printf("Unable to perform pdu encoding,Invalid parameter.\n");
        return -1;
    }

    /* UTF8 -> Unicode */
    if(utf8_to_unicode(sms_buf,unicode_buf) != 0)
    {
        printf("UTF-8 to Unicode failed,Check your input.\n");
        return -2;
    }

    /* Hex -> Str */
    if(Hex2Str(unicode_buf,str_unicode,256) != 0)
    {
        printf("Hex to String failed.\n");
        return -3;
    }

    /* Half of Unicode length, take hex */
    sprintf(temp,"%02x%s",strlen(str_unicode)/2,str_unicode);

    Processing_center_number(center_number);
    Processing_phone_number(phone_number);

    /* Stitching */
    strcat(phone_number,temp);
   *val_cmgs = (int)strlen(phone_number)/2; //This value is used when the AT command sends PDU format SMS

    strcat(center_number,phone_number);

    strcpy(pdu_buf,center_number);

    if(1)
        printf("cmgs:%d\npdu:%s\n",*val_cmgs,pdu_buf);

    return 0;
}
