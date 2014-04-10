#include "stm32f10x.h"
#include "lcd.h"
#include "touch.h"
#include "util.h"

#include <stdlib.h>


/*
STM32F103ZET6，ARM Cortex-M3内核，512kB Flash，64KB RAM，LQFP 144脚封装
32位RISC性能处理器
32位ARM Cortex-M3结构优化
72 MHz 运行频率/90MIPS(1.25DMIPS/MHz)
硬件除法和单周期乘法
快速可嵌套中断，6~12个时钟周期
具有MPU保护设定访问规则
//STM32F103ZET6   -> ILI9325

//PE0  -> LED
//PE1  -> RESET

//FSMC_NE1 [PD7 ] -> CS
//FSMC_A16 [PD11] -> RS
//FSMC_NWE [PD5 ] -> WR
//FSMC_NOE [PD4 ] -> RD

//FSMC_D0  [PD14] -> D0
//FSMC_D1  [PD15] -> D1
//FSMC_D2  [PD0 ] -> D2
//FSMC_D3  [PD1 ] -> D3
//FSMC_D4  [PE7 ] -> D4
//FSMC_D5  [PE8 ] -> D5
//FSMC_D6  [PE9 ] -> D6
//FSMC_D7  [PE10] -> D7
//FSMC_D8  [PE11] -> D8
//FSMC_D9  [PE12] -> D9
//FSMC_D10 [PE13] -> D10
//FSMC_D11 [PE14] -> D11
//FSMC_D12 [PE15] -> D12
//FSMC_D13 [PD8 ] -> D13
//FSMC_D14 [PD9 ] -> D14
//FSMC_D15 [PD10] -> D15
*/

//

int main(void)
{
    uint16_t hai[] = {0x0042,0xffc4,0x0048,0x0020,0x7f80,0x4082,0x4484,0x4888,0xffe0,0x4080,0x4448,0x4848,0xffc4,0x2004,0x2002,0x1c02};
    uint16_t feng[] = {0x0208,0x7e08,0x4108,0x22aa,0x1c2a,0x222a,0xc1aa,0x082a,0xffaa,0x082a,0x7f2a,0x082a,0xffaa,0x082a,0x083e,0x0800};

    //printf("Solever Lee");
    SystemInit();
    
    //初始化系统定时器
    SysTick_Config(SystemCoreClock /1000);
    
    //系统初始化
    initGPIO();  //初始化GPIO
    initFSMC();  //初始化FSMC总线
    lcdInit();   //初始化LCD
    lcdClear(COLOR_BLUE);
    lcdSetColor(COLOR_RED, COLOR_BLUE);
    
    lcdDrawCharArr(0, 290, hai, 16, 16);
    lcdDrawCharArr(16, 290, feng, 16, 16);
    sysTickDelay(200);
    initSPI();
    initExti();

    
    //lcdSetColor(COLOR_RED, COLOR_BLUE);
    //main loop
    while (1)
    {
        /*
        lcdClear(COLOR_BLUE);
        itoa(i, s, 10);
        lcdShowString(0, 0, s);
        
        sysTickDelay(1000);	
        i++;
        */
        sysTickDelay(1000);	
    }
}

