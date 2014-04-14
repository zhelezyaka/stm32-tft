#ifndef _SOLEE_TOUCH_H_
#define _SOLEE_TOUCH_H_

#include "stm32f10x.h"

#define TSC2046_GetCounts	10		/* 每次采集的次数，取其平均值 */
#define  CMD_RDX  0xD0	 //触摸IC读坐标积存器
#define  CMD_RDY  0x90
//系统初始化
void initSPI(void);

void initExti(void);

u8   sendByte(u8 byte);
u8   readByte(void);
void readAdd(uint16_t *x, uint16_t *y);
u8   getPos(uint16_t xAd, uint16_t yAd, uint16_t *x, uint16_t *y);
#endif

