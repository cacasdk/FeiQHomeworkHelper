#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
飞秋消息抓取模块 - 基于 HTTP 服务器接收 C++ 插件推送
"""

import queue
import threading
from pathlib import Path
from typing import Dict, Any, Optional

from flask import Flask, request

from utils.logging_helper import setup_logger


class FeiQMessageCapture:
    """飞秋消息抓取类 - 基于 HTTP 服务器接收 C++ 插件推送"""

    def __init__(self, config: Dict[str, Any]) -> None:
        """初始化飞秋消息抓取器

        Args:
            config: 配置信息，含 server_host, server_port, max_cache_size 等
        """
        self.logger = setup_logger('FeiQMessageCapture', 'feiq')

        # HTTP 服务器配置
        self.server_host: str = config.get('server_host', '127.0.0.1')
        self.server_port: int = config.get('server_port', 51914)
        self.max_cache_size: int = config.get('max_cache_size', 100)

        # 消息队列（线程安全）
        self.message_queue: queue.Queue = queue.Queue(maxsize=self.max_cache_size)

        # 创建 Flask 应用
        self._app: Flask = self._create_flask_app()

        # 服务器线程
        self._server_thread: Optional[threading.Thread] = None

        # 启动 HTTP 服务器
        self._start_server()

        self.logger.info(f"飞秋消息抓取器 (HTTP 服务器模式) 初始化完成，监听地址：http://{self.server_host}:{self.server_port}")

    def _create_flask_app(self) -> Flask:
        """创建 Flask 应用"""
        app = Flask(__name__)

        @app.route('/msg', methods=['POST'])
        def handle_msg():
            """处理 C++ 插件推送的消息"""
            sender = request.form.get('sender', '')
            msg = request.form.get('msg', '')

            if sender and msg:
                # 放入队列
                try:
                    self.message_queue.put_nowait({'sender': sender, 'msg': msg})
                    self.logger.debug(f"收到消息：{sender}: {msg}")
                except queue.Full:
                    self.logger.warning("消息队列已满，丢弃旧消息")

            return 'OK'

        return app

    def _start_server(self) -> None:
        """在后台线程启动 Flask 服务器"""
        def run_server():
            self._app.run(
                host=self.server_host,
                port=self.server_port,
                debug=False,
                use_reloader=False,  # 禁用重载器
                threaded=True
            )

        self._server_thread = threading.Thread(target=run_server, daemon=True)
        self._server_thread.start()

    def capture(self) -> str:
        """
        获取捕获的消息

        Returns:
            捕获到的新消息文本内容，如果没有新内容则返回空字符串
        """
        try:
            # 非阻塞获取
            message_data = self.message_queue.get_nowait()
            return f"{message_data['sender']}: {message_data['msg']}"
        except queue.Empty:
            return ""
