#include "motor_pwm.h"
#include "status.h"
#include "sbus.h"
#include "hostpc.h"

struct rt_device_pwm *pwm_dev;

static void chassis_control(rt_int16_t _x, rt_int16_t _y, rt_int16_t _z);

static void chassis_control_mode1(float _x, float _w);

static rt_thread_t motor_pwm_thread = RT_NULL;


/*----------------------   pwm电机控制线程   --------------------------*/


/* 线程 1 的入口函数 */
static void motor_pwm_thread_entry(void *parameter)
{
    RT_UNUSED(chassis_control);
    
    rt_int32_t x = 0;
    rt_int32_t w = 0;
    rt_int16_t a = 0;
    
    float speed_x, speed_w;
    
    sbus.sa = SBUS_CH_MIN;
    
    while (1)
    {
        /* 读公共内存，写的话用互斥量保护起来 */      
        x = sbus.ly;
        w = sbus.rx;
        a = sbus.sa;


        if (a < SBUS_SW_MID) /* SA 拨在上面，上位机控制 */
        {
            if (status.barrier <= 30) /* 停障功能 距离30cm */
            {
                chassis_control_mode1(0.0, 0.0);
            }
            else
            {
                chassis_control_mode1(status.info_recv.linear_v_x, status.info_recv.angular_v);
            }
        }
        
        else if (a > SBUS_SW_MID) /* SA拨在下面，遥控控制 */
        {
            speed_x = (x - SBUS_CH_OFFSET)/((float)SBUS_CH_LENGTH);
            speed_w = (w - SBUS_CH_OFFSET)/((float)SBUS_CH_LENGTH);
            
            chassis_control_mode1(speed_x, speed_w);
        }
        
        else    /* SA拨在中间，紧急停止 */
        {
            chassis_control_mode1(0.0, 0.0);
        }
 


        
        rt_thread_mdelay(25);
    }
}

int motor_thread_init(void)
{
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

static void chassis_control(rt_int16_t _x, rt_int16_t _y, rt_int16_t _z)
{
    rt_int16_t _lf = 0;
    rt_int16_t _lb = 0;    
    rt_int16_t _rb = 0;
    rt_int16_t _rf = 0;
    
    _lf = _x - _y - _z;
    _lb = _x + _y - _z;
    _rb = _x + _y + _z;
    _rf = _x - _y + _z;
    
	motor_speed_set1(_lf);
	motor_speed_set2(_lb);
	motor_speed_set3(_rb);
	motor_speed_set4(_rf);
}


/*
    _x 最大 0.5
    _w 最大 4.0
*/
static void limit_v(rt_int32_t *p)
{
    if (*p > 6000)
        *p = 6000;
    
    if (*p < -6000)
        *p = -6000;
    
}

static void limit_w(rt_int32_t *p)
{
    if (*p > 30000)
        *p = 30000;
    
    if (*p < -30000)
        *p = -30000;
}

static void chassis_control_mode1(float _x, float _w)
{
    rt_int16_t _lf = 0;
    rt_int16_t _lb = 0;    
    rt_int16_t _rb = 0;
    rt_int16_t _rf = 0;
    
    rt_int32_t x, w;
    
    x = (rt_int32_t)(_x * 10000);
    w = (rt_int32_t)(_w * 30000);
    
    
    limit_v(&x);
    limit_w(&w);
//    if (x > 8000)
//        x = 8000;
//    if (w > 8000)
//        w = 8000;    
    
    _lf = x - w*0.23/2;
    _lb = x - w*0.23/2;
    _rb = x + w*0.23/2;
    _rf = x + w*0.23/2;
    
    /* 限速 */
//    if (_lf > 8000)
//        _lf = 8000;
//    if (_lb > 8000)
//        _lb = 8000;    
//    if (_rf > 8000)
//        _rf = 8000;
//    if (_rb > 8000)
//        _rb = 8000;  

    
    /* 最大传入 10000 */
	motor_speed_set1(_lf);
	motor_speed_set2(_lb);
	motor_speed_set3(_rb);
	motor_speed_set4(_rf);
}


