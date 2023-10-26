#ifndef __PID_H__
#define __PID_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


#define     LPF_pi	        3.141593
#define     LPF_fc	        1
#define     LPF_Ts	        0.005

#define     LPF_alpha       ((2.0*LPF_pi*LPF_fc*LPF_Ts)/(2.0*LPF_pi*LPF_fc*LPF_Ts+1.0))


typedef struct 
{
    float ref;
    float feedback;
    float output;

    float kp;
    float ki;
    float kd;

    float out_lim;

    float int_lim;
    float int_boundary;

	float err_lpf;
	float err_lpf1;
    float err;
    float err1;
    float err_sum;
} controller_t;


float controller_output(controller_t* controller, float ref, float feedback);

void controller_set_pid_parameter(controller_t* controller, float kp, float ki, float kd);

void controller_set_output_limit(controller_t* controller, float limit);


#endif
