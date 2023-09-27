#ifndef __MOTOR_PWM_H__
#define __MOTOR_PWM_H__


#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>





/* pwm电机控制线程 */
#define MOTOR_PWM_PRIORITY         28
#define MOTOR_PWM_STACK_SIZE       1024
#define MOTOR_PWM_TIMESLICE        5




#define PWM_DEV_NAME        "pwm8"  /* PWM设备名称 */


#define MOTOR_LF1_PIN        GET_PIN(B, 15)
#define MOTOR_LF2_PIN        GET_PIN(B, 14)
#define MOTOR_LB1_PIN        GET_PIN(A, 4)
#define MOTOR_LB2_PIN        GET_PIN(A, 5)
#define MOTOR_RF1_PIN        GET_PIN(C, 4)
#define MOTOR_RF2_PIN        GET_PIN(C, 5)
#define MOTOR_RB1_PIN        GET_PIN(B, 12)
#define MOTOR_RB2_PIN        GET_PIN(B, 13)






#endif
