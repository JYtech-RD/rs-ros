#include "motor_pwm.h"
#include "status.h"
#include "sbus.h"
#include "hostpc.h"

struct rt_device_pwm *pwm_dev;

static void chassis_control(rt_int16_t _x, rt_int16_t _y, rt_int16_t _z);

static rt_thread_t motor_pwm_thread = RT_NULL;


/*----------------------   pwm电机控制线程   --------------------------*/


/* 线程 1 的入口函数 */
static void motor_pwm_thread_entry(void *parameter)
{
    rt_int32_t x = 0;
    rt_int32_t y = 0;
    rt_int32_t w = 0;
    rt_int16_t b = 0;
    
    while (1)
    {
        /* 读公共内存，写的话用互斥量保护起来 */      
        
        x = sbus.ly;
        y = sbus.lx;
        w = sbus.rx;
        b = sbus.sf;
        
        x = (x - SBUS_CH_OFFSET)*8000/SBUS_CH_LENGTH;
        y = (y - SBUS_CH_OFFSET)*8000/SBUS_CH_LENGTH;
        w = (w - SBUS_CH_OFFSET)*8000/SBUS_CH_LENGTH;
        
        
        //rt_kprintf("%d %d %d\n", x, y, w);

        if (b < SBUS_SW_MID) /* 遥控控制 */
        {
            chassis_control(x,y,w);
        }
        else
        {
            chassis_control(status.hostpc_control_ref.x, status.hostpc_control_ref.y, status.hostpc_control_ref.w);
            
            //chassis_control(0,0,0);
        }

        rt_thread_mdelay(50);
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


static void motor_speed_set1(rt_int16_t _p)
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


static void motor_speed_set2(rt_int16_t _p)
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

static void motor_speed_set3(rt_int16_t _p)
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

static void motor_speed_set4(rt_int16_t _p)
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


