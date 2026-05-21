/*
 * cpu_peri_qspi.c
 *
 *  Created on: 2020年5月6日
 *      Author: CK
 */


// =============================================================================
#include "string.h"
#include "djyos.h"
#include "cpu_peri.h"
#include <board.h>
#include <dbug.h>
#include <at32f435_437_qspi.h>
#include <at32f435_437_crm.h>
#include "heap.h"
// =============================================================================
#include "project_config.h"     //本文件由IDE中配置界面生成，存放在APP的工程目录中。
                                //允许是个空文件，所有配置将按默认值配置。


//@#$%component configure   ****组件配置开始，用于 DIDE 中图形化配置界面
//****配置块的语法和使用方法，参见源码根目录下的文件：component_config_readme.txt****
//%$#@initcode      ****初始化代码开始，由 DIDE 删除“//”后copy到初始化文件中
//    extern s32 ModuleInstall_QSPI_RAM(void);
//    ModuleInstall_QSPI_RAM();
//%$#@end initcode  ****初始化代码结束

//%$#@describe      ****组件描述开始
//component name:"cpu onchip qspi ram"    //片内sdcard驱动
//parent:"none"                         //填写该组件的父组件名字，none表示没有父组件
//attribute:bsp                         //选填“third、system、bsp、user”，本属性用于在IDE中分组
//select:choosable                      //选填“required、choosable、none”，若填必选且需要配置参数，则IDE裁剪界面中默认勾取，
                                        //不可取消，必选且不需要配置参数的，或是不可选的，IDE裁剪界面中不显示，
//init time:early                      //初始化时机，可选值：early，medium，later, pre-main。
                                        //表示初始化时间，分别是早期、中期、后期
//dependence:none"                      //该组件的依赖组件名（可以是none，表示无依赖组件），
                                        //选中该组件时，被依赖组件将强制选中，
                                        //如果依赖多个组件，则依次列出
//weakdependence:"none"                 //该组件的弱依赖组件名（可以是none，表示无依赖组件），
                                        //选中该组件时，被依赖组件不会被强制选中，
                                        //如果依赖多个组件，则依次列出，用“,”分隔
//mutex:"none"                          //该组件的互斥组件名（可以是none，表示无互斥组件），
                                        //如果与多个组件互斥，则依次列出
//%$#@end describe  ****组件描述结束

//%$#@configue      ****参数配置开始
#if ( CFG_MODULE_ENABLE_CPU_ONCHIP_QSPR_RAM == false )
//#warning  " cpu_onchip_uart  组件参数未配置，使用默认配置"
//%$#@target = header    //header = 生成头文件,cmdline = 命令行变量，DJYOS自有模块禁用
#define CFG_MODULE_ENABLE_CPU_ONCHIP_QSPR_RAM    false //如果勾选了本组件，将由DIDE在project_config.h或命令行中定义为true
//%$#@num,0,1,
#define CFG_QSPI_ENABLE_NUM             0                //"QSPI使能序号，0：使能QSPI1，1：使能QSPI2"
//%$#@num,0,255,
#define CFG_RAM_RESET_ENABLE_CMD      102                //"RAM复位使能命令"
#define CFG_RAM_RESET_CMD             153                //"RAM复位命令"
#define CFG_RAM_READ_CMD              235                //"RAM读命令"
#define CFG_RAM_WRITE_CMD             56                //"RAM写命令"
//%$#@num,0,2147483647,
#define CFG_QSPI_RAM_SIZE               8388608       //"RAM的尺寸，默认8M"
#define CFG_QSPI_RAM_PAGE_SIZE          1024          //"RAM的页尺寸，默认1K"
//%$#@string,1,10,
#define CFG_QSPI_RAM_HEAP_NAME          "PSRAM"      //"新建的heap名字"

//%$#select,        ***从列出的选项中选择若干个定义成宏
//%$#@free,
#endif
//%$#@end configue  ****参数配置结束

//%$#@exclude       ****编译排除文件列表
//%$#@end exclude   ****组件描述结束

//@#$%component end configure
// ============================================================================
extern struct HeapCB *Heap_Add(void* bottom, u32 size, u32 PageSize,u32 AlignSize,
        bool_t proper,char *name);
#define tagQspiReg qspi_type
static tagQspiReg * const tg_QspiReg[] = {(tagQspiReg *)QSPI1_REG_BASE,
                                                 (tagQspiReg *)QSPI2_REG_BASE};

bool_t QSPI_Send_CMD(u8 cmd)
{
    qspi_cmd_type cmd_config;

    cmd_config.pe_mode_enable = FALSE;
    cmd_config.pe_mode_operate_code = 0;
    cmd_config.instruction_code = cmd;
    cmd_config.instruction_length = QSPI_CMD_INSLEN_1_BYTE;
    cmd_config.address_code = 0;
    cmd_config.address_length = QSPI_CMD_ADRLEN_0_BYTE;
    cmd_config.data_counter = 0;
    cmd_config.second_dummy_cycle_num = 0;
    cmd_config.operation_mode = QSPI_OPERATE_MODE_111;
    cmd_config.read_status_config = QSPI_RSTSC_HW_AUTO;
    cmd_config.read_status_enable = FALSE;
    cmd_config.write_data_enable = TRUE;

    qspi_cmd_operation_kick(tg_QspiReg[CFG_QSPI_ENABLE_NUM], &cmd_config);
    /* wait command completed */
    while(qspi_flag_get(tg_QspiReg[CFG_QSPI_ENABLE_NUM], QSPI_CMDSTS_FLAG) == RESET);
    qspi_flag_clear(tg_QspiReg[CFG_QSPI_ENABLE_NUM], QSPI_CMDSTS_FLAG);
    return true;
}

//-----------------------------------------------------------------------------
//功能: 安装QSPI
//参数: 无
//返回: -1 -- 失败; 0 -- 成功;
//备注:
//-----------------------------------------------------------------------------
s32 ModuleInstall_QSPI_RAM(void)
{
    qspi_xip_type xip_init;
    #if CFG_QSPI_ENABLE_NUM == 0
    crm_periph_clock_enable(CRM_QSPI1_PERIPH_CLOCK, TRUE);
    #else
    crm_periph_clock_enable(CRM_QSPI2_PERIPH_CLOCK, TRUE);
    #endif
    qspi_xip_enable(tg_QspiReg[CFG_QSPI_ENABLE_NUM], FALSE);
    qspi_clk_division_set(tg_QspiReg[CFG_QSPI_ENABLE_NUM], QSPI_CLK_DIV_4);
    qspi_sck_mode_set(tg_QspiReg[CFG_QSPI_ENABLE_NUM], QSPI_SCK_MODE_0);
    qspi_busy_config(tg_QspiReg[CFG_QSPI_ENABLE_NUM], QSPI_BUSY_OFFSET_0);

    QSPI_Send_CMD(CFG_RAM_RESET_ENABLE_CMD);
    QSPI_Send_CMD(CFG_RAM_RESET_CMD);

    xip_init.read_instruction_code = CFG_RAM_READ_CMD;
    xip_init.read_address_length = QSPI_XIP_ADDRLEN_3_BYTE;
    xip_init.read_operation_mode = QSPI_OPERATE_MODE_144;
    xip_init.read_second_dummy_cycle_num = 6;
    xip_init.write_instruction_code = CFG_RAM_WRITE_CMD;
    xip_init.write_address_length = QSPI_XIP_ADDRLEN_3_BYTE;
    xip_init.write_operation_mode = QSPI_OPERATE_MODE_144;
    xip_init.write_second_dummy_cycle_num = 0;
    xip_init.write_select_mode = QSPI_XIPW_SEL_MODED;
    xip_init.write_time_counter = 0x7F;
    xip_init.write_data_counter = 0x1F;
    xip_init.read_select_mode = QSPI_XIPR_SEL_MODED;
    xip_init.read_time_counter = 0x7F;
    xip_init.read_data_counter = 0x1F;
    qspi_xip_init(tg_QspiReg[CFG_QSPI_ENABLE_NUM], &xip_init);
    qspi_xip_cache_bypass_set(tg_QspiReg[CFG_QSPI_ENABLE_NUM],TRUE);
    qspi_xip_enable(tg_QspiReg[CFG_QSPI_ENABLE_NUM], TRUE);

    if(Heap_Add((void *)QSPI1_MEM_BASE, CFG_QSPI_RAM_SIZE, CFG_QSPI_RAM_PAGE_SIZE, 0,
         false, CFG_QSPI_RAM_HEAP_NAME) == NULL)       //把QSPI接的RAM添加到heap当中
    {
        error_printf("QSPI RAM", "heap add fail\r\n");
        return -1;
    }
    return 0;
}
