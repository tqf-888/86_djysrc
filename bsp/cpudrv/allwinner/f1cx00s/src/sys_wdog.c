#include "stdint.h"
#include "sys_wdog.h"
#include "cpu_peri_io.h"


#define F1C100S_TIMER_BASE  (0x01C20C00)
#define WDT_IRQ_EN                 F1C100S_TIMER_BASE+(0xA0)
#define WDT_IRQ_STA                F1C100S_TIMER_BASE+(0xA4)
#define WDT_CRTL_REG               F1C100S_TIMER_BASE+(0xB0)
#define WDT_CFG_REG                F1C100S_TIMER_BASE+(0xB4)
#define WDT_MODE_REG               F1C100S_TIMER_BASE+(0xB8)



/*
看门狗初始化
IRQ_EN=中断使能
Mode 1=全模式 2=只中断
TimeOutUs=超时时间 us
*/
void wdt_Init(u32 TimeOut,u8 Mode,u8 IRQ_EN)
{
    //设置模式
    write32(WDT_CFG_REG,(read32(WDT_CFG_REG) & (~(0x3<<0)))   |   (Mode<<0));
    //设置超时时间
    write32(WDT_MODE_REG,(read32(WDT_MODE_REG)    & (~(0xf<<4))) | (TimeOut<<4) );
    //
    //使能中断
    if(IRQ_EN==1)
    {
        write32(WDT_IRQ_EN,read32(WDT_IRQ_EN) | (1)<<0);
    }else
    {
      write32(WDT_IRQ_EN,read32(WDT_IRQ_EN) &(~ ((1)<<0)));
    }
    //清中断标志
    read_last_reset();
}
/*
WDOG开
*/
void wdt_enable(void)
{
    write32(WDT_MODE_REG,read32(WDT_MODE_REG) |   ((1)<<0));
}
/*
WDOG关
*/
void wdt_disable(void)
{
    write32(WDT_MODE_REG,read32(WDT_MODE_REG) &   (~((1)<<0)));
}
/*
喂狗
*/
void wdt_Restart(void)
{
    write32(WDT_CRTL_REG,read32(WDT_CRTL_REG)|    ((0xA57)<<1)    |   ((1)<<0));
}
/*
读最后复位的模式
返回1 wdog复位 0则为其它复位
*/
int read_last_reset(void)
{
    if(read32(WDT_IRQ_STA)&0x1)//读中断标志
    {
        write32(WDT_IRQ_STA,read32(WDT_IRQ_STA)   | (1<<0));//清中断标志
        return 1;
    }else return 0;
}
/*
Wdog测试-系统模式-超时直接复位,不会产生中断【有一定误差为正常】
*/
void wdt_reset(void)
{
    //看门狗初始化
    wdt_Init(TimeOut_0_5s,WDT_WholeSystemMode,0);//大于1秒没有更新看门狗将复位
    wdt_enable();//看门狗使能

    int i=0;
    while(1)
    {
    }
}

