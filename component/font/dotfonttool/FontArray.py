# -*- coding: utf-8 -*-

import traceback
import os
import struct
from typing import Optional, Tuple
from config import FontConfig

class FontArrayGenerator:
    """
    字体数组生成器类，用于处理位图数据并生成C语言数组
    """
    
    def __init__(self):
        """
        初始化FontArrayGenerator类
        """
        pass
    
    def process_bitmap_data(self, bmp_file: str, bit_depth: int) -> Optional[bytes]:
        """
        处理BMP位图文件，转换为指定位深度的二进制数据
        
        参数:
            bmp_file (str): BMP文件路径
            bit_depth (int): 位深度，可选值为1、2、4、8
            
        返回:
            bytes or None: 处理后的二进制数据，处理失败返回None
        """
        try:
            with open(bmp_file, 'rb') as f:
                # 读取BMP文件头
                bmp_header = f.read(14)
                pixel_offset = int.from_bytes(bmp_header[10:14], 'little')
                
                # 读取DIB头信息
                dib_header = f.read(40)
                width = int.from_bytes(dib_header[4:8], 'little')
                height = int.from_bytes(dib_header[8:12], 'little')
                
                # 移动到像素数据开始位置
                f.seek(pixel_offset)
                
                # 计算每行实际需要的字节数
                if bit_depth == 1:
                    row_bytes = (width + 31) // 32 * 4  # BMP的32位对齐
                    valid_bits_per_row = width  # 实际有效位数
                else:
                    row_bytes = (width+3)& ~3  # 2位和4位色深时每个像素占一个字节
                    valid_bytes_per_row = width  # 实际有效字节数
                
                # 读取所有像素数据
                pixel_data = f.read()
                
                output_data = bytearray()
                
                # 从下到上处理每一行
                for y in range(height - 1, -1, -1):
                    row_start = y * row_bytes
                    row_data = pixel_data[row_start:row_start + row_bytes]
                    
                    if bit_depth == 2:
                        # 每4个字节处理一组
                        for i in range(0, valid_bytes_per_row, 4):
                            # if i + 4 <= len(row_data):
                            # 提取4个像素的低2位并组合
                            byte = ((row_data[i] & 0x03) << 6) | \
                                    ((row_data[i+1] & 0x03) << 4) | \
                                    ((row_data[i+2] & 0x03) << 2) | \
                                    (row_data[i+3] & 0x03)
                            # else:
                            #     j = len(row_data) - i
                            #     if j > 0:
                            #         byte = (row_data[i] & 0x03) << 6
                            #     if j > 1:
                            #         byte |= (row_data[i+1] & 0x03) << 4
                            #     if j > 2:
                            #         byte |= (row_data[i+2] & 0x03) << 2
                            output_data.append(byte)

                    elif bit_depth == 4:
                        # 每2个字节处理一组
                        for i in range(0, valid_bytes_per_row, 2):
                            # if i + 2 <= len(row_data):
                            # 提取2个像素的低4位并组合
                            byte = ((row_data[i] & 0x0F) << 4) | \
                                    (row_data[i+1] & 0x0F)
                            output_data.append(byte)
                    
                    elif bit_depth == 1:
                        # 计算需要多少个完整字节来存储这一行
                        valid_bytes = (valid_bits_per_row + 7) // 8
                        output_data.extend(row_data[:valid_bytes])
                    elif bit_depth == 8:
                        output_data.extend(row_data[:valid_bytes_per_row])
                
                # 生成并写入bin文件
                # bin_file = os.path.splitext(bmp_file)[0] + '.bin'
                # with open(bin_file, 'wb') as bin_f:
                #     bin_f.write(output_data)
                
                # print(f"成功生成二进制文件: {bin_file}")  # 添加日志
                return bytes(output_data)
                
        except Exception as e:
            print(f"处理位图数据时出错: {e}")
            traceback.print_exc()
            return None

    def generate_c_array(self, data: bytes, array_name: str, char_count: int, bytes_per_char: int, font_name: str = None) -> str:
        """
        生成C语言数组定义
        
        参数:
            data (bytes): 二进制数据
            array_name (str): 数组名称
            char_count (int): 字符数量
            bytes_per_char (int): 每个字符占用的字节数
            font_name (str): 字体名称，用于注释
            
        返回:
            str: C语言数组定义代码
        """
        # 添加字体名称注释
        c_code = ""
        if font_name:
            c_code = f"/* 字体名称: {font_name} */\n"
        
        c_code += f"const u8 {array_name}[{char_count}][{bytes_per_char}] = {{\n"
        
        for i in range(char_count):
            c_code += "    {"
            start = i * bytes_per_char
            end = start + bytes_per_char
            c_code += ", ".join(f"0x{b:02X}" for b in data[start:end])
            c_code += "},\n"
            
        c_code += "};\n"
        return c_code

    def sanitize_array_name(self, name):
        """将名称转换为合法的C数组名"""
        if not name:
            return "default_font_array"
            
        # 去除前后空格
        name = name.strip()
        
        # 将空格和特殊字符替换为下划线
        import re
        sanitized = re.sub(r'[^a-zA-Z0-9_]', '_', name)
        
        # 确保变量名不以数字开头
        if sanitized and sanitized[0].isdigit():
            sanitized = "array_" + sanitized
        
        # 如果为空，则使用默认名称
        if not sanitized:
            sanitized = "default_font_array"
        
        return sanitized

    def process_and_generate(self, bmp_file: str, bit_depth: int, array_name: str, 
                            char_count: int, bytes_per_char: int, 
                            output_dir: str = 'output', font_name: str = None) -> Tuple[Optional[str], Optional[str]]:
        """
        处理位图文件并生成相应的bin和rsc文件
        
        参数:
            bmp_file (str): BMP文件路径，如为None则创建空文件
            bit_depth (int): 位深度
            array_name (str): 数组名称
            char_count (int): 字符数量
            bytes_per_char (int): 每个字符占用的字节数
            output_dir (str): 输出目录
            font_name (str): 字体名称
            
        返回:
            tuple: (bin文件路径, rsc文件路径)
        """
        # 确保输出目录存在
        os.makedirs(output_dir, exist_ok=True)
        
        # 确保数组名是合法的C变量名
        array_name = self.sanitize_array_name(array_name)
        
        # 处理位图数据或创建空数据
        processed_data = b''
        if bmp_file and os.path.exists(bmp_file):
            data = self.process_bitmap_data(bmp_file, bit_depth)
            if data:
                processed_data = data
        
        # 使用传入的array_name作为文件名基础
        bin_file = os.path.join(output_dir, f"{array_name}.bin")
        c_file = os.path.join(output_dir, f"{array_name}.rsc")
        
        # 保存为bin文件
        with open(bin_file, 'wb') as f:
            f.write(processed_data)
            
        # 生成C语言数组定义
        if char_count > 0 and len(processed_data) > 0:
            c_code = self.generate_c_array(processed_data, array_name, char_count, bytes_per_char, font_name)
        else:
            # 创建空数组定义
            c_code = f"/* 字体名称: {font_name or 'Unknown'} */\n"
            c_code += f"/* 警告: 没有字符数据 */\n"
            c_code += f"const u8 {array_name}[0][{bytes_per_char}] = {{}};\n"
        
        # 保存为C文件
        with open(c_file, 'w') as f:
            f.write(c_code)
            
        return bin_file, c_file

