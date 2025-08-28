#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
飞秋消息抓取模块 - 基于消息文件(feiq.fql)
简化版：只处理GBK编码的文本消息
"""

import logging
import datetime
from pathlib import Path

class FeiQMessageCapture:
    """飞秋消息抓取类 - 基于消息文件(feiq.fql) - 简化版，只处理GBK编码的文本消息"""
    
    def __init__(self, config):
        """
        初始化飞秋消息抓取器 - 简化版，只处理GBK编码的文本消息
        
        Args:
            config (dict): 配置信息
        """
        self.logger = logging.getLogger('FeiQMessageCapture')
        self.logger.setLevel(logging.DEBUG)
        self.log_file = Path("..") / "logs" / "feiq" / f"feiq{datetime.date.today()}.log"
        try:
            handler = logging.FileHandler(self.log_file, mode='a',encoding='utf-8')
        except FileNotFoundError:
            f = open(self.log_file, 'w')
            f.close()
            handler = logging.FileHandler(self.log_file, mode='a',encoding='utf-8')
        formatter = logging.Formatter('%(asctime)s - %(pathname)s[line:%(lineno)d] - %(levelname)s: %(message)s')
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)
        # 消息文件配置
        self.message_file_path = config.get('log_file_path', '')
        self.message_file_encoding = config.get('log_file_encoding', 'gbk')
        
        # 消息存储
        self.message_cache = []
        self.max_cache_size = config.get('max_cache_size', 100)
        self.last_file_size = 0
        self.logger.info(f"飞秋消息抓取器(简化版消息文件模式)初始化完成，消息文件: {self.message_file_path}，编码: {self.message_file_encoding}")
        
        # 初始化时检查消息文件
        self.check_message_file()
    
    def check_message_file(self):
        """检查消息文件是否存在并获取初始大小
        
        Returns:
            bool: 消息文件是否存在且可访问
        """
        # 检查消息文件是否存在
        if not self.message_file_path:
            self.logger.error("未指定飞秋消息文件路径")
            return False
        
        message_file = Path(self.message_file_path)
        if not message_file.exists():
            self.logger.error(f"飞秋消息文件不存在: {self.message_file_path}")
            return False
        
        # 获取文件大小，用于后续增量读取
        self.last_file_size = message_file.stat().st_size
        self.logger.info(f"飞秋消息文件大小: {self.last_file_size} 字节")
        
        return True
    
    def capture(self):
        """
        获取捕获的消息
        
        Returns:
            str: 捕获到的新消息文本内容，如果没有新内容则返回空字符串
        """
        try:
            # 检查消息文件是否存在
            message_file = Path(self.message_file_path)
            if not message_file.exists():
                self.logger.error(f"飞秋消息文件不存在: {self.message_file_path}")
                return ""
            
            # 获取当前文件大小
            current_size = message_file.stat().st_size
            
            # 处理文件大小变化情况
            if current_size < self.last_file_size:
                # 文件可能被截断或重新创建，重置文件大小并从头开始读取
                self.logger.warning(f"文件大小减小 (从 {self.last_file_size} 到 {current_size} 字节)，可能被截断或重新创建")
                self.last_file_size = current_size - 500
                return self.capture()  # 重新尝试捕获
            elif current_size == self.last_file_size:
                # 文件大小没有变化，没有新消息
                return ""
            
            # 读取新增内容
            with open(message_file, 'rb') as f:
                # 定位到上次读取的位置
                f.seek(self.last_file_size)
                # 读取新增内容
                new_content = f.read()
            
            # 更新文件大小
            self.last_file_size = current_size
            
            # 解析新增内容 - 只处理GBK编码的文本消息，直接返回文本
            try:
                # 尝试解码内容并直接返回文本
                new_content_text = new_content.decode(self.message_file_encoding, errors='replace')
            except Exception as e:
                self.logger.error(f"解析消息内容异常: {e}", exc_info=True)
                new_content_text = ""
            
            # 如果有新内容，记录日志
            if new_content_text:
                self.logger.info(f"从消息文件中捕获到新内容，长度: {len(new_content_text)} 字符")        
                return new_content_text
        
        except Exception as e:
            self.logger.error(f"捕获消息异常: {e}", exc_info=True)
        
        return ""
