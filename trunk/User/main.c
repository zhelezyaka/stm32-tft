#include "stm32f10x.h"
#include "lcd.h"
#include "util.h"


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

int main(void)
{
    SystemInit();
    
    //初始化系统定时器
    SysTick_Config(SystemCoreClock /1000);
    
    //系统初始化
    initGPIO();  //初始化GPIO
    initFSMC();  //初始化FSMC总线
    lcdInit();   //初始化LCD
    lcdShowString(0, 0, "Hello World! My Name is LiHaifeng , I am glad to meet you o_O");
    //main loop
    while (1)
    {
        //lcdClear(COLOR_RED);
        //sysTickDelay(150);	
        //lcdClear(COLOR_BLUE);
        //sysTickDelay(150);	
    }
}

