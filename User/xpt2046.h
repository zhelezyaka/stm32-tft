#ifndef _SOLEE_TOUCH_H_
#define _SOLEE_TOUCH_H_

#include "stm32f10x.h"

#define TSC2046_GetCounts	10		/* 每次采集的次数，取其平均值 */
#define  CMD_RDX  0xD0	            //触摸IC读坐标积存器
#define  CMD_RDY  0x90

#define SPI_TOUCH_CS_LOW()       GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define SPI_TOUCH_CS_HIGH()      GPIO_SetBits(GPIOA, GPIO_Pin_4)
//系统初始化
void initSPI(void);

void initExti(void);

u8   sendByte(u8 byte);
u8   readByte(void);
void readAdd(uint32_t *x, uint32_t *y);
u16  readX(void);
u16  readY(void);
u8   getPos(uint16_t xAd, uint16_t yAd, uint16_t *x, uint16_t *y);
#endif

