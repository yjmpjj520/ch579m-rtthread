
#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include "board.h"

#ifdef RT_USING_SERIAL
#include "drv_uart.h"
#endif

#ifdef RT_USING_FINSH
#include <finsh.h>
static void reboot(uint8_t argc, char **argv)
{
    rt_hw_cpu_reset();
}
MSH_CMD_EXPORT(reboot, Reboot System)
#endif /* RT_USING_FINSH */

void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

void rt_hw_board_init()
{
    
    PWR_UnitModCfg(ENABLE, UNIT_SYS_PLL);
    
    SetSysClock(CLK_SOURCE_PLL_40MHz);
    SysTick_Config(GetSysClock() / RT_TICK_PER_SECOND);
   
    NVIC_SetPriority(SysTick_IRQn, 0);
    NVIC_EnableIRQ(SysTick_IRQn);

#if defined(RT_USING_HEAP)
    rt_system_heap_init((void *)HEAP_BEGIN, (void *)HEAP_END);
#endif

#ifdef RT_USING_SERIAL
    rt_hw_uart_init();
#endif

#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

}
