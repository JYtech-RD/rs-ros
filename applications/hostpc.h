#ifndef __HOSTPC_H__
#define __HOSTPC_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


#define HOSTPC_UART       "uart3"      /* 串口设备名称 */


typedef struct {
    rt_int16_t x;
    rt_int16_t y;
    rt_int16_t w;
    
    rt_int16_t distance;
    rt_uint8_t mode;
} hostpc_t;



extern hostpc_t hostpc;





#endif



