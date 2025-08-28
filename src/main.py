#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
飞秋作业助手主程序
"""

import os
import sys
import time
import datetime
import logging
import yaml
from pathlib import Path

# 添加项目根目录到系统路径
ROOT_DIR = Path(__file__).parent.parent

# 导入项目模块
from feiq.message_capture import FeiQMessageCapture
from ai.processor import AIProcessor
from file.md_handler import MarkdownHandler

def load_config():
    """加载配置文件"""
    config_path = ROOT_DIR / "config" / "config.yaml"
    try:
        with open(config_path, 'r', encoding='utf-8') as f:
            return yaml.safe_load(f)
    except Exception as e:
        logging.error(f"加载配置文件失败: {e}")
        return {}

def main():
    """主函数"""
    # 设置日志
    logger = logging.getLogger(__name__)
    logger.setLevel(logging.DEBUG)
    log_file = Path("..") / "logs" / f"main{datetime.date.today()}.log"
    try:
        handler = logging.FileHandler(log_file, mode='a',encoding='utf-8')
    except FileNotFoundError:
        f = open(log_file, 'w')
        f.close()
        handler = logging.FileHandler(log_file, mode='a',encoding='utf-8')
    formatter = logging.Formatter('%(asctime)s - %(pathname)s[line:%(lineno)d] - %(levelname)s: %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)
    logger.info("飞秋作业助手启动")
    
    try:
        # 加载配置
        config = load_config()
        if not config:
            logger.error("无法加载配置，程序退出")
            return
        
        logger.info("配置加载成功")
        
        # 初始化各模块
        feiq_config = config.get('feiq', {})
        message_capture = FeiQMessageCapture(feiq_config)
        ai_processor = AIProcessor(config.get('ai', {}))
        md_handler = MarkdownHandler(config.get('markdown', {}))
        
        # 检查日志文件路径
        log_file_path = feiq_config.get('log_file_path', '')
        if not log_file_path:
            logger.error("未配置飞秋日志文件路径，请在配置文件中设置 feiq.log_file_path")
            return
        
        # 检查日志文件是否存在
        if not Path(log_file_path).exists():
            logger.error(f"飞秋日志文件不存在: {log_file_path}")
            logger.info(f"请确认飞秋软件已安装，并且日志文件路径正确")
            return
            
        logger.info(f"飞秋日志文件: {log_file_path}")
        logger.info("所有模块初始化完成")
        
        # 主循环
        try:
            while True:
                # 1. 抓取消息
                messages = message_capture.capture()
                # print(messages)
                if messages:
                    # 2. AI处理
                    processed_content = ai_processor.process(messages, md_handler.read_content())
                    if processed_content:
                        # 3. 覆盖Markdown文件
                        md_handler.rewrite_content(processed_content)
                        logger.info(f"已用处理后的内容覆盖Markdown文件")
            
                # 休眠一段时间
                time.sleep(config.get('general', {}).get('polling_interval', 5))
                
        except KeyboardInterrupt:
            logger.info("用户中断，程序退出")
        except Exception as e:
            logger.error(f"运行时错误: {e}", exc_info=True)
    
    except Exception as e:
        logger.error(f"初始化错误: {e}", exc_info=True)
    
    logger.info("飞秋作业助手已停止")

if __name__ == "__main__":
    main()