#include "iotask/motor_pwm.h"
#include "iotask/ps2_controller.h"
#include "iotask/hostpc.h"


#include "controltask/pid.h"


#include "status.h"


struct rt_device_pwm *pwm_dev;

static void chassis_control_mode1(float _x, float _w);

static rt_thread_t motor_pwm_thread = RT_NULL;





controller_t mlf, mlb, mrb, mrf;


/*----------------------   pwm电机控制线程   --------------------------*/


/* 线程 1 的入口函数 */
static void motor_pwm_thread_entry(void *parameter)
{
    rt_int32_t x = 0;
    rt_int32_t w = 0;

    float speed_x, speed_w;

    while (1)
    {
        /* 读公共内存，写的话用互斥量保护起来 */
        
        rt_mutex_take(ps2_mutex, RT_WAITING_FOREVER);
        
        if (ps2_is_red_mode()) /* 遥控器启动摇杆模拟量输出 */
        {
            x = ps2_controller.PSS_BG_LY;
            w = ps2_controller.PSS_BG_RX;
        }
        else
        {
            x = 0;
            w = 0;
        }
        
        rt_mutex_release(ps2_mutex);
        
        if (status.mode == AUTOMATIC) /* 自动模式，上位机控制 */
        {
            if (status.barrier <= status.stop_distance) /* 停障功能 距离50cm */
            {
                chassis_control_mode1(0.0, 0.0);
            }
            else
            {
                chassis_control_mode1(status.info_recv.linear_v_x, status.info_recv.angular_v);
            }
        }
        
        else /* 遥控控制 */
        {
            speed_x = ((float)x) / 128.0f * 0.2f;
            speed_w = 4.0f * (((float)w) /  128.0f * 0.3f);
            
            chassis_control_mode1(speed_x, speed_w);
        }
        
        rt_thread_mdelay(25);
    }
}

int motor_thread_init(void)
{
    controller_set_pid_parameter(&mlf, 20000, 0, 0);
    controller_set_pid_parameter(&mlb, 20000, 0, 0);
    controller_set_pid_parameter(&mrb, 20000, 0, 0);
    controller_set_pid_parameter(&mrf, 20000, 0, 0);
    
//    controller_set_output_limit(&mlf, 8000);
//    controller_set_output_limit(&mlb, 8000);
//    controller_set_output_limit(&mrb, 8000);
//    controller_set_output_limit(&mrf, 8000);
    
    
    rt_pin_mode(MOTOR_LF1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(MOTOR_LF2_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(MOTOR_LB1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(MOTOR_LB2_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(MOTOR_RF1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(MOTOR_RF2_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(MOTOR_RB1_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(MOTOR_RB2_PIN, PIN_MODE_OUTPUT);
    
    rt_err_t result = RT_EOK;

    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    
    result = rt_pwm_set(pwm_dev, 1, 1000000, 0);
    result = rt_pwm_set(pwm_dev, 2, 1000000, 0);
    result = rt_pwm_set(pwm_dev, 3, 1000000, 0);
    result = rt_pwm_set(pwm_dev, 4, 1000000, 0);
    
    result = rt_pwm_enable(pwm_dev, 1);
    result = rt_pwm_enable(pwm_dev, 2);
    result = rt_pwm_enable(pwm_dev, 3);
    result = rt_pwm_enable(pwm_dev, 4);
    
    RT_UNUSED(result);


    /* 创建线程 1，名称是 thread1，入口是 thread1_entry*/
    motor_pwm_thread = rt_thread_create("m_pwm",
                            motor_pwm_thread_entry, 
                            RT_NULL,
                            MOTOR_PWM_STACK_SIZE,
                            MOTOR_PWM_PRIORITY, 
                            MOTOR_PWM_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程 */
    if (motor_pwm_thread != RT_NULL)
        rt_thread_startup(motor_pwm_thread);

    return 0;
}

/* 导出到 msh 命令列表中 */
//MSH_CMD_EXPORT(motor_thread_init, motor_thread_init);
INIT_APP_EXPORT(motor_thread_init);


/*---------------    内部功能     ---------------*/

/*--------------------   电机控制引脚、PWM设备初始化  ---------------------*/


static void motor_speed_set1(rt_int32_t _p)
{
    rt_int32_t __p = 0;
    __p = _p;
    
    if (_p >= 0)
    {
        rt_pin_write(MOTOR_LF1_PIN, PIN_HIGH);
        rt_pin_write(MOTOR_LF2_PIN, PIN_LOW);
    }
    else
    {
        rt_pin_write(MOTOR_LF1_PIN, PIN_LOW);
        rt_pin_write(MOTOR_LF2_PIN, PIN_HIGH);
        __p = -__p;
    }
 
    __p = 100*__p;
    
    rt_pwm_set(pwm_dev, 1, 1000000, __p);
}


static void motor_speed_set2(rt_int32_t _p)
{  
    rt_int32_t __p = 0;
    __p = _p;
    
    if (_p >= 0)
    {
        rt_pin_write(MOTOR_LB1_PIN, PIN_HIGH);
        rt_pin_write(MOTOR_LB2_PIN, PIN_LOW);
    }
    else
    {
        rt_pin_write(MOTOR_LB1_PIN, PIN_LOW);
        rt_pin_write(MOTOR_LB2_PIN, PIN_HIGH);
        __p = -__p;
    }
 
    __p = 100*__p;
    
    rt_pwm_set(pwm_dev, 2, 1000000, __p);    
}

static void motor_speed_set3(rt_int32_t _p)
{
    rt_int32_t __p = 0;
    __p = _p;
    
    if (_p >= 0)
    {
        rt_pin_write(MOTOR_RB1_PIN, PIN_HIGH);
        rt_pin_write(MOTOR_RB2_PIN, PIN_LOW);
    }
    else
    {
        rt_pin_write(MOTOR_RB1_PIN, PIN_LOW);
        rt_pin_write(MOTOR_RB2_PIN, PIN_HIGH);
        __p = -__p;
    }
 
    __p = 100*__p;
    
    rt_pwm_set(pwm_dev, 3, 1000000, __p); 
}

static void motor_speed_set4(rt_int32_t _p)
{
    rt_int32_t __p = 0;
    __p = _p;
    
    if (_p >= 0)
    {
        rt_pin_write(MOTOR_RF1_PIN, PIN_HIGH);
        rt_pin_write(MOTOR_RF2_PIN, PIN_LOW);
    }
    else
    {
        rt_pin_write(MOTOR_RF1_PIN, PIN_LOW);
        rt_pin_write(MOTOR_RF2_PIN, PIN_HIGH);
        __p = -__p;
    }
 
    __p = 100*__p;
    
    rt_pwm_set(pwm_dev, 4, 1000000, __p);     
}



/*
    PWM 占空比最大 10000
    _x 最大 0.5
    _w 最大 4.0
*/
static void pwm_limit_v(rt_int32_t *p)
{
    if (*p > 5000)
        *p = 5000;
    
    if (*p < -5000)
        *p = -5000;
    
}

static void pwm_limit_w(rt_int32_t *p)
{
    if (*p > 8000)
        *p = 8000;
    
    if (*p < -8000)
        *p = -8000;
}


static void pwm_limit(rt_int16_t *p)
{
    if (*p > 9000)
        *p = 9000;
    
    if (*p < -9000)
        *p = -9000;
    
}



/*
    最大 _x 1.0 m/s           一般 0.1 ~0.2 m/s 比较合适
    最大 _w 3.0 rad/s         一般 PI/9 rad/s  差不多(20deg/s)
*/
static void chassis_control_mode1(float _x, float _w)
{
    /* 逆运动学，底盘速度计算每个轮子的参考速度 */
    status.chassis.motor_lf.v_ref = _x - _w * 0.23f * 0.5;
    status.chassis.motor_lb.v_ref = _x - _w * 0.23f * 0.5;
    status.chassis.motor_rf.v_ref = _x + _w * 0.23f * 0.5;
    status.chassis.motor_rb.v_ref = _x + _w * 0.23f * 0.5;
    
    status.chassis.motor_lf.pwm = controller_output(&mlf, status.chassis.motor_lf.v_ref, status.chassis.motor_lf.v_feedback);
    status.chassis.motor_lb.pwm = controller_output(&mlb, status.chassis.motor_lb.v_ref, status.chassis.motor_lb.v_feedback);
    status.chassis.motor_rb.pwm = controller_output(&mrb, status.chassis.motor_rb.v_ref, status.chassis.motor_rb.v_feedback);
    status.chassis.motor_rf.pwm = controller_output(&mrf, status.chassis.motor_rf.v_ref, status.chassis.motor_rf.v_feedback);
    
    pwm_limit(&status.chassis.motor_lf.pwm);
    pwm_limit(&status.chassis.motor_lb.pwm);
    pwm_limit(&status.chassis.motor_rb.pwm);
    pwm_limit(&status.chassis.motor_rf.pwm);
    
	motor_speed_set1(status.chassis.motor_lf.pwm);
	motor_speed_set2(status.chassis.motor_lb.pwm);
	motor_speed_set3(status.chassis.motor_rb.pwm);
	motor_speed_set4(status.chassis.motor_rf.pwm);
    
    /*-----------------------------------  开环随便给  ------------------------------------*/
    /* 最大10000 */
//    rt_int16_t _lf = 0;
//    rt_int16_t _lb = 0;    
//    rt_int16_t _rb = 0;
//    rt_int16_t _rf = 0;
//    
//    rt_int32_t x, w;
//    
//    x = (rt_int32_t)(_x * 10000);
//    w = (rt_int32_t)(_w * 0.23 * 30000 / 2);  /* 1/2wL */
//    
//    pwm_limit_v(&x);
//    pwm_limit_w(&w);
//  
//    _lf = x - w;
//    _lb = x - w;
//    _rb = x + w;
//    _rf = x + w;

//    /* 最大传入 10000 */
//	motor_speed_set1(_lf);
//	motor_speed_set2(_lb);
//	motor_speed_set3(_rb);
//	motor_speed_set4(_rf);
    /*-----------------------------------  开环随便给↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑  ---------------------*/
    
    
    
   

}


