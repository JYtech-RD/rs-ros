#include "iotask/ps2_controller.h"
#include "status.h"



ps2_controller_t ps2_controller;

rt_uint16_t Handkey;

rt_uint8_t Comd[2]={0x01,0x42};	//开始命令。请求数据

rt_uint8_t Data[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //数据存储数组

rt_uint16_t MASK[]={
    PSB_SELECT,
    PSB_L3,
    PSB_R3 ,
    PSB_START,
    PSB_PAD_UP,
    PSB_PAD_RIGHT,
    PSB_PAD_DOWN,
    PSB_PAD_LEFT,
    PSB_L2,
    PSB_R2,
    PSB_L1,
    PSB_R1 ,
    PSB_GREEN,
    PSB_RED,
    PSB_BLUE,
    PSB_PINK
	};	     //按键值与按键明

static void ps2_gpio_init(void)
{
    PS2_GPIO_CLK_ENABLE;

    GPIO_InitTypeDef  GPIO_Initstruct;      

    /* 输入引脚配置 */
    GPIO_Initstruct.Mode = GPIO_MODE_INPUT;		//推挽输出
    GPIO_Initstruct.Pull = GPIO_PULLDOWN;			//上拉
    GPIO_Initstruct.Speed = GPIO_SPEED_HIGH;	//高速

    GPIO_Initstruct.Pin = DAT_PIN; 	
    HAL_GPIO_Init(DAT_GPIO,&GPIO_Initstruct);

    /* 输出引脚配置 */
    GPIO_Initstruct.Mode = GPIO_MODE_OUTPUT_PP;	//推挽输出
    GPIO_Initstruct.Pull = GPIO_PULLDOWN;				//上拉
    GPIO_Initstruct.Speed = GPIO_SPEED_HIGH;		//高速

    GPIO_Initstruct.Pin = CLK_PIN; 					
    HAL_GPIO_Init(CLK_GPIO,&GPIO_Initstruct);	

    GPIO_Initstruct.Pin = CMD_PIN; 					
    HAL_GPIO_Init(CMD_GPIO,&GPIO_Initstruct);	

    GPIO_Initstruct.Pin = ATN_PIN; 					
    HAL_GPIO_Init(ATN_GPIO,&GPIO_Initstruct);	
}


//向手柄发送命令
static void PS2_Cmd(rt_uint8_t CMD)
{
	volatile rt_uint16_t ref=0x01;
	Data[1] = 0;
	for(ref=0x01;ref<0x0100;ref<<=1)
	{
		if(ref&CMD)
			DO_H;                   //输出以为控制位

		else 
            DO_L;

		CLK_H;                        //时钟拉高
       
		delay_us(10);
        
		CLK_L;
        
		delay_us(10);
        
		CLK_H;
        
		if(DI)
			Data[1] = ref|Data[1];
	}
}


//读取手柄数据
static void PS2_ReadData(void)
{
	volatile rt_uint8_t byte=0;
	volatile rt_uint16_t ref=0x01;

	CS_L;

	PS2_Cmd(Comd[0]);  //开始命令
	PS2_Cmd(Comd[1]);  //请求数据

	for(byte=2;byte<9;byte++)          //开始接受数据
	{
		for(ref=0x01;ref<0x100;ref<<=1)
		{
			CLK_H;
			delay_us(5);
			CLK_L;
			delay_us(5);
			CLK_H;
		      if(DI)
		      Data[byte] = ref|Data[byte];
				
		}
        delay_us(16);
//		printf("%d\r\n",Data[1]);
	}
	CS_H;	
	
}

//清除数据缓冲区
static void PS2_ClearData()
{
	rt_uint8_t a;
	for(a=0;a<9;a++)
		Data[a]=0x00;
}


/*********手柄配置初始化*********/
//short poll
static void PS2_ShortPoll(void)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x42);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS_H;
	delay_us(16);
}

//进入配置
static void PS2_EnterConfig(void)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x43);
	PS2_Cmd(0x00);
	PS2_Cmd(0x01);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS_H;
	delay_us(16);
}
 //发送模式设置
static void PS2_TurnOnAnologMode(void)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x44);
	PS2_Cmd(0x00);
	PS2_Cmd(0x01);		//0x01:anolog    0x00:digital
	PS2_Cmd(0xEE);		//0x03:锁存设置，不可通过按键“mode"设置模式   0xEE:不锁存设置
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS_H;
	delay_us(16);
}

//完成并保存设置
static void PS2_ExitConfig(void)
{
	CS_L;
	delay_us(16);
	PS2_Cmd(0x01);
	PS2_Cmd(0x43);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);		
	PS2_Cmd(0x5A);		
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	CS_H;
	delay_us(16);
}

//手柄配置初始化
void PS2_init(void)
{
	ps2_gpio_init();
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_EnterConfig();
	PS2_TurnOnAnologMode();
	PS2_ExitConfig();
}

//对读出来的PS2的数据进行处理      只处理了按键部分         默认数据是红灯模式  只有一个按键按下时
//按下为0， 未按下为1
void ps2_refresh(void)
{
	PS2_ClearData();
	PS2_ReadData();
    
	Handkey=(Data[4]<<8)|Data[3];     //这是16个按键  按下为0， 未按下为1
	
	ps2_controller.PSB_KEY_SELECT    = !( (Handkey>>0)  & 1 );
	ps2_controller.PSB_KEY_L3        = !( (Handkey>>1)  & 1 );
	ps2_controller.PSB_KEY_R3        = !( (Handkey>>2)  & 1 );
	ps2_controller.PSB_KEY_START     = !( (Handkey>>3)  & 1 );
	ps2_controller.PSB_KEY_PAD_UP    = !( (Handkey>>4)  & 1 );
	ps2_controller.PSB_KEY_PAD_RIGH  = !( (Handkey>>5)  & 1 );
	ps2_controller.PSB_KEY_PAD_DOWN  = !( (Handkey>>6)  & 1 );
	ps2_controller.PSB_KEY_PAD_LEFT  = !( (Handkey>>7)  & 1 );
	ps2_controller.PSB_KEY_L2        = !( (Handkey>>8)  & 1 );
	ps2_controller.PSB_KEY_R2        = !( (Handkey>>9)  & 1 );
	ps2_controller.PSB_KEY_L1        = !( (Handkey>>10) & 1 );
	ps2_controller.PSB_KEY_R1        = !( (Handkey>>11) & 1 );
	ps2_controller.PSB_KEY_GREEN     = !( (Handkey>>12) & 1 );
	ps2_controller.PSB_KEY_RED       = !( (Handkey>>13) & 1 );
	ps2_controller.PSB_KEY_BLUE      = !( (Handkey>>14) & 1 );
	ps2_controller.PSB_KEY_PINK      = !( (Handkey>>15) & 1 );
	
	ps2_controller.PSS_BG_RX         = -(Data[5]-128);
	ps2_controller.PSS_BG_RY         = -(Data[6]-128);
	ps2_controller.PSS_BG_LX         = (Data[7]-128);
	ps2_controller.PSS_BG_LY         = -(Data[8]-128);
}

//判断是否为红灯模式
//返回值；1，红灯模式
//		  其他，其他模式
rt_uint8_t ps2_is_red_mode(void)
{
	CS_L;
	PS2_Cmd(Comd[0]);  //开始命令
	PS2_Cmd(Comd[1]);  //请求数据
	CS_H;
	if( Data[1] == 0x73)   
		return 1 ;
	return 0;
} 


/*--------------------------  数据处理线程  ---------------------------*/

rt_mutex_t ps2_mutex = RT_NULL;


static void ps2_thread_entry(void *parameter)
{
    PS2_init();

    while (1)
    {
        rt_mutex_take(ps2_mutex, RT_WAITING_FOREVER);
        
        ps2_refresh();
        
        rt_mutex_release(ps2_mutex);
        
        
//        if (ps2_controller.PSB_KEY_START == 1)
//        {
//            rt_hw_cpu_reset();
//        }
        
        if (ps2_controller.PSB_KEY_GREEN == 1)
        {
            status.mode = AUTOMATIC;
        
        }
        else if(ps2_controller.PSB_KEY_BLUE == 1)
        {
            status.mode = REMOTE;
        }
                
//		if(ps2_is_red_mode())
//		{
//            
//            /* 清零 */

//            
//            
//            rt_kprintf("%d %d %d %d\n", ps2_controller.PSB_KEY_PAD_UP, ps2_controller.PSB_KEY_PAD_DOWN, ps2_controller.PSB_KEY_PAD_LEFT, ps2_controller.PSB_KEY_PAD_RIGH);
//            
//            rt_kprintf("%d %d %d %d\n", ps2_controller.PSB_KEY_L1, ps2_controller.PSB_KEY_L2, ps2_controller.PSB_KEY_R1, ps2_controller.PSB_KEY_R2);
//            
//            rt_kprintf("%d %d\n", ps2_controller.PSB_KEY_SELECT, ps2_controller.PSB_KEY_START, ps2_controller.PSB_KEY_L3, ps2_controller.PSB_KEY_R3);
//            
//            rt_kprintf("%d %d %d %d\n", ps2_controller.PSB_KEY_GREEN, ps2_controller.PSB_KEY_RED, ps2_controller.PSB_KEY_BLUE, ps2_controller.PSB_KEY_PINK);
//            
//            rt_kprintf("%3d %3d %3d %3d\n", ps2_controller.PSS_BG_LX, ps2_controller.PSS_BG_LY, ps2_controller.PSS_BG_RX, ps2_controller.PSS_BG_RY);
//            
//            
//		}

        rt_thread_mdelay(50);
    }
}

int ps2_init(void)
{
    rt_err_t ret = RT_EOK;
    
    
    ps2_mutex = rt_mutex_create("psmutex", RT_IPC_FLAG_PRIO);
    

    /* 创建 serial 线程 */
    rt_thread_t thread = rt_thread_create("ps2", 
                                        ps2_thread_entry, 
                                        RT_NULL, 
                                        PS2_THREAD_STACK_SIZE, 
                                        PS2_THREAD_PRIORITY, 
                                        PS2_THREAD_TIMESLICE);
    
    /* 创建成功则启动线程 */
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
INIT_APP_EXPORT(ps2_init);




