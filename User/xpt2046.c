#include "xpt2046.h"
#include "stm32f10x.h"
#include "util.h"
#include "ili9325.h"

//SPI1_NSS  [PA4] -> T_CS
//SPI1_SCK  [PA5] -> T_CLK
//SPI1_MISO [PA6] -> T_DOUT
//SPI1_MOSI [PA7] -> T_DIN
//E3              -> T_IRQ

u16 vx  = 13581, vy  = 10879;  //比例因子，此值除以1000之后表示多少个AD值代表一个像素点
u16 chx = 3676,  chy = 3886;   //默认像素点坐标为0时的AD起始值

void initSPI()
{
 
    GPIO_InitTypeDef gpioTyp;
    SPI_InitTypeDef  spiTyp;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_SPI1, ENABLE);


    gpioTyp.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    gpioTyp.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioTyp.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioTyp);

    GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7); 

    //中断端口GPIO设置 PE3
    gpioTyp.GPIO_Pin = GPIO_Pin_3;
    gpioTyp.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
    GPIO_Init(GPIOE, &gpioTyp);

    //UNKNOWN ：NSS 初始化为推挽输出
    gpioTyp.GPIO_Pin = GPIO_Pin_4;
    gpioTyp.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOA, &gpioTyp);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
    
    //????
    SPI_Cmd(SPI1, DISABLE);
    //SPI1 设置
    spiTyp.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //SPI为双线双向全双工
    spiTyp.SPI_Mode = SPI_Mode_Master;                        //SPI工作模式：主SPI          
    spiTyp.SPI_DataSize = SPI_DataSize_8b;                    //SPI数据大小：SPI发送接收8位帧结构
    spiTyp.SPI_CPOL = SPI_CPOL_Low;                           //选择串行时钟的稳态：时钟悬空高x
    spiTyp.SPI_CPHA = SPI_CPHA_1Edge;                         //数据捕获于第二个时钟沿x
    spiTyp.SPI_NSS = SPI_NSS_Soft;                            //NSS信号由硬件（NSS管脚）还是软件（SSI位）管理
    spiTyp.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; //波特率预分频值为256
    spiTyp.SPI_FirstBit = SPI_FirstBit_MSB;                   //指定数据传输从MSB位开始
    spiTyp.SPI_CRCPolynomial = 7;                             //CRC值计算的多项式
    SPI_Init(SPI1, &spiTyp);                                  //初始化SPI1

    //使能SPI外设
    SPI_Cmd(SPI1, ENABLE);
}

void initExti(void)
{
    GPIO_InitTypeDef gpioTyp;
	EXTI_InitTypeDef exitTyp;
	NVIC_InitTypeDef nvicTyp;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
    
	//中断引脚初始化
	gpioTyp.GPIO_Pin = GPIO_Pin_3;
	//gpioTyp.GPIO_Mode = GPIO_Mode_IPU;
    gpioTyp.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &gpioTyp);

	//PE3中断线以及中断初始化配置
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);

	exitTyp.EXTI_Line = EXTI_Line3;
	exitTyp.EXTI_Mode = EXTI_Mode_Interrupt;
	exitTyp.EXTI_Trigger = EXTI_Trigger_Falling;
	exitTyp.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exitTyp);
    //
    EXTI_GenerateSWInterrupt(EXTI_Line3);  

	nvicTyp.NVIC_IRQChannel = EXTI3_IRQn;
	nvicTyp.NVIC_IRQChannelPreemptionPriority = 0x02;
	nvicTyp.NVIC_IRQChannelSubPriority = 0x02;
	nvicTyp.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicTyp);
}

/*
void EXTI3_IRQHandler(void)
{
    uint16_t x = 0, y = 0, xAd = 0, yAd = 0;
    //char chArr[10];
	if(EXTI_GetITStatus(EXTI_Line3)!= RESET)
    {
        //lcdClear(COLOR_BLUE);
        //readAdd(&xAd, &yAd);
        //getPos(xAd, yAd, &x, &y);
        //itoa(x, chArr, 10);
        //lcdShowString(0, 0, chArr);
        //itoa(y, chArr, 10);
        //lcdShowString(0, 25, chArr);
        //lcdDrawCursor(x, y);
    }
	EXTI_ClearITPendingBit(EXTI_Line3);
}
*/

u8 sendByte(u8 byte)
{
     /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /* Send Half Word through the SPI3 peripheral */
  SPI_I2S_SendData(SPI1, byte);

  /* Wait to receive a Half Word */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  //while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);  

  /* Return the Half Word read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}

u8 readByte(void)
{
    return sendByte(0x00);
}


/* 等检测到触摸中断后调用此函数读取位置，读数正确返回1，否则返回0 */
void readAdd(uint32_t *x, uint32_t *y)
{
    uint32_t temp;

    GPIO_ResetBits(GPIOA, GPIO_Pin_4); // cs = 0
    //delay_ms(1);
    sendByte( CMD_RDY ) ;
    //delay_ms(1);
    //while(TP_GetBusyStatus());	//等待busy信号变低
    temp = 0;
    temp= readByte() ;
    temp<<= 8;
    temp|= readByte() ;
    (*y) = temp >> 3;
    (*y) &=0x0FFF;
	//TP_CS_HIGH();

	//TP_CS_LOW();
	//delay_ms(1);
    sendByte( CMD_RDX ) ;
    //while(TP_GetBusyStatus());	//等待busy信号变低
    //delay_ms(1);
    temp = 0;
    temp= readByte() ;
    temp<<= 8;
    temp|= readByte() ;
    (*x) = temp >> 3;
    (*x) &=0x0FFF;
    GPIO_SetBits(GPIOA, GPIO_Pin_4); // cs = 0

}

u16 readX(void)
{
    u16 xPos = 0, Temp = 0, Temp0 = 0, Temp1 = 0;

    /* Select the TP: Chip Select low */
    SPI_TOUCH_CS_LOW();
    //sysTickDelay(10);
    /* Send Read xPos command */
    sendByte( CMD_RDX ) ;
    //sysTickDelay(10);
    /* Read a byte from the TP */
    Temp0 = readByte();
    //sysTickDelay(10);  
    /* Read a byte from the TP */
    Temp1 = readByte();  
    //sysTickDelay(10);
    /* Deselect the TP: Chip Select high */
    SPI_TOUCH_CS_HIGH();

    Temp = (Temp0 << 8) | Temp1; 

    xPos = Temp>>3;

    return xPos;
}
u16 readY(void)
{
    u16 yPos = 0, Temp = 0, Temp0 = 0, Temp1 = 0;

    /* Select the TP: Chip Select low */
    SPI_TOUCH_CS_LOW();
    //sysTickDelay(10);
    /* Send Read xPos command */
    sendByte( CMD_RDY ) ;
    //sysTickDelay(10);
    /* Read a byte from the TP */
    Temp0 = readByte();
    //sysTickDelay(10);  
    /* Read a byte from the TP */
    Temp1 = readByte();  
    //sysTickDelay(10);
    /* Deselect the TP: Chip Select high */
    SPI_TOUCH_CS_HIGH();

    Temp = (Temp0 << 8) | Temp1; 

    yPos = Temp>>3;

    return yPos;
}
u8 getPos(uint16_t xAd, uint16_t yAd, uint16_t *x, uint16_t *y)
{
    *x = xAd > chx ? ((u32)xAd - (u32)chx) * 1000 / vx : ((u32)chx - (u32)xAd) * 1000 / vx;
	*y = yAd > chy ? ((u32)yAd - (u32)chy) * 1000 / vy : ((u32)chy - (u32)yAd) * 1000 / vy;
    return 1;
}
