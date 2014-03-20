#include "stm32f10x.h"
#include "util.h"


//延时ms毫秒
volatile u16 _timerDelay;
void sysTickDelay(u16 ms)
{
	_timerDelay = ms;
    
    //参考 stm32f10x_it.c SysTick_Handler
	while(_timerDelay);
}

