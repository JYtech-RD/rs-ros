#ifndef __STATUS_H__
#define __STATUS_H__


#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/* 参考速度 */
typedef struct {
    rt_int16_t x;
    rt_int16_t y;
    rt_int16_t w;  
} chassis_speed_ref_t;


typedef enum {
    REMOTE_CONTROL = 0,
    HOSTPC_CONTROL,

} control_mode_t;


typedef struct {
    
    control_mode_t control_mode;    
    
    chassis_speed_ref_t remote_control_ref;
    
    chassis_speed_ref_t hostpc_control_ref;
    
    
    
    
    
    rt_uint8_t barrier;
    

} status_t;


extern status_t status;

#endif


