# -*- coding: utf-8 -*-

import os
import re

class FontConfig:
    """字体配置类，管理全局配置和文件名生成"""
    
    def __init__(self):
        # 基础文件名
        self.base_filename = None
        # ASCII字符集文件名
        self.ascii_filename = None
        # 合并文件名
        self.merged_filename = None
        self.custom_charset = False

    def generate_filenames(self, charset, input_file, font_name, width, height, ascii_width, ascii_height, bit_depth):
        """
        生成统一的文件名
        
        参数:
            charset (str): 用户选择的字符集名称
            input_file (str): 输入文件路径（可选）
            font_name (str): 字体名称
            width (int): 字符宽度
            height (int): 字符高度
            bit_depth (int): 位深度
            actual_charset (str): 实际使用的字符集（用于自定义字符集情况）
        """
        # 处理字体名，使其符合C语言规则
        safe_font_name = self.sanitize_name(font_name)
        
        # 处理输入文件名（如果有）
        input_part = ""
        if input_file:
            input_basename = os.path.basename(input_file).split('.')[0]
            input_part = f"_{input_basename}"
        
        # 使用实际字符集（如果指定）或用户选择的字符集
        # used_charset = actual_charset.upper() if actual_charset else charset.upper()
        
        # 生成基本文件名
        if charset == 'ASCII':
            self.base_filename = f"dot_{charset}{input_part}_{safe_font_name}_{ascii_width}_{ascii_height}_{bit_depth}bit"
        else:
            self.base_filename = f"dot_{charset}{input_part}_{safe_font_name}_{width}_{height}_{bit_depth}bit"
        self.ascii_filename = f"dot_ASCII{input_part}_{safe_font_name}_{ascii_width}_{ascii_height}_{bit_depth}bit"
        # 预生成合并后的文件名
        self.merged_filename = f"{self.base_filename}_with_ascii"

    def sanitize_name(self, name):
        """将名称转换为合法的C变量名"""
        if not name:
            return "default_font"
            
        # 去除前后空格
        name = name.strip()
        
        # 将空格和特殊字符替换为下划线
        sanitized = re.sub(r'[^a-zA-Z0-9_]', '_', name)
        
        # 确保变量名不以数字开头
        if sanitized and sanitized[0].isdigit():
            sanitized = "font_" + sanitized
        
        # 如果为空，则使用默认名称
        if not sanitized:
            sanitized = "default_font"
        
        # 转为小写，更符合命名习惯
        return sanitized.lower()
    
    def get_base_filepath(self, output_dir, extension):
        """获取基本文件的完整路径"""
        return os.path.join(output_dir, f"{self.base_filename}.{extension}")
    
    def get_ascii_filepath(self, output_dir, extension):
        """获取ASCII文件的完整路径"""
        if self.ascii_filename:
            return os.path.join(output_dir, f"{self.ascii_filename}.{extension}")
        return None
    
    def get_merged_filepath(self, output_dir, extension):
        """获取合并文件的完整路径"""
        if self.merged_filename:
            return os.path.join(output_dir, f"{self.merged_filename}.{extension}")
        return None

# 全局配置实例
font_config = FontConfig()
