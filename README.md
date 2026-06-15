# 鸿蒙OS智能设备开发资料

本仓库为 OpenHarmony（鸿蒙OS）智能设备开发的教学资料与配套代码，涵盖从环境搭建、C 语言设备开发到传感器模块实战的完整学习内容。

## 学生快速开始

如果你是学生，按下面顺序开始课程设计：

1. **了解课程要求**：阅读 [`docs/course-guide.md`](./docs/course-guide.md)
2. **确定选题**：参考 [`docs/project-ideas.md`](./docs/project-ideas.md) 的 5 个鸿蒙项目方案，或自拟课题（需教师审核）
3. **填写选题报告**：复制 [`templates/选题报告.md`](./templates/选题报告.md) 开始填写；如果选择智能家居环境监测系统，可直接参考 [`docs/smart-home-monitoring/`](./docs/smart-home-monitoring/)
4. **搭建开发环境**：参考 `bossay开发板/HarmonyOS设备开发环境搭建.pptx` 和 `bossay开发板/第1章 C语言HarmonyOS设备开发环境.pptx`
5. **跑通 HelloWorld**：使用 `启航KP_IOT智能开发套件/code-master.zip` 验证环境
6. **按学习路线开发**：阅读 [`docs/learning-path.md`](./docs/learning-path.md)
7. **写方案和报告**：使用 [`templates/`](./templates/) 下的模板

---

## 仓库结构

```
.
├── bossay开发板/                  # Bossay 开发板教学资料（PPT/DOCX）
│   ├── 第1章 C语言HarmonyOS设备开发环境.pptx
│   ├── 第2章 C语言HarmonyOS设备程序开发.pptx
│   ├── 第3章 C语言HarmonyOS设备程序开发方法、步骤.pptx
│   ├── HarmonyOS设备开发环境搭建.pptx
│   └── 实验用源码（适配bossay开发板）.docx
│
├── 启航KP_IOT智能开发套件/        # 启航 KP-IOT 智能开发套件
│   ├── wifiiot_app/               # 应用层代码（多传感器模块）
│   ├── OHOS/                      # OpenHarmony 硬件接口头文件
│   ├── 022_OH_KP_OLED_SAMPLE/     # OLED 显示模块示例
│   ├── *.docx / *.pdf             # 配套文档与数据手册
│   └── *.zip                      # 示例代码压缩包
│
├── docs/                          # 补充文档
│   ├── course-guide.md            # 课程目标与要求（来自课程指导书）
│   ├── learning-path.md           # 学习路径指南
│   ├── project-ideas.md           # 鸿蒙项目选题方案
│   └── smart-home-monitoring/      # 智能家居环境监测系统课程设计文档
│
├── templates/                     # 课程设计文档模板
│   ├── 选题报告.md
│   ├── 系统设计方案.md
│   ├── 测试报告.md
│   ├── 课程设计报告.md
│   └── 分工表.md
│
├── CHANGELOG.md                   # 更新日志
├── FAQ.md                         # 常见问题
└── README.md                      # 本文件
```

## 支持的开发板

### Bossay 开发板

- 适合入门学习，配套 PPT 课件详细讲解 C 语言 HarmonyOS 设备开发
- 资料涵盖：环境搭建 → 程序开发 → 开发方法与步骤

### 启航 KP-IOT 智能开发套件

- 基于华为海思 Hi3861 芯片，支持 OpenHarmony 轻量系统
- **10 个传感器/功能模块**：OLED 显示、温湿度（SHT30）、可燃气体、GPS、心率（MAX30102）、人体红外、智能语音、电机驱动、NFC、烟雾报警等
- 提供完整的 `wifiiot_app` 应用框架代码

## 快速开始

### 1. 环境搭建

参考 `bossay开发板/第1章 C语言HarmonyOS设备开发环境.pptx` 或 `bossay开发板/HarmonyOS设备开发环境搭建.pptx` 完成 DevEco Device Tool 及编译环境的安装。

### 2. 代码编译

```bash
# 以启航 KP-IOT 套件为例
# 1. 使用 code-master.zip 搭建编译环境
# 2. 将对应模块代码放入 application 目录
# 3. 结合课程环境或 OpenHarmony 工程要求完成编译集成
```

### 3. 烧录运行

编译成功后通过串口烧录至 Hi3861 开发板，按下 RESET 键运行。

## 文档与资料类型

| 类型 | 说明 | 推荐打开方式 |
|------|------|-------------|
| `.pptx` | 教学课件 | Microsoft PowerPoint / WPS |
| `.docx` | 实验手册 / 开发指南 | Microsoft Word / WPS |
| `.pdf` | 数据手册 / 原理图 | 任意 PDF 阅读器 |
| `.zip` | 示例代码包 | 解压后查看源码 |
| `.c/.h` | C 语言源码 | 任意代码编辑器 / IDE |

> 文件较大或有二进制文件需求？请参考 [FAQ.md](./FAQ.md) 了解如何处理 PPT/DOCX/PDF/ZIP 文件。

## 学习路线

推荐按以下顺序学习：

1. **环境搭建** → 安装 DevEco Device Tool，完成 HelloWorld 编译烧录
2. **C 语言基础** → 学习 HarmonyOS 设备端 C 语言编程规范
3. **GPIO / I2C / SPI** → 掌握外设通信协议
4. **传感器实战** → 逐模块学习 OLED、温湿度、电机、NFC 等案例
5. **综合项目** → 整合多模块完成一个完整物联网应用

详见 [docs/learning-path.md](./docs/learning-path.md)。

## 贡献指南

欢迎提交 Pull Request 完善本仓库资料。请遵循 [PR 模板](./.github/pull_request_template.md) 提交改动。

## 许可证

本仓库资料仅供教学与学习使用。
