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

//所属模块: 字体驱动
//作者：lst
//版本：V1.0.0
//文件描述: 字体资源头文件
//其他说明:
//修订历史:
//    2. ...
//    1. 日期:2009-11-21
//       作者:lst
//       新版本号：
//       修改说明:初始版本
//------------------------------------------------------
#ifndef __FONT_H__
#define __FONT_H__
#include "stdint.h"
#include "gkernel.h"
#ifdef __cplusplus
extern "C" {
#endif

struct RectBitmap;
#define CN_FONT_RSC_TREE            "font"

/* 字体属性 */
#define CN_FONT_TYPE_DOT            (1<<0)      // 1=点阵,0=矢量
#define CN_FONT_ALPHA               (1<<1)      // 1=alpha使能，0=不使能

struct FontObj
{
    struct Object *HostObj;

    s32 Width;       //最宽字符的宽度,纵向显示时可用作为竖行宽
    s32 Height;      //最高字符的高度
    u32 Attr;

    bool_t (*LoadFont)(void *zk_addr);      //加载字体

    void (*UnloadFont)(void);      //卸载字体

// 获取字体对应编码编码为charcode字符的显示点阵，把点阵填充到font_bitmap中，调用者应该提
// 供font_bitmap所需内存。
// 如果font_bitmap参数中的bm_bits参数为NULL，则本函数退化为查询函数。在
// font_bitmap参数中返回图像参数，利用它可以计算bm_bits所需内存尺寸，这在一个文
// 档中使用多种尺寸的文字时，特别有用。
// width,height,字符尺寸，一般矢量字库才支持，点阵字库都是固定尺寸。
// 返回: true = 成功执行，false=不支持字符
    bool_t (*GetBitmap)(char *charcode, s32 width, s32 height,
                        struct RectBitmap *font_bitmap);

//----从字符串中获取一个字符---------------------------------------------------
//功能: 按该字体对应的编码格式，获取一个字符。
//参数: outstr，保存转换结果的指针，若为NULL,则不返回转换结果
//      instr, 指向待转换的多字节字符字节序列的指针(由调用函数判断s的合法性)
//      n，最大检测长度，-1表示检测全串。
//返回: 被转换的字符长度（字节数），遇到非法字符则返回 -1
    s32 (*GetOneChar)(char * outstr, const char* instr, s32 n);
    s32 (*GetCharWidth)(const char* instr);      //获取某字符宽度
    s32 (*GetCharHeight)(const char* instr);     //获取某字符高度
};

bool_t Font_InstallFont(struct FontObj *font,const char *name);

struct FontObj* Font_GetCurFont(void);
struct FontObj* Font_SetCurFont(struct FontObj* font);
struct FontObj* Font_SearchFont(const char* name);
s32 Font_GetFontLineHeight(struct FontObj* font);
s32 Font_GetFontLineWidth(struct FontObj* font);
u32 Font_GetFontAttr(struct FontObj* font);
u32 Font_SetFontAttr(struct FontObj* font,u32 Attr);
u32 Font_ClrFontAttr(struct FontObj* font,u32 AttrMsk);

#ifdef __cplusplus
}
#endif

#endif //__FONT_H__

