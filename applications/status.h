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





typedef struct {
    
    chassis_speed_ref_t chassis_speed_ref;
    
    rt_uint8_t barrier;
    

} status_t;


extern status_t status;

#endif


