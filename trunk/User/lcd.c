#include "stm32f10x.h"
#include "lcd.h"
#include "util.h"
#include "fonts.h"

void initGPIO(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
  
    // RCC使能FSMC的时钟直接来自AHB时钟, 也就是HCLK, 中间没有分频. 控制位是RCC_AHBENR中的FSMCEN位
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    // GPIO端口和AFIO端口时钟来自APB2, 也就是PCLK2, 控制位是RCC_APB2ERN中的IOPxEN 和AFIOEN位
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOD, ENABLE); 
                             
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;              
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;         //LCD 背光控制
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;        //LCD-RST
    GPIO_Init(GPIOE, &GPIO_InitStructure);      

    // Set PD.00(D2), PD.01(D3), PD.04(NOE/RD), PD.05(NWE/WR), PD.08(D13), PD.09(D14),
    // PD.10(D15), PD.14(D0), PD.15(D1) as alternate function push pull      
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
    // PE.14(D11), PE.15(D12) as alternate function push pull 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 

    // CS 为FSMC_NE1(PD7) 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // RS 为FSMC_A16(PD11)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
    GPIO_Init(GPIOD, &GPIO_InitStructure); 

    GPIO_SetBits(GPIOD, GPIO_Pin_7);            //CS=1 
    GPIO_SetBits(GPIOD, GPIO_Pin_11);           //RS=1
    GPIO_SetBits(GPIOD, GPIO_Pin_14| GPIO_Pin_15 |GPIO_Pin_0 | GPIO_Pin_1);       
    GPIO_SetBits(GPIOE, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);   
    GPIO_SetBits(GPIOE, GPIO_Pin_0);            //LIGHT关
    GPIO_SetBits(GPIOE, GPIO_Pin_1);            //RESET=1
    GPIO_SetBits(GPIOD, GPIO_Pin_4);            //RD=1
    GPIO_SetBits(GPIOD, GPIO_Pin_5);            //WR=1
}

void initFSMC(void)
{
    FSMC_NORSRAMInitTypeDef  fsmcSet;
    FSMC_NORSRAMTimingInitTypeDef  p;

    // FSMC_Bank1_NORSRAM1 timing configuration 
    // 2 0 5
    p.FSMC_AddressSetupTime = 0;            //地址建立时间期限                                           
    p.FSMC_AddressHoldTime = 0;             //地址的持续时间
    p.FSMC_DataSetupTime = 3;               //设定数据时间期限
    p.FSMC_BusTurnAroundDuration = 0;       //总线转向时间
    p.FSMC_CLKDivision = 0;                 //CLK时钟输出信号的HCLK周期数表示时间
    p.FSMC_DataLatency = 0;                 //指定在获得第一个数据前的时钟周期
    p.FSMC_AccessMode = FSMC_AccessMode_A;  //FSMC 访问模式

    fsmcSet.FSMC_Bank = FSMC_Bank1_NORSRAM1;                               //指定BANK及片选信号
    fsmcSet.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;             //地址和数据值不复用的数据总线                                        
    fsmcSet.FSMC_MemoryType = FSMC_MemoryType_NOR;                         //设置外部存储器的类型
    fsmcSet.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;               //设置数据宽度
    fsmcSet.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;           //禁用突发访问模式
    fsmcSet.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;         //设置WAIT信号的有效电平
    fsmcSet.FSMC_WrapMode = FSMC_WrapMode_Disable;                         //设置是否使用Flash环回模式
    fsmcSet.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState; //设置WAIT信号有效时机
    fsmcSet.FSMC_WriteOperation = FSMC_WriteOperation_Enable;              //启用指定的FSMC块的写操作
    fsmcSet.FSMC_WaitSignal = FSMC_WaitSignal_Disable;                     //设定是否使用WAIT信号
    fsmcSet.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;                 //设定是否使用单独的写时序
    fsmcSet.FSMC_WriteBurst = FSMC_WriteBurst_Disable;                     //禁用写突发操作
    
    //设定读写时序
    fsmcSet.FSMC_ReadWriteTimingStruct = &p;
    fsmcSet.FSMC_WriteTimingStruct = &p;      
 
    FSMC_NORSRAMInit(&fsmcSet); 

    // Enable FSMC Bank1_SRAM Bank
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
    sysTickDelay(50);
}


void lcdWriteReg(u8 reg, u16 value)
{
    ili9325_REG = reg;
    ili9325_RAM = value;
}

u16 lcdReadReg(u8 reg)
{
    ili9325_REG = reg;
    return (ili9325_RAM);
}

void lcdWriteRamPrepare(void)
{
    ili9325_REG = 0x22;
}

void lcdWriteRam(u16 rgbColor)
{
    ili9325_RAM = rgbColor;
}
unsigned short lcdReadGRAM(u16 x, u16 y)
{
    u16 temp;
    lcdSetCursor(x, y);
    ili9325_REG = 0x22;
    /* dummy read */
    temp = ili9325_RAM;
    temp = ili9325_RAM;
    return temp;
}

void lcdInit(void)
{
    volatile unsigned int i;
    lcdReset();
    sysTickDelay(150);	
    _deviceCode = lcdReadReg(0x0000);
    if (_deviceCode==0x9325||_deviceCode==0x9328)
    {
        //Start Initial Sequence//
        lcdWriteReg(0x0001, 0x0100); // set SS and SM bit
        lcdWriteReg(0x0002, 0x0700); // set 1 line inversion
        lcdWriteReg(0x0003, 0x1030); // set GRAM write direction and BGR=1.
        lcdWriteReg(0x0004, 0x0000); // Resize register
        lcdWriteReg(0x0008, 0x0207); // set the back porch and front porch
        lcdWriteReg(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
        lcdWriteReg(0x000a, 0x0000); // FMARK function
        lcdWriteReg(0x000c, 0x0000); // RGB interface setting
        lcdWriteReg(0x000d, 0x0000); // Frame marker Position
        lcdWriteReg(0x000f, 0x0000); // RGB interface polarity

        //Power On sequence VGHVGL
        lcdWriteReg(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
        lcdWriteReg(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
        lcdWriteReg(0x0012, 0x0000); // VREG1OUT voltage
        lcdWriteReg(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
        lcdWriteReg(0x0010, 0x1290); //vgh 
        lcdWriteReg(0x0011, 0x0227);
        sysTickDelay(100);	
        
        lcdWriteReg(0x0012, 0x001d); // Internal reference voltage= Vci;
        sysTickDelay(100);	
        lcdWriteReg(0x0013, 0x1500); // Set VDV[4:0] for VCOM amplitude
        sysTickDelay(100);	
        lcdWriteReg(0x0029, 0x0018); // Set VCM[5:0] for VCOMH
        lcdWriteReg(0x002b, 0x000d); // Set Frame Rate
        
        // Adjust the Gamma Curve
        lcdWriteReg(0x0030, 0x0004);
        lcdWriteReg(0x0031, 0x0307);
        lcdWriteReg(0x0032, 0x0002);
        lcdWriteReg(0x0035, 0x0206);
        lcdWriteReg(0x0036, 0x0408);
        lcdWriteReg(0x0037, 0x0507);
        lcdWriteReg(0x0038, 0x0204);
        lcdWriteReg(0x0039, 0x0707);
        lcdWriteReg(0x003c, 0x0405);
        lcdWriteReg(0x003d, 0x0f02);	
        
        // Set GRAM area
        lcdWriteReg(0x0050, 0x0000); // Horizontal GRAM Start Address
        lcdWriteReg(0x0051, 0x00ef); // Horizontal GRAM End Address
        lcdWriteReg(0x0052, 0x0000); // Vertical GRAM Start Address
        lcdWriteReg(0x0053, 0x013f); // Vertical GRAM Start Address
        lcdWriteReg(0x0060, 0xa700); // Gate Scan Line
        lcdWriteReg(0x0061, 0x0001); // NDL,VLE, REV
        lcdWriteReg(0x006a, 0x0000); // set scrolling line
        
        // Partial Display Control
        lcdWriteReg(0x0080, 0x0000);
        lcdWriteReg(0x0081, 0x0000);
        lcdWriteReg(0x0082, 0x0000);
        lcdWriteReg(0x0083, 0x0000);
        lcdWriteReg(0x0084, 0x0000);
        lcdWriteReg(0x0085, 0x0000);	
        
        // Panel Control
        lcdWriteReg(0x0090, 0x0010);
        lcdWriteReg(0x0092, 0x0600);
        lcdWriteReg(0x0093, 0x0003); 
        lcdWriteReg(0x0095, 0x0110); 
        lcdWriteReg(0x0097, 0x0000); 
        lcdWriteReg(0x0098, 0x0000);

        //display on sequence
        lcdWriteReg(0x0007, 0x0133); // 262K color and display ON
    }
    lcdClear(COLOR_BLUE);
}
void lcdSetCursor(u16 x, u16 y)
{
    lcdWriteReg(0x20, x); // 0-239
    lcdWriteReg(0x21, y); // 0-319
}
void lcdClear(u16 Color)
{
    u32 index=0;
    lcdSetCursor(0,0);
    lcdWriteRamPrepare(); // Prepare to write GRAM
    for (index = 0; index < 76800; index++)
    {
        ili9325_RAM = Color;
    }
}

void lcdReset(void)
{
    GPIO_ResetBits(GPIOF, GPIO_Pin_1);
    sysTickDelay(50);					   
    GPIO_SetBits(GPIOF, GPIO_Pin_1 );		 	 
	sysTickDelay(50);	
    GPIO_ResetBits(GPIOD, GPIO_Pin_7);            //CS=0 
}

void lcdSetColor(u16 color, u16 background)
{
    _color = color;
    _background = background;
}

void lcdDrawChar(u32 x, u32 y, const u16 *c)
{
  u32 yOffset = 0;
  int i = 0;
    
  //从0开始 逐行向下进行扫描
  for(yOffset = 0; yOffset < FONT.Height; yOffset ++)
  {
    lcdSetCursor(x, y + yOffset);
  	lcdWriteRamPrepare();
    for(i = 0; i < FONT.Width; i++)    
    {
      if((c[yOffset] & (1 << i)) == 0x00) 
        lcdWriteRam(_background);
      else 
        lcdWriteRam(_color);
    }
  }
}

void lcdShowChar(u32 ln, u32 col, u16 c)
{
    //参加font.h 空格第0个数组
  c -= ' ';
  lcdDrawChar(ln, col, &FONT.table[c * FONT.Height]);
}

void lcdShowString(u32 x, u32 y, const char *str)
{
    const char *pos = str;
    while(*pos)
    {
        lcdShowChar(x, y, *pos);
        pos ++;
        if(x + FONT.Width > 222)
        {
            x = 0;
            y += FONT.Height;
        }
        else
        {
            x += FONT.Width;
        }
        
    }
}

