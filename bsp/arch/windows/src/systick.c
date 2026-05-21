//----------------------------------------------------
// Copyright (c) 2018, Djyos Open source Development team. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
// Copyright (c) 2018，著作权由都江堰操作系统开源开发团队所有。著作权人保留一切权利。
//
// 这份授权条款，在使用者符合下列条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主体、无论此责任为因合约关系、无过失责任主体或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------
//所属模块: BSP
//作者：lst
//版本：V1.0.0
//文件描述: 利用cortex-m系列CPU的systick作为系统ticks时钟源，默认勾选，但dynamic-tick
//         模式会有误差。低功耗要求高的场合，应使用低功耗定时器以替代之。
//其他说明:
//1. 日期: 2019-08-11
//   作者: lst
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------

#include <stdbool.h>
#include <stdint.h>
#include <cpu.h>
#include <int.h>
#include <systime.h>
#include <cpu_peri_int_line.h>
#include <int_hard.h>
#include "cpu_peri.h"

#define CFG_TICKMODE_DYNAMIC        false
#define CFG_REAL_CRITICAL           100     //若距离当前tick中断时间小于此 uS 数，则不修改tick中断时间

static s32 s_gCurrentTicks = 1;     //当前tick运行周期。
static s32 s_gCriticalCycle;        //CFG_REAL_CRITICAL对应的定时器计数值
static s32 s_gTicksLimit;           //24bit定时器单次定时最大ticks数
extern s64  g_s64OsTicks;
volatile UINT TID;
void CALLBACK TimerCallBack(UINT uTimerID, UINT uMsg, DWORD dwUser,
                            DWORD dw1, DWORD dw2)
{
//    msgout("tick tap \r\n");
    Int_TapLine(cn_int_line_timer_event);
}

u32 __DjyIsrTick(ptu32_t line)
{
    u32 inc;
    inc = s_gCurrentTicks;
    s_gCurrentTicks = 1;
    DJY_ScheduleIsr(inc);
//    msgout("tick isr \r\n");
//  Djy_IsrTick(1);
    return 0;
}

//----初始化tick---------------------------------------------------------------
//功能: 初始化定时器,并连接tick中断函数,启动定时器.
//参数: 无
//返回: 无
//备注: 本函数是移植敏感函数.
//      weak函数，用户也可以在boarddrv中重新实现。
//-----------------------------------------------------------------------------
//__attribute__((weak))
void __DjyInitTick(void)
{
    Int_Register(cn_int_line_timer_event);
    Int_IsrConnect(cn_int_line_timer_event,__DjyIsrTick);
    Int_SettoAsynSignal(cn_int_line_timer_event); //tick中断被设为异步信号
    Int_RestoreAsynLine(cn_int_line_timer_event);
    TID = timeSetEvent(CN_CFG_TICK_US/mS,CN_CFG_TICK_US/mS,TimerCallBack,12345678,TIME_PERIODIC);
//  timeSetEvent(500,CN_CFG_TICK_US/mS,TimerCallBack,0,TIME_PERIODIC);
}

//------------------------------------------------------------------------------
//功能：清理CPU的休眠唤醒事件状态，使CPU在关中断后，确实能被发生在关中断期间的唤醒事件
//     （中断或多处理机信号）唤醒，无论唤醒事件发生在 WFE（ARM的指令，其他CPU可能不同）
//     之前或者之后。
//参数：无
//返回：无
//注：本函数典型地被用于低功耗控制。
//------------------------------------------------------------------------------
void CleanWakeupEvent(void)
{
}

extern s64 s64g_freq;
//----读取当前时间(uS)---------------------------------------------------------
//功能：读取当前时间(uS),从计算机启动以来经历的us数，64位，默认不会溢出
//      g_s64OsTicks 为64位变量，非64位系统中，读取 g_s64OsTicks 需要超过1个
//      周期,需要使用原子操作。
//参数：无
//返回：当前时钟
//说明: 这是一个桩函数,被rtc.c文件的 DJY_GetSysTime 函数调用
//-----------------------------------------------------------------------------
s64 __DjyGetSysTime(void)
{
    LARGE_INTEGER litmp;
    s64 cnt;
    s64 time;
    atom_low_t atom_low;
    atom_low = Int_LowAtomStart();

    QueryPerformanceCounter(&litmp);
    cnt = litmp.QuadPart;
    time = (u32)(cnt*1000000/s64g_freq);
    Int_LowAtomEnd(atom_low);
    return time;// (((u32)CN_CFG_TICK_US*1000/CN_CFG_FINE_US)-pg_timer_reg->TCNTO3) *CN_CFG_FINE_US;
}

//----设置下次ticks中断时间-----------------------------------------------------
//功能：恒定tick模式下，由于tick间隔恒等于1个tick，故直接返回1，定时器设置维持不变。
//     动态tick模式下，通过修改定时器参数，把当次tick间隔设为参数Ticks。由于只有idle
//      事件运行时，才会重设tick间隔，而CPU被唤醒后，idle并不一定能及时获得运行时间，
//      故每次定时结束后，须把中断间隔设为1，以确保idle没有机会运行的期间，ticks能正常
//      中断。参数Ticks的含义，是从上次tick中断算起的，而定时器是持续不断在走的，因此
//      需要考虑从上次tick中断以来定时器已经走的时间，下述两类定时器有不同的处理。
//     定时器有两大类：
//     1、从0计数到设定值，然后请求中断，定时器归0开始重新计时。这种定时器，只需要修改设
//        定值即可，无须考虑当前已经走过的时间。把下次中断间隔设为1的工作在tick中断的ISR
//        中完成。
//     2、类似cortex-m的24位systick定时器，从reload开始减计数到0，然后从reload重新开始
//        计数的，则设置完当次tick后，还要把reload设置到1个tick。特别注意，设置定时器
//        时，要减去从上次ISR中断以来定时器已经走过的时间。
//
//参数：ticks,希望的下次中断距上次的中断间隔数
//返回：实际的下次中断距上次的中断间隔数
//-----------------------------------------------------------------------------
//__attribute__((weak))
 u32 Tick_SetNextTimeTick(s32 Ticks)
{
#if CFG_TICKMODE_DYNAMIC == true
    return 1;

#else       // for CFG_TICKMODE_DYNAMIC == true
    return 1;
#endif      // for CFG_TICKMODE_DYNAMIC == true
}

