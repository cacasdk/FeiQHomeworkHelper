#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
飞秋作业助手主程序
"""

import time
import yaml
from pathlib import Path

ROOT_DIR = Path(__file__).parent.parent

from feiq.message_capture import FeiQMessageCapture  # noqa: E402
from ai.processor import AIProcessor  # noqa: E402
from file.md_handler import MarkdownHandler  # noqa: E402
from utils.logging_helper import setup_logger  # noqa: E402

logger = setup_logger(__name__)


def load_config() -> dict:
    """加载配置文件"""
    config_path = ROOT_DIR / "config" / "config.yaml"
    try:
        with open(config_path, 'r', encoding='utf-8') as f:
            return yaml.safe_load(f)
    except yaml.YAMLError as e:
        logger.error(f"配置文件YAML解析失败: {e}")
        return {}
    except Exception as e:
        logger.error(f"加载配置文件失败: {e}")
        return {}


def main() -> None:
    """主函数"""
    logger.info("飞秋作业助手启动")

    try:
        config = load_config()
        if not config:
            logger.error("无法加载配置，程序退出")
            return

        logger.info("配置加载成功")

        feiq_config = config.get('feiq', {})
        feiq_config['root_dir'] = ROOT_DIR
        message_capture = FeiQMessageCapture(feiq_config)
        ai_config = config.get('ai', {})
        ai_config['root_dir'] = ROOT_DIR
        ai_processor = AIProcessor(ai_config)
        md_config = config.get('markdown', {})
        md_config['root_dir'] = ROOT_DIR
        md_handler = MarkdownHandler(md_config)

        log_file_path = message_capture.message_file_path
        if not log_file_path:
            logger.error("未配置飞秋日志文件路径，请在配置文件中设置 feiq.log_file_path")
            return

        if not Path(log_file_path).exists():
            logger.error(f"飞秋日志文件不存在: {log_file_path}")
            logger.info("请确认飞秋软件已安装，并且日志文件路径正确")
            return

        logger.info(f"飞秋日志文件: {log_file_path}")
        logger.info("所有模块初始化完成")

        try:
            while True:
                messages = message_capture.capture()
                if messages:
                    processed_content = ai_processor.process(
                        messages, md_handler.read_content()
                    )
                    if processed_content:
                        md_handler.rewrite_content(processed_content)
                        logger.info("已用处理后的内容覆盖Markdown文件")
                    else:
                        logger.info("无作业内容更新")

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
