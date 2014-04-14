#include "stm32f10x.h"
#include "ili9325.h"
#include "xpt2406.h"
#include "util.h"
#include "reg.h"

#include <stdlib.h>


/*
STM32F103ZET6，ARM Cortex-M3内核，512kB Flash，64KB RAM，LQFP 144脚封装
32位RISC性能处理器
32位ARM Cortex-M3结构优化
72 MHz 运行频率/90MIPS(1.25DMIPS/MHz)
硬件除法和单周期乘法
快速可嵌套中断，6~12个时钟周期
具有MPU保护设定访问规则
*/

void Led_GPIO_Configuration(void);
void Delay(uint32_t nCount);
int main(void)
{
    SystemInit();
    SysTick_Config(SystemCoreClock /1000);
  
    ili9325_lcdInit();
    lcdLightOn();
    lcdClear(COLOR_YELLOW);
    lcdDrawCircle(50, 50, 20);
    lcdDisplayChar(100, 100, 'R');
    lcdDrawUniLine(0, 0, 240, 320);
    lcdDisplayStringLine(1, "Hello!");
    while (1)
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_13);
        sysTickDelay(100);
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        sysTickDelay(100);
        sysTickDelay(100);
    }
}

void Led_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 						 
//=============================================================================
//LED -> PC13
//=============================================================================			 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);

}

