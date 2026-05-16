#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Markdown文件处理模块
"""

from __future__ import annotations

from pathlib import Path

from utils.logging_helper import setup_logger


class MarkdownHandler:
    """Markdown文件处理类"""

    def __init__(self, config: dict) -> None:
        """初始化Markdown文件处理器

        Args:
            config: 配置信息，含 root_dir (Path) 用于解析相对路径
        """
        self.logger = setup_logger('MarkDownHandler', 'file')

        root_dir = config.get('root_dir', Path(__file__).parent.parent.parent)
        raw_path = config.get('file_path', 'homework.md')
        self.file_path = self._resolve_path(root_dir, raw_path)

        # 确保文件和目录存在
        if not self._ensure_file_exists():
            self.file_path.touch()
            self.logger.warning(f"目标文件不存在，已创建: {self.file_path}")

        self.logger.info(f"Markdown处理器初始化完成，目标文件: {self.file_path}")

    @staticmethod
    def _resolve_path(root_dir: Path, raw_path: str) -> Path:
        p = Path(raw_path)
        if p.is_absolute():
            return p
        return root_dir / p

    def _ensure_file_exists(self) -> bool:
        """确保目标文件及其父目录存在"""
        self.file_path.parent.mkdir(parents=True, exist_ok=True)
        if not self.file_path.exists():
            self.logger.error("目标文件不存在")
            return False
        return True

    def rewrite_content(self, content: str) -> bool:
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

    def read_content(self) -> str | None:
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
