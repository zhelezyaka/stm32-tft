#include "stm32f10x.h"
#include "ili9325.h"
#include "xpt2046.h"
#include "util.h"
#include "reg.h"
#include "touchscreen.h"
#include "hw_config.h"
#include "cursor.h"
#include "graphicObject.h"
#include "menu.h"
#include "pictures.h"


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

/*
在触摸屏数据与其位置偏移关系且屏幕像素与其位置偏移关系同为线性关系假设情况下，触摸屏返回的位置信息与像素位置信息之间成2D坐标变换关系。则对于触摸屏按下点的触摸屏坐标(Tx,Ty)与其在显示设备位置关系上匹配的点的屏幕坐标(Sx,Sy)之间的转换关系，可以通过下述坐标变换表示：
Sx = A1*Tx + B1*Ty + C1
Sy = A2*Tx + B2*Ty + C2
*/

//MARK:TI GUI

GL_Page_TypeDef *pageTest;
void BtnClick(void)
{
    GL_Clear(Blue);
}
void testBtn(void)
{
  GL_PageControls_TypeDef* DemoBtn;


  /* ************** GRAPHIC LIBRARY - PAGE 0 ************** */
  pageTest = malloc(sizeof(GL_Page_TypeDef));
  Create_PageObj( pageTest );

  DemoBtn = NewButton( 1, (uint8_t*)"DEMO", BtnClick );
  AddPageControlObj( 100, 20, DemoBtn, pageTest );
  
  GL_Clear(White);
  GL_SetTextColor(GL_Blue);
  GL_DrawLine(20,100, 26, Vertical);
    GL_DrawLine(20,100, 40, Horizontal);
  pageTest->ShowPage( pageTest, GL_TRUE );
}


#if defined(USE_STM32100E_EVAL)
#define LAST_FLASH_MEMORY_ADDRESS	((uint32_t)0x08080000)
#elif defined(USE_STM322xG_EVAL)
#define LAST_FLASH_MEMORY_ADDRESS	((uint32_t)0x08100000)
#elif defined(USE_STM3210C_EVAL)
#define LAST_FLASH_MEMORY_ADDRESS	((uint32_t)0x08040000)
#endif

void Led_GPIO_Configuration(void);
void Delay(uint32_t nCount);
void InputInterface_Init(void);
void ShowLoadingLogo(void);
void CatchInputEvents(void);


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  RCC_ClocksTypeDef RCC_Clocks;

  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  /* Setup SysTick Timer for 10 msec interrupts  */
  RCC_GetClocksFreq(&RCC_Clocks);
  if (SysTick_Config(RCC_Clocks.SYSCLK_Frequency / 1000))
  {
    /* Capture error */
    while (1);
  }
  /* configure Systick priority */
  NVIC_SetPriority(SysTick_IRQn, 0x0B);

  /* Set HW structure parameters */
  HWConfig_SetHardwareParams();


  /* If the LCD Panel has a resolution of 320x240 this command is not needed, it's set by default */
  /* Set_LCD_Resolution( 320, 240 ); */

  /* Initialize the LCD */
  GL_LCD_Init();

  GL_Clear(GL_White);

  InputInterface_Init();

  ShowLoadingLogo();

#if TOUCH_SCREEN_CAPABILITY
  /* Check if Calibration has been done*/
  TS_CheckCalibration();
#endif

  /*Initialize cursor*/
  GL_Clear(White);
  CursorInit(GL_NULL);

  /* Menu Initialisation*/
  Show_HomeScreen();
  //testBtn();
  
  CursorShow(195, 80);

  
  //tmpSize = GetObjSize(PagesList[0]->PageControls[0]);
  /* Infinite main loop ------------------------------------------------------*/
  while (1)
  {

#if defined(USE_STM3210C_EVAL) || defined(USE_STM32100E_EVAL)
    /* Catching touch events */
    if ( TS_IsCalibrationDone() == SET )
#endif
    {
      ProcessInputData();
    }

    /* Time out calculate for power saving mode */
    TimeOutCalculate();

    CatchInputEvents();
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

/**
  * @brief  Poll on Input devices to check for a new event
  * @param  None
  * @retval None
  */
void CatchInputEvents(void)
{
#if TOUCH_SCREEN_CAPABILITY
  /* catch touch screen events */
  TSC_Read();
#endif

#if defined(USE_STM3210C_EVAL) || defined(USE_STM322xG_EVAL)
  /* for STM3210C/STM322xG eval board joystick is interfaced with IOE */
  CursorReadJoystick(IOEXP_MODE);
#else
  /* Use IO polling */
  //CursorReadJoystick(POLLING_MODE);
#endif
}

/**
  * @brief  Enable input interfaces
  * @param  None
  * @retval None
  */
void InputInterface_Init(void)
{
#if TOUCH_SCREEN_CAPABILITY
  uint32_t time_out = 0xFFF;
#endif

#if defined(USE_STM3210E_EVAL) || defined(USE_STM32100B_EVAL) ||\
  defined(USE_STM3210B_EVAL) || defined(USE_STM32100E_EVAL)
  /* Joystick Init */
  GL_JoyStickConfig_GPIO();

#elif  defined(USE_STM3210C_EVAL) || defined(USE_STM322xG_EVAL)
  /* Joystick Init */
  GL_JoyStickConfig_IOExpander();
#endif
  GL_ButtonInit();
#if TOUCH_SCREEN_CAPABILITY


  /* Set the last Flash Memory address */
  Set_LastFlashMemoryAddress( LAST_FLASH_MEMORY_ADDRESS );

  /* Touch Screen Init */
  TSC_Init();

  /* If key is pressed - Start Calibration */
  while ( GPIO_ReadInputDataBit(USER_BUTTON_PORT, USER_BUTTON_PIN) && (time_out > 0) )
  {
    time_out--;
  }
  if (time_out > 0)
  {
    TS_Calibration();
  }

#endif
}

/**
  * @brief  Display loading... logo
  * @param  None
  * @retval None
  */
void ShowLoadingLogo(void)
{
  GL_SetTextColor(Black);
  GL_SetBackColor(White);

#if defined(STM32F10X_MD) || defined(STM32F10X_MD_VL) || defined(STM32L1XX_MD)
  GL_DrawButtonBMP( 185, 135, (LCD_Height / 10)*2 + 80, (LCD_Height / 10)*2 + 30, (uint8_t*) STM32Logo );
#else
  GL_DrawButtonBMP( 210, 110, (LCD_Height / 10)*2 + 100, (LCD_Height / 10)*2, (uint8_t*) STM32Logo );
#endif
  GL_DisplayAdjStringLine(3 * (LCD_Height / 5), (LCD_Width / 3)*2 + 6, (uint8_t*)"Loading.", GL_FALSE);

  GL_Delay(30);
  GL_DisplayAdjStringLine(3 * (LCD_Height / 5), (LCD_Width / 3)*2 + 6, (uint8_t*)"Loading..", GL_FALSE);
  GL_Delay(30);
  GL_DisplayAdjStringLine(3 * (LCD_Height / 5), (LCD_Width / 3)*2 + 6, (uint8_t*)"Loading...", GL_FALSE);
  GL_Delay(30);
  GL_DisplayAdjStringLine(3 * (LCD_Height / 5), (LCD_Width / 3)*2 + 6, (uint8_t*)"Loading.   ", GL_FALSE);
  GL_Delay(30);
  GL_DisplayAdjStringLine(3 * (LCD_Height / 5), (LCD_Width / 3)*2 + 6, (uint8_t*)"Loading..", GL_FALSE);
  GL_Delay(30);
  GL_DisplayAdjStringLine(3 * (LCD_Height / 5), (LCD_Width / 3)*2 + 6, (uint8_t*)"Loading...", GL_FALSE);
  GL_Delay(15);
}


