//----------------------------------------------------
// Copyright (c) 2018, Djyos Open source Development team. All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. As a constituent part of djyos,do not transplant it to other software
//    without specific prior written permission.

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
// 这份授权条款，在使用者符合以下三条件的情形下，授予使用者使用及再散播本
// 软件包装原始码及二进位可执行形式的权利，无论此包装是否经改作皆然：
//
// 1. 对于本软件源代码的再散播，必须保留上述的版权宣告、本条件列表，以
//    及下述的免责声明。
// 2. 对于本套件二进位可执行形式的再散播，必须连带以文件以及／或者其他附
//    于散播包装中的媒介方式，重制上述之版权宣告、本条件列表，以及下述
//    的免责声明。
// 3. 本软件作为都江堰操作系统的组成部分，未获事前取得的书面许可，不允许移植到非
//    都江堰操作系统环境下运行。

// 免责声明：本软件是本软件版权持有人以及贡献者以现状（"as is"）提供，
// 本软件包装不负任何明示或默示之担保责任，包括但不限于就适售性以及特定目
// 的的适用性为默示性担保。版权持有人及本软件之贡献者，无论任何条件、
// 无论成因或任何责任主体、无论此责任为因合约关系、无过失责任主体或因非违
// 约之侵权（包括过失或其他原因等）而起，对于任何因使用本软件包装所产生的
// 任何直接性、间接性、偶发性、特殊性、惩罚性或任何结果的损害（包括但不限
// 于替代商品或劳务之购用、使用损失、资料损失、利益损失、业务中断等等），
// 不负任何责任，即在该种使用已获事前告知可能会造成此类损害的情形下亦然。
//-----------------------------------------------------------------------------

//所属模块: GB2312-80 16*16点阵@宋体驱动
//作者：mjh
//版本：V1.0.0
//文件描述: GB2312-80字体解析
//其他说明:
//修订历史:
//    2. ...
//    1. 日期:2011-05-17
//       作者:lst
//       新版本号：
//       修改说明:初始版本
//------------------------------------------------------
#include <stdbool.h>
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include "gkernel.h"
#include "charset.h"
#include "dbug.h"
#include "./font/font.h"
#include "./font/font_GB2312.h"
#include "project_config.h"     //本文件由IDE中配置界面生成，存放在APP的工程目录中。
                                //允许是个空文件，所有配置将按默认值配置。
#define zk_disable      0
#define from_file       1
#define from_array      2
//@#$%component configure   ****组件配置开始，用于 DIDE 中图形化配置界面
//****配置块的语法和使用方法，参见源码根目录下的文件：component_config_readme.txt****
//%$#@initcode      ****初始化代码开始，由 DIDE 删除“//”后copy到初始化文件中
//  extern void ModuleInstall_FontGB2312_16_16_songti(void);
//  ModuleInstall_FontGB2312_16_16_songti();
//%$#@end initcode  ****初始化代码结束

//%$#@describe      ****组件描述开始
//component name:"GB2312 16_16点阵@宋体"//GB2312 16_16 点阵@宋体字库
//parent:"font"                 //填写该组件的父组件名字，none表示没有父组件
//attribute:system              //选填“third、system、bsp、user”，本属性用于在IDE中分组
//select:choosable              //选填“required、choosable、none”，若填必选且需要配置参数，则IDE裁剪界面中默认勾取，
                                //不可取消，必选且不需要配置参数的，或是不可选的，IDE裁剪界面中不显示，
//init time:medium              //初始化时机，可选值：early，medium，later, pre-main。
                                //表示初始化时间，分别是早期、中期、后期
//dependence:"font"             //该组件的依赖组件名（可以是none，表示无依赖组件），
                                //选中该组件时，被依赖组件将强制选中，
                                //如果依赖多个组件，则依次列出，用“,”分隔
//weakdependence:"none"         //该组件的弱依赖组件名（可以是none，表示无依赖组件），
                                //选中该组件时，被依赖组件不会被强制选中，
                                //如果依赖多个组件，则依次列出，用“,”分隔
//mutex:                        //该组件的互斥组件名（可以是none，表示无互斥组件），
                                //如果与多个组件互斥，则依次列出，用“,”分隔
//%$#@end describe  ****组件描述结束

//%$#@configue      ****参数配置开始
#if ( CFG_MODULE_ENABLE_GB2312_16_16_DOT_songti == false )
//#warning  " GB2312_dot  组件参数未配置，使用默认配置"
//%$#@target = header           //header = 生成头文件,cmdline = 命令行变量，DJYOS自有模块禁用
#define CFG_MODULE_ENABLE_GB2312_16_16_DOT_songti    false //如果勾选了本组件，将由DIDE在project_config.h或命令行中定义为true
//%$#@enum,zk_disable,from_file,from_array
#define CFG_GB2312_16_16_songti_1BIT         from_array              //"非抗锯齿16点阵@宋体",GB2312字体,zk_disable：不需要，from_array：从数组读取，from_file：从文件读
//%$#@string,0,255,
    #define CFG_GB2312_16_16_songti_1BIT_FILENAME "dot_GB2312_songti_16_16_1bit_with_ascii.bin"  //"字库文件名",若从文件读取，则配置文件名
//%$#@enum,zk_disable,from_file,from_array
#define CFG_GB2312_16_16_songti_2BIT         zk_disable              //"2bit抗锯齿16点阵@宋体",GB2312字体,zk_disable：不需要，from_array：从数组读取，from_file：从文件读
//%$#@string,0,255,
    #define CFG_GB2312_16_16_songti_2BIT_FILENAME "dot_GB2312_songti_16_16_2bit_with_ascii.bin"  //"字库文件名",若从文件读取，则配置文件名
//%$#@enum,zk_disable,from_file,from_array
#define CFG_GB2312_16_16_songti_4BIT         zk_disable              //"4bit抗锯齿16点阵@宋体",GB2312字体,zk_disable：不需要，from_array：从数组读取，from_file：从文件读
//%$#@string,0,255,
    #define CFG_GB2312_16_16_songti_4BIT_FILENAME "dot_GB2312_songti_16_16_4bit_with_ascii.bin"  //"字库文件名",若从文件读取，则配置文件名
//%$#@enum,zk_disable,from_file,from_array
#define CFG_GB2312_16_16_songti_8BIT         zk_disable              //"8bit抗锯齿16点阵@宋体",GB2312字体,zk_disable：不需要，from_array：从数组读取，from_file：从文件读
//%$#@string,0,255,
    #define CFG_GB2312_16_16_songti_8BIT_FILENAME "dot_GB2312_songti_16_16_8bit_with_ascii.bin"  //"字库文件名",若从文件读取，则配置文件名
//%$#@string,0,255,
//%$#select,        ***从列出的选项中选择若干个定义成宏
//%$#@free,
#endif
//%$#@end configue  ****参数配置结束
//@#$%component end configure


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#define FONT_GB2312_CHARS       7445        //GB2312中文字符数量
#define FONT_ASCII_CHARS        95          //ascii字符数量
//16点阵字模定义
#define FONT_GB2312_16_16_W 20              // 汉字 Width
#define FONT_GB2312_16_16_H 20              // 汉字 Height
#define FONT_GB2312_16_16_ASCII_W 10               // ASCII Width

#define FONT_GB2312_16_16_1BIT_GLYPH_BYTES  ((FONT_GB2312_16_16_W*4+7)/8*FONT_GB2312_16_16_H)               // 无抗锯齿汉字字模的字节数
#define FONT_GB2312_16_16_1BIT_ASCII_BYTES  ((FONT_GB2312_16_16_ASCII_W*1+7)/8*FONT_GB2312_16_16_H)         // 无抗锯齿ASCII的字节数
#define FONT_GB2312_16_16_1BIT_LINEBYTES    ((FONT_GB2312_16_16_W*4+7)/8)                                   // 汉字每行字节数
#define FONT_GB2312_16_16_1BIT_A_LINEBYTES  ((FONT_GB2312_16_16_ASCII_W*1+7)/8)                             // ASCII每行字节数

#define FONT_GB2312_16_16_2BIT_GLYPH_BYTES  ((FONT_GB2312_16_16_W*2+7)/8*FONT_GB2312_16_16_H)               // 2位色深抗锯齿汉字字模的字节数
#define FONT_GB2312_16_16_2BIT_ASCII_BYTES  ((FONT_GB2312_16_16_ASCII_W*2+7)/8*FONT_GB2312_16_16_H)         // 2位色深抗锯齿ASCII的字节数
#define FONT_GB2312_16_16_2BIT_LINEBYTES    ((FONT_GB2312_16_16_W*2+7)/8)                                   // 汉字每行字节数
#define FONT_GB2312_16_16_2BIT_A_LINEBYTES  ((FONT_GB2312_16_16_ASCII_W*2+7)/8)                             // ASCII每行字节数

#define FONT_GB2312_16_16_4BIT_GLYPH_BYTES  ((FONT_GB2312_16_16_W*4+7)/8*FONT_GB2312_16_16_H)               // 4位色深抗锯齿汉字字模的字节数
#define FONT_GB2312_16_16_4BIT_ASCII_BYTES  ((FONT_GB2312_16_16_ASCII_W*4+7)/8*FONT_GB2312_16_16_H)         // 4位色深抗锯齿ASCII的字节数
#define FONT_GB2312_16_16_4BIT_LINEBYTES    ((FONT_GB2312_16_16_W*4+7)/8)                                   // 汉字每行字节数
#define FONT_GB2312_16_16_4BIT_A_LINEBYTES  ((FONT_GB2312_16_16_ASCII_W*4+7)/8)                             // ASCII每行字节数

#define FONT_GB2312_16_16_8BIT_GLYPH_BYTES  ((FONT_GB2312_16_16_W*8+7)/8*FONT_GB2312_16_16_H)               // 8位色深抗锯齿汉字字模的字节数
#define FONT_GB2312_16_16_8BIT_ASCII_BYTES  ((FONT_GB2312_16_16_ASCII_W*8+7)/8*FONT_GB2312_16_16_H)         // 8位色深抗锯齿ASCII的字节数
#define FONT_GB2312_16_16_8BIT_LINEBYTES    ((FONT_GB2312_16_16_W*8+7)/8)                                   // 汉字每行字节数
#define FONT_GB2312_16_16_8BIT_A_LINEBYTES  ((FONT_GB2312_16_16_ASCII_W*8+7)/8)                             // ASCII每行字节数

s32 __GB2312_GetOffset(u8 region, u8 position);

//----获取字符宽度-------------------------------------------------------------
//功能: 取某字符的点阵宽度
//参数: CharCode,被查询的字符
//返回: 字符宽度
//-----------------------------------------------------------------------------
static s32 __Font_GB2312_16_16_GetCharWidth(const char * CharCode)
{
    int a=0;


    switch((u8)CharCode[0])
    {
        case '\r':
        case '\n':
            a=0;
            break;
        default:
            if((u8)CharCode[0]<0x80)
            {
                a=FONT_GB2312_16_16_ASCII_W;
            }
            else
            {
                a=FONT_GB2312_16_16_W;
            }
            break;
    }

    return a;

}

//----获取字符高度-------------------------------------------------------------
//功能: 取某字符的点阵高度
//参数: CharCode,被查询的字符
//返回: 字符高度
//-----------------------------------------------------------------------------
static s32 __Font_GB2312_16_16_GetCharHeight(const char * CharCode)
{
    return FONT_GB2312_16_16_H;
}

#if CFG_GB2312_16_16_songti_1BIT != zk_disable

bool_t __Font_GB2312_16_16_LoadFromFile_songti_1bit(const char* FileName);
void __Font_GB2312_16_16_UnLoadFromFile_songti_1bit(void);
bool_t __Font_GB2312_16_16_GetCharBitMap_songti_1bit(char * charcode, s32 width,s32 height,
                                    struct RectBitmap *bitmap);

u8 *pg_GB2312_16_16_songti_1bit;
static struct FontObj pg_font_GB2312_16_16_songti_1bit;

//----点阵提取-----------------------------------------------------------------
//功能: 提取16点阵@宋体汉字和ascii 8*16点阵字体，汉字仅限于GB2312字符集内，超出的
//      显示GB2312编码为0xa1a1的字符，ascii仅限于基本ascii码，对于扩展的ascii
//      不予显示
//      如果bitmap参数中的bm_bits参数为NULL，则不copy点阵
//参数: charcode，待显示的字符指针，可能是GB2312字符集内的汉字，也可能是ascii码
//      width，height，字符尺寸，本函数不用
//      bitmap，保存所提取的点阵的位图，缓冲区由调用者提供
//返回: true=正常，false=charcode不是该字体所支持的字符集范围，但此时仍然返回
//      默认字符的点阵
//-----------------------------------------------------------------------------
bool_t __Font_GB2312_16_16_GetCharBitMap_songti_1bit(char * charcode, s32 width,s32 height,
                                    struct RectBitmap *bitmap)
{
    u32 offset;
    bool_t result = true;
    // if(pg_font_GB2312_16_16_songti_1bit.Attr & CN_FONT_ALPHA)
    //     bitmap->PixelFormat = CN_SYS_PF_ALPHA4;
    // else
    //     bitmap->PixelFormat = CN_SYS_PF_GRAY4;
    if((u8)charcode[0] < 0x80)
    {
        bitmap->PixelFormat = CN_SYS_PF_GRAY1;
        offset = ((u8)charcode[0] - 32) * FONT_GB2312_16_16_1BIT_ASCII_BYTES;
        bitmap->width = FONT_GB2312_16_16_ASCII_W;
        bitmap->height = 20;
        bitmap->linebytes = FONT_GB2312_16_16_1BIT_A_LINEBYTES;
        if(bitmap->bm_bits != NULL)
        {
            memcpy(bitmap->bm_bits,pg_GB2312_16_16_songti_1bit+offset,FONT_GB2312_16_16_1BIT_ASCII_BYTES);
        }
    }else
    {
        bitmap->PixelFormat = CN_SYS_PF_ALPHA4;
        // gbk 文本文件中，汉字按大端存储
        offset = __GB2312_GetOffset(charcode[0], charcode[1])*FONT_GB2312_16_16_1BIT_GLYPH_BYTES
                        +FONT_ASCII_CHARS*FONT_GB2312_16_16_1BIT_ASCII_BYTES;
        bitmap->width = FONT_GB2312_16_16_W;
        bitmap->height = FONT_GB2312_16_16_H;
        bitmap->linebytes = FONT_GB2312_16_16_1BIT_LINEBYTES;
        if(bitmap->bm_bits != NULL)
        {
            memcpy(bitmap->bm_bits,pg_GB2312_16_16_songti_1bit+offset,FONT_GB2312_16_16_1BIT_GLYPH_BYTES);
        }
    }

    return result;
}

#if CFG_GB2312_16_16_songti_1BIT == from_array

#include "dot_GB2312_songti_16_16_1bit_with_ascii.rsc"

#else       //CFG_GB2312_16_16_songti_1BIT == from_array

bool_t __Font_GB2312_16_16_LoadFromFile_songti_1bit(const char* FileName)
{
    bool_t result;
    FILE *zk_GB2312;
    //从字库中加载汉字点阵
    zk_GB2312 = fopen(FileName,"rb");
    if(zk_GB2312 == NULL)
        return false;
    pg_GB2312_16_16_songti_1bit = M_Malloc(FONT_GB2312_16_16_1BIT_GLYPH_BYTES*FONT_GB2312_CHARS,0);
    if(pg_GB2312_16_16_songti_1bit != NULL)
    {
        fread(pg_GB2312_16_16_songti_1bit,FONT_GB2312_16_16_1BIT_GLYPH_BYTES*FONT_GB2312_CHARS,1,zk_GB2312);
        result = true;
    }
    else
        result = false;
    fclose(zk_GB2312);
    return result;
}

void __Font_GB2312_16_16_UnLoadFromFile_songti_1bit(void)
{
    free(pg_GB2312_16_16_songti_1bit);
}

#endif      //CFG_GB2312_16_16_songti_1BIT == from_array

//----安装GB2312 16点阵字体----------------------------------------------------
//功能: 安装GB2312 16点阵字体,字模保存在文件中,文件名由参数传入。
//参数: 无
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t ModuleInstall_FontGB2312_16_16_songti_1bit(void)
{
//  s_ptGB2312Set = Charset_NlsSearchCharset(CN_NLS_CHARSET_GB2312);
//  if (s_ptGB2312Set == NULL)
//  {
//      debug_printf("dot-GB2312","GB2312 字符集未安装\n\r");
//      return false;
//  }

    pg_font_GB2312_16_16_songti_1bit.Width = FONT_GB2312_16_16_W;
    pg_font_GB2312_16_16_songti_1bit.Height = FONT_GB2312_16_16_H;
    pg_font_GB2312_16_16_songti_1bit.Attr = CN_FONT_TYPE_DOT|CN_FONT_ALPHA;
#if CFG_GB2312_16_16_songti_1BIT == from_array
    pg_GB2312_16_16_songti_1bit = (u8*)dot_GB2312_songti_16_16_1bit_with_ascii;
    pg_font_GB2312_16_16_songti_1bit.LoadFont = NULL;
    pg_font_GB2312_16_16_songti_1bit.UnloadFont = NULL;
#else       //CFG_GB2312_16_16_songti_1BIT == from_array
    __Font_GB2312_16_16_LoadFromFile_songti_1bit(CFG_GB2312_16_16_songti_1BIT_FILENAME);  //加载字库
    pg_font_GB2312_16_16_songti_1bit.LoadFont = __Font_GB2312_16_16_LoadFromFile_songti_1bit;
    pg_font_GB2312_16_16_songti_1bit.UnloadFont = __Font_GB2312_16_16_UnLoadFromFile_songti_1bit;
#endif      //CFG_GB2312_16_16_songti_1BIT == from_array
    pg_font_GB2312_16_16_songti_1bit.GetBitmap = __Font_GB2312_16_16_GetCharBitMap_songti_1bit;
    pg_font_GB2312_16_16_songti_1bit.GetOneChar = __Font_GB2312GetOneChar;
    pg_font_GB2312_16_16_songti_1bit.GetCharWidth = __Font_GB2312_16_16_GetCharWidth;
    pg_font_GB2312_16_16_songti_1bit.GetCharHeight = __Font_GB2312_16_16_GetCharHeight;

    if(Font_InstallFont(&pg_font_GB2312_16_16_songti_1bit, "GB2312_16_16_songti_1bit"))
    {
        debug_printf("dot-GB2312","GB2312 16点阵@宋体安装完成\n\r");
        return 1;
    }else
    {
        DJY_SaveLastError(EN_GK_FONT_INSTALL_ERROR);
        debug_printf("dot-GB2312","GB2312 16点阵@宋体安装失败\n\r");
        return 0;
    }
}

#endif      //CFG_GB2312_16_16_songti_1BIT != zk_disable

#if CFG_GB2312_16_16_songti_2BIT != zk_disable

bool_t __Font_GB2312_16_16_LoadFromFile_songti_2bit(const char* FileName);
void __Font_GB2312_16_16_UnLoadFromFile_songti_2bit(void);
bool_t __Font_GB2312_16_16_GetCharBitMap_songti_2bit(char * charcode, s32 width,s32 height,
                                    struct RectBitmap *bitmap);

u8 *pg_GB2312_16_16_songti_2bit;
static struct FontObj pg_font_GB2312_16_16_songti_2bit;

//----点阵提取-----------------------------------------------------------------
//功能: 提取16点阵@宋体汉字和ascii 8*16点阵字体，汉字仅限于GB2312字符集内，超出的
//      显示GB2312编码为0xa1a1的字符，ascii仅限于基本ascii码，对于扩展的ascii
//      不予显示
//      如果bitmap参数中的bm_bits参数为NULL，则不copy点阵
//参数: charcode，待显示的字符指针，可能是GB2312字符集内的汉字，也可能是ascii码
//      width，height，字符尺寸，本函数不用
//      bitmap，保存所提取的点阵的位图，缓冲区由调用者提供
//返回: true=正常，false=charcode不是该字体所支持的字符集范围，但此时仍然返回
//      默认字符的点阵
//-----------------------------------------------------------------------------
bool_t __Font_GB2312_16_16_GetCharBitMap_songti_2bit(char * charcode, s32 width,s32 height,
                                    struct RectBitmap *bitmap)
{
    u32 offset;
    bool_t result = true;
    if(pg_font_GB2312_16_16_songti_2bit.Attr & CN_FONT_ALPHA)
        bitmap->PixelFormat = CN_SYS_PF_ALPHA2;
    else
        bitmap->PixelFormat = CN_SYS_PF_GRAY2;
    if((u8)charcode[0] < 0x80)
    {
        offset = ((u8)charcode[0] - 32) * FONT_GB2312_16_16_2BIT_ASCII_BYTES;
        bitmap->width = FONT_GB2312_16_16_ASCII_W;
        bitmap->height = FONT_GB2312_16_16_H;
        bitmap->linebytes = FONT_GB2312_16_16_2BIT_A_LINEBYTES;
        if(bitmap->bm_bits != NULL)
        {
            memcpy(bitmap->bm_bits,pg_GB2312_16_16_songti_2bit+offset,FONT_GB2312_16_16_2BIT_ASCII_BYTES);
        }
    }else
    {
        // gbk 文本文件中，汉字按大端存储
        offset = __GB2312_GetOffset(charcode[0], charcode[1])*FONT_GB2312_16_16_2BIT_GLYPH_BYTES
                        +FONT_ASCII_CHARS*FONT_GB2312_16_16_2BIT_ASCII_BYTES;
        bitmap->width = FONT_GB2312_16_16_W;
        bitmap->height = FONT_GB2312_16_16_H;
        bitmap->linebytes = FONT_GB2312_16_16_2BIT_LINEBYTES;
        if(bitmap->bm_bits != NULL)
        {
            memcpy(bitmap->bm_bits,pg_GB2312_16_16_songti_2bit+offset,FONT_GB2312_16_16_2BIT_GLYPH_BYTES);
        }
    }

    return result;
}

#if CFG_GB2312_16_16_songti_2BIT == from_array

#include "dot_GB2312_songti_16_16_2bit_with_ascii.rsc"

#else       //CFG_GB2312_16_16_songti_2BIT == from_array

bool_t __Font_GB2312_16_16_LoadFromFile_songti_2bit(const char* FileName)
{
    bool_t result;
    FILE *zk_GB2312;
    //从字库中加载汉字点阵
    zk_GB2312 = fopen(FileName,"rb");
    if(zk_GB2312 == NULL)
        return false;
    pg_GB2312_16_16_songti_2bit = M_Malloc(FONT_GB2312_16_16_2BIT_GLYPH_BYTES*FONT_GB2312_CHARS,0);
    if(pg_GB2312_16_16_songti_2bit != NULL)
    {
        fread(pg_GB2312_16_16_songti_2bit,FONT_GB2312_16_16_2BIT_GLYPH_BYTES*FONT_GB2312_CHARS,1,zk_GB2312);
        result = true;
    }
    else
        result = false;
    fclose(zk_GB2312);
    return result;
}

void __Font_GB2312_16_16_UnLoadFromFile_songti_2bit(void)
{
    free(pg_GB2312_16_16_songti_2bit);
}

#endif      //CFG_GB2312_16_16_songti_2BIT == from_array

//----安装GB2312 16点阵字体----------------------------------------------------
//功能: 安装GB2312 16点阵字体,字模保存在文件中,文件名由参数传入。
//参数: 无
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t ModuleInstall_FontGB2312_16_16_songti_2bit(void)
{
//  s_ptGB2312Set = Charset_NlsSearchCharset(CN_NLS_CHARSET_GB2312);
//  if (s_ptGB2312Set == NULL)
//  {
//      debug_printf("dot-GB2312","GB2312 字符集未安装\n\r");
//      return false;
//  }

    pg_font_GB2312_16_16_songti_2bit.Width = FONT_GB2312_16_16_W;
    pg_font_GB2312_16_16_songti_2bit.Height = FONT_GB2312_16_16_H;
    pg_font_GB2312_16_16_songti_2bit.Attr = CN_FONT_TYPE_DOT;
#if CFG_GB2312_16_16_songti_2BIT == from_array
    pg_GB2312_16_16_songti_2bit = (u8*)dot_GB2312_songti_16_16_2bit_with_ascii;
    pg_font_GB2312_16_16_songti_2bit.LoadFont = NULL;
    pg_font_GB2312_16_16_songti_2bit.UnloadFont = NULL;
#else       //CFG_GB2312_16_16_songti_2BIT == from_array
    __Font_GB2312_16_16_LoadFromFile_songti_2bit(CFG_GB2312_16_16_songti_2BIT_FILENAME);  //加载字库
    pg_font_GB2312_16_16_songti_2bit.LoadFont = __Font_GB2312_16_16_LoadFromFile_songti_2bit;
    pg_font_GB2312_16_16_songti_2bit.UnloadFont = __Font_GB2312_16_16_UnLoadFromFile_songti_2bit;
#endif      //CFG_GB2312_16_16_songti_2BIT == from_array
    pg_font_GB2312_16_16_songti_2bit.GetBitmap = __Font_GB2312_16_16_GetCharBitMap_songti_2bit;
    pg_font_GB2312_16_16_songti_2bit.GetOneChar = __Font_GB2312GetOneChar;
    pg_font_GB2312_16_16_songti_2bit.GetCharWidth = __Font_GB2312_16_16_GetCharWidth;
    pg_font_GB2312_16_16_songti_2bit.GetCharHeight = __Font_GB2312_16_16_GetCharHeight;

    if(Font_InstallFont(&pg_font_GB2312_16_16_songti_2bit, "GB2312_16_16_songti_2bit"))
    {
        debug_printf("dot-GB2312","GB2312 16点阵@宋体安装完成\n\r");
        return 1;
    }else
    {
        DJY_SaveLastError(EN_GK_FONT_INSTALL_ERROR);
        debug_printf("dot-GB2312","GB2312 16点阵@宋体安装失败\n\r");
        return 0;
    }
}

#endif      //CFG_GB2312_16_16_songti_2BIT != zk_disable

#if CFG_GB2312_16_16_songti_4BIT != zk_disable

bool_t __Font_GB2312_16_16_LoadFromFile_songti_4bit(const char* FileName);
void __Font_GB2312_16_16_UnLoadFromFile_songti_4bit(void);
bool_t __Font_GB2312_16_16_GetCharBitMap_songti_4bit(char * charcode, s32 width,s32 height,
                                    struct RectBitmap *bitmap);

u8 *pg_GB2312_16_16_songti_4bit;
static struct FontObj pg_font_GB2312_16_16_songti_4bit;

//----点阵提取-----------------------------------------------------------------
//功能: 提取16点阵@宋体汉字和ascii 8*16点阵字体，汉字仅限于GB2312字符集内，超出的
//      显示GB2312编码为0xa1a1的字符，ascii仅限于基本ascii码，对于扩展的ascii
//      不予显示
//      如果bitmap参数中的bm_bits参数为NULL，则不copy点阵
//参数: charcode，待显示的字符指针，可能是GB2312字符集内的汉字，也可能是ascii码
//      width，height，字符尺寸，本函数不用
//      bitmap，保存所提取的点阵的位图，缓冲区由调用者提供
//返回: true=正常，false=charcode不是该字体所支持的字符集范围，但此时仍然返回
//      默认字符的点阵
//-----------------------------------------------------------------------------
bool_t __Font_GB2312_16_16_GetCharBitMap_songti_4bit(char * charcode, s32 width,s32 height,
                                    struct RectBitmap *bitmap)
{
    u32 offset;
    bool_t result = true;
    if(pg_font_GB2312_16_16_songti_4bit.Attr & CN_FONT_ALPHA)
        bitmap->PixelFormat = CN_SYS_PF_ALPHA4;
    else
        bitmap->PixelFormat = CN_SYS_PF_GRAY4;
    if((u8)charcode[0] < 0x80)
    {
        offset = ((u8)charcode[0] - 32) * FONT_GB2312_16_16_4BIT_ASCII_BYTES;
        bitmap->width = FONT_GB2312_16_16_ASCII_W;
        bitmap->height = FONT_GB2312_16_16_H;
        bitmap->linebytes = FONT_GB2312_16_16_4BIT_A_LINEBYTES;
        if(bitmap->bm_bits != NULL)
        {
            memcpy(bitmap->bm_bits,pg_GB2312_16_16_songti_4bit+offset,FONT_GB2312_16_16_4BIT_ASCII_BYTES);
        }
    }else
    {
        // gbk 文本文件中，汉字按大端存储
        offset = __GB2312_GetOffset(charcode[0], charcode[1])*FONT_GB2312_16_16_4BIT_GLYPH_BYTES
                        +FONT_ASCII_CHARS*FONT_GB2312_16_16_4BIT_ASCII_BYTES;
        bitmap->width = FONT_GB2312_16_16_W;
        bitmap->height = FONT_GB2312_16_16_H;
        bitmap->linebytes = FONT_GB2312_16_16_4BIT_LINEBYTES;
        if(bitmap->bm_bits != NULL)
        {
            memcpy(bitmap->bm_bits,pg_GB2312_16_16_songti_4bit+offset,FONT_GB2312_16_16_4BIT_GLYPH_BYTES);
        }
    }

    return result;
}

#if CFG_GB2312_16_16_songti_4BIT == from_array

#include "dot_GB2312_songti_16_16_4bit_with_ascii.rsc"

#else       //CFG_GB2312_16_16_songti_4BIT == from_array

bool_t __Font_GB2312_16_16_LoadFromFile_songti_4bit(const char* FileName)
{
    bool_t result;
    FILE *zk_GB2312;
    //从字库中加载汉字点阵
    zk_GB2312 = fopen(FileName,"rb");
    if(zk_GB2312 == NULL)
        return false;
    pg_GB2312_16_16_songti_4bit = M_Malloc(FONT_GB2312_16_16_4BIT_GLYPH_BYTES*FONT_GB2312_CHARS,0);
    if(pg_GB2312_16_16_songti_4bit != NULL)
    {
        fread(pg_GB2312_16_16_songti_4bit,FONT_GB2312_16_16_4BIT_GLYPH_BYTES*FONT_GB2312_CHARS,1,zk_GB2312);
        result = true;
    }
    else
        result = false;
    fclose(zk_GB2312);
    return result;
}

void __Font_GB2312_16_16_UnLoadFromFile_songti_4bit(void)
{
    free(pg_GB2312_16_16_songti_4bit);
}

#endif      //CFG_GB2312_16_16_songti_4BIT == from_array

//----安装GB2312 16点阵字体----------------------------------------------------
//功能: 安装GB2312 16点阵字体,字模保存在文件中,文件名由参数传入。
//参数: 无
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t ModuleInstall_FontGB2312_16_16_songti_4bit(void)
{
//  s_ptGB2312Set = Charset_NlsSearchCharset(CN_NLS_CHARSET_GB2312);
//  if (s_ptGB2312Set == NULL)
//  {
//      debug_printf("dot-GB2312","GB2312 字符集未安装\n\r");
//      return false;
//  }

    pg_font_GB2312_16_16_songti_4bit.Width = FONT_GB2312_16_16_W;
    pg_font_GB2312_16_16_songti_4bit.Height = FONT_GB2312_16_16_H;
    pg_font_GB2312_16_16_songti_4bit.Attr = CN_FONT_TYPE_DOT;
#if CFG_GB2312_16_16_songti_4BIT == from_array
    pg_GB2312_16_16_songti_4bit = (u8*)dot_GB2312_songti_16_16_4bit_with_ascii;
    pg_font_GB2312_16_16_songti_4bit.LoadFont = NULL;
    pg_font_GB2312_16_16_songti_4bit.UnloadFont = NULL;
#else       //CFG_GB2312_16_16_songti_4BIT == from_array
    __Font_GB2312_16_16_LoadFromFile_songti_4bit(CFG_GB2312_16_16_songti_4BIT_FILENAME);  //加载字库
    pg_font_GB2312_16_16_songti_4bit.LoadFont = __Font_GB2312_16_16_LoadFromFile_songti_4bit;
    pg_font_GB2312_16_16_songti_4bit.UnloadFont = __Font_GB2312_16_16_UnLoadFromFile_songti_4bit;
#endif      //CFG_GB2312_16_16_songti_4BIT == from_array
    pg_font_GB2312_16_16_songti_4bit.GetBitmap = __Font_GB2312_16_16_GetCharBitMap_songti_4bit;
    pg_font_GB2312_16_16_songti_4bit.GetOneChar = __Font_GB2312GetOneChar;
    pg_font_GB2312_16_16_songti_4bit.GetCharWidth = __Font_GB2312_16_16_GetCharWidth;
    pg_font_GB2312_16_16_songti_4bit.GetCharHeight = __Font_GB2312_16_16_GetCharHeight;

    if(Font_InstallFont(&pg_font_GB2312_16_16_songti_4bit, "GB2312_16_16_songti_4bit"))
    {
        debug_printf("dot-GB2312","GB2312 16点阵@宋体安装完成\n\r");
        return 1;
    }else
    {
        DJY_SaveLastError(EN_GK_FONT_INSTALL_ERROR);
        debug_printf("dot-GB2312","GB2312 16点阵@宋体安装失败\n\r");
        return 0;
    }
}

#endif      //CFG_GB2312_16_16_songti_4BIT != zk_disable

#if CFG_GB2312_16_16_songti_8BIT != zk_disable

bool_t __Font_GB2312_16_16_LoadFromFile_songti_8bit(const char* FileName);
void __Font_GB2312_16_16_UnLoadFromFile_songti_8bit(void);
bool_t __Font_GB2312_16_16_GetCharBitMap_songti_8bit(char * charcode, s32 width,s32 height,
                                    struct RectBitmap *bitmap);

u8 *pg_GB2312_16_16_songti_8bit;
static struct FontObj pg_font_GB2312_16_16_songti_8bit;

//----点阵提取-----------------------------------------------------------------
//功能: 提取16点阵@宋体汉字和ascii 8*16点阵字体，汉字仅限于GB2312字符集内，超出的
//      显示GB2312编码为0xa1a1的字符，ascii仅限于基本ascii码，对于扩展的ascii
//      不予显示
//      如果bitmap参数中的bm_bits参数为NULL，则不copy点阵
//参数: charcode，待显示的字符指针，可能是GB2312字符集内的汉字，也可能是ascii码
//      width，height，字符尺寸，本函数不用
//      bitmap，保存所提取的点阵的位图，缓冲区由调用者提供
//返回: true=正常，false=charcode不是该字体所支持的字符集范围，但此时仍然返回
//      默认字符的点阵
//-----------------------------------------------------------------------------
bool_t __Font_GB2312_16_16_GetCharBitMap_songti_8bit(char * charcode, s32 width,s32 height,
                                    struct RectBitmap *bitmap)
{
    u32 offset;
    bool_t result = true;
    if(pg_font_GB2312_16_16_songti_8bit.Attr & CN_FONT_ALPHA)
        bitmap->PixelFormat = CN_SYS_PF_ALPHA8;
    else
        bitmap->PixelFormat = CN_SYS_PF_GRAY8;
    if((u8)charcode[0] < 0x80)
    {
        offset = ((u8)charcode[0] - 32) * FONT_GB2312_16_16_8BIT_ASCII_BYTES;
        bitmap->width = FONT_GB2312_16_16_ASCII_W;
        bitmap->height = FONT_GB2312_16_16_H;
        bitmap->linebytes = FONT_GB2312_16_16_8BIT_A_LINEBYTES;
        if(bitmap->bm_bits != NULL)
        {
            memcpy(bitmap->bm_bits,pg_GB2312_16_16_songti_8bit+offset,FONT_GB2312_16_16_8BIT_ASCII_BYTES);
        }
    }else
    {
        // gbk 文本文件中，汉字按大端存储
        offset = __GB2312_GetOffset(charcode[0], charcode[1])*FONT_GB2312_16_16_8BIT_GLYPH_BYTES
                        +FONT_ASCII_CHARS*FONT_GB2312_16_16_8BIT_ASCII_BYTES;
        bitmap->width = FONT_GB2312_16_16_W;
        bitmap->height = FONT_GB2312_16_16_H;
        bitmap->linebytes = FONT_GB2312_16_16_8BIT_LINEBYTES;
        if(bitmap->bm_bits != NULL)
        {
            memcpy(bitmap->bm_bits,pg_GB2312_16_16_songti_8bit+offset,FONT_GB2312_16_16_8BIT_GLYPH_BYTES);
        }
    }

    return result;
}

#if CFG_GB2312_16_16_songti_8BIT == from_array

#include "dot_GB2312_songti_16_16_8bit_with_ascii.rsc"

#else       //CFG_GB2312_16_16_songti_8BIT == from_array

bool_t __Font_GB2312_16_16_LoadFromFile_songti_8bit(const char* FileName)
{
    bool_t result;
    FILE *zk_GB2312;
    //从字库中加载汉字点阵
    zk_GB2312 = fopen(FileName,"rb");
    if(zk_GB2312 == NULL)
        return false;
    pg_GB2312_16_16_songti_8bit = M_Malloc(FONT_GB2312_16_16_8BIT_GLYPH_BYTES*FONT_GB2312_CHARS,0);
    if(pg_GB2312_16_16_songti_8bit != NULL)
    {
        fread(pg_GB2312_16_16_songti_8bit,FONT_GB2312_16_16_8BIT_GLYPH_BYTES*FONT_GB2312_CHARS,1,zk_GB2312);
        result = true;
    }
    else
        result = false;
    fclose(zk_GB2312);
    return result;
}

void __Font_GB2312_16_16_UnLoadFromFile_songti_8bit(void)
{
    free(pg_GB2312_16_16_songti_8bit);
}

#endif      //CFG_GB2312_16_16_songti_8BIT == from_array

//----安装GB2312 16点阵字体----------------------------------------------------
//功能: 安装GB2312 16点阵字体,字模保存在文件中,文件名由参数传入。
//参数: 无
//返回: true=成功，false=失败
//-----------------------------------------------------------------------------
bool_t ModuleInstall_FontGB2312_16_16_songti_8bit(void)
{
//  s_ptGB2312Set = Charset_NlsSearchCharset(CN_NLS_CHARSET_GB2312);
//  if (s_ptGB2312Set == NULL)
//  {
//      debug_printf("dot-GB2312","GB2312 字符集未安装\n\r");
//      return false;
//  }

    pg_font_GB2312_16_16_songti_8bit.Width = FONT_GB2312_16_16_W;
    pg_font_GB2312_16_16_songti_8bit.Height = FONT_GB2312_16_16_H;
    pg_font_GB2312_16_16_songti_8bit.Attr = CN_FONT_TYPE_DOT;
#if CFG_GB2312_16_16_songti_8BIT == from_array
    pg_GB2312_16_16_songti_8bit = (u8*)dot_GB2312_songti_16_16_8bit_with_ascii;
    pg_font_GB2312_16_16_songti_8bit.LoadFont = NULL;
    pg_font_GB2312_16_16_songti_8bit.UnloadFont = NULL;
#else       //CFG_GB2312_16_16_songti_8BIT == from_array
    __Font_GB2312_16_16_LoadFromFile_songti_8bit(CFG_GB2312_16_16_songti_8BIT_FILENAME);  //加载字库
    pg_font_GB2312_16_16_songti_8bit.LoadFont = __Font_GB2312_16_16_LoadFromFile_songti_8bit;
    pg_font_GB2312_16_16_songti_8bit.UnloadFont = __Font_GB2312_16_16_UnLoadFromFile_songti_8bit;
#endif      //CFG_GB2312_16_16_songti_8BIT == from_array
    pg_font_GB2312_16_16_songti_8bit.GetBitmap = __Font_GB2312_16_16_GetCharBitMap_songti_8bit;
    pg_font_GB2312_16_16_songti_8bit.GetOneChar = __Font_GB2312GetOneChar;
    pg_font_GB2312_16_16_songti_8bit.GetCharWidth = __Font_GB2312_16_16_GetCharWidth;
    pg_font_GB2312_16_16_songti_8bit.GetCharHeight = __Font_GB2312_16_16_GetCharHeight;

    if(Font_InstallFont(&pg_font_GB2312_16_16_songti_8bit, "GB2312_16_16_songti_8bit"))
    {
        debug_printf("dot-GB2312","GB2312 16点阵@宋体安装完成\n\r");
        return 1;
    }else
    {
        DJY_SaveLastError(EN_GK_FONT_INSTALL_ERROR);
        debug_printf("dot-GB2312","GB2312 16点阵@宋体安装失败\n\r");
        return 0;
    }
}

#endif      //CFG_GB2312_16_16_songti_8BIT != zk_disable

#pragma GCC diagnostic pop

void ModuleInstall_FontGB2312_16_16_songti(void)
{
    #if CFG_GB2312_16_16_songti_1BIT != zk_disable
    ModuleInstall_FontGB2312_16_16_songti_1bit( );
    #endif      //CFG_GB2312_16_16_songti_1BIT != zk_disable
    #if CFG_GB2312_16_16_songti_2BIT != zk_disable
    ModuleInstall_FontGB2312_16_16_songti_2bit( );
    #endif      //CFG_GB2312_16_16_songti_2BIT != zk_disable
    #if CFG_GB2312_16_16_songti_4BIT != zk_disable
    ModuleInstall_FontGB2312_16_16_songti_4bit( );
    #endif      //CFG_GB2312_16_16_songti_4BIT != zk_disable
    #if CFG_GB2312_16_16_songti_8BIT != zk_disable
    ModuleInstall_FontGB2312_16_16_songti_8bit( );
    #endif      //CFG_GB2312_16_16_songti_8BIT != zk_disable
}
