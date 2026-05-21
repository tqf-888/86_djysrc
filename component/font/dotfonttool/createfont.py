# -*- coding: utf-8 -*-

import os
import re
import sys
import tkinter as tk
from tkinter import ttk, filedialog, scrolledtext
import threading
import locale
import platform
import traceback
from FontImage import FontImageGenerator
from FontArray import FontArrayGenerator
from config import FontConfig

class RedirectText:
    """用于重定向stdout到Text控件"""
    def __init__(self, text_widget):
        self.text_widget = text_widget
        self.buffer = ""

    def write(self, string):
        self.buffer += string
        self.text_widget.configure(state="normal")
        self.text_widget.insert(tk.END, string)
        self.text_widget.see(tk.END)
        self.text_widget.configure(state="disabled")
    
    def flush(self):
        pass

class FontPreview:
    """字体预览类 - 使用主窗口内的悬浮标签而非独立窗口"""
    def __init__(self, root):
        self.root = root
        self.current_font = None
        self.visible = False
        
        # 创建一个带边框的Frame作为预览容器
        self.preview_container = tk.Frame(
            self.root, 
            background="#f0f0f0", 
            borderwidth=1, 
            relief="raised"
        )
        
        # 创建预览内容框架
        self.preview_frame = tk.Frame(
            self.preview_container, 
            background="#f0f0f0", 
            padx=5, 
            pady=5
        )
        self.preview_frame.pack(fill=tk.BOTH, expand=True)
        
        # 预先创建一些标签用于显示字体预览
        self.preview_labels = []
        self.size_labels = []
        
        sizes = [16, 24, 32]
        for size in sizes:
            # 预览文本标签
            preview_label = tk.Label(
                self.preview_frame,
                text="都江堰操作系统DJYOS",
                background="#f0f0f0",
                padx=5,
                pady=2
            )
            preview_label.pack(anchor="w")
            self.preview_labels.append(preview_label)
            
            # 尺寸标签
            size_label = tk.Label(
                self.preview_frame,
                text=f"{size}x{size}",
                font=("Arial", 8),
                background="#f0f0f0"
            )
            size_label.pack(anchor="w", padx=5)
            self.size_labels.append(size_label)
        
        # 默认不显示
        self.preview_container.place_forget()
    
class FontPreview:
    """字体预览类 - 使用主窗口内的悬浮标签"""
    def __init__(self, root):
        self.root = root
        self.current_font = None
        self.visible = False
        
        # 创建一个带边框的Frame作为预览容器
        self.preview_container = tk.Frame(
            self.root, 
            background="#f0f0f0", 
            borderwidth=1, 
            relief="raised"
        )
        
        # 创建预览内容框架
        self.preview_frame = tk.Frame(
            self.preview_container, 
            background="#f0f0f0", 
            padx=5, 
            pady=5
        )
        self.preview_frame.pack(fill=tk.BOTH, expand=True)
        
        # 预先创建标签
        self.preview_labels = []
        self.size_labels = []
        
        sizes = [16, 24, 32]
        for size in sizes:
            # 预览文本标签
            preview_label = tk.Label(
                self.preview_frame,
                text="都江堰操作系统DJYOS",
                background="#f0f0f0",
                padx=5,
                pady=2
            )
            preview_label.pack(anchor="w")
            self.preview_labels.append(preview_label)
            
            # 尺寸标签
            size_label = tk.Label(
                self.preview_frame,
                text=f"{size}x{size}",
                font=("Arial", 8),
                background="#f0f0f0"
            )
            size_label.pack(anchor="w", padx=5)
            self.size_labels.append(size_label)
        
        # 添加调试信息标签
#        self.debug_label = tk.Label(
#            self.preview_frame,
#            text="预览窗口已加载",
#            background="#f0f0f0",
#            font=("Arial", 8),
#            foreground="blue"
#        )
#        self.debug_label.pack(anchor="w", pady=5)
        
        # 确保初始尺寸
        self.preview_container.update_idletasks()
        
        # 默认不显示
        self.preview_container.place_forget()
    
    def show(self, event, font_name):
        # 更新调试信息
#        self.debug_label.config(text=f"正在预览: {font_name}")
        
        # 如果字体变化，则更新内容
        if self.current_font != font_name:
            self.update_content(font_name)
        
        # 计算显示位置 - 使用绝对坐标更可靠
        x = event.x_root - self.root.winfo_rootx() + 20  # 向右偏移20像素
        y = event.y_root - self.root.winfo_rooty()
        
        # 调整位置并显示
        self.update_position(x, y)
        
        # 强制显示
        self.preview_container.lift()  # 确保在顶层
        self.preview_container.place(x=x, y=y)
        self.visible = True
    
    def update_content(self, font_name):
        """更新预览内容"""
        # 记录当前字体
        self.current_font = font_name
        
        # 创建字体对象 - 移除字体名中的 @ 前缀
        font_name_clean = font_name.replace('@', '')
        
        # 更新各个尺寸的标签
        sizes = [16, 24, 32]
        for i, size in enumerate(sizes):
            try:
                # 使用try/except确保字体设置不会失败
                preview_font = (font_name_clean, size)
                self.preview_labels[i].config(font=preview_font)
                self.preview_labels[i].config(text="都江堰操作系统DJYOS")
                self.size_labels[i].config(text=f"{size}px")
                
                # 确保标签可见
                self.preview_labels[i].pack(anchor="w")
                self.size_labels[i].pack(anchor="w", padx=5)
            except Exception as e:
                # 如果字体不可用，显示错误
                self.preview_labels[i].config(
                    font=("Arial", 8),
                    text=f"无法显示 {size}px 字体: {str(e)}",
                    foreground="red"
                )
                self.size_labels[i].pack_forget()
                
        # 调整容器大小
        self.preview_frame.update_idletasks()
        self.preview_container.update_idletasks()
    
    def update_position(self, x, y):
        """更新预览位置"""
        # 获取预览容器大小
        width = self.preview_container.winfo_reqwidth()
        height = self.preview_container.winfo_reqheight()
        
        # 防止宽高为0的情况
        if width == 0 or height == 0:
            self.preview_container.update_idletasks()
            width = self.preview_container.winfo_reqwidth() or 200  # 默认宽度
            height = self.preview_container.winfo_reqheight() or 200  # 默认高度
        
        # 获取主窗口尺寸
        root_width = self.root.winfo_width()
        root_height = self.root.winfo_height()
        
        # 如果超出右边界，向左移动
        if x + width > root_width:
            x = max(0, x - width - 40)
        
        # 如果超出下边界，向上移动
        if y + height > root_height:
            y = max(0, root_height - height)
        
        # 调试信息
#        self.debug_label.config(text=f"位置: x={x}, y={y}, 容器: {width}x{height}")
        
        # 设置位置
        self.preview_container.place(x=x, y=y)
    
    def hide(self):
        """隐藏预览"""
        if self.visible:
            self.preview_container.place_forget()
            self.visible = False
    
    def move(self, event):
        """移动预览窗口"""
        if self.visible:
            # 计算新位置
            x = event.x_root - self.root.winfo_rootx() + 20
            y = event.y_root - self.root.winfo_rooty()
            self.update_position(x, y)
    
    def destroy(self):
        """销毁预览"""
        self.preview_container.destroy()

class FontConverterApp:
    def __init__(self, root):
        self.root = root
        self.root.title("DJYGUI点阵字体转换工具")
        self.root.geometry("1024x800")
        self.root.minsize(800, 800)
        
        # 设置程序图标
        try:
            icon_path = "djyos-logo.png"  # 确保图标文件存在于程序目录
            icon = tk.PhotoImage(file=icon_path)
            self.root.iconphoto(True, icon)
        except Exception as e:
            print(f"无法加载程序图标: {str(e)}")

        # 获取系统支持的字符集（现在只有GB2312和ASCII是已实现的）
        self.supported_charsets = ['GB2312', 'ASCII']
        self.system_charsets = self.get_system_charsets()
        
        # 获取系统支持的字体
        self.system_fonts = self.get_system_fonts()
        
        # 创建变量
        self.width_var = tk.IntVar(value=16)
        self.height_var = tk.IntVar(value=16)
        self.ascii_width_var = tk.IntVar(value=8)  # ASCII默认宽度
        self.ascii_height_var = tk.IntVar(value=16)  # ASCII默认高度
        
        # 创建界面
        self.create_widgets()
        
        # 设置默认值
        self.charset_combobox.set('GB2312')
        
        # 标记是否在处理中
        self.processing = False

    def list_system_fonts(self):
        """列出系统字体"""
        # 清除当前字体列表
        self.clear_font_lists()
        
        # 重新加载系统字体
        self.system_fonts = self.get_system_fonts()
        
        # 更新未选中的字体列表
        self.populate_font_list(self.system_fonts)

    def open_font_file(self):
        """打开字体文件并列出其中的字体"""
        font_file = filedialog.askopenfilename(
            title="选择字体文件",
            filetypes=[
                ("TrueType字体", "*.ttf"), 
                ("OpenType字体", "*.otf"),
                ("TrueType Collection", "*.ttc"),
                ("所有字体文件", "*.ttf;*.otf;*.ttc"),
                ("所有文件", "*.*")
            ]
        )
        
        if not font_file:
            return
        
        # 清除当前字体列表
        self.clear_font_lists()
        
        # 提取字体文件中的字体
        fonts = self.get_fonts_from_file(font_file)
        
        if fonts:
            # 更新未选中的字体列表
            self.populate_font_list(fonts)
        else:
            print(f"未能从文件中提取字体: {font_file}")

    def get_fonts_from_file(self, font_file):
        """从字体文件中提取字体名称列表"""
        fonts = []
        
        try:
            from PIL import ImageFont
            
            # 检查是否是ttc文件（TrueType Collection）
            is_collection = font_file.lower().endswith('.ttc')
            
            if is_collection:
                # 尝试加载集合中的每个字体
                index = 0
                while True:
                    try:
                        # 尝试加载指定索引的字体
                        font = ImageFont.truetype(font_file, size=16, index=index)
                        
                        # 获取字体名称
                        font_name = None
                        try:
                            # 尝试获取字体的全名
                            font_name = font.getname()[0]
                        except:
                            # 如果无法获取名称，使用基本文件名加索引
                            base_name = os.path.basename(font_file)
                            font_name = f"{os.path.splitext(base_name)[0]}_{index}"
                        
                        fonts.append(font_name)
                        
                        # 将字体路径加入映射表
                        self.fonts_mapping[font_name] = (font_file, index)
                        
                        # 尝试下一个索引
                        index += 1
                    except:
                        # 没有更多字体，退出循环
                        break
                        
                # 如果没有找到任何字体，添加基本文件名作为字体名
                if not fonts:
                    base_name = os.path.basename(font_file)
                    font_name = os.path.splitext(base_name)[0]
                    fonts.append(font_name)
                    self.fonts_mapping[font_name] = (font_file, 0)
            else:
                # 单一字体文件
                try:
                    # 尝试加载字体
                    font = ImageFont.truetype(font_file, size=16)
                    
                    # 获取字体名称
                    try:
                        font_name = font.getname()[0]
                    except:
                        # 如果无法获取名称，使用基本文件名
                        base_name = os.path.basename(font_file)
                        font_name = os.path.splitext(base_name)[0]
                    
                    fonts.append(font_name)
                    
                    # 将字体路径加入映射表
                    self.fonts_mapping[font_name] = font_file
                except Exception as e:
                    print(f"加载字体文件失败: {str(e)}")
            
            print(f"从文件中找到 {len(fonts)} 个字体")
            
        except Exception as e:
            print(f"处理字体文件时出错: {str(e)}")
        
        return fonts

    def clear_font_lists(self):
        """清除当前的字体列表"""
        # 清除未选中列表
        for checkbox in self.unselected_checkboxes.values():
            checkbox.grid_forget()
        
        # 清除已选中列表
        for checkbox in self.selected_checkboxes.values():
            checkbox.grid_forget()
        
        # 重置字体变量和复选框字典
        self.font_vars = {}
        self.unselected_checkboxes = {}
        self.selected_checkboxes = {}

    def populate_font_list(self, fonts):
        """填充字体列表"""
        for i, font_name in enumerate(fonts):
            var = tk.BooleanVar(value=False)
            self.font_vars[font_name] = var
            
            # 创建复选框并添加到未选中区域
            checkbox = ttk.Checkbutton(
                self.unselected_scrollable_frame, 
                text=font_name, 
                variable=var,
                command=lambda v=var, f=font_name: self.update_font_lists(v, f)
            )
            checkbox.grid(row=i, column=0, sticky="w", padx=5, pady=2)
            self.unselected_checkboxes[font_name] = checkbox
            
            # 添加鼠标悬停事件以显示字体预览
            checkbox.bind("<Enter>", lambda event, font=font_name: self.show_font_preview(event, font))
            checkbox.bind("<Leave>", lambda event: self.hide_font_preview())
            checkbox.bind("<Motion>", lambda event: self.move_font_preview(event))
        
        # 更新Canvas的滚动区域
        self.unselected_canvas.configure(scrollregion=self.unselected_canvas.bbox("all"))

    def get_system_charsets(self):
        """获取系统支持的字符集"""
        charsets = []
        try:
            if platform.system() == 'Windows':
                import ctypes
                kernel32 = ctypes.windll.kernel32
                codepage_count = kernel32.GetACP()
                if codepage_count > 0:
                    # Windows上常见编码
                    charsets = ['cp1252', 'cp936', 'cp950', 'cp932', 'cp949']
            else:
                # Unix/Linux/Mac上常见编码
                charsets = ['latin-1', 'iso-8859-1', 'iso-8859-15', 
                           'cp1252', 'gb2312', 'gbk', 'big5', 'euc-jp', 'shift-jis']
        except:
            pass
        
        # 添加Python支持的编码，但排除utf-8/16/32
        import encodings
        all_encodings = set(charsets)
        for encoding in ['ascii', 'latin-1']:
            all_encodings.add(encoding)
        
        return sorted(list(all_encodings))
    
    def get_system_fonts(self):
        """获取系统支持的字体"""
        fonts = []
        self.fonts_mapping = {}  # 添加字体映射字典
        
        try:
            if platform.system() == 'Windows':
                # 在Windows上获取字体
                fonts_dir = os.path.join(os.environ.get('WINDIR', 'C:\\Windows'), 'Fonts')
                
                try:
                    import win32com.client
                    shell = win32com.client.Dispatch("Shell.Application")
                    fonts_folder = shell.NameSpace(fonts_dir)
                    
                    for i in range(fonts_folder.Items().Count):
                        try:
                            item = fonts_folder.Items().Item(i)
                            font_name = fonts_folder.GetDetailsOf(item, 0)  # 0是名称索引
                            font_file = item.Path
                            if font_name and font_file:
                                fonts.append(font_name)
                                self.fonts_mapping[font_name] = font_file
                                # 如果字体名以@开头，添加不带@的版本
                                if font_name.startswith('@'):
                                    clean_name = font_name.replace('@', '')
                                    fonts.append(clean_name)
                                    self.fonts_mapping[clean_name] = font_file
                        except:
                            continue
                    
                    if not fonts:  # 如果获取失败，使用tkinter的字体列表
                        from tkinter import font
                        fonts = list(font.families())
                except:
                    # 如果win32com失败，使用tkinter的字体列表
                    from tkinter import font
                    fonts = list(font.families())
                    
            elif platform.system() == 'Darwin':
                # Mac上获取字体 - 保持原有代码
                import subprocess
                result = subprocess.run(['system_profiler', 'SPFontsDataType'], 
                                    capture_output=True, text=True)
                import re
                fonts = re.findall(r'Location:.*?/([^/]+)\.(?:ttf|otf)', result.stdout)
            else:
                # Linux上获取字体 - 保持原有代码
                import subprocess
                result = subprocess.run(['fc-list', ':', 'family'], 
                                    capture_output=True, text=True)
                fonts = [line.strip() for line in result.stdout.split('\n') if line.strip()]
        except:
            # 失败时提供一些常见字体
            fonts = ['Arial', 'Courier New', 'Times New Roman', 'SimSun', 'SimHei']
        
        return sorted(list(set(fonts)))
    def update_font_lists(self, var, font_name):
        # 获取复选框的状态
        is_selected = var.get()
        
        if is_selected:  # 如果选中了
            # 从未选中列表中移除
            if font_name in self.unselected_checkboxes:
                self.unselected_checkboxes[font_name].grid_forget()
            
            # 创建新的复选框添加到已选中列表
            if font_name not in self.selected_checkboxes:
                checkbox = ttk.Checkbutton(
                    self.selected_scrollable_frame,
                    text=font_name, 
                    variable=var,
                    command=lambda v=var, f=font_name: self.update_font_lists(v, f)
                )
                row = len(self.selected_checkboxes)
                checkbox.grid(row=row, column=0, sticky="w", padx=5, pady=2)
                self.selected_checkboxes[font_name] = checkbox
                
                # 也给已选中列表中的复选框添加鼠标悬停事件
                checkbox.bind("<Enter>", lambda event, font=font_name: self.show_font_preview(event, font))
                checkbox.bind("<Leave>", lambda event: self.hide_font_preview())
                checkbox.bind("<Motion>", lambda event: self.move_font_preview(event))
        else:  # 如果取消选中
            # 从已选中列表中移除
            if font_name in self.selected_checkboxes:
                self.selected_checkboxes[font_name].grid_forget()
                del self.selected_checkboxes[font_name]
            
            # 显示在未选中列表中的复选框
            if font_name in self.unselected_checkboxes:
                checkbox = self.unselected_checkboxes[font_name]
                row = len([f for f in self.font_vars.keys() if not self.font_vars[f].get() and f < font_name])
                checkbox.grid(row=row, column=0, sticky="w", padx=5, pady=2)
        
        # 更新两个画布的滚动区域，确保滚动条正确显示
        self.unselected_canvas.configure(scrollregion=self.unselected_canvas.bbox("all"))
        self.selected_canvas.configure(scrollregion=self.selected_canvas.bbox("all"))

    def on_frame_configure(self, canvas, frame):
        canvas.configure(scrollregion=canvas.bbox("all"))
    def show_font_preview(self, event, font_name):
        """显示字体预览"""
        self.font_preview.show(event, font_name)

    def hide_font_preview(self):
        """隐藏字体预览"""
        self.font_preview.hide()

    def move_font_preview(self, event):
        """移动字体预览"""
        self.font_preview.move(event)

    def create_widgets(self):
        # 初始化字体预览处理器
        self.font_preview = FontPreview(self.root)
        
        main_frame = ttk.Frame(self.root, padding=10)
        main_frame.pack(fill=tk.BOTH, expand=True)
        
        # 创建左侧设置面板
        settings_frame = ttk.LabelFrame(main_frame, text="设置", padding=10)
        settings_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=False, padx=(0, 10))
        
        # 字符宽高设置
        dim_frame = ttk.Frame(settings_frame)
        dim_frame.pack(fill=tk.X, pady=5)
        
        # 宽度设置
        ttk.Label(dim_frame, text="字符宽度:").grid(row=0, column=0, sticky=tk.W)
        self.width_var = tk.IntVar(value=16)  # Set the default value here
        width_spin = ttk.Spinbox(dim_frame, from_=6, to=1000, textvariable=self.width_var, width=5)
        width_spin.grid(row=0, column=1, sticky=tk.W, padx=5)
        
        # 高度设置
        ttk.Label(dim_frame, text="字符高度:").grid(row=0, column=2, sticky=tk.W)
        self.height_var = tk.IntVar(value=16)  # Set the default value here
        height_spin = ttk.Spinbox(dim_frame, from_=6, to=1000, textvariable=self.height_var, width=5)
        height_spin.grid(row=0, column=3, sticky=tk.W, padx=5)
            
        # ASCII字符尺寸设置
        self.ascii_dim_frame = ttk.LabelFrame(settings_frame, text="ASCII字符尺寸")
        self.ascii_dim_frame.pack(fill=tk.X, pady=5)
        
        # ASCII宽度设置
        ttk.Label(self.ascii_dim_frame, text="ASCII宽度:").grid(row=0, column=0, sticky=tk.W)
        ascii_width_spin = ttk.Spinbox(self.ascii_dim_frame, from_=6, to=500, textvariable=self.ascii_width_var, width=5)
        ascii_width_spin.grid(row=0, column=1, sticky=tk.W, padx=5)
        
        # ASCII高度设置
        ttk.Label(self.ascii_dim_frame, text="ASCII高度:").grid(row=0, column=2, sticky=tk.W)
        ascii_height_spin = ttk.Spinbox(self.ascii_dim_frame, from_=6, to=1000, textvariable=self.ascii_height_var, width=5)
        ascii_height_spin.grid(row=0, column=3, sticky=tk.W, padx=5)
        
        # 字符集设置
        charset_frame = ttk.Frame(settings_frame)
        charset_frame.pack(fill=tk.X, pady=5)
        
        ttk.Label(charset_frame, text="字符集:").pack(side=tk.LEFT)
        
        # 合并并排序字符集
        all_charsets = []
        # 已支持的字符集放在前面
        for charset in self.supported_charsets:
            all_charsets.append((charset, True))
        # 未支持的字符集放在后面
        for charset in self.system_charsets:
            if charset.upper() not in [c.upper() for c in self.supported_charsets]:
                all_charsets.append((charset, False))
        
        self.charset_combobox = ttk.Combobox(charset_frame, width=15)
        # 设置值列表
        charset_values = [charset for charset, _ in all_charsets]
        self.charset_combobox['values'] = charset_values
        self.charset_combobox.pack(side=tk.LEFT, padx=5)
        
        # 自定义字符集
        input_frame = ttk.Frame(settings_frame)
        input_frame.pack(fill=tk.X, pady=5)
        
        ttk.Label(input_frame, text="自定义字库:").pack(side=tk.LEFT)
        self.input_file_var = tk.StringVar()
        self.input_entry = ttk.Entry(input_frame, textvariable=self.input_file_var, width=20)
        self.input_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)
        
        # 添加灰色提示文字
        self.input_entry.insert(0, "自动排序并删除重复字符")
        self.input_entry.config(foreground="gray")
        
        def on_entry_focus_in(event):
            if self.input_file_var.get() == "自动排序并删除重复字符":
                self.input_entry.delete(0, tk.END)
                self.input_entry.config(foreground="black")
                
        def on_entry_focus_out(event):
            if not self.input_file_var.get():
                self.input_entry.insert(0, "自动排序并删除重复字符")
                self.input_entry.config(foreground="gray")
        
        self.input_entry.bind("<FocusIn>", on_entry_focus_in)
        self.input_entry.bind("<FocusOut>", on_entry_focus_out)
        
        browse_btn = ttk.Button(input_frame, text="浏览...", command=self.browse_input_file)
        browse_btn.pack(side=tk.RIGHT)
        
        # 字体选择框结构
        font_frame = ttk.LabelFrame(settings_frame, text="字体")
        font_frame.pack(fill=tk.BOTH, expand=True, pady=5)

        font_buttons_frame = ttk.Frame(font_frame)
        font_buttons_frame.pack(fill=tk.X, pady=(0, 5))

        list_sys_fonts_btn = ttk.Button(font_buttons_frame, text="列出系统字体", command=self.list_system_fonts)
        list_sys_fonts_btn.pack(side=tk.LEFT, padx=(0, 5))

        open_font_file_btn = ttk.Button(font_buttons_frame, text="打开字体文件", command=self.open_font_file)
        open_font_file_btn.pack(side=tk.LEFT)

        # 未选中字体部分（上部）
        unselected_frame = ttk.LabelFrame(font_frame, text="未选中的字体")
        unselected_frame.pack(fill=tk.BOTH, expand=True, pady=(0, 5))
        unselected_frame.bind("<Configure>", lambda e: self.on_frame_configure(unselected_canvas, unselected_frame))
        unselected_frame.configure(height=200)
        unselected_frame.pack_propagate(False)

        # 为未选中字体添加滚动条 - 使用Frame+Scrollbar+Canvas的组合确保滚动正常工作
        # 创建Canvas和Scrollbar容器
        unselected_container = ttk.Frame(unselected_frame)
        unselected_container.pack(fill=tk.BOTH, expand=True)
        
        # 创建Canvas用于显示内容
        unselected_canvas = tk.Canvas(unselected_container, borderwidth=0, highlightthickness=0)
        # 创建垂直滚动条
        unselected_scrollbar = ttk.Scrollbar(unselected_container, orient="vertical", command=unselected_canvas.yview)
        
        # 配置Canvas的滚动区域
        unselected_scrollable_frame = ttk.Frame(unselected_canvas)
        unselected_scrollable_frame.bind(
            "<Configure>",
            lambda e: unselected_canvas.configure(scrollregion=unselected_canvas.bbox("all"))
        )
        
        # 将frame放入canvas
        unselected_canvas.create_window((0, 0), window=unselected_scrollable_frame, anchor="nw")
        # 设置canvas的yscrollcommand为scrollbar的set方法
        unselected_canvas.configure(yscrollcommand=unselected_scrollbar.set)
        
        # 放置组件
        unselected_canvas.config(width=200, height=300)
        unselected_canvas.pack(side="left", fill="both", expand=True)
        unselected_scrollbar.pack(side="right", fill="y")

        # 已选中字体部分（下部）
        selected_frame = ttk.LabelFrame(font_frame, text="已选中的字体")
        selected_frame.pack(fill=tk.BOTH, expand=True)
        selected_frame.bind("<Configure>", lambda e: self.on_frame_configure(selected_canvas, selected_frame))
        selected_frame.configure(height=100)
        selected_frame.pack_propagate(False)

        # 为已选中字体添加滚动条 - 使用相同的模式
        selected_container = ttk.Frame(selected_frame)
        selected_container.pack(fill=tk.BOTH, expand=True)
        
        selected_canvas = tk.Canvas(selected_container, borderwidth=0, highlightthickness=0)
        selected_scrollbar = ttk.Scrollbar(selected_container, orient="vertical", command=selected_canvas.yview)
        
        selected_scrollable_frame = ttk.Frame(selected_canvas)
        selected_scrollable_frame.bind(
            "<Configure>",
            lambda e: selected_canvas.configure(scrollregion=selected_canvas.bbox("all"))
        )
        
        selected_canvas.create_window((0, 0), window=selected_scrollable_frame, anchor="nw")
        selected_canvas.configure(yscrollcommand=selected_scrollbar.set)
        
        selected_canvas.config(width=200, height=300)
        selected_canvas.pack(side="left", fill="both", expand=True)
        selected_scrollbar.pack(side="right", fill="y")

        # 修改滚轮事件，根据鼠标位置滚动对应的框
        def on_mousewheel(event):
            # 判断鼠标所在区域
            widget = self.root.winfo_containing(event.x_root, event.y_root)
            
            # 向上查找父组件，确定鼠标所在的区域
            current = widget
            in_unselected = False
            in_selected = False
            
            while current:
                if current == unselected_frame or current == unselected_canvas or current == unselected_scrollable_frame:
                    in_unselected = True
                    break
                if current == selected_frame or current == selected_canvas or current == selected_scrollable_frame:
                    in_selected = True
                    break
                try:
                    current = current.master
                except AttributeError:
                    break
            
            # 根据鼠标所在区域滚动相应的画布
            if in_unselected:
                # Windows上使用event.delta, Linux/Mac可能需要不同的方法
                unselected_canvas.yview_scroll(int(-1 * (event.delta / 120)), "units")
            elif in_selected:
                selected_canvas.yview_scroll(int(-1 * (event.delta / 120)), "units")
        
        # 绑定滚轮事件到根窗口，但会根据鼠标位置判断作用区域
        self.root.bind_all("<MouseWheel>", on_mousewheel)
        
        # 字体变量和复选框字典
        self.font_vars = {}
        self.unselected_checkboxes = {}  # 未选中字体的复选框
        self.selected_checkboxes = {}    # 已选中字体的复选框
        
        # 添加字体复选框到未选中区域
        for i, font_name in enumerate(self.system_fonts):
            var = tk.BooleanVar(value=False)
            self.font_vars[font_name] = var
            
            # 创建复选框并添加到未选中区域的scrollable_frame
            checkbox = ttk.Checkbutton(
                unselected_scrollable_frame, 
                text=font_name, 
                variable=var,
                command=lambda v=var, f=font_name: self.update_font_lists(v, f)
            )
            checkbox.grid(row=i, column=0, sticky="w", padx=5, pady=2)
            self.unselected_checkboxes[font_name] = checkbox
            
            # 添加鼠标悬停事件以显示字体预览
            checkbox.bind("<Enter>", lambda event, font=font_name: self.show_font_preview(event, font))
            checkbox.bind("<Leave>", lambda event: self.hide_font_preview())
            checkbox.bind("<Motion>", lambda event: self.move_font_preview(event))
        
        # 存储引用，以便其他方法可以访问
        self.unselected_scrollable_frame = unselected_scrollable_frame
        self.selected_scrollable_frame = selected_scrollable_frame
        self.unselected_canvas = unselected_canvas
        self.selected_canvas = selected_canvas
        
        # 抗锯齿色深复选框
        depth_frame = ttk.LabelFrame(settings_frame, text="抗锯齿色深")
        depth_frame.pack(fill=tk.X, pady=5)
        
        self.depth_vars = {}
        depths = [1, 2, 4, 8]
        for i, depth in enumerate(depths):
            var = tk.BooleanVar(value=True)  # 默认选择2位色深
            self.depth_vars[depth] = var
            chk = ttk.Checkbutton(depth_frame, text=str(depth), variable=var)
            chk.grid(row=0, column=i, padx=10, pady=5)
        
        # 输出目录
        output_frame = ttk.Frame(settings_frame)
        output_frame.pack(fill=tk.X, pady=5)
        
        ttk.Label(output_frame, text="输出目录:").pack(side=tk.LEFT)
        self.output_dir_var = tk.StringVar(value="..\dotfont-gb2312\myname")
        output_entry = ttk.Entry(output_frame, textvariable=self.output_dir_var, width=20)
        output_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)
        
        output_btn = ttk.Button(output_frame, text="浏览...", command=self.browse_output_dir)
        output_btn.pack(side=tk.RIGHT)
        
        # 启动按钮
        self.start_btn = ttk.Button(settings_frame, text="开始转换", command=self.start_conversion)
        self.start_btn.pack(fill=tk.X, pady=10)
        
        # 创建右侧日志面板
        log_frame = ttk.LabelFrame(main_frame, text="日志", padding=10)
        log_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)
        
        # 进度条
        self.progress_var = tk.DoubleVar()
        self.progress = ttk.Progressbar(log_frame, variable=self.progress_var, maximum=100)
        self.progress.pack(fill=tk.X, pady=(0, 10))
        
        # 日志文本框
        self.log_text = scrolledtext.ScrolledText(log_frame, wrap=tk.WORD, state="disabled")
        self.log_text.pack(fill=tk.BOTH, expand=True)
        
        # 重定向stdout到日志文本框
        self.redirect = RedirectText(self.log_text)
        sys.stdout = self.redirect
        
        # 设置下拉框样式，使不可用的字符集以灰色显示
        self.charset_combobox['values'] = [
            f"{charset}" if supported else f"{charset} (未实现)" 
            for charset, supported in all_charsets
        ]
    
    def browse_input_file(self):
        filename = filedialog.askopenfilename(
            title="选择输入文件",
            filetypes=[("文本文件", "*.txt"), ("C/C++源文件", "*.c;*.h;*.cpp;*.hpp"), ("所有文件", "*.*")]
        )
        if filename:
            self.input_file_var.set(filename)
            self.input_entry.config(foreground="black")
    
    def browse_output_dir(self):
        directory = filedialog.askdirectory(title="选择输出目录")
        if directory:
            self.output_dir_var.set(directory)
    
    def start_conversion(self):
        if self.processing:
            return
        
        # 获取选中的字体
        selected_fonts = [font for font, var in self.font_vars.items() if var.get()]
        if not selected_fonts:
            print("请至少选择一种字体")
            return
        
        # 获取选中的色深
        selected_depths = [depth for depth, var in self.depth_vars.items() if var.get()]
        if not selected_depths:
            print("请至少选择一种色深")
            return
        
        # 获取其他参数
        width = self.width_var.get()
        height = self.height_var.get()
        ascii_width = self.ascii_width_var.get()
        ascii_height = self.ascii_height_var.get()
        charset = self.charset_combobox.get().split(" ")[0]  # 移除可能的"(未实现)"后缀
        
        # 检查所选字符集是否支持
        if charset not in self.supported_charsets:
            print(f"警告: 字符集 '{charset}' 尚未实现，可能会导致转换失败。")
        
        # 获取自定义字符集文件
        input_file = None
        if self.input_file_var.get() and self.input_file_var.get() != "自动排序并删除重复字符":
            input_file = self.input_file_var.get()
        
        output_dir = self.output_dir_var.get()
        
        if not output_dir:
            print("请指定输出目录")
            return
        
        # 创建输出目录
        os.makedirs(output_dir, exist_ok=True)
        
        # 开始转换进程
        self.processing = True
        self.start_btn.configure(state="disabled")
        self.progress_var.set(0)
        
        # 计算总任务数
        total_tasks = len(selected_fonts) * len(selected_depths)
        
        # 启动线程进行处理
        thread = threading.Thread(
            target=self.process_conversion,
            args=(width, height, ascii_width, ascii_height, charset, input_file, output_dir, selected_fonts, selected_depths, total_tasks)
        )
        thread.daemon = True
        thread.start()
    
    def process_conversion(self, width, height, ascii_width, ascii_height, charset, 
                        input_file, output_dir, fonts, depths, total_tasks):
        try:
            from config import font_config
            completed_tasks = 0
            custom_charset = False
            for font in fonts:
                for depth in depths:
                    print(f"\n开始处理: 字体={font}, 色深={depth}, 字符集={charset}")
                    
                    main_chars = []
                    main_bin_file = None
                    if charset.upper() != 'ASCII':
                        # 处理主字符集
                        generator = FontImageGenerator(
                            charset=charset,
                            font_name=font,
                            char_width=width,
                            char_height=height,
                            bit_depth=depth,
                            input_file=input_file
                        )
                        
                        # 生成统一的文件名
                        font_config.generate_filenames(
                            charset=charset,
                            input_file=input_file,
                            font_name=generator.font_pinyin_name,
                            width=width,
                            height=height,
                            ascii_width=ascii_width,
                            ascii_height=ascii_height,
                            bit_depth=depth,
                        )
                        
                        # 使用配置类指定的文件名生成主字符集图像
                        main_bmp_file = generator.generate_image(output_dir=output_dir)
                        custom_charset = generator.custom_charset
                        
                        # 如果生成了BMP文件，就正常处理
                        if main_bmp_file:
                            main_chars = [char_info['char'] for char_info in generator.valid_chars]
                            array_generator = FontArrayGenerator()
                            main_bin_file, _ = array_generator.process_and_generate(
                                bmp_file=main_bmp_file,
                                bit_depth=depth,
                                array_name=font_config.base_filename,  # 使用配置的文件名
                                char_count=len(generator.valid_chars),
                                bytes_per_char=generator.get_bytes_per_char(),
                                output_dir=output_dir,
                                font_name=font
                            )
                        # 即使没有生成BMP文件，也生成一个空的bin文件，保持流程一致
                        else:
                            print(f"主字符集没有BMP文件，创建空的bin文件")
                            array_generator = FontArrayGenerator()
                            main_bin_file = font_config.get_base_filepath(output_dir, "bin")
                            array_generator.process_and_generate(
                                bmp_file=None,  # 传入None
                                bit_depth=depth,
                                array_name=font_config.base_filename,  # 使用配置的文件名
                                char_count=0,  # 字符数为0
                                bytes_per_char=((width * depth + 7) // 8) * height,  # 计算每个字符的字节数
                                output_dir=output_dir,
                                font_name=font
                            )
                    
                    # 处理ASCII字符集
                    ascii_chars = []
                    ascii_bin_file = None
                    
                    ascii_generator = FontImageGenerator(
                        charset='ASCII',
                        font_name=font,
                        char_width=ascii_width,
                        char_height=ascii_height,
                        bit_depth=depth,
                        input_file=input_file
                    )

                     # 生成统一的文件名
                    font_config.generate_filenames(
                        charset=charset,
                        input_file=input_file,
                        font_name=ascii_generator.font_pinyin_name,
                        width=width,
                        height=height,
                        ascii_width=ascii_width,
                        ascii_height=ascii_height,
                        bit_depth=depth,
                    )
                        
                   # 如果使用自定义字符集，使用已提取的ASCII字符
                    if input_file and hasattr(generator, 'ascii_chars'):
                        ascii_chars = generator.ascii_chars
                    
                    # 生成ASCII字符集图像
                    ascii_bmp_file = ascii_generator.generate_image(output_dir=output_dir)
                    
                    # 如果生成了BMP文件，就正常处理
                    if ascii_bmp_file and ascii_generator.valid_chars:
                        array_generator = FontArrayGenerator()
                        ascii_bin_file, _ = array_generator.process_and_generate(
                            bmp_file=ascii_bmp_file,
                            bit_depth=depth,
                            array_name=font_config.ascii_filename,  # 使用配置的ASCII文件名
                            char_count=len(ascii_generator.valid_chars),
                            bytes_per_char=ascii_generator.get_bytes_per_char(),
                            output_dir=output_dir,
                            font_name=font
                        )
                    # 即使没有生成BMP文件，也生成一个空的bin文件，保持流程一致
                    elif ascii_chars:  # 只有当有ASCII字符时才需要生成
                        print(f"ASCII字符集没有BMP文件，创建空的bin文件")
                        array_generator = FontArrayGenerator()
                        ascii_bin_file = font_config.get_ascii_filepath(output_dir, "bin")
                        array_generator.process_and_generate(
                            bmp_file=None,  # 传入None
                            bit_depth=depth,
                            array_name=font_config.ascii_filename,  # 使用配置的ASCII文件名
                            char_count=0,  # 字符数为0
                            bytes_per_char=((ascii_width * depth + 7) // 8) * ascii_height,  # 计算每个字符的字节数
                            output_dir=output_dir,
                            font_name=font
                        )
                
                    # 非ASCII，执行合并
                    if charset != 'ASCII':
                        self.merge_font_libraries(
                            charset = charset,
                            ascii_bin_file=ascii_bin_file,
                            main_bin_file=main_bin_file,
                            ascii_chars=ascii_chars,
                            main_chars=main_chars,
                            ascii_width=ascii_width,
                            ascii_height=ascii_height,
                            main_width=width,
                            main_height=height,
                            bit_depth=depth,
                            output_dir=output_dir,
                            font_name=font
                        )
                    
                    # 更新进度
                    completed_tasks += 1
                    progress = (completed_tasks / total_tasks) * 100
                    self.update_progress(progress)
                # 所有色深处理完成后，为这个字体更新DJYOS组件
                self.update_djyos_component(
                    custom_charset = custom_charset,
                    font=font,
                    charset=charset,
                    width=width,
                    height=height,
                    ascii_width=ascii_width,
                    ascii_height=ascii_height,
                    input_file=input_file,
                    output_dir=output_dir,
                    ascii_chars=ascii_chars,
                    main_chars=main_chars
                )

            print("\n所有转换任务已完成!")
            
        except Exception as e:
            print(f"转换过程中出错: {str(e)}")
            traceback.print_exc()
        
        finally:
            self.root.after(0, self.reset_ui)

    def update_djyos_component(self, custom_charset, font, charset, width, height, ascii_width, ascii_height, input_file, output_dir, ascii_chars, main_chars):
        """更新DJYOS组件文件"""
        verbose = False
        font_pinyin_name = ""
        
        # 获取字体拼音名
        try:
            from FontImage import FontImageGenerator
            temp_generator = FontImageGenerator(charset=charset, font_name=font)
            font_pinyin_name = temp_generator.font_pinyin_name
        except Exception as e:
            print(f"获取字体拼音名失败: {str(e)}")
            font_pinyin_name = "default_font"
        
        # 处理ASCII字符集
        if charset.upper() == 'ASCII':
            # 删除非必要文件
            if not verbose:
                for file in os.listdir(output_dir):
                    file_path = os.path.join(output_dir, file)
                    if not (file.endswith('.bin') or file.endswith('.rsc')) and os.path.isfile(file_path):
                        try:
                            os.remove(file_path)
                            print(f"删除文件: {file_path}")
                        except Exception as e:
                            print(f"删除文件失败: {file_path}, 错误: {str(e)}")
            
            # 复制并修改ASCII模板文件
            template_file = "dot_ascii_songti_8_16.font"
            output_file = f"dot_ascii_{font_pinyin_name}_{ascii_width}_{ascii_height}.c"
            output_path = os.path.join(output_dir, output_file)
            
            try:
                # 读取模板文件
                with open(template_file, 'r', encoding='gb2312') as f:
                    content = f.read()
                
                # 修改内容
                content = content.replace("songti", font_pinyin_name)
                content = content.replace("FONT_ASCII_8_16_W", f"FONT_ASCII_{ascii_width}_{ascii_height}_W")
                content = content.replace("8_16", f"{ascii_width}_{ascii_height}")
                content = content.replace("@宋体", font)
                
                # 更新常量值
                content = re.sub(r'FONT_ASCII_\d+_\d+_W\s+\d+', f"FONT_ASCII_{ascii_width}_{ascii_height}_W {ascii_width}", content)
                content = re.sub(r'FONT_ASCII_\d+_\d+_H\s+\d+', f"FONT_ASCII_{ascii_width}_{ascii_height}_H {ascii_height}", content)
                
                # 写入新文件
                with open(output_path, 'w', encoding='gb2312') as f:
                    f.write(content)
                
                print(f"成功创建ASCII组件文件: {output_path}")
            except Exception as e:
                print(f"创建ASCII组件文件失败: {str(e)}")
                traceback.print_exc()
        else:
            # 处理非ASCII字符集
            # 删除非必要文件
            if not verbose:
                for file in os.listdir(output_dir):
                    file_path = os.path.join(output_dir, file)
                    if not (file.endswith('with_ascii.bin') or file.endswith('with_ascii.rsc')) and os.path.isfile(file_path):
                        try:
                            os.remove(file_path)
                            print(f"删除文件: {file_path}")
                        except Exception as e:
                            print(f"删除文件失败: {file_path}, 错误: {str(e)}")
            
            # 检查是否使用自定义字符集
            # custom_charset = input_file is not None and input_file != "自动排序并删除重复字符"
            
            if not custom_charset:
                # 标准字符集处理
                template_file = "dot_gb2312_songti_16_16.font"
                output_file = f"dot_{charset.lower()}_{font_pinyin_name}_{width}_{height}.c"
                output_path = os.path.join(output_dir, output_file)
                
                try:
                    # 读取模板文件
                    with open(template_file, 'r', encoding=charset) as f:
                        content = f.read()
                    
                    # 修改内容
                    content = content.replace("GB2312", charset.upper())
                    content = content.replace("songti", font_pinyin_name)
                    content = content.replace("FONT_GB2312_16_16_W", f"FONT_{charset.upper()}_{width}_{height}_W")
                    content = content.replace("FONT_GB2312_16_16_H", f"FONT_{charset.upper()}_{width}_{height}_H")
                    content = content.replace("FONT_GB2312_16_16_ASCII_W", f"FONT_{charset.upper()}_{width}_{height}_ASCII_W")
                    content = content.replace("16_16", f"{width}_{height}")
                    content = content.replace("@宋体", font)
                    
                    # 更新常量值
                    content = re.sub(r'FONT_[A-Z0-9]+_\d+_\d+_W\s+\d+', f"FONT_{charset.upper()}_{width}_{height}_W {width}", content)
                    content = re.sub(r'FONT_[A-Z0-9]+_\d+_\d+_H\s+\d+', f"FONT_{charset.upper()}_{width}_{height}_H {height}", content)
                    content = re.sub(r'FONT_[A-Z0-9]+_\d+_\d+_ASCII_W\s+\d+', f"FONT_{charset.upper()}_{width}_{height}_ASCII_W {ascii_width}", content)
                    
                    # 写入新文件
                    with open(output_path, 'w', encoding=charset) as f:
                        f.write(content)
                    
                    print(f"成功创建{charset.upper()}组件文件: {output_path}")
                except Exception as e:
                    print(f"创建{charset.upper()}组件文件失败: {str(e)}")
                    traceback.print_exc()
            else:
                # 自定义字符集处理
                input_filename = os.path.splitext(os.path.basename(input_file))[0]
                template_file = "dot_gb2312_bigfont_songti_120_120.font"
                output_file = f"dot_{charset.lower()}_{input_filename}_{font_pinyin_name}_{width}_{height}.c"
                output_path = os.path.join(output_dir, output_file)
                
                try:
                    # 读取模板文件
                    with open(template_file, 'r', encoding=charset) as f:
                        content = f.read()
                    
                    # 修改内容
                    content = content.replace("GB2312", charset.upper())
                    content = content.replace("songti", font_pinyin_name)
                    content = content.replace("bigfont", input_filename)
                    content = content.replace("FONT_GB2312_120_120_W", f"FONT_{charset.upper()}_{width}_{height}_W")
                    content = content.replace("FONT_GB2312_120_120_H", f"FONT_{charset.upper()}_{width}_{height}_H")
                    content = content.replace("FONT_GB2312_60_120_ASCII_W", f"FONT_{charset.upper()}_{width}_{height}_ASCII_W")
                    content = content.replace("120_120", f"{width}_{height}")
                    content = content.replace("@宋体", font)
                    
                    # 更新常量值
                    content = re.sub(r'FONT_[A-Z0-9]+_\d+_\d+_W\s+\d+', f"FONT_{charset.upper()}_{width}_{height}_W {width}", content)
                    content = re.sub(r'FONT_[A-Z0-9]+_\d+_\d+_H\s+\d+', f"FONT_{charset.upper()}_{width}_{height}_H {height}", content)
                    content = re.sub(r'FONT_[A-Z0-9]+_\d+_\d+_ASCII_W\s+\d+', f"FONT_{charset.upper()}_{width}_{height}_ASCII_W {ascii_width}", content)
                    
                    # 获取字符数量
                    try:                        
                        ascii_count = len(ascii_chars)
                        main_count = len(main_chars)
                        # 更新字符数量常量
                        content = re.sub(r'#define\s+FONT_GB2312_CHARS\s+\d+', f"#define FONT_{charset.upper()}_CHARS {main_count}", content)
                        content = re.sub(r'#define\s+FONT_ASCII_CHARS\s+\d+', f"#define FONT_ASCII_CHARS {ascii_count}", content)
                        
                        # 直接修改第129行和131行的字符串值
                        # 将内容分割成行以便精确修改第129行和第131行
                        lines = content.split('\n')
                    
                        # 找到包含"ascii_chars"的行和包含"main_chars"的行
                        for i, line in enumerate(lines):
                            if "const static char *ascii_chars" in line:
                                lines[i] = 'const static char *ascii_chars = "' + ''.join(ascii_chars or []) + '";'
                            elif "const static char *main_chars" in line:
                                lines[i] ='const static char *main_chars = "' + ''.join(main_chars or []) + '";' 
                              
                        # 重新组合内容
                        content = '\n'.join(lines)
                        
                    except Exception as e:
                        print(f"处理自定义字符集时出错: {str(e)}")
                        # 使用默认值
                        content = re.sub(r'#define\s+FONT_[A-Z0-9]+_CHARS\s+\d+', f"#define FONT_{charset.upper()}_CHARS 5", content)
                        content = re.sub(r'#define\s+FONT_ASCII_CHARS\s+\d+', f"#define FONT_ASCII_CHARS 5", content)
                    
                    # 写入新文件
                    with open(output_path, 'w', encoding=charset) as f:
                        f.write(content)
                    
                    print(f"成功创建自定义{charset.upper()}组件文件: {output_path}")
                except Exception as e:
                    print(f"创建自定义{charset.upper()}组件文件失败: {str(e)}")
                    traceback.print_exc()

    def merge_font_libraries(self, charset, ascii_bin_file, main_bin_file, ascii_chars, main_chars,
                            ascii_width, ascii_height, main_width, main_height, 
                            bit_depth, output_dir, font_name):
        """合并ASCII和主字符集字库"""
        try:
            from config import font_config
            
            # 使用配置中已生成的合并文件名
            merged_bin = font_config.get_merged_filepath(output_dir, "bin")
            merged_rsc = font_config.get_merged_filepath(output_dir, "rsc")
            
            # 如果没有合并文件名（可能是纯ASCII字符集），则不需要合并
            if not merged_bin or not merged_rsc:
                print("不需要合并字库，跳过合并步骤")
                return None, None
            
            # 读取二进制数据 - 对不存在的文件返回空bytes
            ascii_data = b''
            if ascii_bin_file and os.path.exists(ascii_bin_file):
                with open(ascii_bin_file, 'rb') as f:
                    ascii_data = f.read()
            
            main_data = b''
            if main_bin_file and os.path.exists(main_bin_file):
                with open(main_bin_file, 'rb') as f:
                    main_data = f.read()
            
            # 合并数据
            combined_data = ascii_data + main_data
            
            # 写入合并的二进制文件
            with open(merged_bin, 'wb') as f:
                f.write(combined_data)
            
            # 计算字符数量
            ascii_bytes_per_char = ((ascii_width * bit_depth + 7) // 8) * ascii_height
            ascii_char_count = len(ascii_data) // ascii_bytes_per_char if ascii_data else 0
            
            main_bytes_per_char = ((main_width * bit_depth + 7) // 8) * main_height
            main_char_count = len(main_data) // main_bytes_per_char if main_data else 0
            
            # 生成C代码
            with open(merged_rsc, 'w', encoding=charset) as f:
                f.write(f"/* 字体库: {font_name} */\n")
                f.write(f"/* ASCII部分: {ascii_width}x{ascii_height}, {ascii_char_count}个字符 */\n")
                f.write(f"/* 主字符集部分: {main_width}x{main_height}, {main_char_count}个字符 */\n\n")
                
                # 使用配置类中的合并文件名生成大写的宏定义名称
                # merged_macro = font_config.merged_filename.upper()
                
                # # 写入字符计数常量
                # f.write(f"#define {merged_macro}_ASCII_COUNT {ascii_char_count}\n")
                # f.write(f"#define {merged_macro}_MAIN_COUNT {main_char_count}\n\n")
                
                # # 写入ASCII和主字符集字符串
                # f.write('const char ascii_chars[] = "' + ''.join(ascii_chars or []) + '";\n\n')
                # f.write('const char main_chars[] = "' + ''.join(main_chars or []) + '";\n\n')
                
                # 写入字体数据数组
                f.write(f"const u8 {font_config.merged_filename}[] = {{\n")
                for i in range(0, len(combined_data), 16):
                    row = combined_data[i:i+16]
                    hex_values = [f"0x{b:02X}" for b in row]
                    f.write("    " + ", ".join(hex_values))
                    if i + 16 < len(combined_data):
                        f.write(",\n")
                    else:
                        f.write("\n")
                f.write("};\n")
            
            print(f"成功生成合并字库: {merged_bin}")
            print(f"成功生成合并资源文件: {merged_rsc}")
            return merged_bin, merged_rsc
            
        except Exception as e:
            print(f"合并字库时出错: {str(e)}")
            traceback.print_exc()
            return None, None


    def update_progress(self, value):
        self.root.after(0, lambda: self.progress_var.set(value))
    
    def reset_ui(self):
        self.processing = False
        self.start_btn.configure(state="normal")

def main():
    root = tk.Tk()
    app = FontConverterApp(root)
    root.mainloop()

if __name__ == '__main__':
    main()

