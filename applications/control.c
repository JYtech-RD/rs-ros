#include "control.h"
#include "pid.h"

#include "status.h"

/*--------------------------  遥控器控制  ---------------------------*/

static void motion_thread_entry(void *parameter)
{
	rt_thread_mdelay(5000);
    
    /* 偏转控制器 */
    controller_t mlf, mlb, mrf, mrb;
    
    controller_set_pid_parameter(&mlf, 120, 0, 0);
    controller_set_pid_parameter(&mlb, 120, 0, 0);
    controller_set_pid_parameter(&mrf, 120, 0, 0);
    controller_set_pid_parameter(&mrb, 120, 0, 0);
    
    controller_set_output_limit(&mlf, 7000);
    controller_set_output_limit(&mlb, 7000);
    controller_set_output_limit(&mrf, 7000);
    controller_set_output_limit(&mrb, 7000);
    
    while (1)
    {
        status.chassis.motor_lf.pwm = controller_output(&mlf, status.chassis.motor_lf.v_ref, status.chassis.motor_lf.v_feedback);
        status.chassis.motor_lb.pwm = controller_output(&mlb, status.chassis.motor_lb.v_ref, status.chassis.motor_lb.v_feedback);
        status.chassis.motor_rf.pwm = controller_output(&mrf, status.chassis.motor_rf.v_ref, status.chassis.motor_rf.v_feedback);
        status.chassis.motor_rb.pwm = controller_output(&mrb, status.chassis.motor_rb.v_ref, status.chassis.motor_rb.v_feedback);
        
        
        
        
		rt_thread_mdelay(25);
    }
	
}

int motion_init(void)
{
    /* 创建 motion 线程 */
    rt_thread_t thread = rt_thread_create("motion", 
                                        motion_thread_entry, 
                                        RT_NULL, 
                                        MOTION_THREAD_STACK_SIZE, 
                                        MOTION_THREAD_PRIORITY, 
										MOTION_THREAD_TIMESLICE);
    
    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
	
	return 0;
}

/* 导出命令 or 自动初始化 */
INIT_APP_EXPORT(motion_init);


