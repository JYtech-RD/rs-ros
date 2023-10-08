#include "motor_encoder.h"
#include "status.h"

#include "math.h"

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
    rt_int32_t countsum_lf, countsum_lb, countsum_rf, countsum_rb;
    
    float distance_lf, distance_lb, distance_rf, distance_rb;
    
    float v_lf, v_lb, v_rf, v_rb;
    
    float v_l, v_r;
    
    while(1)
    {
        /*
            1. 读编码器原始值
            2. 转换成 4 个轮子的移动距离，并计算 4 个轮子的速度
            3. 计算累计位置，合成底盘中心点的速度
        */
        
        /* 读编码器原始值 */
        rt_device_read(pulse_encoder_dev1, 0, &count, 1);
        count_lf = trans(count, 1);
        
        rt_device_read(pulse_encoder_dev2, 0, &count, 1);
        count_lb = trans(count, 0);
        
        rt_device_read(pulse_encoder_dev3, 0, &count, 1);
        count_rf = trans(count, 1);        
        
        rt_device_read(pulse_encoder_dev4, 0, &count, 1);
        count_rb = trans(count, 1);
        
        countsum_lf += count_lf;
        countsum_lb += count_lb;
        countsum_rf += count_rf;
        countsum_rb += count_rb;
        
//        rt_kprintf("%6d ", count_lf);
//        rt_kprintf("%6d ", count_lb);
//        rt_kprintf("%6d ", count_rf);        
//        rt_kprintf("%6d \n", count_rb);        
        
        
        /* 计算每个轮子移动的距离 */
        #define PI  3.141593f
        #define N   2460.0f         /* 转一圈的编码器计数值 */
        #define D   0.095           /* 车轮直径 9.5 cm */
        
        distance_lf = ((float)count_lf)*PI*D/N;
        distance_lb = ((float)count_lb)*PI*D/N; 
        distance_rf = ((float)count_rf)*PI*D/N; 
        distance_rb = ((float)count_rb)*PI*D/N;
        

        
//        rt_kprintf("%6d mm", (int)(distance_lf*1000));
//        rt_kprintf("%6d mm", (int)(distance_lb*1000));
//        rt_kprintf("%6d mm", (int)(distance_rf*1000));        
//        rt_kprintf("%6d \n", (int)(distance_rb*1000));

        
        /* 计算每个轮子的速度 */
        #define DELAY_TIME      0.1f /* s = 100ms */
        
        v_lf = distance_lf / DELAY_TIME;
        v_lb = distance_lb / DELAY_TIME;
        v_rf = distance_rf / DELAY_TIME;
        v_rb = distance_rb / DELAY_TIME;
        
//        rt_kprintf("%6d mm/s", (int)(v_lf*1000));
//        rt_kprintf("%6d mm/s", (int)(v_lb*1000));
//        rt_kprintf("%6d mm/s", (int)(v_rf*1000));        
//        rt_kprintf("%6d mm/s\n", (int)(v_rb*1000));

        
        /* 里程计数据解算 */
        #define     L   0.23    /* 左右轮间距 */
        
        v_l = (v_lf + v_lb)/2.0f;
        v_r = (v_rf + v_rb)/2.0f;

        status.info_send.speed_x = (v_l + v_r) / 2.0f;
        status.info_send.speed_y = 0.0f;
        status.info_send.speed_angular = (v_l - v_r) / L;

        status.info_send.pose_angula += status.info_send.speed_angular*DELAY_TIME;
        status.info_send.position_x  += status.info_send.speed_x*cosf(status.info_send.pose_angula)*DELAY_TIME;
        status.info_send.position_y  += status.info_send.speed_x*sinf(status.info_send.pose_angula)*DELAY_TIME;

//        rt_kprintf("\x1b[2J\x1b[H");
//        rt_kprintf("sx %6d mm/s\n", (int)(status.info_send.speed_x*1000));
//        rt_kprintf("sa %6d rad/s\n", (int)(status.info_send.speed_angular*1000));
//        rt_kprintf("pa %6d rad\n", (int)(status.info_send.pose_angula*1000));        
//        rt_kprintf("px %6d mm\n", (int)(status.info_send.position_x*1000));        
//        rt_kprintf("py %6d mm\n", (int)(status.info_send.position_y*1000));

        rt_device_control(pulse_encoder_dev1, PULSE_ENCODER_CMD_CLEAR_COUNT, RT_NULL);
        rt_device_control(pulse_encoder_dev2, PULSE_ENCODER_CMD_CLEAR_COUNT, RT_NULL);
        rt_device_control(pulse_encoder_dev3, PULSE_ENCODER_CMD_CLEAR_COUNT, RT_NULL);
        rt_device_control(pulse_encoder_dev4, PULSE_ENCODER_CMD_CLEAR_COUNT, RT_NULL);
        
        rt_thread_mdelay(100);
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

    return result;
}

INIT_APP_EXPORT(encoder_thread_init);


