#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "status.h"

#define LED0_PIN    GET_PIN(A, 8)

int main(void)
{
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

    while (1)
    {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(200);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(800);
        
//        rt_kprintf("\x1b[2J\x1b[H");
//        rt_kprintf("sx %6d mm/s\n", (int)(status.info_send.speed_x*1000));
//        rt_kprintf("sa %6d rad/s\n", (int)(status.info_send.speed_angular*1000));
//        rt_kprintf("pa %6d rad\n", (int)(status.info_send.pose_angula*1000));
//        rt_kprintf("px %6d mm\n", (int)(status.info_send.position_x*1000));
//        rt_kprintf("py %6d mm\n", (int)(status.info_send.position_y*1000));
    }
}
