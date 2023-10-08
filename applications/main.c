#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>


#define LED0_PIN    GET_PIN(A, 8)

int main(void)
{
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);

    while (1)
    {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(200);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(300);
    }
}
