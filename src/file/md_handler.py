#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Markdown文件处理模块
"""

import logging
import datetime
from pathlib import Path

class MarkdownHandler:
    """Markdown文件处理类"""
    
    def __init__(self, config):
        """
        初始化Markdown文件处理器
        
        Args:
            config (dict): 配置信息
        """
        self.logger = logging.getLogger('MarkDownHandler')
        self.logger.setLevel(logging.DEBUG)
        self.log_file = Path("..") / "logs" / "file" / f"file{datetime.date.today()}.log"
        try:
            handler = logging.FileHandler(self.log_file, mode='a',encoding='utf-8')
        except FileNotFoundError:
            f = open(self.log_file, 'w')
            f.close()
            handler = logging.FileHandler(self.log_file, mode='a',encoding='utf-8')
        formatter = logging.Formatter('%(asctime)s - %(pathname)s[line:%(lineno)d] - %(levelname)s: %(message)s')
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)
        
        # 文件配置
        self.file_path = Path(config.get('file_path', '..\\homework.md'))
        
        # 确保文件和目录存在
        self._ensure_file_exists()
        
        self.logger.info(f"Markdown处理器初始化完成，目标文件: {self.file_path}")
    
    def _ensure_file_exists(self):
        """确保目标文件存在"""
        if not self.file_path.exists():
            self.logger.error(f"目标文件不存在")
            return False
        return True
    def rewrite_content(self, content):
        """
        覆盖Markdown文件
        
        Args:
            content (str): 内容
            
        Returns:
            bool: 是否成功覆盖
        """
        if not content:
            return False
        
        try:
            # 写入文件
            with open(self.file_path, 'w', encoding='utf-8') as f:
                f.write(content)
            
            self.logger.info(f"成功将内容覆盖文件: {self.file_path}")
            return True
        except Exception as e:
            self.logger.error(f"覆盖内容失败: {e}", exc_info=True)
            return False
    
    def read_content(self):
        """
        读取Markdown文件内容
        
        Returns:
            str: 文件内容，如果读取失败则返回None
        """
        try:
            with open(self.file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            self.logger.info(f"成功读取文件内容: {self.file_path}")
            return content
        except Exception as e:
            self.logger.error(f"读取文件内容失败: {e}", exc_info=True)
            return None