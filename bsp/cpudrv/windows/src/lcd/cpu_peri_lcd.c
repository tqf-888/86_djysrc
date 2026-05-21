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
//extern ptu32_t ModuleInstall_Lcd(char *DisplayName,char *HeapName);
//  ModuleInstall_Lcd(CFG_SIMULATOR_DISPLAY_NAME,NULL);
//%$#@end initcode  ****初始化代码结束

//%$#@describe      ****组件描述开始
//component name:"windows-lcd" //组件名
//parent:"graphical kernel"     //填写该组件的父组件名字，none表示没有父组件
//attribute:bsp                 //选填“third、system、bsp、user”，本属性用于在IDE中分组
//select:choosable              //选填“required、choosable、none”，若填必选且需要配置参数，则IDE裁剪界面中默认勾取，
                                //不可取消，必选且不需要配置参数的，或是不可选的，IDE裁剪界面中不显示，
//init time:medium              //初始化时机，可选值：early，medium，later, pre-main。
                                //表示初始化时间，分别是早期、中期、后期
//dependence:"graphical kernel" //该组件的依赖组件名（可以是none，表示无依赖组件），
                                //选中该组件时，被依赖组件将强制选中，
                                //如果依赖多个组件，则依次列出，用“,”分隔
//weakdependence:"none"          //该组件的弱依赖组件名（可以是none，表示无依赖组件），
                                //选中该组件时，被依赖组件不会被强制选中，
                                //如果依赖多个组件，则依次列出，用“,”分隔
//mutex:"none"                  //该组件的互斥组件名（可以是none，表示无互斥组件），
                                //如果与多个组件互斥，则依次列出，用“,”分隔
//%$#@end describe  ****组件描述结束

//%$#@configue      ****参数配置开始
#if ( CFG_MODULE_ENABLE_WINDOWS_LCD == false )
//#warning  " myname 组件参数未配置，使用默认配置"
//%$#@target = header           //header = 生成头文件,cmdline = 命令行变量，DJYOS自有模块禁用
#define CFG_MODULE_ENABLE_WINDOWS_LCD    false //如果勾选了本组件，将由DIDE在project_config.h或命令行中定义为true
//%$#@num,0,65536,
#define CFG_LCD_XSIZE   480             //"LCD宽度",
#define CFG_LCD_YSIZE   320             //"LCD高度",
//%$#@enum,true,false,
//%$#@string,1,127,
#define CFG_SIMULATOR_DISPLAY_NAME        "WINDOWS-LCD"    //"显示器名称",配置液晶显示的名称
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

//坐标系说明,描述了显存偏移地址与屏上像素点的坐标映射关系,注意单色和灰度显示器
//每字节可能包含不止一个像素点,图中没有加以区分.
//不同的系统有不同的对应关系,这种变化影响 部分显示函数的编写方法.
//应用程序以及gui驱动程序无需关心这种映射关系.
//例外的是,如果你用数组与屏幕直接映射的方法显示bmp图形,需要了解这种映射关系.
//----------------------------------------------------->x   右上角
// |0x0000,0x0001,0x0002--------------------------xmax-1
// |xmax,xmax+1,xmax+2---------------------------2*max-1
// |----------------------------------------------------
// |----------------------------------------------------
// |----------------------------------------------------
// |(ymax-1)*xmax,(ymax-1)*xmax+1------------ymax*xmax-1    右下角
//\/

#define CN_LCD_PIXEL_FORMAT       CN_SYS_PF_RGB565

#define cn_lcd_pixel_size   (((CN_LCD_PIXEL_FORMAT>>8)&0xff)/8)
#define cn_lcd_line_size    (CFG_LCD_XSIZE*cn_lcd_pixel_size)

HDC     lcd_hdc ;
BITMAPINFO *lcdbmi;
TCHAR szLcdDriver[]       =_TEXT("都江堰操作系统");    // 标题栏文本

LRESULT CALLBACK    WndProc_lcd(HWND, UINT, WPARAM, LPARAM);

struct DisplayObj tg_lcd_display;

extern bool_t touched;
extern s32 touch_x,touch_y,touch_z;
extern HINSTANCE hInstance;
RECT tg_WindowRect;
s32 s32g_window_height,s32g_window_width;

extern HWND tg_keyboard_wnd;

u8 *pg_frame_buffer;

u16* getframebuff()
{
   return pg_frame_buffer;

}

bool_t __lcd_line_bm_ie(struct RectBitmap *bitmap,struct Rectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code);
bool_t __lcd_blt_bm_to_bm( struct RectBitmap *dst_bitmap,
                            struct Rectangle *DstRect,
                            struct RectBitmap *src_bitmap,
                            struct Rectangle *SrcRect,
                            struct RopGroup RopCode, u32 HyalineColor);
//----初始化lcd硬件------------------------------------------------------------
//功能: 如名
//参数: 无
//返回: 无
//备注: 本函数是移植关键函数.
//----------------------------------------------------------------------------
void __lcd_hard_init(void)
{
    RECT ClientRect;
    HWND lcd_wnd;
//    HBITMAP hbmi;
    lcdbmi = malloc(sizeof(BITMAPINFO)+64);
    lcdbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lcdbmi->bmiHeader.biWidth = CFG_LCD_XSIZE;
    lcdbmi->bmiHeader.biHeight = CFG_LCD_YSIZE;
    lcdbmi->bmiHeader.biPlanes = 1;
    lcdbmi->bmiHeader.biSizeImage = CFG_LCD_XSIZE*CFG_LCD_YSIZE*2;
    lcdbmi->bmiHeader.biXPelsPerMeter = 1;
    lcdbmi->bmiHeader.biYPelsPerMeter = 1;
    lcdbmi->bmiHeader.biClrUsed = 0;
    lcdbmi->bmiHeader.biClrImportant = 0;
    lcdbmi->bmiHeader.biBitCount = 16;
    lcdbmi->bmiHeader.biCompression = BI_BITFIELDS;
    DWORD* masks = (DWORD*)lcdbmi->bmiColors;
    masks[0] = 0xF800; // 红色掩码 (5 位)
    masks[1] = 0x07E0; // 绿色掩码 (6 位)
    masks[2] = 0x001F; // 蓝色掩码 (5 位)

    CreateDIBSection(0,lcdbmi,DIB_RGB_COLORS,(void**)&pg_frame_buffer,NULL,0);
    RegisterWinClass(hInstance,WndProc_lcd,szLcdDriver);
    //WS_CAPTION在djygui中也有定义，应使用其在windows中的值： 0xc00000
    lcd_wnd = CreateWindow(szLcdDriver, szLcdDriver, 0xc00000 | WS_MINIMIZEBOX,
                       CW_USEDEFAULT,CW_USEDEFAULT,CFG_LCD_XSIZE,CFG_LCD_YSIZE,
                       NULL, NULL, hInstance, NULL);

    if (!lcd_wnd)
    {
        return;
    }
    GetWindowRect(lcd_wnd, &tg_WindowRect);
    GetClientRect(lcd_wnd, &ClientRect);
    tg_WindowRect.right += (CFG_LCD_XSIZE - ClientRect.right);
    tg_WindowRect.bottom += (CFG_LCD_YSIZE - ClientRect.bottom);
    s32g_window_height =  tg_WindowRect.bottom-tg_WindowRect.top;
    s32g_window_width = tg_WindowRect.right-tg_WindowRect.left;
    MoveWindow(lcd_wnd, tg_WindowRect.left, tg_WindowRect.top, s32g_window_width, s32g_window_height, false);

    ShowWindow(lcd_wnd, SW_SHOW);
    UpdateWindow(lcd_wnd);

    lcd_hdc = GetDC (lcd_wnd) ;
}

//
//  函数: WndProc_lcd(HWND, unsigned, WORD, LONG)
//
//  目的: 处理主窗口的消息。
//
LRESULT CALLBACK WndProc_lcd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT         ps ;
    switch (message)
    {
//        case WM_PAINT:
//          BeginPaint (hWnd, &ps) ;
//          StretchDIBits (lcd_hdc,    0, 0,CFG_LCD_XSIZE, CFG_LCD_YSIZE,
//                     0, CFG_LCD_YSIZE-10, CFG_LCD_XSIZE,-CFG_LCD_YSIZE,pg_frame_buffer,
//                      lcdbmi,DIB_RGB_COLORS,SRCCOPY) ;
//          EndPaint (hWnd, &ps) ;
//          printf("re paint main windows\n\r");
//            break;
        case WM_MOVE:
            BeginPaint (hWnd, &ps) ;
            GetWindowRect(hWnd, &tg_WindowRect);
            MoveWindow(tg_keyboard_wnd, tg_WindowRect.left+s32g_window_width + 5,
                     tg_WindowRect.top, s32g_window_width, s32g_window_height,
                     true);
            EndPaint (hWnd, &ps) ;
            break ;
//        case WM_ACTIVATE:     //好像没什么用，注释掉 lst
//            BeginPaint (hWnd, &ps) ;
//            printf("i activing\n\r");
////            StretchDIBits (lcd_hdc,    0, 0,CFG_LCD_XSIZE, CFG_LCD_YSIZE,
////                        0, CFG_LCD_YSIZE, CFG_LCD_XSIZE,-CFG_LCD_YSIZE,pg_frame_buffer,
////                        lcdbmi,DIB_RGB_COLORS,SRCCOPY) ;
//
//            EndPaint (hWnd, &ps) ;
//            break ;
        case WM_LBUTTONDOWN:
            touch_x = LOWORD(lParam);
            touch_y = HIWORD(lParam);
            touch_z = 1;
            touched = true;
            break;
        case WM_LBUTTONUP:
            touch_x = LOWORD(lParam);
            touch_y = HIWORD(lParam);
            touch_z = 0;
            touched = false;
            break;
        case WM_MOUSEMOVE:
            touch_x = LOWORD(lParam);
            touch_y = HIWORD(lParam);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


//----LCD控制信号------------------------------------------------------------
//功能: LCD 视频输出和逻辑使能/禁止,1=允许视频输出和 LCD控制信号,
//      0=禁止视频输出和 LCD控制信号
//参数: onoff，控制信号
//返回: 无
//-----------------------------------------------------------------------------
void __lcd_envid_of(int onoff)
{

}
//----控制LCD电源--------------------------------------------------------------
//功能: LCD 电源控制引脚使能
//参数: invpwren，
//      pwren,
//返回: 无
//-----------------------------------------------------------------------------
void __lcd_power_enable(int invpwren,int pwren)
{

}
//----控制显示器---------------------------------------------------------------
//功能: 这是由driver提供商编写的，用于设置显示器各种参数的函数，属于应用程序的
//      范畴，功能由设备厂商定义。
//参数: disp，显示器指针
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_disp_ctrl(struct Displayobj *disp, s32 cmd, ...)
{
    return true;
}


//----位图中画像素-------------------------------------------------------------
//功能: 在位图中画一个像素，只有在bitmap的像素格式为cn_custom_pf时，才需要绘制。
//      如果显卡不打算支持自定义格式，本函数直接返回。
//参数: bitmap，目标位图
//      limit，限制矩形，只绘制在该矩形内部的部分。
//      x，y，坐标
//      color，绘图用的颜色
//      r2_code，二元光栅操作码
//返回: 无
//-----------------------------------------------------------------------------
bool_t __lcd_set_pixel_bm(struct RectBitmap *bitmap,
                     s32 x,s32 y,u32 color,u32 r2_code)
{
    return false;
}
//----位图中画线---------------------------------------------------------------
//功能: 在位图中画一条直线，不绘制(x2,y2)点，只绘制在limit限定的区域内的部分。
//参数: bitmap，目标位图
//      limit，限制矩形，只绘制在该矩形内部的部分。
//      x1,y1,x2,y2，起点终点坐标
//      color，绘图用的颜色
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线。
//-----------------------------------------------------------------------------
bool_t __lcd_line_bm(struct RectBitmap *bitmap,struct Rectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----位图中画线(含端点)-------------------------------------------------------
//功能: 在位图中画一条直线，只绘制在limit限定的区域内的部分
//参数: bitmap，目标位图
//      limit，限制矩形，只绘制在该矩形内部的部分。
//      x1,y1,x2,y2，起点终点坐标
//      color，绘图用的颜色
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线。
//-----------------------------------------------------------------------------
bool_t __lcd_line_bm_ie(struct RectBitmap *bitmap,struct Rectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----位图中填充矩形-----------------------------------------------------------
//功能: 把位图中的矩形用color颜色填充
//参数: bitmap，目标位图
//      dst_rect,待填充的矩形
//      color，填充颜色
//返回:  true=成功绘制，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_fill_rect_bm(struct RectBitmap *dst_bitmap,
                          struct Rectangle *Target,
                          struct Rectangle *Focus,
                          u32 Color0,u32 Color1,u32 Mode, s32 radius)
{
    return false;
}
//----bitmap中位块传送---------------------------------------------------------
//功能: 把一个位图中的一个矩形区域传送到另一个区域，两个位图的像素格式不必相同。
//      源和目的位图可以相同
//参数: dst_bitmap，目标位图
//      dst_rect，目标矩形
//      src_bitmap，源位图
//      xsrc,ysrc，源位图中被传送的区域左上角坐标
//      pat，pattern指针
//      mask，rop4操作需要的掩码
//      rop_code，三元光栅操作码
//      transparentcolor，关键颜色
//返回: true=成功,false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_blt_bm_to_bm( struct RectBitmap *dst_bitmap,
                            struct Rectangle *DstRect,
                            struct RectBitmap *src_bitmap,
                            struct Rectangle *SrcRect,
                            struct RopGroup RopCode, u32 HyalineColor)
{
    return false;
}
//----读取位图中的像素---------------------------------------------------------
//功能: 取位图中的一个像素，只有在bitmap的像素格式为cn_custom_pf时，才需要读取。
//      如果显卡不打算支持自定义格式，本函数直接返回false。
//参数: bitmap，目标位图
//      x，y，坐标
//返回:  true=成功读取，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_get_pixel_bm(struct RectBitmap *bitmap,s32 x,s32 y)
{
    return false;
}
//----screen中画像素-----------------------------------------------------------
//功能: 在screen中画一个像素，有frame buffer的driver，直接返回即可
//参数: x,y,像素坐标
//      color，颜色
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=没有绘制，或无需绘制
//-----------------------------------------------------------------------------
bool_t __lcd_set_pixel_screen(s32 x,s32 y,u32 color,u32 r2_code)
{
    return false;
}
//----screen中画线---------------------------------------------------------------
//功能: 在screen中画一条任意直线，不含端点，只绘制在limit限定的区域内的部分。如
//      不支持画线的硬件加速，直接返回false。
//参数: limit，限制矩形，只绘制在该矩形内部的部分。
//      x1,y1,x2,y2，起点终点坐标
//      color，绘图用的颜色
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线。
//-----------------------------------------------------------------------------
bool_t __lcd_line_screen(struct Rectangle *limit,
                    s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----screen中画线(含端点)------------------------------------------------------
//功能: 在screen中画一条直线，只绘制在limit限定的区域内的部分
//参数: limit，限制矩形，只绘制在该矩形内部的部分。
//      x1,y1,x2,y2，起点终点坐标
//      color，绘图用的颜色
//      r2_code，二元光栅操作码
//返回: true=成功绘制，false=失败，无硬件加速或不支持按r2_code画线。
//-----------------------------------------------------------------------------
bool_t __lcd_line_screen_ie(struct Rectangle *limit,
                        s32 x1,s32 y1,s32 x2,s32 y2,u32 color,u32 r2_code)
{
    return false;
}
//----screen中填充矩形-----------------------------------------------------------
//功能: 把screen中的矩形用color颜色填充，color的格式是CN_SYS_PF_ERGB8888
//参数: dst_rect,待填充的矩形
//      color，填充颜色
//返回:  true=成功绘制，false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_fill_rect_screen(struct Rectangle *Target,
                              struct Rectangle *Focus,
                              u32 Color0,u32 Color1,u32 Mode)
{
    return false;
}

//----bitmap到screen位块传送---------------------------------------------------
//功能: 把一个位图中的一个矩形区域传送到另一个区域，两个位图的像素格式必须相同。
//      源和目的位图可以相同
//参数: dst_rect，目标矩形
//      src_bitmap，源位图
//      xsrc,ysrc，源位图中被传送的区域左上角坐标
//返回: true=成功,false=失败
//-----------------------------------------------------------------------------
bool_t __lcd_bm_to_screen(struct Rectangle *dst_rect,
                struct RectBitmap *src_bitmap,s32 xsrc,s32 ysrc)
{
    StretchDIBits (lcd_hdc,   dst_rect->left, dst_rect->top,
                            (dst_rect->right - dst_rect->left),
                            (dst_rect->bottom - dst_rect->top),
                            //这个+1有点莫名其妙，但不加，图形就错位
                            xsrc,ysrc + dst_rect->bottom - dst_rect->top+1,
                            dst_rect->right - dst_rect->left,
                            -(dst_rect->bottom - dst_rect->top),
                            pg_frame_buffer,lcdbmi,DIB_RGB_COLORS,SRCCOPY) ;
    return true;
}
//----从screen中取像素---------------------------------------------------------
//功能: 从screen中取一像素，并转换成CN_SYS_PF_ERGB8888或cn_sys_pf_a8r8g8b8格式
//参数: x,y，坐标
//返回: 像素颜色值
//-----------------------------------------------------------------------------
u32 __lcd_get_pixel_screen(s32 x,s32 y)
{
    //x86_windows下，就是利用windows提取函数提取。
    return 0;
}

//----读取bitmap中矩形块-------------------------------------------------------
//功能: 把一个位图中的矩形读取到另一个位图中
//参数: rect，欲读取的矩形。
//      dest，保存矩形的位图，其长宽必须与rect相同。
//返回: true=成功读取，false=失败，原因可能是提供了不支持的像素格式。
//-----------------------------------------------------------------------------
bool_t __lcd_get_rect_screen(struct Rectangle *rect,struct RectBitmap *dest)
{
    return false;
 }
//----初始化lcd设备------------------------------------------------------------
//功能: 如名
//参数: 无
//返回: 显示器资源指针
//-----------------------------------------------------------------------------
ptu32_t ModuleInstall_Lcd(char *DisplayName,char *HeapName)
{
    static struct GkWinObj frame_win;
    static struct RectBitmap FrameBitmap;
    __lcd_hard_init();
//  __lcd_power_enable(0,1);
//  __lcd_envid_of(1);

    FrameBitmap.bm_bits = (u8*)pg_frame_buffer;
    FrameBitmap.width = CFG_LCD_XSIZE;
    FrameBitmap.height = CFG_LCD_YSIZE;
    FrameBitmap.PixelFormat = CN_SYS_PF_RGB565;
//    FrameBitmap.PixelFormat = CN_SYS_PF_GRAY4;
    FrameBitmap.linebytes = cn_lcd_line_size;
//  FrameBitmap.linebytes = 160;
    FrameBitmap.ExColor = 0xffffff;
    frame_win.wm_bitmap = &FrameBitmap;
    tg_lcd_display.reset_clip = false;
    tg_lcd_display.frame_buffer = &frame_win;


    tg_lcd_display.width_um = 0;
    tg_lcd_display.height_um = 0;
    tg_lcd_display.width = CFG_LCD_XSIZE;
    tg_lcd_display.height = CFG_LCD_YSIZE;
//  tg_lcd_display.pixel_format = CN_SYS_PF_RGB565;
    tg_lcd_display.pixel_format = CN_SYS_PF_RGB565;
    tg_lcd_display.framebuf_direct = false;
    //无须初始化frame_buffer和desktop，z_topmost三个成员

    tg_lcd_display.draw.SetPixelToBitmap = __lcd_set_pixel_bm;
    tg_lcd_display.draw.FillRectToBitmap = __lcd_fill_rect_bm;
    tg_lcd_display.draw.LineToBitmap = __lcd_line_bm;
    tg_lcd_display.draw.LineToBitmapIe = __lcd_line_bm_ie;
    tg_lcd_display.draw.BltBitmapToBitmap = __lcd_blt_bm_to_bm;
//    tg_lcd_display.draw.check_raster = __lcd_check_raster;
    tg_lcd_display.draw.SetPixelToScreen = __lcd_set_pixel_screen;
    tg_lcd_display.draw.LineToScreen = __lcd_line_screen;
    tg_lcd_display.draw.LineToScreenIe = __lcd_line_screen_ie;
    tg_lcd_display.draw.FillRectToScreen = __lcd_fill_rect_screen;
    tg_lcd_display.draw.CopyBitmapToScreen = __lcd_bm_to_screen;
    tg_lcd_display.draw.GetPixelFromScreen = __lcd_get_pixel_screen;
    tg_lcd_display.draw.GetRectFromScreen = __lcd_get_rect_screen;

    tg_lcd_display.DisplayHeap = Heap_FindHeap(HeapName);
    tg_lcd_display.disp_ctrl = __lcd_disp_ctrl;

    if(GK_InstallDisplay(&tg_lcd_display,DisplayName))
        return (ptu32_t)&tg_lcd_display;
    else
        return 0;
}


