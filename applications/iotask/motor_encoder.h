#ifndef __MOTOR_ENCODER_H
#define __MOTOR_ENCODER_H


#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define ENCODER_NAME_LF     "pulse3"
#define ENCODER_NAME_LB     "pulse2"    /* 脉冲编码器名称 */
#define ENCODER_NAME_RF     "pulse4"    /* 脉冲编码器名称 */
#define ENCODER_NAME_RB     "pulse5"    /* 脉冲编码器名称 */



#define     ENCODER_STACK_SIZE      1024
#define     ENCODER_PRIORITY        5
#define     ENCODER_TIMESLICE       10






#endif

