#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
AI处理模块
"""

import os
import logging
import openai
import datetime
from pathlib import Path

class AIProcessor:
    """AI处理类"""
    
    def __init__(self, config):
        """
        初始化AI处理器
        
        Args:
            config (dict): 配置信息
        """
        self.logger = logging.getLogger('AIProcessor')
        self.logger.setLevel(logging.DEBUG)
        self.log_file = Path("..") / "logs" / "ai" / f"ai{datetime.date.today()}.log"
        try:
            handler = logging.FileHandler(self.log_file, mode='a',encoding='utf-8')
        except FileNotFoundError:
            f = open(self.log_file, 'w')
            f.close()
            handler = logging.FileHandler(self.log_file, mode='a',encoding='utf-8')
        formatter = logging.Formatter('%(asctime)s - %(pathname)s[line:%(lineno)d] - %(levelname)s: %(message)s')
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)
        
        # API配置
        self.api_key = config.get('api_key', os.environ.get('OPENAI_API_KEY', ''))
        self.api_endpoint = config.get('api_endpoint', 'https://api.openai.com/v1')
        self.model = config.get('model', 'gpt-3.5-turbo')
        self.temperature = config.get('temperature', 0.7)
        self.max_tokens = config.get('max_tokens', 1000)
        
        # 提示词配置
        self.system_prompt = config.get('system_prompt', 
            "# 输入：\n"
            "你将获得2段文本，分别为messages与markdown\n"
            "其中，messages是消息日志其中可能带有乱码等不相关的信息，markdown是一个以Markdown格式写成的作业登记版\n"
            "你的任务是，从messages中提取出作业内容，并将其添加到markdown中\n"
            "# 输出：\n"
            "输出追加messages后的markdown，**如果其中没有作业内容，则输出\"NOCHANGE\"**\n"
            "# 要求：\n"
            "1. 从messages中提取出作业内容，不允许改变作业原话，仅可改变格式，相关格式为：1. ......\\n2. ......\\n\n"
            "2. markdown中可能已经存在messages的科目title，请将追加文本放入到该title下\n"
            "3. 不允许改变其他科目内容，但可以覆盖自己科目的内容\n"
            "4. 输出时**仅输出**追加后的markdown，不要输出其他内容、提示词\n"
            "5. 对于副科（除语文数学英语外）的作业，如果作业只有一条，则无需序号，否则按照普通方式处理，如：物理只有一项作业，则 物理：......\\n\n"
            "# 示例：\n"
            "##输入示例1："
            "messages:\n"
            "X      测试用户       Carro   CARROTPC   00155D006F00?@	y   侭    测试用户 17:35:52\n"
            "   y      测试用户       Carro   CARROTPC   00155D006F00?@	y   @€ 3     语文作业：\n"
            "1. 阅读《我爱飞秋》\n"
            "2. 默写第2课生词\n"
            "markdown:\n"
            "#12.3 作业\n"
            "## 语文\n"
            "\n"
            "## 输出示例1：\n"
            "#12.3 作业\n"
            "## 语文\n"
            "1. 阅读《我爱飞秋》\n"
            "2. 默写第2课生词\n"
            "##输入示例2："
            "messages:\n"
            "      X      测试用户       Carro   CARROTPC   00155D006F00?@	y   侭    测试用户 17:43:48\n"
            "   d      测试用户       Carro   CARROTPC   00155D006F00?@	y   @€      英语：1. E听说 2. 阅读Unit2\n"
            "markdown:\n"
            "#12.3 作业 勾股定理：$a^2+b^2=c^2$\n"
            "## 语文\n"
            "1. 阅读《我爱飞秋》\n"
            "2. 默写第2课生词\n"
            "## 数学\n"
            "1. 默写勾股定理\n"
            "## 输出示例2：\n"
            "#12.3 作业 勾股定理：$a^2+b^2=c^2$\n"
            "## 语文\n"
            "1. 阅读《我爱飞秋》\n"
            "2. 默写第2课生词\n"
            "## 数学\n"
            "1. 默写勾股定理\n"
            "## 英语\n"
            "1. E听说\n"
            "2. 阅读Unit2\n"
            "## 输入示例3："
            "messages:\n"
            "        X       laoshi        Ca11    VM   00155D006F00?@	y   侭    测试用户 17:43:48\n"
            "    d      laoshi        Carro    CA   00155D006F00?@	y   @€      英语：1. E听说 2. 阅读Unit2\n"
            "    X      laoshi用户        Carro    CA11TPC   00155D006F00?@	y   侭  测试用户 17:49:46\n"
            "    Y      用户测试        Calaoshio    CARMPC   00155D006F00?@	y   @€      物理：《一》P1~4\n"
            "    ?     用测试户   v    Carro    CHIC   00155D006F00?@	y  !Y?B   /<V> 测试用户 给您发送了 1 个文件(夹),请在右侧接收.时间 17:49:54.\n"
            "    ?      测laoshi户   自己    Claoshiro    CARROTPC   00155D006F00?@	y    € ]   /<V> 文件 FeiqCfg.xml 接收完成，保存路径: “D:\\Program Files\\feiq\\Recv Files\\FeiqCfg.xml”。\n"
            "    ?      测laoshi户   laoshi   Carro    CAVMPC   00155D006F00?@	y   ?           ?     ?  宋体 P茠w冨深??^:憌  r?       ?     测试用户   自己   Carro   CARROTPC   00155D006F00?@	y  Y?Z   /<V> 文件全部接收完成。总共大小: 16KB 平均速度:498KB/s 费时:0.03秒  共1个文件:FeiqCfg.xml\n"
            "    €       测试laoshi   自己    Carro    CAFUTPC   00155D006F00?@	y  !Y?6   /<V> 文件 D:\\Program Files\\feiq\\FeiqCfg.xml 发送完成!\n"
            "markdown:\n"
            "#12.3 作业 勾股定理：$a^2+b^2=c^2$\n"
            "## 语文\n"
            "1. 阅读《我爱飞秋》\n"
            "2. 默写第2课生词\n"
            "## 数学\n"
            "1. 默写勾股定理\n"
            "## 英语\n"
            "1. E听说\n"
            "2. 阅读Unit1\n"
            "## 化学：hi\n"
            "## 输出样例3：\n"
            "#12.3 作业 勾股定理：$a^2+b^2=c^2$\n"
            "## 语文\n"
            "1. 阅读《我爱飞秋》\n"
            "2. 默写第2课生词\n"
            "## 数学\n"
            "1. 默写勾股定理\n"
            "## 英语\n"
            "1. E听说\n"
            "2. 阅读Unit2\n"
            "## 化学：hi\n"
            "物理：《一》P1~4\n"
            "## 输入示例4：\n"
            "messages:\n"
            "        X       laoshi        Ca11    VM   00155D006F00?@	y   侭    测试用户 17:43:48\n"
            "    d      laoshi        Carro    CA   00155D006F00?@	y   @€      hihihi,bushizuoye\n"
            "markdown:\n"
            "#12.3 作业 勾股定理：$a^2+b^2=c^2$\n"
            "## 语文\n"
            "1. 阅读《我爱飞秋》\n"
            "2. 默写第2课生词\n"
            "## 数学\n"
            "1. 默写勾股定理\n"
            "## 英语\n"
            "1. E听说\n"
            "2. 阅读Unit1\n"
            "## 化学：hi\n"
            "## 输出样例4：\n" 
            "NOCHANGE\n"
        )
        
        # 重试配置
        self.max_retries = config.get('max_retries', 3)
        self.retry_delay = config.get('retry_delay', 2)
        
        if not self.api_key:
            self.logger.warning("未设置API密钥，AI处理功能可能无法正常工作")
        
        self.logger.info(f"AI处理器初始化完成")
    
    def process(self, messages, markdown):
        """
        处理消息
        
        Args:
            message (dict): 消息对象
            markdown (str): Markdown处理器
            
        Returns:
            str: 处理后的格式化内容
        """
        try:
            # 构建用户提示
            user_prompt = (
                f"message:\n{messages}\nmarkdown:\n{markdown}"
            )
            
            # 构建API请求
            client = openai.OpenAI(
                base_url=self.api_endpoint,
                api_key=self.api_key
            )
            # 发送请求
            for attempt in range(self.max_retries):
                try:
                    response = client.chat.completions.create(
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
                    if(response.choices[0].message.content == 'NOCHANGE'):
                        return ""
                    return response.choices[0].message.content
                except Exception as e:
                    self.logger.error(f"API请求异常: {e}", exc_info=True)
                    if attempt < self.max_retries - 1:
                        import time
                        time.sleep(self.retry_delay)
            
            return ""
        except Exception as e:
            self.logger.error(f"AI处理失败: {e}", exc_info=True)
            return None
    
