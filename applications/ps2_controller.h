#ifndef __PS2_CONTROLLER_H__
#define __PS2_CONTROLLER_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/*------------------------------   驱动移植   -----------------------------*/

#define PS2_GPIO_CLK_ENABLE   do {  __HAL_RCC_GPIOB_CLK_ENABLE(); \
                                    __HAL_RCC_GPIOC_CLK_ENABLE(); } while(0)


#define delay_us(n)     rt_hw_us_delay(n)

/* 时钟线 */
#define CLK_GPIO		GPIOB
#define CLK_PIN			GPIO_PIN_13

/* 数据输入 */
#define DAT_GPIO		GPIOC
#define DAT_PIN			GPIO_PIN_10

/* 数据输出，command，COM */
#define CMD_GPIO		GPIOC
#define CMD_PIN			GPIO_PIN_11

/* CS位，attention，ATT */
#define ATN_GPIO		GPIOC
#define ATN_PIN			GPIO_PIN_12

                                    
                                    
                                    
/*--------------------------    --------------------------------*/                                   
#define     PS2_THREAD_STACK_SIZE       1024
#define     PS2_THREAD_PRIORITY         5               
#define     PS2_THREAD_TIMESLICE        10






/*-------------------------------------------------------------------------*/

#define DI		HAL_GPIO_ReadPin(DAT_GPIO, DAT_PIN)

#define DO_H	HAL_GPIO_WritePin(CMD_GPIO, CMD_PIN, GPIO_PIN_SET)
#define DO_L	HAL_GPIO_WritePin(CMD_GPIO, CMD_PIN, GPIO_PIN_RESET)

#define CS_H 	HAL_GPIO_WritePin(ATN_GPIO, ATN_PIN, GPIO_PIN_SET)
#define CS_L	HAL_GPIO_WritePin(ATN_GPIO, ATN_PIN, GPIO_PIN_RESET)

#define CLK_H HAL_GPIO_WritePin(CLK_GPIO, CLK_PIN, GPIO_PIN_SET)
#define CLK_L HAL_GPIO_WritePin(CLK_GPIO, CLK_PIN, GPIO_PIN_RESET)


//These are our button constants
#define PSB_SELECT      1
#define PSB_L3          2
#define PSB_R3          3
#define PSB_START       4
#define PSB_PAD_UP      5
#define PSB_PAD_RIGHT   6
#define PSB_PAD_DOWN    7
#define PSB_PAD_LEFT    8
#define PSB_L2          9
#define PSB_R2          10
#define PSB_L1          11
#define PSB_R1          12
#define PSB_GREEN       13
#define PSB_RED         14
#define PSB_BLUE        15
#define PSB_PINK        16
#define PSB_TRIANGLE    13
#define PSB_CIRCLE      14
#define PSB_CROSS       15
#define PSB_SQUARE      16

#define PSS_RX 5                //右摇杆X轴数据
#define PSS_RY 6
#define PSS_LX 7
#define PSS_LY 8

typedef struct
{
	char PSB_KEY_SELECT;      
	char PSB_KEY_L3;         
	char PSB_KEY_R3;         
	char PSB_KEY_START;       
	char PSB_KEY_PAD_UP;     
	char PSB_KEY_PAD_RIGH;  
	char PSB_KEY_PAD_DOWN;  
	char PSB_KEY_PAD_LEFT; 
	char PSB_KEY_L2;   
	char PSB_KEY_R2;    
	char PSB_KEY_L1;     
	char PSB_KEY_R1;     
	char PSB_KEY_GREEN;    
	char PSB_KEY_RED;    
	char PSB_KEY_BLUE;   
	char PSB_KEY_PINK;   
	int PSS_BG_RX;
	int PSS_BG_RY;
	int PSS_BG_LX;
	int PSS_BG_LY;
}ps2_controller_t;


extern ps2_controller_t ps2_controller;

void PS2_init(void);

rt_uint8_t ps2_is_red_mode(void);

void ps2_refresh(void);


#endif

