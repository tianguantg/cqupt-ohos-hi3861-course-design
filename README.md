# 重庆邮电大学 OpenHarmony Hi3861 课程设计资料库

本仓库整理为后续学生可直接复现的课程设计资料库。目标是减少环境试错，把难下载资源、最终项目源码、固件、烧录步骤、MQTT/Web 控制台和验收清单放到清晰位置。

## 先从这里开始

推荐使用 Gitee 获取仓库，国内学生通常无需额外网络配置：

```bash
git clone --depth 1 https://gitee.com/tianguantg/cqupt-ohos-hi3861-course-design.git
```

备用镜像：

```text
GitHub：https://github.com/tianguantg/cqupt-ohos-hi3861-course-design.git
GitCode：https://gitcode.com/tianguantg/cqupt-ohos-hi3861-course-design.git
```

只想完成课程设计并复现最终效果：

1. 阅读 [一页路线图](./docs/course-design/00_一页路线图.md)，确认整体流程。
2. 按 [通用配置步骤](./docs/course-design/01_通用配置步骤.md) 配好编译、烧录和串口环境。
3. 直接烧录最终固件：`firmware/final/qihang_comfort_fan_mqtt_v6_fast_report_allinone.bin`。
4. 使用 HiBurn：`resources/tools/HiBurn.exe`。
5. 启动 Web 控制台：`comfort-fan-final-package/dashboard/`。
6. 如果只想拿完整交付包，下载：`releases/comfort-fan-final-package-20260618.zip`。

常用入口：

- [编译与烧录指南](./comfort-fan-final-package/docs/编译烧录指南.md)
- [MQTT 测试指南](./comfort-fan-final-package/docs/MQTT测试指南.md)
- [Web 控制台使用指南](./comfort-fan-final-package/docs/Web控制台使用指南.md)
- [难下载资源清单](./resources/README.md)

## 学生最快路线

如果你只想完成本课程设计，按这个顺序阅读：

1. [一页路线图](./docs/course-design/00_一页路线图.md)
2. [通用配置步骤](./docs/course-design/01_通用配置步骤.md)
3. [最终项目包说明](./comfort-fan-final-package/README.md)
4. [编译与烧录指南](./comfort-fan-final-package/docs/编译烧录指南.md)
5. [MQTT 测试指南](./comfort-fan-final-package/docs/MQTT测试指南.md)
6. [Web 控制台使用指南](./comfort-fan-final-package/docs/Web控制台使用指南.md)
7. [资源获取清单](./resources/README.md)

## 当前最终项目

项目：启航舒适风扇 v6

功能：

- SHT30 温湿度采集
- SH1106 OLED 本地显示
- OFF / QUIET / AUTO / ECO 模式
- MQTT 状态上报和命令控制
- Web 控制台远程控制
- 默认 Motor ON 低功率常转，`MOTOR_OFF` 可关闭常转但保留敏感脉冲反馈；噪音较大时可关闭常转

最终固件：

```text
firmware/final/qihang_comfort_fan_mqtt_v6_fast_report_allinone.bin
```

最终项目目录：

```text
comfort-fan-final-package/
```

最终发布压缩包：

```text
releases/comfort-fan-final-package-20260618.zip
```

## 仓库结构

```text
.
├── README.md
├── docs/
│   ├── course-design/             # 本课程设计从零到验收的主线文档
│   ├── course-guide.md
│   ├── learning-path.md
│   ├── project-ideas.md
│   └── smart-home-monitoring/
├── comfort-fan-final-package/     # 最终项目源码、固件、Web 控制台、脚本和证据
├── firmware/
│   └── final/                     # 方便直接烧录的最终固件
├── resources/
│   ├── README.md                  # 难下载资源索引
│   ├── tools/                     # 本地缓存工具，例如 HiBurn
│   ├── course-materials/          # 课程指导书和课件
│   └── vendor-kits/               # Bossay 与启航套件资料
├── releases/                      # 交付压缩包
├── archive/                       # 历史临时文件和重复文件归档
└── templates/                     # 课程设计报告模板
```

## 核心命令

连接虚拟机：

```powershell
ssh ohos@192.168.158.129
```

编译：

```bash
export PATH="$HOME/.local/bin:$HOME/tools/gcc_riscv32/bin:$PATH"
cd /home/ohos/workspace/openharmony-3.2-release
hb build -f
```

复制固件：

```powershell
scp ohos@192.168.158.129:/home/ohos/workspace/openharmony-3.2-release/out/qihang/qihang/Hi3861_wifiiot_app_allinone.bin "C:\OpenHarmony\share\"
```

启动 Web 控制台：

```powershell
Set-Location "C:\OpenHarmony\cqupt-ohos-hi3861-course-design\comfort-fan-final-package\dashboard"
npm install
npm start
```

默认访问：

```text
http://127.0.0.1:8787
```

## 烧录与串口

标准流程：

```text
Type-C 连接开发板
→ 设备管理器查看“端口（COM 和 LPT）”中新出现的 COM 口
→ HiBurn 选择该串口
→ 勾选 Auto burn
→ 点击 Connect
→ 按开发板 Reset
→ 烧录完成后点击 Disconnect
→ Xshell 选择 Serial
→ 属性 -> 连接 -> 串口 -> 选择同一 COM 口
→ 波特率 115200
→ 连接后按 Reset 查看日志
```

## 资源原则

- 能公开提交的小工具和资料放在 `resources/`。
- 不适合直接提交或可能更新的资源，在 `resources/README.md` 中写清官方下载地址、版本和用途。
- 真实 Wi-Fi 密码、`network_config.h`、`node_modules/`、OpenHarmony 编译输出不提交。
- 旧文件不直接删除，统一放入 `archive/` 追溯。

## 许可证与用途

本仓库用于课程学习、课程设计复现和教学归档。第三方工具、课件、数据手册和开发板资料请遵循其原始许可和学校课程要求。
