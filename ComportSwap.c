/*********************************************************************************
 *      Copyright:  (C) 2020 LuXiaoyang<920916829@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  ComportSwap.c
 *    Description:  This file is swap message with ttyUSBx
 *                 
 *        Version:  1.0.0(01/07/20)
 *         Author:  LuXiaoyang <920916829@qq.com>
 *      ChangeLog:  1, Release initial version on "01/07/20 09:07:28"
 *                 
 ********************************************************************************/
#include "ComportSwap.h"

int comport_send(struct _st_mystructure *mystructure, char *buf, int buf_size) 
{
    char    *ptr, *end;
    int     retval = -1;

    if((!buf) || (buf_size <= 0))
    {
        printf("%s Invailed parameter!\n", __FUNCTION__) ;
        goto cleanup ;
    }


    if (mystructure->frag_size < buf_size)
    {
        ptr = buf;
        end = buf + buf_size;
        do
        {
            if(mystructure->frag_size < (end - ptr))
            {
                retval = write(mystructure->fd, ptr, mystructure->frag_size);
                if (0 >= retval || mystructure->frag_size != retval)
                {
                    goto cleanup;
                }
                ptr += mystructure->frag_size;
            }
            else
            {
                retval = write(mystructure->fd, ptr, (end - ptr));
                if (0 >= retval || (end - ptr) != retval)
                {
                    goto cleanup;
                }

                ptr += (end - ptr);
            }
        }while(ptr < end);
    }
    else{

        retval = write(mystructure->fd, buf, buf_size);
        if (0 >= retval || buf_size != retval)
        {
            goto cleanup;
        }
}

cleanup:

    return retval;

}


int comport_recv(struct _st_mystructure *mystructure, char *buf, int buf_size, int timeout)
{
    fd_set                  rset;
    struct timeval          tim_out; 
    int                     rv;

    if((!buf) || (buf_size <= 0))
    {
        printf("Invaild input\n") ;
        return -1 ;
    }

    if(timeout) 
    {
        tim_out.tv_sec = (time_t) (timeout / 1000);
        tim_out.tv_usec = 0;
        FD_ZERO(&rset);

        FD_SET(mystructure->fd, &rset);
        rv = select(mystructure->fd+1, &rset, NULL, NULL, &tim_out) ;
        if(rv < 0) 
        {
            printf("select() failure: %s\n", strerror(errno)) ;
            return -2;
        }
        if( rv== 0) 
        {
            printf("recv timeval\n");
            return 0;
        }
    }

    usleep(10000); 

    rv = read(mystructure->fd, buf, buf_size) ;
    if(rv <= 0)
    {
        printf("Read from buf error or disconnect:%s\n", strerror(errno)) ;
        return -5 ;
    }

    return rv ;

}




