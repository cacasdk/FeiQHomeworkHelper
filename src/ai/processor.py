#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
AI处理模块
"""

import os
import time
import openai
from pathlib import Path


from utils.logging_helper import setup_logger


class AIProcessor:
    """AI处理类"""

    def __init__(self, config: dict) -> None:
        """
        初始化AI处理器

        Args:
            config: 配置信息
        """
        self.logger = setup_logger('AIProcessor', 'ai')

        # API配置
        self.api_key: str = config.get('api_key', os.environ.get('OPENAI_API_KEY', ''))
        self.api_endpoint: str = config.get('api_endpoint', 'https://api.openai.com/v1')
        self.model: str = config.get('model', 'gpt-3.5-turbo')
        self.temperature: float = config.get('temperature', 0.3)
        self.max_tokens: int = config.get('max_tokens', 1000)

        self.client: openai.OpenAI = openai.OpenAI(
            base_url=self.api_endpoint,
            api_key=self.api_key
        )

        # 提示词配置 - 优先从config读取，支持文件路径或纯文本
        project_root = config.get('root_dir', Path(__file__).parent.parent.parent)
        raw_prompt = config.get('system_prompt', '')
        if not raw_prompt:
            default_prompt_path = project_root / "config" / "default_prompt.txt"
            self.system_prompt: str = default_prompt_path.read_text(encoding='utf-8') if default_prompt_path.exists() else ''
        else:
            prompt_path = project_root / raw_prompt
            if prompt_path.is_file():
                self.system_prompt = prompt_path.read_text(encoding='utf-8')
                self.logger.info(f"从文件加载系统提示词: {raw_prompt}")
            else:
                self.system_prompt = raw_prompt

        # 重试配置
        self.max_retries: int = config.get('max_retries', 3)
        self.retry_delay: int = config.get('retry_delay', 2)

        if not self.api_key:
            self.logger.warning("未设置API密钥，AI处理功能可能无法正常工作")

        self.logger.info("AI处理器初始化完成")

    def process(self, messages: str, markdown: str) -> str:
        """
        处理消息

        Args:
            messages: 消息文本
            markdown: 当前Markdown内容

        Returns:
            处理后的格式化内容，无变更或出错时返回空字符串
        """
        try:
            # 构建用户提示
            user_prompt = (
                f"message:\n{messages}\nmarkdown:\n{markdown}"
            )

            # 发送请求
            for attempt in range(self.max_retries):
                try:
                    response = self.client.chat.completions.create(
                        model=self.model,
                        temperature=self.temperature,
                        max_tokens=self.max_tokens,
                        messages=[
                            {
                                'role': 'system',
                                'content': self.system_prompt
                            },
                            {
                                'role': 'user',
                                'content': user_prompt
                            }
                        ],
                        stream=False
                    )

                    content = response.choices[0].message.content
                    if content is None:
                        self.logger.warning("API返回内容为None")
                        return ""

                    if content.strip().upper() == 'NOCHANGE':
                        return ""

                    return content

                except Exception as e:
                    self.logger.error(f"API请求异常: {e}", exc_info=True)
                    if attempt < self.max_retries - 1:
                        time.sleep(self.retry_delay)

            return ""

        except Exception as e:
            self.logger.error(f"AI处理失败: {e}", exc_info=True)
            return ""
