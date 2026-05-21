//----------------------------------------------------
//Copyright (C), 2004-2011,  罗侍田.
//版权所有 (C), 2004-2011,   罗侍田.
//所属模块:时钟模块
//作者:  罗侍田
//版本：V1.0.0
//文件描述: windows窗口模拟液晶驱动
//其他说明:
//修订历史:
// 1. 日期: 2012-10-09
//   作者:  罗侍田
//   新版本号: V1.0.0
//   修改说明: 原始版本
//------------------------------------------------------
#include <stdbool.h>
#include <windows.h>
#include <tchar.h>
#include <string.h>
//#include <process.h>
#include <winGDI.h>
#include "stdint.h"
#include "Lcd_Touch_Key.h"
#include "gkernel.h"
#include "hmi-input.h"
#include "cpu_peri.h"
#include "cpu.h"
#include <gui/gk_display.h>
#include "touch.h"
#include "project_config.h"

//@#$%component configure   ****组件配置开始，用于 DIDE 中图形化配置界面
//****配置块的语法和使用方法，参见源码根目录下的文件：component_config_readme.txt****
//****新建组件，可以copy component_config_template.txt的内容到这里。
//%$#@initcode      ****初始化代码开始，由 DIDE 删除“//”后copy到初始化文件中
//    extern bool_t ModuleInstall_TouchWindows (void);
//    ModuleInstall_TouchWindows( );
//#if(CFG_MODULE_ENABLE_GRAPHICAL_DECORATE_DEVELOPMENT == true)
//    extern bool_t GDD_AddInputDev(const char *InputDevName);
//    GDD_AddInputDev(CFG_WINDOWS_TOUCH_NAME);
//#endif
//%$#@end initcode  ****初始化代码结束

//%$#@describe      ****组件描述开始
//component name:"touch and mouse" //组件名
//parent:"touch"                 //填写该组件的父组件名字，none表示没有父组件
//attribute:bsp                 //选填“third、system、bsp、user”，本属性用于在IDE中分组
//select:choosable              //选填“required、choosable、none”，若填必选且需要配置参数，则IDE裁剪界面中默认勾取，
                                //不可取消，必选且不需要配置参数的，或是不可选的，IDE裁剪界面中不显示，
//init time:medium              //初始化时机，可选值：early，medium，later, pre-main。
                                //表示初始化时间，分别是早期、中期、后期
//dependence:"touch"            //该组件的依赖组件名（可以是none，表示无依赖组件），
                                //选中该组件时，被依赖组件将强制选中，
                                //如果依赖多个组件，则依次列出，用“,”分隔
//weakdependence:"graphical decorate development"         //该组件的弱依赖组件名（可以是none，表示无依赖组件），
                                //选中该组件时，被依赖组件不会被强制选中，
                                //如果依赖多个组件，则依次列出，用“,”分隔
//mutex:"none"                  //该组件的互斥组件名（可以是none，表示无互斥组件），
                                //如果与多个组件互斥，则依次列出，用“,”分隔
//%$#@end describe  ****组件描述结束

//%$#@configue      ****参数配置开始
#if ( CFG_MODULE_ENABLE_TOOUCH_MOUSE == false )
//#warning  " myname 组件参数未配置，使用默认配置"
//%$#@target = header           //header = 生成头文件,cmdline = 命令行变量，DJYOS自有模块禁用
#define CFG_MODULE_ENABLE_TOOUCH_MOUSE    false //如果勾选了本组件，将由DIDE在project_config.h或命令行中定义为true
//%$#@num,0,100,
//%$#@enum,true,false,
//%$#@string,1,127,
#define CFG_WINDOWS_TOUCH_NAME   "WINDOWS-TOUCH"       //"触摸屏名称",配置触摸屏名称
#define CFG_TARGET_DISPLAY_NAME  "WINDOWS-LCD"      //"触屏所在显示器名称",配置触摸屏所在显示器的名称
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

bool_t touched = false;
s32 touch_x=0,touch_y=0,touch_z=0;

//因为windows只有一个鼠标，只能模拟一个定位设备，触摸屏和鼠标
//是不能同时使能的

//----读取触摸点坐标-----------------------------------------------------------
//功能: 读取stmpe811采集到的触摸点坐标，如果有多点，则平均之
//参数: touch_data，采集到的坐标
//返回: 1=触摸笔按下，0=触摸笔提起，
//-----------------------------------------------------------------------------
ufast_t read_touch_windows(struct SingleTouchMsg *touch_data)
{
    touch_data->display = NULL;
    touch_data->z = touch_z;
    if(touched == true)
    {
        touch_data->x = touch_x;
        touch_data->y = touch_y;
        return 1;
    }
    else
    {
        return 0;
    }
}

//----初始化触摸屏模块---------------------------------------------------------
//功能:
//参数: display_name,本触摸屏对应的显示器名(资源名)
//返回: 无
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_TouchWindows(void)
{
    static struct SingleTouchPrivate windows_touch;
    windows_touch.read_touch = read_touch_windows;
    windows_touch.touch_loc.display = GK_GetDisplay(CFG_TARGET_DISPLAY_NAME);
    Touch_InstallDevice(CFG_WINDOWS_TOUCH_NAME,&windows_touch);
    return 1;
}

