#include "PDU.h"

/*  处理收件人电话号码 */
void Handling_phone_number(char *phone_number)
{
    int    i;
    char   temp;
    char   head[35] = "11000D91";
    char   tail[10] = "000800";

    if(phone_number == NULL)
    {
        printf("Invalid input\n");
        return;
    }

    /* 去掉‘+’号，添加‘F’交换奇偶位 */
    strcat(phone_number,"F");
    strcpy(phone_number,&phone_number[1]);
    for(i = 0; i < 14;i+=2)
    {
        temp = phone_number[i];
        phone_number[i] = phone_number[i+1];
        phone_number[i+1] = temp;
    }

    /* 添加首位 */
    strcat(head,phone_number);
    strcat(head,tail);
    strcpy(phone_number,head);
}

/* 处理短信中心号码 */
void Handling_center_number(char *center_number)
{
    int i;
    char temp;
    char head[20] = "0891";

    if(center_number == NULL)
    {
        printf("invalid input\n");
        return;
    }  
       
    /* 去掉‘+’号，添加‘F’ */
    strcpy(center_number,&center_number[1]);

    strcat(center_number,"F");

    for(i = 0; i < 14;i+=2)  //交换奇偶位
    {
        temp = center_number[i];
        center_number[i] = center_number[i+1];
        center_number[i+1] = temp;
    }


    /* 添加首部 */
    strcat(head,center_number);
    strcpy(center_number,head);

}   


int utf8_to_unicode(char* utf8_buf,char* unic_buf)
{
    if(utf8_buf == NULL)
    {
        printf("Invalid parameter\n");
        return -1;
    }
    char *temp = unic_buf;

    char b1,b2,b3,b4;  //b1表示高位，b2为次高位

    while(*utf8_buf)
    {
        if(*utf8_buf > 0x00 && *utf8_buf <= 0x7E)  //处理单字节
        {
            *temp = 0;
            temp++;
            *temp = *utf8_buf;
            temp++;
            utf8_buf++;  //指向下一个字符
        }

    //    else if(((*utf8_buf) & 0xE0) == 0xC)  //处理双字节
        else if(*utf8_buf > 0xC0 && *utf8_buf <= 0xE0)
        {
            b1 = *utf8_buf;
            b2 = *(utf8_buf+1);

            if((b2 & 0xC0) != 0x80)  //判断字符的合法性，双字节UTF-8编码是 110xxxxx 10xxxxxx
                return -1;

            *temp = (b1 >> 2) & 0x07;
            temp++;
            *temp = (b1 << 6) + (b2 & 0x3F);
            temp++;
            utf8_buf+=2;
        }

        else if(((*utf8_buf) & 0xF0) == 0xE0)  //处理三字节
        {
            b1 = *utf8_buf;
            b2 = *(utf8_buf+1);
            b3 = *(utf8_buf+2);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )  //检查合法性，三字节的UTF-8编码是 1110xxxx 10xxxxxx 10xxxxxx
                return -1;

            *temp = (b1 << 4) + ((b2 >> 2) & 0x0F);
            temp++;
            *temp = (b2 << 6) + (b3 & 0x3F);
            temp++;
            utf8_buf+=3;
        }

        else if(*utf8_buf >= 0xF0 && *utf8_buf < 0xF8) //处理四字节
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
        {
            return -1;
        }


    }

    /*  以FFFE作为结束标致，便于拼接 */
    *temp = 0xFF;
    temp++;
    *temp = 0xFE;

    return 0;
}


/* 将字节流的数组转为字符流 */
void Hex2Str( const char *sSrc,  char *sDest, int nSrcLen )  
{  
    int  i;  
    char szTmp[3];  
  
    for( i = 0; i < nSrcLen; i++ )  
    { 
        if(sSrc[i] != 0xFF && sSrc[i+1] != 0xFE)  //0xFF 0xFE 是Unicode字节流的结束标致
        {
            sprintf( szTmp, "%02X", (unsigned char) sSrc[i] );  
            memcpy( &sDest[i * 2], szTmp, 2 );  
        } 
        else 
            break;
    }
    return ;  
}


/* 发送PDU格式短信函数 */
int pdu_sms(Serial_attr attr,char *sms_buf,char *center_number,char *phone_number)
{
    int  str_unicode_len = 0;
    char unic_buf[256] = {0};
    char str_unicode[256] = {0};
    char temp[256] = {0};
    char rbuf[128] = {0};
    char sbuf[128] = {0};

    if(!center_number || !phone_number)
    {
        printf("Invalid input\n");
        return -1;
    }

    /* UTF-8 转 Unicode */
    utf8_to_unicode(sms_buf,unic_buf);

    /* 字节流转字符流 */
    Hex2Str(unic_buf,str_unicode,128);
    sprintf(temp,"%02x%s",strlen(str_unicode)/2,str_unicode);
    strcpy(str_unicode,temp);


    /* 处理电话号码 */
    Handling_center_number(center_number);
    Handling_phone_number(phone_number);

    strcat(phone_number,str_unicode);
    str_unicode_len = strlen(phone_number)/2;

    strcat(center_number,phone_number);
    printf("%d\n%s\n",str_unicode_len,center_number);

    if(comport_send(&attr,"AT+CMGF=0\r",12) < 0)
    {
        printf("\"AT+CMGF=0\" send failed:%s\n",strerror(errno));
        return -2;
    }
 
    usleep(10000);

    /* 若接收不到，或接收到的内容中没有OK,则表示失败 */
    if(comport_recv(&attr,rbuf,sizeof(rbuf),0) <= 0 || !strstr(rbuf,"OK") )
    {
        printf("Recving from serial (AT+CMGF) failed\n");
        return -3;
    }

    sprintf(sbuf,"AT+CMGS=%d\r",str_unicode_len);

    if(comport_send(&attr,sbuf,sizeof(sbuf)) < 0)
    {
        printf("\"AT+CMGS\" send failed.\n");
        return -4;
    }
    usleep(10000);

    if(comport_recv(&attr,rbuf,sizeof(rbuf),0) <= 0 || !strstr(rbuf,">") )
    {
        printf("Recving from serial (AT+CMGS) failed\n");
        return -5;
    }

    /* \x1a 为短信结束标致 */
    strcat(center_number,"\x1a");
    if(comport_send(&attr,center_number,128) < 0)
    {
        printf("Send SMS failed.\n");
        return -6;
    }

    printf("Sending.......\n");
    if(comport_recv(&attr,rbuf,sizeof(rbuf),15) <= 0 || !strstr(rbuf,"OK") )
    {
        printf("%s",rbuf);
        printf("Send over buf can't recieve OK\n");
        return -7;
    }

    printf("Over\n");

    return 0;

}
