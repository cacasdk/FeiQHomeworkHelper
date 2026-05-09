#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
共享日志工具 - 避免各模块重复日志初始化代码
"""

import logging
import datetime
from pathlib import Path

PROJECT_ROOT = Path(__file__).parent.parent.parent


def setup_logger(name: str, subdir: str = "") -> logging.Logger:
    """创建并配置一个带有文件输出的日志记录器

    Args:
        name: 日志记录器名称
        subdir: 日志子目录名称（如 'ai', 'feiq', 'file'），为空则写入 logs/ 根目录

    Returns:
        logging.Logger: 配置好的日志记录器
    """
    logger = logging.getLogger(name)
    logger.setLevel(logging.DEBUG)

    if subdir:
        log_dir = PROJECT_ROOT / "logs" / subdir
    else:
        log_dir = PROJECT_ROOT / "logs"

    log_dir.mkdir(parents=True, exist_ok=True)

    log_file = log_dir / f"{subdir if subdir else 'main'}{datetime.date.today()}.log"

    handler = logging.FileHandler(log_file, mode='a', encoding='utf-8')
    formatter = logging.Formatter(
        '%(asctime)s - %(pathname)s[line:%(lineno)d] - %(levelname)s: %(message)s'
    )
    handler.setFormatter(formatter)
    logger.addHandler(handler)

    return logger
