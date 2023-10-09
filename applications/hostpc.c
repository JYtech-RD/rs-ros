#include "hostpc.h"
#include "status.h"

hostpc_t hostpc;




/* ----------------------- Data Struct ------------------------------------- */
/* 串口接收消息结构*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};


/*--------------------------  控制块  ---------------------------*/

/* 串口设备句柄 */
static rt_device_t serial;

/* 消息队列控制块 */
static struct rt_messagequeue rx_mq;


/*--------------------------  DMA中断回调  ---------------------------*/

/* 接收数据回调函数 */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    rt_err_t result;
    msg.dev = dev;
    msg.size = size;

    result = rt_mq_send(&rx_mq, &msg, sizeof(msg));
    if ( result == -RT_EFULL)
    {
        /* 消息队列满 */
        rt_kprintf("message queue full！\n");
    }
    return result;
}

/*--------------------------  数据处理线程  ---------------------------*/


static void hostpc_uart_thread_entry(void *parameter)
{
    struct rx_msg msg;
    rt_err_t result;
    rt_uint32_t rx_length;
    
    rt_uint8_t rx_buffer[RT_SERIAL_RB_BUFSZ + 1];
    rt_uint8_t* byte = rx_buffer;                   /* 缓冲区别名 */
    
    rt_uint8_t data_count = 0;
    
    rt_uint32_t distance;
    
    while (1)
    {
        rt_memset(&msg, 0, sizeof(msg));
        
        /* 从消息队列中读取消息*/
        result = rt_mq_recv(&rx_mq, &msg, sizeof(msg), RT_WAITING_FOREVER);
        if (result == RT_EOK)
        {
            /* 从串口读取数据*/
            rx_length = rt_device_read(msg.dev, 0, &rx_buffer[data_count], msg.size);
            data_count = data_count + rx_length;
                       
            
            /*  FF FF XL XH YL YH WL WH DIS SUM   */
            
            /* 可以使用状态机解包 */
            if ( (byte[0] == 0xFF) && (byte[1] == 0xFF) )
            {
                /* 满足帧头才判断帧长 */
                if (data_count >= 19) 
                {
                    hostpc.recv.linear_v_x.cvalue[0] = byte[2];
                    hostpc.recv.linear_v_x.cvalue[1] = byte[3];
                    hostpc.recv.linear_v_x.cvalue[2] = byte[4];
                    hostpc.recv.linear_v_x.cvalue[3] = byte[5];
                    
                    hostpc.recv.linear_v_y.cvalue[0] = byte[6];
                    hostpc.recv.linear_v_y.cvalue[1] = byte[7];
                    hostpc.recv.linear_v_y.cvalue[2] = byte[8];
                    hostpc.recv.linear_v_y.cvalue[3] = byte[9];                    

                    hostpc.recv.angular_v.cvalue[0] = byte[10];
                    hostpc.recv.angular_v.cvalue[1] = byte[11];
                    hostpc.recv.angular_v.cvalue[2] = byte[12];
                    hostpc.recv.angular_v.cvalue[3] = byte[13];
                    
                    hostpc.recv.barrier_distance.cvalue[0] = byte[14];
                    hostpc.recv.barrier_distance.cvalue[1] = byte[15];
                    hostpc.recv.barrier_distance.cvalue[2] = byte[16];
                    hostpc.recv.barrier_distance.cvalue[3] = byte[17];
                    
                    status.info_recv.linear_v_x         = hostpc.recv.linear_v_x.fvalue;
                    status.info_recv.linear_v_y         = hostpc.recv.linear_v_y.fvalue;
                    status.info_recv.angular_v          = hostpc.recv.angular_v.fvalue;
                    status.info_recv.barrier_distance   = hostpc.recv.barrier_distance.fvalue;
                    
                    distance = (rt_uint32_t)(status.info_recv.barrier_distance*100); /* 障碍物距离转换为cm */
                    
                    if (distance > 200)
                        distance = 200;
                    
                    status.barrier = distance;
                    
                    //rt_kprintf("%d cm  %d cm\n", distance, status.barrier);
                    
                    
                    /* 准备下一数据帧接收 */
                    data_count = 0;                
                }
            }
            else /*帧头不对，重新接收等待帧头*/
            {
                data_count = 0;
            }
        }
    }
}



/* 线程 1 的入口函数 */
static void hostpc_send_thread_entry(void *parameter)
{
    rt_uint8_t send_buf[27];
    
    rt_uint8_t *buffer = send_buf;
    
    send_buf[0] = 0xFF;
    send_buf[1] = 0xFF;
    
//    hostpc.send.speed_x.fvalue = 1.0;
//    hostpc.send.speed_y.fvalue = 2.0;
    
    while (1)
    {
        hostpc.send.position_x.fvalue   = status.info_send.position_x;
        hostpc.send.position_y.fvalue   = status.info_send.position_y;
        hostpc.send.speed_x.fvalue      = status.info_send.speed_x;
        hostpc.send.speed_y.fvalue      = status.info_send.speed_y;
        hostpc.send.speed_angular.fvalue= status.info_send.speed_angular;
        hostpc.send.pose_angula.fvalue  = status.info_send.pose_angula;
        
        
        send_buf[2] = hostpc.send.position_x.cvalue[0];
        send_buf[3] = hostpc.send.position_x.cvalue[1];
        send_buf[4] = hostpc.send.position_x.cvalue[2];
        send_buf[5] = hostpc.send.position_x.cvalue[3];
        
        send_buf[6] = hostpc.send.position_y.cvalue[0];
        send_buf[7] = hostpc.send.position_y.cvalue[1];
        send_buf[8] = hostpc.send.position_y.cvalue[2];
        send_buf[9] = hostpc.send.position_y.cvalue[3];        
        
        send_buf[10] = hostpc.send.speed_x.cvalue[0];
        send_buf[11] = hostpc.send.speed_x.cvalue[1];
        send_buf[12] = hostpc.send.speed_x.cvalue[2];
        send_buf[13] = hostpc.send.speed_x.cvalue[3];        
                
        send_buf[14] = hostpc.send.speed_y.cvalue[0];
        send_buf[15] = hostpc.send.speed_y.cvalue[1];
        send_buf[16] = hostpc.send.speed_y.cvalue[2];
        send_buf[17] = hostpc.send.speed_y.cvalue[3];         
        
        send_buf[18] = hostpc.send.speed_angular.cvalue[0];
        send_buf[19] = hostpc.send.speed_angular.cvalue[1];
        send_buf[20] = hostpc.send.speed_angular.cvalue[2];
        send_buf[21] = hostpc.send.speed_angular.cvalue[3]; 
        
        send_buf[22] = hostpc.send.pose_angula.cvalue[0];
        send_buf[23] = hostpc.send.pose_angula.cvalue[1];
        send_buf[24] = hostpc.send.pose_angula.cvalue[2];
        send_buf[25] = hostpc.send.pose_angula.cvalue[3];         
        
        send_buf[26] =  buffer[2]^buffer[3]^buffer[4]^buffer[5]^buffer[6]^buffer[7]^
                        buffer[8]^buffer[9]^buffer[10]^buffer[11]^buffer[12]^buffer[13]^
                        buffer[14]^buffer[15]^buffer[16]^buffer[17]^buffer[18]^buffer[19]^
                        buffer[20]^buffer[21]^buffer[22]^buffer[23]^buffer[24]^buffer[25];
        
        rt_device_write(serial, 0, send_buf, 27);

        rt_thread_mdelay(100);
    }
}




int dr16_init(void)
{
    rt_err_t ret = RT_EOK;
    static char msg_pool[256];

    /* 查找串口设备 */
    serial = rt_device_find(HOSTPC_UART);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", HOSTPC_UART);
        return RT_ERROR;
    }

    /* 初始化消息队列 */
    rt_mq_init(&rx_mq, "mq_pc",
               msg_pool,                 /* 存放消息的缓冲区 */
               sizeof(struct rx_msg),    /* 一条消息的最大长度 */
               sizeof(msg_pool),         /* 存放消息的缓冲区大小 */
               RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

    
//    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);      
    
    /* 以 DMA 接收及轮询发送方式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_DMA_RX);
    
               /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_input);

    /* 创建 serial 线程 */
    rt_thread_t thread = rt_thread_create("pcrecv", hostpc_uart_thread_entry, RT_NULL, 1024, 25, 10);
               
    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
    }
    
    thread = rt_thread_create("pcsend", hostpc_send_thread_entry, RT_NULL, 1024, 25, 10);
    
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
    }    
    
    return ret;
}

/* 导出命令 or 自动初始化 */
//MSH_CMD_EXPORT(dr16_init, dr16 remote controller init);
INIT_APP_EXPORT(dr16_init);


/*--------------------------  调试输出线程  ---------------------------*/

static void dr16_debug_thread_entry(void *parameter)
{
    while (1)
    {
//        rt_kprintf("%4d %4d\n", dr16.rc.ch3, dr16.rc.ch1);
//        rt_kprintf("%4d %4d\n", dr16.rc.ch2, dr16.rc.ch0);
//        rt_kprintf("%4d %4d\n", dr16.rc.s1, dr16.rc.s2);
        
//        rt_kprintf("x:%d \n", dr16.mouse.x);
//        rt_kprintf("y:%d \n", dr16.mouse.y);
//        rt_kprintf("z:%d \n", dr16.mouse.z);
//        rt_kprintf("%d \n", dr16.mouse.press_l);
//        rt_kprintf("%d \n", dr16.mouse.press_r);
//        rt_kprintf("%d \n", dr16.key.v);
        
        rt_kprintf("---------------\n");
        
        rt_thread_mdelay(500);
    }
}

static int dr16_output(int argc, char *argv[])
{
    rt_thread_t thread = rt_thread_create("dr16_output", dr16_debug_thread_entry, RT_NULL, 1024, 25, 10);
    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }

    return 0;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(dr16_output, remote controller data debug outputuart);

