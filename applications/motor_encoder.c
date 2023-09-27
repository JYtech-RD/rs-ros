#include <rtthread.h>
#include <rtdevice.h>

#include "motor_encoder.h"



static int pulse_encoder_init(void)
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
        return ret;
    }
    
    while(1)
    {
        rt_thread_mdelay(500);
        
        /* 读取脉冲编码器计数值 */
        rt_device_read(pulse_encoder_dev1, 0, &count, 1);
        rt_kprintf("%6d ",count);
        rt_device_read(pulse_encoder_dev2, 0, &count, 1);
        rt_kprintf("%6d ",count);
        rt_device_read(pulse_encoder_dev3, 0, &count, 1);
        rt_kprintf("%6d ",count);        
        rt_device_read(pulse_encoder_dev4, 0, &count, 1);
        rt_kprintf("%6d \n",count);
    }
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(pulse_encoder_init, pulse encoder sample);





