#ifndef __HOSTPC_H__
#define __HOSTPC_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


#define HOSTPC_UART       "uart3"      /* 串口设备名称 */





typedef struct
{
    struct
    {
        rt_int16_t  ch0;
        rt_int16_t  ch1;
        rt_int16_t  ch2;
        rt_int16_t  ch3;
        rt_int8_t   s1;
        rt_int8_t   s2;
    }rc;
    struct
    {
        int16_t x;
        int16_t y;
        int16_t z;
        uint8_t press_l;
        uint8_t press_r;
    }mouse;
    struct
    {
        uint16_t v;
    }key;
}controller_t;



extern controller_t dr16;





#endif



