#ifndef _SOLEE_lcdH_
#define _SOLEE_lcdH_

#include "colors.h"
#include "fonts.h"
#include <stdint.h>

/*
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

/*
NE1 0x600000000
NE2 0x640000000
NE3 0x680000000
NE4 0x6C0000000
*/

#define ili9325_REG          (*((volatile u16 *) 0x60000000)) 
#define ili9325_RAM          (*((volatile u16 *) 0x60020000)) // A16(RS)

/**
 * @brief Uncomment the line below if you want to use user defined Delay function
 *        (for precise timing), otherwise default _delay_ function defined within
 *         this driver is used (less precise timing).  
 */
#define USE_Delay

#ifdef USE_Delay
    #include "util.h" 
    #define _delay_     sysTickDelay
#else
    #define _delay_     delay
#endif

typedef struct 
{
    int16_t X;
    int16_t Y;
} Point, * pPoint;  

void LCD_DeInit(void);  
void LCD_ILI9325_Init(void);
void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor); 
void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor);
void LCD_SetTextColor(__IO uint16_t Color);
void LCD_SetBackColor(__IO uint16_t Color);
void LCD_ClearLine(uint8_t Line);
void LCD_Clear(uint16_t Color);
void LCD_SetCursor(uint8_t Xpos, uint16_t Ypos);
void LCD_DrawChar(uint8_t Xpos, uint16_t Ypos, const uint16_t *c);
void LCD_DisplayChar(uint8_t Line, uint16_t Column, uint8_t Ascii);
void LCD_SetFont(sFONT *fonts);
sFONT *LCD_GetFont(void);
void LCD_DisplayStringLine(uint8_t Line, uint8_t *ptr);
void LCD_SetDisplayWindow(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
void LCD_WindowModeDisable(void);
void LCD_DrawLine(uint8_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction);
void LCD_DrawRect(uint8_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width);
void LCD_DrawCircle(uint8_t Xpos, uint16_t Ypos, uint16_t Radius);
void LCD_DrawMonoPict(const uint32_t *Pict);
void LCD_WriteBMP(uint32_t BmpAddress);
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void LCD_PolyLine(pPoint Points, uint16_t PointCount);
void LCD_PolyLineRelative(pPoint Points, uint16_t PointCount);
void LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount);
void LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount);
void LCD_FillPolyLine(pPoint Points, uint16_t PointCount);

void LCD_WriteReg(uint8_t lcdReg, uint16_t LCD_RegValue);
uint16_t LCD_ReadReg(uint8_t lcdReg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(uint16_t RGB_Code);
uint16_t LCD_ReadRAM(void);
void LCD_PowerOn(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);

void LCD_Reset(void);
void LCD_LightOn(void);
void LCD_LightOff(void);

void LCD_CtrlLinesConfig(void);
void LCD_FSMCConfig(void);


#endif

