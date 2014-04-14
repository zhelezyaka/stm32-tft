#include <stdio.h>
#include "stm32f10x.h"
#include "util.h"


//延时ms毫秒
volatile u16 _timerDelay;
void sysTickDelay(u16 ms)
{
	_timerDelay = ms;
    
    //参考 stm32f10x_it.c SysTick_Handler
	while(_timerDelay);
}

char *itoa(int val, char *buf, unsigned radix)
{
        char *p;
        char *firstdig;
        char temp;
        unsigned digval;
        p = buf;
        if(val<0)
        { 
                *p++ = '-';
                val = (unsigned long)(-(long)val);
        }
        firstdig = p;
        do
        {
                digval = (unsigned)(val%radix); 
                val /= radix;
                if(digval>9)
                {
                        *p++ = (char)(digval-10+'a');
                }
                else
                {
                        *p++=(char)(digval+'0');
                }
        }while(val>0); 
        *p--='\0';
        do
        {
                temp=*p;
                *p=*firstdig;
                *firstdig=temp;
                --p;
                ++firstdig;
        }while(firstdig<p);
        return buf; 
}

int fputc(int c, FILE *stream)
{
   return ITM_SendChar(c);
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
void delay(vu32 nCount)
{
  vu32 index = 0; 
  for(index = (34000 * nCount); index != 0; index--)
  {
  }
}
