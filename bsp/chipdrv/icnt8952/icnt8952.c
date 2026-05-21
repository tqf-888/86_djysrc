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
//所属模块:标准输入--触摸屏模块

//------------------------------------------------------

#include "stdint.h"
#include "stdio.h"
#include "stddef.h"
#include <stdbool.h>
#include "Touch.h"
#include "string.h"
#include "gkernel.h"
//#include "cpu_peri_iic.h"
#include "iicbus.h"
//#include "ctiic.h"
#include "board.h"
#include "project_config.h"     //本文件由IDE中配置界面生成，存放在APP的工程目录中。
                                //允许是个空文件，所有配置将按默认值配置。

//@#$%component configure   ****组件配置开始，用于 DIDE 中图形化配置界面
//****配置块的语法和使用方法，参见源码根目录下的文件：component_config_readme.txt****
//%$#@initcode      ****初始化代码开始，由 DIDE 删除“//”后copy到初始化文件中
//    extern bool_t ModuleInstall_ICNT8952(void);
//    ModuleInstall_ICNT8952( );
//#if(CFG_MODULE_ENABLE_GRAPHICAL_DECORATE_DEVELOPMENT == true)
//    extern bool_t GDD_AddInputDev(const char *InputDevName);
//    GDD_AddInputDev(CFG_ICNT8952_TOUCH_NAME);
//#endif
//%$#@end initcode  ****初始化代码结束

//%$#@describe      ****组件描述开始
//component name:"touchscreen ICNT8952"//iic接口的触摸屏控制
//parent:"touch"                 //填写该组件的父组件名字，none表示没有父组件
//attribute:bsp                 //选填“third、system、bsp、user”，本属性用于在IDE中分组
//select:choosable              //选填“required、choosable、none”，若填必选且需要配置参数，则IDE裁剪界面中默认勾取，
                                //不可取消，必选且不需要配置参数的，或是不可选的，IDE裁剪界面中不显示，
//init time:pre-main            //初始化时机，可选值：early，medium，later, pre-main。
                                //表示初始化时间，分别是早期、中期、后期
//dependence:"touch"            //该组件的依赖组件名（可以是none，表示无依赖组件），
                                //选中该组件时，被依赖组件将强制选中，
                                //如果依赖多个组件，则依次列出，用“,”分隔
//weakdependence:"none"          //该组件的弱依赖组件名（可以是none，表示无依赖组件），
                                //选中该组件时，被依赖组件不会被强制选中，
                                //如果依赖多个组件，则依次列出，用“,”分隔
//mutex:"none"                  //该组件的互斥组件名（可以是none，表示无互斥组件），
                                //如果与多个组件互斥，则依次列出，用“,”分隔
//%$#@end describe  ****组件描述结束

//%$#@configue      ****参数配置开始
#if ( CFG_MODULE_ENABLE_TOUCHSCREEN_ICNT8952 == false )
//#warning  " touchscreen_ICNT8952  组件参数未配置，使用默认配置"
//%$#@target = header           //header = 生成头文件,cmdline = 命令行变量，DJYOS自有模块禁用
#define CFG_MODULE_ENABLE_TOUCHSCREEN_ICNT8952    false //如果勾选了本组件，将由DIDE在project_config.h或命令行中定义为true
//%$#@enum,true,false,
//%$#@string,1,128,
#define CFG_ICNT8952_BUS_NAME     "IIC1"        //"IIC总线名称",触摸芯片使用的IIC总线名称
#define CFG_ICNT8952_TOUCH_NAME   "icnt8952"       //"触摸屏名称",配置触摸屏名称
#define CFG_TARGET_DISPLAY_NAME "LCD_F7"      //"触屏所在显示器名称",配置触摸屏所在显示器的名称
//%$#@select,        ***从列出的选项中选择若干个定义成宏
//%$#@free,
#endif
//%$#@end configue  ****参数配置结束
//@#$%component end configure

#define ICNT8952_ADDR                0x48       //7bit address
#define ICNT8952_ADDR_BITS           16

#define ICNT8952_INFO_START         0
#define ICNT8952_INFO_SIZE          16
#define ICNT8952_COMMAND            0x0004
#define ICNT8952_CONFIG_VERSION     0x000b     //2 bytes
//#define ICNT8952_CONFIG_CHKSUM      0x80FF
#define ICNT8952_PRODUCT_ID         0x000f
//#define ICNT8952_SENSOR_ID          0x814A

#define ICNT8952_TOUCH_NUM          0x1001  //读触点数地址
#define ICNT8952_TP1_REG            0X1002  //第一个触摸点数据地址
#define ICNT8952_TP2_REG            0X1009  //第二个触摸点数据地址
#define ICNT8952_TP3_REG            0X1010  //第三个触摸点数据地址
#define ICNT8952_TP4_REG            0X1017  //第四个触摸点数据地址
#define ICNT8952_TP5_REG            0X101e  //第五个触摸点数据地址
#define ICNT8952_TP6_REG            0X1025  //第六个触摸点数据地址
#define ICNT8952_TP7_REG            0X102c  //第七个触摸点数据地址
#define ICNT8952_TP8_REG            0X1033  //第八个触摸点数据地址
#define ICNT8952_TP9_REG            0X103a  //第九个触摸点数据地址
#define ICNT8952_TP10_REG           0X1041  //第十个触摸点数据地址

#define ICNT8952_MAX_TOUCH          10       //最大触摸点数



const u16 ICNT8952_TPX_TBL[]={ICNT8952_TP1_REG, ICNT8952_TP2_REG, ICNT8952_TP3_REG,
        ICNT8952_TP4_REG, ICNT8952_TP5_REG, ICNT8952_TP6_REG, ICNT8952_TP7_REG, ICNT8952_TP8_REG,
        ICNT8952_TP9_REG, ICNT8952_TP10_REG};

//定义IICBUS架构下的IIC设备结构
static struct IIC_Device *ps_ICNT8952_Dev = NULL;
static u32 s_ICNT8952_Timeout = CN_TIMEOUT_FOREVER;
static struct SingleTouchPrivate ICNT8952;
__attribute__((weak))  void ICNT8952_Pin_Init(void)
{
    return;
}
#if 0       //这是康和用的观海5寸屏的配置
u8 cfg_info_group[] = {\
    0x5D,0xE0,0x01,0x56,0x03,0x02,0x35,0x00,0x11,0x28,0x28,0x0A,0x64,0x4B,0x03,\
    0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x18,0x1A,0x1E,0x14,0x8A,0x29,0x0B,\
    0x30,0x32,0x06,0x04,0x00,0x00,0x02,0x9B,0x02,0x10,0x32,0x01,0x00,0x00,0x00,\
    0x03,0x64,0x32,0x00,0x00,0x00,0x1E,0x3C,0x94,0xD5,0x02,0x07,0x00,0x00,0x04,\
    0xB0,0x20,0x00,0x99,0x25,0x00,0x88,0x2A,0x00,0x78,0x30,0x00,0x68,0x38,0x00,\
    0x68,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
    0x00,0x00,0x00,0x03,0xFA,0x00,0x00,0x16,0x14,0x12,0x10,0x0E,0x0C,0x0A,0x08,\
    0x06,0x04,0x02,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,\
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0F,\
    0x10,0x12,0x24,0x22,0x21,0x20,0x1F,0x1E,0x1D,0x1C,0x18,0xFF,0xFF,0xFF,0xFF,\
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,\
    0xFF,0xFF,0xFF,0xFF,0x78,0x01
};
#endif
// =============================================================================
// 功能：    读函数，
// 参数：    reg: 寄存器地址 ; buf: 数据缓存; len: 数据长度
// 返回：    接收的长度
// =============================================================================
static bool_t ICNT8952_RD_Reg ( u16 reg,u8 *buf, u8 len)
{
    return IIC_Read(ps_ICNT8952_Dev, reg&0xffff, buf, len, s_ICNT8952_Timeout);
}
// =============================================================================
// 功能：  写寄存器
// 参数：  reg: 寄存器地址 ; buf: 数据缓存; len: 数据长度
// 返回：  写的长度
// =============================================================================
static  bool_t ICNT8952_WR_Reg (u16 reg, u8 *buf,u8 len)
{
   return  IIC_Write(ps_ICNT8952_Dev, reg, buf, len, true, s_ICNT8952_Timeout);
}

/* =============================================================================
 功能：ICNT8952芯片初始化，初始化和加载设备到对应的IIC总线.
 参数：无
 返回：true,成功;false,失败
 =============================================================================*/
static bool_t ICNT8952_Init( )
{
    u8 temp[ICNT8952_INFO_SIZE];
//    u8 test_buf[512];
    u8 count = 0, Val_1 = 0, Val_2 = 0;
    s8 ret = 0;
    s32 x,y;
    LcdTp_Reset_OnOff(0);         //RST 0
    DJY_DelayUs(1);
    LcdTp_Reset_OnOff(1);         //RST 1
    DJY_EventDelay(50000);
    ret = ICNT8952_RD_Reg(ICNT8952_INFO_START, temp, ICNT8952_INFO_SIZE);
    if (ret == ICNT8952_INFO_SIZE)
    {
        printf("IC Version: %02X%02X.\r\n",temp[10], temp[11]);
        return true;
    }
    else
        return false;
}

//扫描触摸屏(采用查询方式)
//mode:0,正常扫描.
//返回值:当前触屏状态.
// 1=触摸笔按下，0=触摸笔提起，
static bool_t ICNT8952_Scan(struct SingleTouchMsg *touch_data)
{
    u8 buf[8];
    if(ICNT8952_RD_Reg(ICNT8952_TOUCH_NUM, buf, 8))
    {
//        printf("touch num: %d.\r\n",touch_num);
        //不响应抬起手指的消息，这个消息由系统自动产生
        //不区分move和stay，只把是否有触摸和当前坐标告诉系统即可
        if((buf[7] > 0) && (buf[7] < 4))    //0:None 1:Down 2:Move 3:Stay 4:Up
        {
            touch_data->x = buf[2] + ((s32)buf[3] << 8);
            touch_data->y = buf[4] + ((s32)buf[5] << 8);;
            touch_data->z = buf[4];
//            printf("touch_data->x = %d\r\n",touch_data->x);
//            printf("touch_data->y = %d\r\n",touch_data->y);
//            printf("touch_data->z = %d\r\n",touch_data->z);
//            printf("touch_data->id = %d\r\n",buf[5]);
            return 1;
        }
        else
            return 0;
    }
    else
        return 0;
}

// =============================================================================
// 功能：ICNT8952模块初始化，
// 参数：无
// 返回：true,成功;false,失败
// =============================================================================
bool_t ModuleInstall_ICNT8952(void)
{
    bool_t result = false;
    static struct IIC_Device* s_ICNT8952_Dev;

    //添加ICNT8952到IIC总线
    s_ICNT8952_Dev = IIC_DevAdd(CFG_ICNT8952_BUS_NAME,"IIC_Dev_ICNT8952", ICNT8952_ADDR, 0, ICNT8952_ADDR_BITS);
    if(NULL != s_ICNT8952_Dev)
    {
        IIC_BusCtrl(s_ICNT8952_Dev,CN_IIC_SET_CLK,100*1000,0);
//        IIC_BusCtrl(s_ICNT8952_Dev,CN_IIC_SET_POLL,0,0);
        ps_ICNT8952_Dev = s_ICNT8952_Dev;

        ICNT8952.read_touch = ICNT8952_Scan;    //读触摸点的坐标函数
        ICNT8952.touch_loc.display = GK_GetDisplay(CFG_TARGET_DISPLAY_NAME);
        result=ICNT8952_Init();
        if(!result)
            return false;
        result=Touch_InstallDevice(CFG_ICNT8952_TOUCH_NAME,&ICNT8952);//添加驱动到Touch
        if(!result)
            return false;

        result = true;
    }

    return result;
}

void touch_test(void)
{
    u8 temp[6];
    u8 i,touch_num = 0, Val_1 = 0;
    u16 xy;

    while(1)
    {
        do
        {
            ICNT8952_RD_Reg(0x814E, &Val_1, 1);
            DJY_EventDelay(10000);
        }while(!(Val_1 & (1 << 7)));
//        ICNT8952_RD_Reg(0x814f, &Val, 1);
        if(Val_1 & (1 << 7))
        {
            touch_num = Val_1&0x0f;
            printf("touch_num = %d \r\n",touch_num);
            if(touch_num == 0)
            {
                ICNT8952_WR_Reg(0x814E,&touch_num,1);
                printf("没有触摸\r\n");
            }
            else
            {
//                for(i=0;i<touch_num;i++)
//                {
                   ICNT8952_RD_Reg(0X814f,temp,7); //读取XY坐标值
                   printf("track id = %d \r\n",temp[0]);
                   xy = temp[5] | (temp[6] << 8);
                   printf("size = %d \r\n", xy);
                   xy = temp[1] | (temp[2] << 8);
                   printf("x=%d.\r\n",xy);
                   xy = temp[3] | (temp[4] << 8);
                   printf("y=%d.\r\n",xy);
//                }
            }
//            else
//            {
//                printf("touch_num = %d ",touch_num);
//            }
        }
//        if(ICNT8952_RD_Reg(0x814f, &temp[0], 7) == 7)
//        {
//            printf("track id = %d \r\n",temp[0]);
//            xy = temp[5] | (temp[6] << 8);
//            printf("size = %d \r\n", xy);
//            xy = temp[1] | (temp[2] << 8);
//            printf("x=%d.\r\n",xy);
//            xy = temp[3] | (temp[4] << 8);
//            printf("y=%d.\r\n",xy);
////            DJY_EventDelay(1000*1000);
//        }
        touch_num = 0;
        ICNT8952_WR_Reg(0x814E,&touch_num,1);
    }
}
