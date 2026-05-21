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

//所属模块: GB2312-80 16*16点阵宋体驱动
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
#include "gkernel.h"
#include "charset.h"
#include "dbug.h"
#include "./font/font.h"
#include "./font/font_gb2312.h"

#define FONT_GB2312_DEF_CHAR      0xa1a1

//-----------------------------------------------------------------------------
//功能：计算一个本地编码格式的GB2312字符在字符表中的序号，只计算该字符在有效GB2312编码中
//      的序号，如果该字符无效，将返回0（显示方框）。有效字符定义如下：
//      有定义字符总数: 7445

//      有定义编码区间:(后面是有定义编码的计数)
//      有定义编码：0xA1A1~0xA1FE，0~93
//      有定义编码：0xA2B1~0xA2E2，94~143
//      有定义编码：0xA2E5~0xA2EE，144~153
//      有定义编码：0xA2F1~0xA2FC，154~165
//      有定义编码：0xA3A1~0xA4F3，166~342
//      有定义编码：0xA5A1~0xA5F6，343~428
//      有定义编码：0xA6A1~0xA6B8，429~452
//      有定义编码：0xA6C1~0xA6D8，453~476
//      有定义编码：0xA7A1~0xA7C1，477~509
//      有定义编码：0xA7D1~0xA7F1，510~542
//      有定义编码：0xA8A1~0xA8BA，543~568
//      有定义编码：0xA8C5~0xA8E9，569~605
//      有定义编码：0xA9A4~0xA9EF，606~681
//      有定义编码：0xB0A1~0xD7F9，682~4436
//      有定义编码：0xD8A1~0xF7FE，4437~7444
//
//      未定义编码区间:(后面是其在GB2312编码中的序号)
//      未定义编码：0xA2A1~0xA2B0，94~109
//      未定义编码：0xA2E3~0xA2E4，160~161
//      未定义编码：0xA2EF~0xA2F0，172~173
//      未定义编码：0xA2FD~0xA2FE，186~187
//      未定义编码：0xA4F4~0xA4FE，365~375
//      未定义编码：0xA5F7~0xA5FE，462~469
//      未定义编码：0xA6B9~0xA6C0，494~501
//      未定义编码：0xA6D9~0xA6FE，526~563
//      未定义编码：0xA7C2~0xA7D0，597~611
//      未定义编码：0xA7F2~0xA7FE，645~657
//      未定义编码：0xA8BB~0xA8C4，684~693
//      未定义编码：0xA8EA~0xA9A3，731~754
//      未定义编码：0xA9F0~0xAFFE，831~1409
//      未定义编码：0xD7FA~0xD7FE，5165~5169
//参数：region，position，汉字区位码
//返回：给定汉字在有定义GB2312编码中的序号，即在字库中的偏移。错误编码则返回-1
//-----------------------------------------------------------------------------
s32 __GB2312_GetOffset(u8 region, u8 position)
{
    s32 offset;
    offset = 94*(region-0xa1) + position - 0xa1;    //由 Ucs4ToMb 保证字符的合法性
    if(offset > 8177)
        offset = -1;
    else if(offset > 5169)
        offset = offset -5169 +4436;
    else if(offset > 1409)
        offset = offset -1409 +681;
    else if(offset > 754)
        offset = offset -754 +605;
    else if(offset > 693)
        offset = offset -693 +568;
    else if(offset > 657)
        offset = offset -657 +542;
    else if(offset > 611)
        offset = offset -611 +509;
    else if(offset > 563)
        offset = offset -563 +476;
    else if(offset > 501)
        offset = offset -501 +452;
    else if(offset > 469)
        offset = offset -469 +428;
    else if(offset > 375)
        offset = offset -375 +342;
    else if(offset > 187)
        offset = offset -187 +165;
    else if(offset > 173)
        offset = offset -173 +153;
    else if(offset > 161)
        offset = offset -161 +143;
    else if(offset > 109)
        offset = offset -109 +93;
    return offset;
}

//----从字符串中获取一个字符---------------------------------------------------
//功能: 按该字体对应的编码格式，获取一个字符。
//参数: outstr，保存转换结果的指针，若为NULL,则不返回转换结果
//      instr, 指向待转换的多字节字符字节序列的指针
//      n，最大检测长度，-1表示检测全串。
//返回: 被转换的字符长度（字节数），遇到非法字符则返回 -1
//-----------------------------------------------------------------------------
s32 __Font_GB2312GetOneChar(char * outstr, const char* instr,s32 n)
{
    u8 c1,c2;
    u32 i;
    u32 wc;

    if((outstr ==NULL) || (instr ==NULL))
    {
        return 0;
    }
    c1 = (u8)instr[0];

    //是个ascii码
    if(c1 == 0)
    {
        return 0;
    }
    else if(c1 < 0x80)
    {
        *outstr = (char)c1;
        return 1;
    }
    if((n >= 2) || (n == -1) )
        c2 = (u8)instr[1];
    else
        return -1;

    if((c1>=0xa1)&&(c1<=0xf7) && (c2 >=0xa1)&&(c2<=0xfe))
    {
        outstr[0] = c1;
        outstr[1] = c2;
        return 2;
    }
    else
        return -1;
}

