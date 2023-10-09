#ifndef __HOSTPC_H__
#define __HOSTPC_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


#define HOSTPC_UART       "uart3"      /* 串口设备名称 */


typedef union{
    rt_uint8_t  cvalue[4];
	float       fvalue;
}float_union;


typedef struct {
    float_union linear_v_x;
    float_union linear_v_y;
    float_union angular_v;
    
    float_union barrier_distance;
} recv_t;

typedef struct {
    float_union position_x;
    float_union position_y;
    float_union speed_x;
    float_union speed_y;
    float_union speed_angular;
    float_union pose_angula;
    
} send_t;


typedef struct {
    
    send_t send;
    recv_t recv;
    
    rt_uint8_t barrier;
    
} hostpc_t;



extern hostpc_t hostpc;



#endif


