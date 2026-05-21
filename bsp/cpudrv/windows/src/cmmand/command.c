//----------------------------------------------------
//Copyright (C), 2004-2011,  罗侍田.
//版权所有 (C), 2004-2011,   罗侍田.
//所属模块:windows模拟字符输入模块
//作者:  罗侍田
//版本：V1.0.0
//文件描述: 截取windows的命令行输入，作为djyos的字符输入输出设备；把djyos输出到
//          终端的字符输出到windows的cmd窗口
//其他说明:
//修订历史:
// 1. 日期: 2012-10-09
//   作者:  罗侍田
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include <windows.h>
#include "stdbool.h"
#include "stdint.h"
#include <device.h>
#include "ring.h"
#include "cpu_peri.h"
#include "stdio.h"
#include "djyos.h"
#include "int.h"
#include "multiplex.h"
#include "project_config.h"

//@#$%component configure   ****组件配置开始，用于 DIDE 中图形化配置界面
//****配置块的语法和使用方法，参见源码根目录下的文件：component_config_readme.txt****
//****新建组件，可以copy component_config_template.txt的内容到这里。
//%$#@initcode      ****初始化代码开始，由 DIDE 删除“//”后copy到初始化文件中
//extern ptu32_t ModuleInstall_Cmd(ptu32_t para);
//  ModuleInstall_Cmd(0);
//%$#@end initcode  ****初始化代码结束

//%$#@describe      ****组件描述开始
//component name:"console driver" //组件名
//parent:"device file system"   //填写该组件的父组件名字，none表示没有父组件
//attribute:bsp                 //选填“third、system、bsp、user”，本属性用于在IDE中分组
//select:choosable              //选填“required、choosable”，若填必选且需要配置参数，则IDE裁剪界面中默认勾取，
                                //不可取消，必选且不需要配置参数的，或是不可选的，IDE裁剪界面中不显示，
//init time:medium              //初始化时机，可选值：early，medium，later, pre-main。
                                //表示初始化时间，分别是早期、中期、后期
//dependence:"device file system"//该组件的依赖组件名（可以是none，表示无依赖组件），
                                //选中该组件时，被依赖组件将强制选中，
                                //如果依赖多个组件，则依次列出，用“,”分隔
//weakdependence:"none"         //该组件的弱依赖组件名（可以是none，表示无依赖组件），
                                //选中该组件时，被依赖组件不会被强制选中，
                                //如果依赖多个组件，则依次列出，用“,”分隔
//mutex:"none"                  //该组件的互斥组件名（可以是none，表示无互斥组件），
                                //如果与多个组件互斥，则依次列出，用“,”分隔
//%$#@end describe  ****组件描述结束

//%$#@configue      ****参数配置开始
#if ( CFG_MODULE_ENABLE_CONSOLE_DRIVER == false )
//#warning  " myname 组件参数未配置，使用默认配置"
//%$#@target = header           //header = 生成头文件,cmdline = 命令行变量，DJYOS自有模块禁用
#define CFG_MODULE_ENABLE_CONSOLE_DRIVER    false //如果勾选了本组件，将由DIDE在project_config.h或命令行中定义为true
//%$#@num,0,100,
//%$#@enum,true,false,
//%$#@string,1,10,
//%$#@SYMBOL,
//%$#select,        ***从列出的选项中选择若干个定义成宏
//%$#@free,
//%$#@object_num,0,1,2,..,100   ————配置object的数量，紧跟着罗列出全部可选值,与object_para绑定，选择此项中任意一值时，object_para项要做出动态增加与减少
//%$#@object_para,              ————配置每个object的参数，与object_num绑定，目前必须以上组件参数配置块的最后
#endif
//%$#@end configue  ****参数配置结束

//%$#@exclude       ****编译排除文件列表
//%$#@end exclude   ****排除文件列表结束

//%$#@API include path         ****API头文件路径列表，用分号“;”隔开，以本文件所在目录为当前目录，允许用通配符，应用本库时，DIDE将自动把本列表中的头文件加到 -I 搜索路径中
//./include;
//%$#@end API include path

//%$#@self include path         ****编译自身需要的头文件路径列表，用分号“;”隔开，以本文件所在目录为当前目录，允许用通配符，编译本库时，DIDE将自动把本列表中的头文件加到 -I 搜索路径中
//./include;
//%$#@end self include path

//%$#@library path             ****列出本组件引用的.a库文件列表，用分号“;”隔开，以本文件所在目录为当前目录，允许用通配符
//./ip/my_lib_name.a
//%$#@end library path

//@#$%component end configure


#define cmd_buf_len  1024

HANDLE win32_scanf;
struct Object * sg_ptCmdDev;

char cmd_ptcl_recv_buf[cmd_buf_len];
//static struct DjyDevice * pg_cmd_hdl;
static struct RingBuf s_tRecvRingBuf;           //环形接收缓冲区.
static struct SemaphoreLCB *s_ptRecvRingBufSemp;
char scanf_buf[cmd_buf_len-2];
HANDLE hNewScreenBuffer;
extern HANDLE hStdOut;
extern HANDLE hStdIn;

s32 cmd_DriverWrite(struct objhandle *Kfp,u8 *buf,u32 len,u32 off,u32 timeout);
s32 cmd_DriverRead(struct objhandle *Kfp,u8 *buf,u32 len,u32 off,u32 timeout);
s32 cmd_DriverCtrl(struct objhandle *Kfp,u32 cmd,u32 data1,u32 data2);

u32 cmd_int(ptu32_t cmd_int_line)
{
    u32 len;
    len = strlen(scanf_buf);
    //copy整个硬件缓冲区到协议缓冲区
    Ring_Write(&s_tRecvRingBuf, (u8*)scanf_buf,len);
    Lock_SempPost(s_ptRecvRingBufSemp);
    OBJ_SetMultiplexEvent(sg_ptCmdDev, CN_MULTIPLEX_SENSINGBIT_READ);
//  SetObjectAccessStatus(sg_ptCmdDev, CN_MULTIPLEX_SENSINGBIT_READ);
    return 0;
}

u32 WINAPI win32_scanf_pro( LPVOID lpParameter )
{
    u32 num;
    while(1)
    {
        ReadConsoleA( hStdIn,
                        &scanf_buf,
                        256,
                        (PDWORD)&num,
                        NULL);
        scanf_buf[num] = '\0';
        Int_TapLine(cn_int_line_cmd);
    }
}

//----命令行设备open函数-------------------------------------------------------
//功能：打开命令行设备时的回调函数，只把文件当前状态设置为可写。文件是安装命令行
//      模块时创建的，本函数不负责创建文件。
//参数：Fp，待操作的串口文件指针
//      Mode，打开模式
//      timeout，超时时间。
//返回：0=success，-1=error
//-----------------------------------------------------------------------------
u32 cmd_Open(struct objhandle *Kfp, u32 Mode,u32 timeout)
{
    if(Kfp == NULL)
        return -1;
//  of_setmevent(Kfp, CN_MULTIPLEX_SENSINGBIT_WRITE);
    Handle_SetMultiplexEvent(Kfp, CN_MULTIPLEX_SENSINGBIT_WRITE);
    return 0;
}

//----初始化命令行设备---------------------------------------------------------
//功能: 如名
//参数: 无
//返回: 无意义
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_Cmd(ptu32_t para)
{
    Ring_Init(  &s_tRecvRingBuf,
                (u8 *)cmd_ptcl_recv_buf,
                cmd_buf_len);
    s_ptRecvRingBufSemp = Lock_SempCreate(1,0,CN_BLOCK_FIFO,NULL);
    //以下建立windows 终端输入设备
    sg_ptCmdDev = Device_Create("windows_cmd",
                                cmd_Open,NULL,
                               (fnDevWrite) cmd_DriverWrite,
                               (fnDevRead ) cmd_DriverRead,
                               (fnDevCntl ) cmd_DriverCtrl,
                               0
                               );
    if((sg_ptCmdDev == NULL) || (s_ptRecvRingBufSemp == NULL))
        goto exit_from_add_device;
//    pg_cmd_hdl = Device_Open("windows_cmd",O_RDWR,0);      //打开右手句柄
    Int_Register(cn_int_line_cmd);
    Int_IsrConnect(cn_int_line_cmd,cmd_int);
    Int_SettoAsynSignal(cn_int_line_cmd);
    Int_RestoreAsynLine(cn_int_line_cmd);
    win32_scanf = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)win32_scanf_pro, 0,
                                     0, NULL );
    SetThreadAffinityMask( win32_scanf, 1 );

//    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
//    hStdIn = GetStdHandle(STD_INPUT_HANDLE);
/*    hNewScreenBuffer = CreateConsoleScreenBuffer(
                       GENERIC_READ | GENERIC_WRITE,          // read/write access
                       FILE_SHARE_READ | FILE_SHARE_WRITE,    // shared
                       NULL,                    // default security attributes
                       CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE
                       NULL);                   // reserved; must be NULL
    SetConsoleActiveScreenBuffer(hNewScreenBuffer);
*/
    return 1;

exit_from_add_device:
    Lock_SempDelete(s_ptRecvRingBufSemp);
    Device_DeleteByObject(sg_ptCmdDev);
    return 0;
}

//----uart设备左手写函数-------------------------------------------------------
//功能: 从左手写入uart设备,一般由uart的使用者调用.
//      1.把数据写入串口serial_DCB_t的环形发送缓冲区中.
//      2.如果串口硬件空闲,便把数据写入硬件发送缓冲区,并启动发送,否则不操作
//参数: uart_dev,被操作的设备的左手指针.
//      src_buf,数据源指针
//      len,要发送的序列长度
//返回: 实际写入环形缓冲区的字符数
//-----------------------------------------------------------------------------
s32 cmd_DriverWrite(struct objhandle *Kfp,u8 *buf,u32 len,u32 off,u32 timeout)
{
    u32 sended;

    if((len==0) || (buf == 0))
        return 0;

    sended = WriteConsole(   hStdOut, // screen buffer to write to
                    buf,        // buffer to copy from
                    len,
                    NULL,
                    NULL);
    return sended;
}

//----命令行设备读函数------------------------------------------------------
//功能: 读取命令行设备接收环形缓冲区的数据
//参数: PrivateTag,被操作的设备私有标签。.
//      dst_buf,读缓冲区
//      len,读入长度,
//返回: 实际读出长度
//----------------------------------------------------------------------------
s32 cmd_DriverRead(struct objhandle *Kfp,u8 *buf,u32 len,u32 off,u32 timeout)
{
    uint32_t completed = 0;
    uint32_t ReadLen;
    u32 base_time,rel_timeout=timeout;

    if((Kfp == NULL)||(len==0) || ((u8*)buf == NULL) )
        return 0;

//    Fd = ofno( Kfp );

    base_time = (u32)DJY_GetSysTime();
    completed = Ring_Read(&s_tRecvRingBuf,(uint8_t*)buf,len);
    if(completed < len)    //缓冲区中数据不够，则等待接收
    {
        ReadLen = len - completed;
        while(1)
        {
            Lock_SempPend(s_ptRecvRingBufSemp,rel_timeout);
            completed += Ring_Read(&s_tRecvRingBuf,
                               ((u8*)buf) + completed,
                               ReadLen);
            if(completed < len)
            {
                //每次pend的时间要递减
                rel_timeout = (u32)DJY_GetSysTime() - base_time;
                if(rel_timeout > timeout)
                    break;
                else
                {
                    rel_timeout = timeout - rel_timeout;
                    ReadLen = len - completed;
                }
            }
            else
                break;
        }
    }
    //若缓冲区中不再有数据，清掉多路复用触发状态。
    if(Ring_IsEmpty(&s_tRecvRingBuf))
    {
        // ClrFdAccessStatus(Fd, CN_MULTIPLEX_SENSINGBIT_READ);
        // fcntl(Fd, F_OF_SETACCESS, CN_MULTIPLEX_SENSINGBIT_READ);
        Handle_ClrMultiplexEvent(Kfp, CN_MULTIPLEX_SENSINGBIT_READ);
//      of_setmevent(Kfp, CN_MULTIPLEX_SENSINGBIT_READ);
    }

    return completed;
}

void msgout_PutStrDirect(char *str,u32 len)
{
    WriteConsole(   hStdOut, // screen buffer to write to
                    str,        // buffer to copy from
                    len,
                    NULL,
                    NULL);
}

char msgout_GetStrDirect(void)
{
    u32 num;
    char ch;
    ReadConsoleA( hStdIn,
                    &ch,
                    1,
                    (PDWORD)&num,
                    NULL);
}

//----初始化内核级IO-----------------------------------------------------------
//功能：初始化内核级输入和输出所需的runtime函数指针。
//参数：无
//返回：无
//-----------------------------------------------------------------------------
void Stdio_KnlInOutInit(char * StdioIn, char *StdioOut)
{
    PutStrDirect = msgout_PutStrDirect;
    GetCharDirect = msgout_GetStrDirect;
    return;
}

//----串口设备控制函数---------------------------------------------------------
//功能: 串口设备的控制函数
//参数: uart_handle,被操作的串口设备指针.
//      cmd,操作类型
//      data,含义依cmd而定
//返回: 无意义.
//-----------------------------------------------------------------------------
s32 cmd_DriverCtrl(struct objhandle *Kfp,u32 cmd,u32 data1,u32 data2)
{
    return 0;
}

#include "shell.h"
typedef void (*Ex_shell_func)(void);
void __asm_ExecuteCmd(union param *ptab,
       enum param_typr *pflag,Ex_shell_func fun,u32 num)
{

}

