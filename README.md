# 飞秋作业助手 (FeiQ Homework Helper)

一个自动从飞秋软件抓取作业信息，通过AI处理后格式化保存到Markdown文件的Python工具。

## 功能特点

- 通过读取飞秋日志文件(feiq.fql)自动抓取接收到的消息
- 智能筛选与作业相关的信息
- 使用AI处理和格式化作业内容
- 将处理后的内容追加到指定的Markdown文件中

## 系统要求

- Python 3.8+
- 飞秋软件（需要启用消息记录功能）
- 互联网连接（用于AI服务）

## 安装方法

1. 克隆或下载本仓库
2. 安装依赖包：
   ```
   pip install -r requirements.txt
   ```
3. 配置 `config/config.yaml` 文件

## 使用方法

1. 启动飞秋软件
2. 运行本程序：
   ```
   python src/main.py
   ```
3. 程序将自动监控飞秋消息，处理作业信息，并更新到指定的Markdown文件

## 配置说明

详细配置选项请参考 `config/config.yaml` 文件中的注释说明。

## 许可证

MIT

