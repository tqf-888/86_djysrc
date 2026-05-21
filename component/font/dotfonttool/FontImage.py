# -*- coding: utf-8 -*-

import traceback
import re
import os
import argparse
import platform
from PIL import Image, ImageDraw, ImageFont, ImageOps,ImageEnhance
import numpy as np
from config import FontConfig

class FontImageGenerator:
    def __init__(self, 
                charset='GB2312',
                font_name=None,
                char_width=16,
                char_height=16,
                bit_depth=2,
                input_file=None):
        self.charset = charset
        self.original_font_name = font_name  # 保存原始字体名
        
        # 处理字体名
        if font_name:
            # 移除可能的@前缀
            if font_name.startswith('@'):
                self.font_name = font_name.replace('@', '')
            else:
                self.font_name = font_name
        else:
            self.font_name = self.get_default_font()
            
        # 字符宽高分开设置
        self.char_width = char_width
        self.char_height = char_height
        self.bit_depth = bit_depth
        self.input_file = input_file
        self.characters = []  # 存储所有字符或占位符
        self.valid_chars = []  # 存储有定义字符
        self.antialias_threshold = 16  # PIL抗锯齿阈值
        self.custom_charset = False  # 是否使用自定义字符集
        
        # 获取系统字体映射（FontImage初始化时就建立完整映射）
        self.fonts_mapping = self.get_system_fonts_mapping()
        
        # 提取字体名称（拼音形式）
        self.font_pinyin_name = self.extract_font_pinyin_name()
        
    def get_system_fonts_mapping(self):
        """获取系统字体名称到文件路径的映射"""
        fonts_map = {}
        
        try:
            # Windows系统
            if platform.system() == 'Windows':
                fonts_dir = os.path.join(os.environ.get('WINDIR', 'C:\\Windows'), 'Fonts')
                
                # 尝试使用win32com获取更详细的字体信息
                try:
                    import win32com.client
                    shell = win32com.client.Dispatch("Shell.Application")
                    fonts_folder = shell.NameSpace(fonts_dir)
                    
                    for i in range(fonts_folder.Items().Count):
                        try:
                            item = fonts_folder.Items().Item(i)
                            # 获取字体名称和文件名
                            font_name = fonts_folder.GetDetailsOf(item, 0)  # 0是名称索引
                            font_file = item.Path
                            if font_name and font_file:
                                fonts_map[font_name] = font_file
                                
                                # 添加去掉@符号的版本
                                if font_name.startswith('@'):
                                    fonts_map[font_name.replace('@', '')] = font_file
                        except:
                            continue
                    
                    # 如果映射为空，使用备选方法
                    if not fonts_map:
                        fonts_map = self._get_basic_fonts_mapping()
                except:
                    # 如果win32com失败，使用基本方法
                    fonts_map = self._get_basic_fonts_mapping()
                    
            # macOS系统
            elif platform.system() == 'Darwin':
                try:
                    fonts_dir = '/Library/Fonts/'
                    system_fonts_dir = '/System/Library/Fonts/'
                    
                    # 添加用户字体目录
                    user_fonts_dir = os.path.expanduser('~/Library/Fonts/')
                    
                    # 从这些目录收集字体文件
                    for dir_path in [fonts_dir, system_fonts_dir, user_fonts_dir]:
                        if os.path.exists(dir_path):
                            for file in os.listdir(dir_path):
                                if file.lower().endswith(('.ttf', '.ttc', '.otf')):
                                    # 从文件名推断字体名（移除扩展名）
                                    font_name = os.path.splitext(file)[0]
                                    fonts_map[font_name] = os.path.join(dir_path, file)
                except:
                    # 基本Mac字体映射
                    fonts_map = {
                        'PingFang SC': '/System/Library/Fonts/PingFang.ttc',
                        'Heiti SC': '/System/Library/Fonts/STHeiti Light.ttc',
                        'Songti SC': '/System/Library/Fonts/STSongti-SC-Regular.ttf'
                    }
                    
            # Linux系统
            else:
                try:
                    # 使用fc-list命令获取字体信息
                    import subprocess
                    result = subprocess.run(['fc-list', ':', 'file,family'], 
                                        capture_output=True, text=True)
                    
                    lines = result.stdout.split('\n')
                    for line in lines:
                        if ':' in line:
                            parts = line.split(':')
                            file_path = parts[0].strip()
                            if len(parts) > 1 and parts[1].strip() and os.path.isfile(file_path):
                                families = [f.strip() for f in parts[1].split(',')]
                                for family in families:
                                    if family:
                                        fonts_map[family] = file_path
                except:
                    # 基本Linux字体映射
                    fonts_map = {
                        'WenQuanYi Micro Hei': '/usr/share/fonts/truetype/wqy/wqy-microhei.ttc',
                        'Droid Sans': '/usr/share/fonts/truetype/droid/DroidSansFallbackFull.ttf'
                    }
        except:
            # 出错时使用基本映射
            fonts_map = self._get_basic_fonts_mapping()
        
        # 确保包含主要中文字体的映射
        basic_map = self._get_basic_fonts_mapping()
        for font, path in basic_map.items():
            if font not in fonts_map and os.path.isfile(path):
                fonts_map[font] = path
        
        print(f"获取到{len(fonts_map)}个系统字体映射")
        return fonts_map

    def _get_basic_fonts_mapping(self):
        """基本的字体映射方法，作为备选"""
        fonts_dir = ""
        
        # 根据操作系统设置字体目录
        if platform.system() == 'Windows':
            fonts_dir = os.path.join(os.environ.get('WINDIR', 'C:\\Windows'), 'Fonts')
        elif platform.system() == 'Darwin':  # macOS
            fonts_dir = '/Library/Fonts/'
        else:  # Linux
            fonts_dir = '/usr/share/fonts/'
        
        # 通用字体映射（不同系统通用字体名）
        common_fonts = {
            # 中文字体
            "宋体": "simsun.ttc",
            "新宋体": "simsun.ttc",
            "黑体": "simhei.ttf",
            "仿宋": "simfang.ttf",
            "楷体": "simkai.ttf",
            "微软雅黑": "msyh.ttc",
            "等线": "dengxian.ttf",
            "隶书": "simli.ttf",
            "幼圆": "simyou.ttf",
            "华文细黑": "STXihei.ttf",
            "华文宋体": "STSong.ttf",
            
            # 西文字体
            "Arial": "arial.ttf",
            "Arial Black": "ariblk.ttf",
            "Calibri": "calibri.ttf",
            "Cambria": "cambria.ttf",
            "Courier New": "cour.ttf",
            "Times New Roman": "times.ttf",
            "Verdana": "verdana.ttf",
            
            # 日文字体
            "MS Gothic": "msgothic.ttc",
            "MS Mincho": "msmincho.ttc",
            
            # 韩文字体
            "Malgun Gothic": "malgun.ttf",
            "Batang": "batang.ttc"
        }
        
        # 将相对路径转换为绝对路径
        return {name: os.path.join(fonts_dir, file) for name, file in common_fonts.items()}

    def get_char_count(self):
        """获取字符数量"""
        return len(self.valid_chars)
    
    def get_bytes_per_char(self):
        """获取每个字符占用的字节数"""
        return ((self.char_width * self.bit_depth + 7) // 8) * self.char_height
    
    def get_total_bytes(self):
        """获取所有字符占用的总字节数"""
        return self.get_char_count() * self.get_bytes_per_char()

    def extract_font_pinyin_name(self):
        """从字体名称提取拼音形式的字体名"""
        try:
            from pypinyin import lazy_pinyin
            
            font_name = self.font_name if self.font_name else ''
            
            # 将中文字符转换为拼音
            pinyin_list = lazy_pinyin(font_name, errors='ignore')
            
            # 拼接拼音,移除非字母数字字符
            result_name = ''.join(pinyin_list)
            
            # 清理和格式化
            import re
            # 保留字母、数字,其他字符替换为下划线
            result_name = re.sub(r'[^a-zA-Z0-9]', '_', result_name)
            # 合并多个下划线
            result_name = re.sub(r'_+', '_', result_name)
            # 移除首尾下划线并转小写
            result_name = result_name.strip('_').lower()
            
            return result_name if result_name else 'default_font'
            
        except ImportError:
            # 如果没有安装pypinyin,使用基本的清理
            if not self.font_name:
                return 'default_font'
                
            import re
            result = re.sub(r'[^a-zA-Z0-9]', '_', self.font_name)
            result = re.sub(r'_+', '_', result)
            return result.strip('_').lower()
            
        except Exception as e:
            print(f"提取字体拼音名称时出错: {e}")
            return 'default_font'

    def get_default_font(self):
        """根据操作系统选择合适的默认字体"""
        system = platform.system()
        if system == 'Windows':
            # Windows系统下常见的中文字体
            font_options = [
                'C:\\Windows\\Fonts\\simsun.ttc',    # 宋体
                'C:\\Windows\\Fonts\\simhei.ttf',    # 黑体
                'C:\\Windows\\Fonts\\msyh.ttc',      # 微软雅黑
                'C:\\Windows\\Fonts\\simkai.ttf',    # 楷体
                'C:\\Windows\\Fonts\\simfang.ttf'    # 仿宋
            ]
        elif system == 'Darwin':  # macOS
            font_options = [
                '/System/Library/Fonts/PingFang.ttc',
                '/Library/Fonts/Arial Unicode.ttf',
                '/System/Library/Fonts/STHeiti Light.ttc',
                '/System/Library/Fonts/STHeiti Medium.ttc'
            ]
        else:  # Linux和其他系统
            font_options = [
                '/usr/share/fonts/truetype/droid/DroidSansFallbackFull.ttf',
                '/usr/share/fonts/truetype/wqy/wqy-microhei.ttc',
                '/usr/share/fonts/truetype/arphic/uming.ttc'
            ]
            
        # 尝试列出的每一个字体，找到第一个可用的
        for font_path in font_options:
            if os.path.exists(font_path):
                print(f"使用字体: {font_path}")
                return font_path
                
        # 如果都不可用，返回默认值
        print("未找到合适的中文字体，将使用默认的simsun.ttc")
        return 'simsun.ttc'

    def sanitize_name(self, name):
        """将字体名转换为合法的C变量名"""
        if not name:
            return "default_font"
            
        # 去除前后空格
        name = name.strip()
        
        # 将空格和特殊字符替换为下划线
        import re
        # 保留字母、数字和下划线，其他字符替换为下划线
        sanitized = re.sub(r'[^a-zA-Z0-9_]', '_', name)
        
        # 确保变量名不以数字开头
        if sanitized and sanitized[0].isdigit():
            sanitized = "font_" + sanitized
        
        # 如果为空，则使用默认名称
        if not sanitized:
            sanitized = "default_font"
        
        return sanitized

    def load_font(self, size_factor=1):
        """加载字体，使用系统字体映射"""
        # 使用高度作为字体大小基准
        size = self.char_height * size_factor
        
        # 1. 首先检查字体名称是否在fonts_mapping中
        if self.font_name in self.fonts_mapping:
            try:
                font_info = self.fonts_mapping[self.font_name]
                
                # 检查是否是(文件路径, 索引)格式(ttc文件)
                if isinstance(font_info, tuple) and len(font_info) == 2:
                    font_path, font_index = font_info
                    font = ImageFont.truetype(font_path, size, index=font_index)
                else:
                    # 普通字体文件
                    font_path = font_info
                    font = ImageFont.truetype(font_path, size)
                    
                print(f"成功加载字体: {self.font_name} -> {font_path}")
                return font
            except Exception as e:
                print(f"通过字体映射加载失败: {e}")
        
        # 2. 尝试直接加载字体（如果是文件路径）
        if os.path.isfile(self.font_name):
            try:
                font = ImageFont.truetype(self.font_name, size)
                print(f"成功加载字体文件: {self.font_name}")
                return font
            except Exception as e:
                print(f"加载字体文件失败: {e}")
        
        # 3. 尝试通过部分名称匹配
        for font_name, font_path in self.fonts_mapping.items():
            if (self.font_name.lower() in font_name.lower() or 
                font_name.lower() in self.font_name.lower()):
                try:
                    # 检查是否是(文件路径, 索引)格式(ttc文件)
                    if isinstance(font_path, tuple) and len(font_path) == 2:
                        path, index = font_path
                        font = ImageFont.truetype(path, size, index=index)
                    else:
                        # 普通字体文件
                        font = ImageFont.truetype(font_path, size)
                        
                    print(f"成功通过部分匹配加载字体: {self.font_name} -> {font_path}")
                    self.font_name = font_path  # 更新为找到的字体路径
                    self.font_pinyin_name = self.extract_font_pinyin_name()
                    return font
                except:
                    continue
        
        # 4. 尝试常见的备选字体
        backup_fonts = ["simsun.ttc", "simfang.ttf", "simkai.ttf", "simhei.ttf", "msyh.ttc"]
        for backup in backup_fonts:
            for font_path in self.fonts_mapping.values():
                # 提取路径(处理元组情况)
                if isinstance(font_path, tuple):
                    path = font_path[0]
                else:
                    path = font_path
                    
                if backup.lower() in str(path).lower():
                    try:
                        if isinstance(font_path, tuple):
                            font = ImageFont.truetype(path, size, index=font_path[1])
                        else:
                            font = ImageFont.truetype(path, size)
                            
                        print(f"使用备选字体: {path}")
                        self.font_name = path  # 更新为成功加载的字体
                        self.font_pinyin_name = self.extract_font_pinyin_name()
                        return font
                    except:
                        continue
                
        # 5. 如果所有方法都失败，使用默认字体
        print("所有字体加载方法失败，使用系统默认字体")
        return ImageFont.load_default()

    def get_backup_fonts(self):
        """获取备选字体列表"""
        system = platform.system()
        if system == 'Windows':
            return [
                'C:\\Windows\\Fonts\\simsun.ttc',
                'C:\\Windows\\Fonts\\simhei.ttf',
                'C:\\Windows\\Fonts\\msyh.ttc',
                'C:\\Windows\\Fonts\\simkai.ttf',
                'C:\\Windows\\Fonts\\simfang.ttf',
                'C:\\Windows\\Fonts\\STSong.ttf',
                'C:\\Windows\\Fonts\\STKAITI.TTF'
            ]
        elif system == 'Darwin':  # macOS
            return [
                '/System/Library/Fonts/PingFang.ttc',
                '/Library/Fonts/Arial Unicode.ttf',
                '/System/Library/Fonts/STHeiti Light.ttc',
                '/System/Library/Fonts/Hiragino Sans GB.ttc'
            ]
        else:  # Linux
            return [
                '/usr/share/fonts/truetype/wqy/wqy-microhei.ttc',
                '/usr/share/fonts/truetype/wqy/wqy-zenhei.ttc',
                '/usr/share/fonts/truetype/arphic/uming.ttc',
                '/usr/share/fonts/truetype/droid/DroidSansFallbackFull.ttf'
            ]
    
    def is_character_in_charset(self, char):
        """检查字符是否属于指定的字符集"""
        if self.charset.upper() == 'GB2312':
            try:
                # 尝试使用GB2312编码，如果成功则属于该字符集
                char.encode('gb2312')
                return True
            except UnicodeEncodeError:
                return False
        # 可以添加其他字符集的支持
        return False
    
    def is_defined_in_gb2312(self, i, j):
        """检查指定区位码在GB2312中是否有定义"""
        try:
            # 尝试解码GB2312字符
            bytes([i, j]).decode('gb2312')
            return True
        except UnicodeDecodeError:
            return False
        except:
            return False
    
    def is_renderable(self, char, font):
        """检查字符是否可渲染"""
        try:
            # 尝试获取字符的边界框
            bbox = font.getbbox(char)
            # 检查边界框是否有效
            return not (bbox[0] == bbox[2] or bbox[1] == bbox[3])
        except:
            return False
    
    def load_custom_charset(self, file_path, output_dir):
        """从文件中提取与当前字符集匹配的字符"""
        if not os.path.exists(file_path):
            print(f"文件不存在: {file_path}")
            return False
        
        try:
            # 初始化ranges列表
            self.defined_ranges = []
            self.undefined_ranges = []
            
            # 尝试用UTF-8或其他编码读取文件
            content = None
            encodings = ['utf-8', 'gb2312', 'gbk']
            
            for encoding in encodings:
                try:
                    with open(file_path, 'r', encoding=encoding) as f:
                        content = f.read()
#                    print(f"成功使用 {encoding} 编码读取文件")
                    break
                except UnicodeDecodeError:
                    continue
            
            if content is None:
                print(f"无法读取文件，尝试的编码: {', '.join(encodings)}")
                return False
            
            # 提取字符并分类
            ascii_chars = []
            gb2312_chars = []
            
            # 遍历文件中的每个字符
            for char in content:
                if 0x20 <= ord(char) <= 0x7E:  # ASCII可打印字符
                    ascii_chars.append(char)
                elif not char.isspace():  # 非ASCII且非空白字符
                    try:
                        # 尝试GB2312编码
                        char_bytes = char.encode('gb2312')
                        if len(char_bytes) == 2:  # 双字节字符
                            gb2312_chars.append(char)
                    except UnicodeEncodeError:
                        continue
            
            # 去重并排序
            ascii_chars = sorted(set(ascii_chars))
            gb2312_chars = sorted(set(gb2312_chars))
            
            # 根据选择的字符集处理
            need_ascii = False
            need_gb2312 = False
            
            # 确定需要包含哪些字符集字符
            if self.charset.upper() == 'ASCII':
                # 如果是ASCII字符集，只需要ASCII字符
                need_ascii = True
                need_gb2312 = False
                print("需要ASCII字符集，不需要GB2312字符集")
            elif self.charset.upper() == 'GB2312':
                # 如果是GB2312字符集，两种字符都需要
                need_ascii = True
                need_gb2312 = True
                print("需要ASCII字符集，需要GB2312字符集")
            
            # 处理ASCII字符集（如果需要）
            if need_ascii:
                if self.charset.upper() == 'ASCII':
                    # ASCII字符作为主字符集
                    self.valid_chars = []
                    if ascii_chars:
                        for index, char in enumerate(ascii_chars):
                            code = ord(char)
                            self.valid_chars.append({
                                'code': f"0x{code:02X}",
                                'char': char,
                                'valid_index': index
                            })
                        
                        # 添加ASCII的defined_ranges
                        if self.valid_chars:
                            first_char = self.valid_chars[0]
                            last_char = self.valid_chars[-1]
                            self.defined_ranges.append((
                                first_char['code'],
                                last_char['code'],
                                0,
                                len(self.valid_chars) - 1
                            ))
                    
                    print(f"使用 {len(ascii_chars)} 个ASCII字符")
                else:
                    # ASCII字符供GB2312模式使用
                    self.ascii_chars = ascii_chars
                    print(f"保存 {len(ascii_chars)} 个ASCII字符，供GB2312模式使用")
            
            # 处理GB2312字符集（如果需要）
            if need_gb2312 and self.charset.upper() == 'GB2312':
                self.valid_chars = []
                if gb2312_chars:
                    for index, char in enumerate(gb2312_chars):
                        char_bytes = char.encode('gb2312')
                        code = (char_bytes[0] << 8) | char_bytes[1]
                        self.valid_chars.append({
                            'code': f"0x{code:04X}",
                            'char': char,
                            'valid_index': index
                        })
                    
                    # 添加GB2312的defined_ranges
                    if self.valid_chars:
                        first_char = self.valid_chars[0]
                        last_char = self.valid_chars[-1]
                        self.defined_ranges.append((
                            first_char['code'],
                            last_char['code'],
                            0,
                            len(self.valid_chars) - 1
                        ))
                
                print(f"使用 {len(gb2312_chars)} 个GB2312字符")
            
            # 设置自定义字符集标志
            self.custom_charset = True
            return True
                
        except Exception as e:
            print(f"处理字符集文件时出错: {e}")
            traceback.print_exc()
            return False

    
    def generate_charset(self):
        """生成完整字符集"""
        self.valid_chars = []
        self.defined_ranges = []
        self.undefined_ranges = []
        
        # 根据字符集规则处理不同编码范围
        if self.charset.upper() == 'GB2312':
            # 存储所有编码点的状态（有定义/未定义）
            encoding_status = {}
            current_index = 0
            valid_index = 0
            
            # 先检测每个编码点是否有定义
            for high in range(0xA1, 0xF8):
                for low in range(0xA1, 0xFF):
                    # 跳过超出范围的编码点
                    if low > 0xFE:
                        continue
                    
                    # 计算GB2312编码
                    code_str = f"0x{high:02X}{low:02X}"
                    
                    # 尝试用GB2312编码解码为字符
                    try:
                        bytes_val = bytes([high, low])
                        char = bytes_val.decode('gb2312')
                        # 编码点有定义
                        encoding_status[code_str] = {
                            'defined': True,
                            'char': char,
                            'current_index': current_index,
                            'valid_index': valid_index
                        }
                        self.valid_chars.append({
                            'code': code_str,
                            'char': char,
                            'valid_index': valid_index
                        })
                        valid_index += 1
                    except UnicodeDecodeError:
                        # 编码点未定义
                        encoding_status[code_str] = {
                            'defined': False,
                            'current_index': current_index
                        }
                    current_index += 1
            
            # 按照编码顺序对编码点进行排序
            sorted_codes = sorted(encoding_status.keys(), key=lambda x: int(x[2:], 16))
            
            # 找出连续的有定义和未定义区间
            in_defined_range = None
            range_start = None
            range_start_index = None
            
            for i, code in enumerate(sorted_codes):
                status = encoding_status[code]
                is_defined = status['defined']
                
                # 开始新的区间
                if range_start is None:
                    range_start = code
                    range_start_index = status['current_index'] if not is_defined else status['valid_index']
                    in_defined_range = is_defined
                    continue
                
                # 检查区间是否改变
                if is_defined != in_defined_range or i == len(sorted_codes) - 1:
                    # 处理最后一个编码点
                    if i == len(sorted_codes) - 1:
                        if is_defined == in_defined_range:
                            end_code = code
                            end_index = status['valid_index'] if is_defined else status['current_index']
                        else:
                            # 最后一个编码点开始了新区间，先结束之前的区间
                            prev_code = sorted_codes[i-1]
                            prev_status = encoding_status[prev_code]
                            end_code = prev_code
                            end_index = prev_status['valid_index'] if in_defined_range else prev_status['current_index']
                            
                            # 然后单独处理最后一个编码点
                            if is_defined:
                                self.defined_ranges.append((code, code, status['valid_index'], status['valid_index']))
                            else:
                                self.undefined_ranges.append((code, code, status['current_index'], status['current_index']))
                    else:
                        prev_code = sorted_codes[i-1]
                        prev_status = encoding_status[prev_code]
                        end_code = prev_code
                        end_index = prev_status['valid_index'] if in_defined_range else prev_status['current_index']
                    
                    # 添加当前区间
                    if in_defined_range:
                        self.defined_ranges.append((range_start, end_code, range_start_index, end_index))
                    else:
                        self.undefined_ranges.append((range_start, end_code, range_start_index, end_index))
                    
                    # 开始新的区间
                    range_start = code
                    range_start_index = status['valid_index'] if is_defined else status['current_index']
                    in_defined_range = is_defined
            
        elif self.charset.upper() == 'ASCII':
            # ASCII可打印字符范围 0x20-0x7E
            for code in range(0x20, 0x7F):
                char = chr(code)
                code_str = f"0x{code:02X}"
                
                self.valid_chars.append({
                    'code': code_str,
                    'char': char,
                    'valid_index': code - 0x20
                })
            
            # ASCII只有一个连续的定义区间
            self.defined_ranges.append((f"0x{0x20:02X}", f"0x{0x7E:02X}", 0, 0x7E - 0x20))
        
        # 打印有效字符信息
        print(f"生成了{len(self.valid_chars)}个有定义字符")
        print(f"有定义区间: {len(self.defined_ranges)}个")
        print(f"未定义区间: {len(self.undefined_ranges)}个")
            
    def create_blank_bitmap(self):
        """创建空白位图（用于全角空格）"""
        return Image.new('1', (self.char_width, self.char_height), 0)
    
    def create_box_bitmap(self):
        """创建方框位图（用于不可渲染但有定义的字符）"""
        img = Image.new('1', (self.char_width, self.char_height), 0)
        draw = ImageDraw.Draw(img)
        # 绘制边框
        box_margin = min(self.char_width, self.char_height) // 8
        draw.rectangle(
            [box_margin, box_margin, 
            self.char_width - box_margin, self.char_height - box_margin], 
            outline=1, fill=None)  # 使用白色轮廓
        return img
    
    def generate_bitmap(self, output_path):
        """生成位图文件"""
        # 确定是否需要使用抗锯齿技术（小于等于阈值时使用）
        use_antialias = max(self.char_width, self.char_height) <= self.antialias_threshold
        
        if self.bit_depth == 1:
            # 1位深度特殊处理，直接生成单色位图
            # 注意：对于1位深度，不使用抗锯齿处理
            final_width = self.char_width
            final_height = self.char_height * len(self.valid_chars)
            
            final_image = Image.new('1', (final_width, final_height), 0)  # 0表示黑色背景
            draw = ImageDraw.Draw(final_image)
            font = self.load_font()
            
            for i, char_info in enumerate(self.valid_chars):
                y_pos = i * self.char_height
                
                # 处理全角空格特殊情况
                if char_info.get('is_full_width_space', False):
                    # 全角空格应生成空白位图，不做任何绘制
                    continue
                
                # 其他字符根据是否可渲染处理
                if char_info.get('renderable', True):
                    # 可渲染字符正常绘制
                    draw.text((0, y_pos), char_info['char'], font=font, fill=1)  # 1表示白色文字
                else:
                    # 不可渲染但有定义的字符绘制方框
                    box_margin = min(self.char_width, self.char_height) // 8
                    draw.rectangle(
                        [box_margin, y_pos + box_margin, 
                        self.char_width - box_margin, y_pos + self.char_height - box_margin], 
                        outline=1, fill=None)  # 使用白色轮廓
        else:
            # 多级灰度情况
            if use_antialias:
                # 使用缩放方法实现抗锯齿效果
                scale_factor = 4  # 放大倍数
                temp_width = self.char_width * scale_factor
                temp_height = self.char_height * scale_factor * len(self.valid_chars)
                
                temp_image = Image.new('L', (temp_width, temp_height), 0)  # 0表示黑色背景
                draw = ImageDraw.Draw(temp_image)
                font = self.load_font(scale_factor)
                
                for i, char_info in enumerate(self.valid_chars):
                    y_pos = i * (self.char_height * scale_factor)
                    
                    # 处理全角空格特殊情况
                    if char_info.get('is_full_width_space', False):
                        # 全角空格不做任何绘制，保持空白
                        continue
                    
                    # 其他字符根据是否可渲染处理
                    if char_info.get('renderable', True):
                        # 可渲染字符正常绘制 - 直接使用255以获得最大对比度
                        draw.text((0, y_pos), char_info['char'], font=font, fill=255)  # 255表示白色文字
                    else:
                        # 不可渲染但有定义的字符绘制方框
                        box_margin = min(temp_width, self.char_height * scale_factor) // 8
                        draw.rectangle(
                            [box_margin, y_pos + box_margin, 
                            temp_width - box_margin, y_pos + (self.char_height * scale_factor) - box_margin], 
                            outline=255, fill=None)  # 使用白色轮廓
                
                # 使用像素增强器
                enhancer = ImageEnhance.Contrast(temp_image)
                temp_image = enhancer.enhance(1.5)  # 增强对比度，使用PIL的内置方法提高性能
                
                # 缩小到目标尺寸
                final_width = self.char_width
                final_height = self.char_height * len(self.valid_chars)
                final_image = temp_image.resize((final_width, final_height), Image.LANCZOS)
                
            else:
                # 直接渲染，不使用抗锯齿
                final_width = self.char_width
                final_height = self.char_height * len(self.valid_chars)
                
                final_image = Image.new('L', (final_width, final_height), 0)  # 0表示黑色背景
                draw = ImageDraw.Draw(final_image)
                font = self.load_font()
                
                for i, char_info in enumerate(self.valid_chars):
                    y_pos = i * self.char_height
                    
                    # 处理全角空格特殊情况
                    if char_info.get('is_full_width_space', False):
                        # 全角空格不做任何绘制，保持空白
                        continue
                    
                    # 其他字符根据是否可渲染处理
                    if char_info.get('renderable', True):
                        # 可渲染字符正常绘制
                        draw.text((0, y_pos), char_info['char'], font=font, fill=255)  # 255表示白色文字
                    else:
                        # 不可渲染但有定义的字符绘制方框
                        box_margin = min(self.char_width, self.char_height) // 8
                        draw.rectangle(
                            [box_margin, y_pos + box_margin, 
                            self.char_width - box_margin, y_pos + self.char_height - box_margin], 
                            outline=255, fill=None)  # 使用白色轮廓
            
            # 根据位深度进行自定义颜色量化
            if self.bit_depth < 8:
                # 创建新的 P 模式图像（调色板模式）
                palette_size = 1 << self.bit_depth
                new_image = Image.new('P', final_image.size, 0)
                
                # 创建自定义调色板
                palette = []
                for i in range(palette_size):
                    # 计算适当的灰度级别（从黑到白）
                    gray_value = int(255 * i / (palette_size - 1)) if palette_size > 1 else 0
                    # 添加RGB值到调色板
                    palette.extend([gray_value, gray_value, gray_value])
                
                # 填充调色板到256个条目（PIL需要）
                palette.extend([0, 0, 0] * (256 - palette_size))
                
                # 设置调色板
                new_image.putpalette(palette)
                
                # 优化映射逻辑 - 使用numpy进行批量处理而不是逐像素循环
                import numpy as np
                
                # 将图像转换为numpy数组以加速处理
                img_array = np.array(final_image)
                
                # 创建映射函数
                def map_to_palette(pixel_value):
                    # 为了让抗锯齿效果更明显，稍微偏向于更亮的索引
                    return min(palette_size - 1, (pixel_value * palette_size) // 256)
                
                # 应用映射（使用numpy的vectorize函数进行批量处理）
                map_func = np.vectorize(map_to_palette)
                palette_indices = map_func(img_array)
                
                # 将numpy数组转回PIL图像
                for y in range(final_image.height):
                    for x in range(final_image.width):
                        new_image.putpixel((x, y), int(palette_indices[y, x]))
                
                final_image = new_image

        # 保存为BMP格式
        final_image.save(output_path, 'BMP')
        return True
    
    def generate_image(self, output_dir='output'):
        """生成字体图片"""
        from config import font_config
        
        # 确保输出目录存在
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        
        # 首先尝试加载自定义字符集
        if self.input_file and self.load_custom_charset(self.input_file, output_dir):
            # 使用自定义字符集
            print(f"使用自定义字符集: {os.path.basename(self.input_file)}")
            self.custom_charset = True
        else:
            # 使用完整字符集
            print(f"使用完整{self.charset}字符集")
            self.generate_charset()
            self.custom_charset = False
        
        # 如果没有有效字符，退出
        if not self.valid_chars:
            print("没有有定义字符可以生成图片")
            return None
        
        # 使用配置类中的文件名
        if self.charset.upper() == 'ASCII':
            bitmap_path = font_config.get_ascii_filepath(output_dir, "bmp")
            info_path = os.path.join(output_dir, f"{font_config.ascii_filename}_info.txt")
        else:
            # 主字符集
            # if hasattr(self, 'ascii_chars') and self.ascii_chars:
            #     # 如果是从GB2312中提取的ASCII字符
            #     bitmap_path = font_config.get_ascii_filepath(output_dir, "bmp")
            #     info_path = os.path.join(output_dir, f"{font_config.ascii_filename}_info.txt")
            # else:
                # 常规GB2312字符集
            bitmap_path = font_config.get_base_filepath(output_dir, "bmp")
            info_path = os.path.join(output_dir, f"{font_config.base_filename}_info.txt")
        
        # 生成位图文件
        try:
            self.generate_bitmap(bitmap_path)
            print(f"成功生成位图: {bitmap_path}")
            
            # 生成信息文件
            # 如果使用完整字符集，输出编码区间信息
            if not self.custom_charset:
                with open(info_path, 'w') as f:
                    f.write(f"字符集: {self.charset}\n")
                    f.write(f"字体: {self.font_name} ({self.font_pinyin_name})\n")
                    f.write(f"字符尺寸: {self.char_width}x{self.char_height}\n")
                    f.write(f"色深: {self.bit_depth}位\n")
                    f.write(f"有定义字符总数: {len(self.valid_chars)}\n\n")
                    f.write("有定义编码区间:\n")
                    for start_code, end_code, start_idx, end_idx in self.defined_ranges:
                        f.write(f"有定义编码：{start_code}~{end_code}，{start_idx}~{end_idx}\n")
                    
                    f.write("\n未定义编码区间:\n")
                    for start_code, end_code, start_idx, end_idx in self.undefined_ranges:
                        f.write(f"未定义编码：{start_code}~{end_code}，{start_idx}~{end_idx}\n")
                
            # 自定义字符集只输出字符列表
            else:
                with open(info_path, 'w') as f:
                    for i, char_info in enumerate(self.valid_chars):
                        renderable_status = "" if char_info.get('renderable', True) else "（不可渲染）"
                        f.write(f"{char_info['char']}")
            
            print(f"成功生成信息文件: {info_path}")
            return bitmap_path  # 返回生成的位图文件路径
        except Exception as e:
            print(f"生成位图时出错: {e}")
            traceback.print_exc()
            return None

