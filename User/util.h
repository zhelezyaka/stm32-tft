#ifndef _SOLEE_UTIL_H_
#define _SOLEE_UTIL_H_

void sysTickDelay(u16 ms);    //“精确”延时函数
char *itoa(int val, char *buf, unsigned radix);
void delay(vu32 nCount);

#define ABS(X)  ((X) > 0 ? (X) : -(X))   

#endif

