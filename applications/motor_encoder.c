#include <rtthread.h>
#include <rtdevice.h>

#include "motor_encoder.h"



static rt_int32_t trans(rt_int32_t c, rt_uint8_t m)
{
    rt_int32_t temp;
    
    if (c > 32768/2)
    {
        temp = c - 32768;    
    }
    else
    {
        temp = c;
    }
    
    if (m == 1)
    {
        temp = -temp;
    }

    return temp;
}



static rt_thread_t encoder_thread = RT_NULL;


static void encoder_thread_entry(void *parameter)
{
    rt_err_t ret = RT_EOK;
    
    rt_device_t pulse_encoder_dev1 = RT_NULL;   
    rt_device_t pulse_encoder_dev2 = RT_NULL;   
    rt_device_t pulse_encoder_dev3 = RT_NULL;   
    rt_device_t pulse_encoder_dev4 = RT_NULL;   
    
    
    
    rt_int32_t count;

    /* 查找脉冲编码器设备 */
    pulse_encoder_dev1 = rt_device_find(ENCODER_NAME_LF);
    pulse_encoder_dev2 = rt_device_find(ENCODER_NAME_LB);
    pulse_encoder_dev3 = rt_device_find(ENCODER_NAME_RF);
    pulse_encoder_dev4 = rt_device_find(ENCODER_NAME_RB);

    /* 以只读方式打开设备 */
    ret = rt_device_open(pulse_encoder_dev1, RT_DEVICE_OFLAG_RDONLY);
    ret = rt_device_open(pulse_encoder_dev2, RT_DEVICE_OFLAG_RDONLY);
    ret = rt_device_open(pulse_encoder_dev3, RT_DEVICE_OFLAG_RDONLY);
    ret = rt_device_open(pulse_encoder_dev4, RT_DEVICE_OFLAG_RDONLY);
       
    
    if (ret != RT_EOK)
    {
        rt_kprintf("open encoder device failed!\n");
    }
    
    
    rt_int32_t count_lf, count_lb, count_rf, count_rb;
    
    
    while(1)
    {
        rt_thread_mdelay(500);
        
        
        rt_device_read(pulse_encoder_dev1, 0, &count, 1);
        count_lf = trans(count, 1);
        
        rt_device_read(pulse_encoder_dev2, 0, &count, 1);
        count_lb = trans(count, 1);
        
        rt_device_read(pulse_encoder_dev3, 0, &count, 1);
        count_rf = trans(count, 1);        
        
        rt_device_read(pulse_encoder_dev4, 0, &count, 1);
        count_rb = trans(count, 1);

        
        /* 读取脉冲编码器计数值 */
        rt_kprintf("%6d ", count_lf);
        rt_kprintf("%6d ", count_lb);
        rt_kprintf("%6d ", count_rf);        
        rt_kprintf("%6d \n", count_rb);
    }
}

int encoder_thread_init(void)
{
    rt_err_t result = RT_EOK;

    /* 创建线程 1，名称是 thread1，入口是 thread1_entry*/
    encoder_thread = rt_thread_create("encoder",
                            encoder_thread_entry, 
                            RT_NULL,
                            ENCODER_STACK_SIZE,
                            ENCODER_PRIORITY, 
                            ENCODER_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程 */
    if (encoder_thread != RT_NULL)
        rt_thread_startup(encoder_thread);

    return 0;
}

INIT_APP_EXPORT(encoder_thread_init);


