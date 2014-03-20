#ifndef _SOLEE_LCD_H_
#define _SOLEE_LCD_H_

//常用颜色值定义
#define COLOR_WHITE          0xFFFF
#define COLOR_BLACK          0x0000
#define COLOR_GREY           0xF7DE
#define COLOR_BLUE     		 0x001F
#define COLOR_BLUE2          0x051F
#define COLOR_RED            0xF800
#define COLOR_MAGENTA        0xF81F
#define COLOR_GREEN          0x07E0
#define COLOR_CYAN           0x7FFF
#define COLOR_YELLOW         0xFFE0

//驱动Ic
static u16 _deviceCode;

/*
NE1 0x600000000
NE2 0x640000000
NE3 0x680000000
NE4 0x6C0000000
*/

#define ili9325_REG          (*((volatile unsigned short *) 0x60000000)) 
#define ili9325_RAM          (*((volatile unsigned short *) 0x60020000)) // A16(RS)

//系统初始化
void initGPIO(void);
void initFSMC(void);

//寄存器操作
void lcdWriteReg(u8 reg, u16 value);
u16  lcdReadReg(u8 reg);
void lcdWriteRamPrepare(void);
void lcdWriteRam(u16 rgbColor);

void lcdInit(void);
void lcdReset(void);
void lcdSetCursor(u16 x,u16 y);
void lcdClear(u16 color);

unsigned short lcdReadGRAM(unsigned short x,unsigned short y);

#endif

