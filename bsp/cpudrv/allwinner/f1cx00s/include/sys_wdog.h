#ifndef __SYS_WDOG_H__
#define __SYS_WDOG_H__

//看门狗超时时间
#define TimeOut_0_5s    0 //0.5秒
#define TimeOut_1s      1
#define TimeOut_2s      2
#define TimeOut_3s      3
#define TimeOut_4s      4
#define TimeOut_5s      5
#define TimeOut_6s      6
#define TimeOut_8s      7
#define TimeOut_10s     8
#define TimeOut_12s     9
#define TimeOut_14s     10
#define TimeOut_16s     11
//看门狗模式
#define WDT_WholeSystemMode     1 //系统模式-可复位
#define WDT_OnlyInterruprMode   2 //中断模式-不可复位

void wdt_enable(void);
void wdt_Restart(void);
void wdt_Init(u32 TimeOut,u8 Mode,u8 IRQ_EN);
int read_last_reset(void);
void wdt_reset(void);
#endif

