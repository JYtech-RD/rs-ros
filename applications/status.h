#ifndef __STATUS_H__
#define __STATUS_H__


#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>



typedef struct {
    
    /*  */
    float position_x;
    float position_y;
    
    /*  */
    float speed_x;
    float speed_y;
    
    /*  */
    float speed_angular;
    float pose_angula;
} info_send_t;

typedef struct {
    float linear_v_x;
    float linear_v_y;
    float angular_v;
    
    float barrier_distance;
} info_recv_t;


typedef struct {
    
    info_send_t info_send;
    info_recv_t info_recv;

    rt_uint8_t barrier;
    

} status_t;


extern status_t status;

#endif


