# 重庆邮电大学 OpenHarmony Hi3861 课程设计资料库

本仓库整理为后续学生可按步骤完成课程设计的资料库。目标是减少环境试错，把难下载资源、配置步骤、设备端源码、MQTT 验证方法、前端模板和验收清单放到清晰位置。

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

建议按下面顺序完成课程设计：

1. 阅读 [一页路线图](./docs/course-design/00_一页路线图.md)，确认整体流程。
2. 按 [通用配置步骤](./docs/course-design/01_通用配置步骤.md) 从硬件准备开始实操，配好编译、烧录和串口环境。
3. 按 [最终项目联调指南](./comfort-fan-final-package/docs/最终项目联调指南.md) 自己合入设备端代码并重新编译固件。
4. 使用 `resources/tools/HiBurn.exe` 烧录自己编译出的 `Hi3861_wifiiot_app_allinone.bin`。
5. 先用 [MQTT 测试指南](./comfort-fan-final-package/docs/MQTT测试指南.md) 跑通命令行收发。
6. 基于 [simple-dashboard 模板](./templates/simple-dashboard/README.md) 编写自己的前端，或参考完整 dashboard 进一步优化。

常用入口：

- [编译与烧录指南](./comfort-fan-final-package/docs/编译烧录指南.md)
- [MQTT 测试指南](./comfort-fan-final-package/docs/MQTT测试指南.md)
- [简单前端模板](./templates/simple-dashboard/README.md)
- [Web 控制台使用指南](./comfort-fan-final-package/docs/Web控制台使用指南.md)：完整参考实现，可选
- [难下载资源清单](./resources/README.md)

## 学生最快路线

如果你只想完成本课程设计，按这个顺序阅读：

1. [一页路线图](./docs/course-design/00_一页路线图.md)
2. [通用配置步骤](./docs/course-design/01_通用配置步骤.md)：第一次实操主线，从准备硬件开始
3. [最终项目包说明](./comfort-fan-final-package/README.md)
4. [最终项目联调指南](./comfort-fan-final-package/docs/最终项目联调指南.md)
5. [编译与烧录指南](./comfort-fan-final-package/docs/编译烧录指南.md)
6. [MQTT 测试指南](./comfort-fan-final-package/docs/MQTT测试指南.md)
7. [简单前端模板](./templates/simple-dashboard/README.md)
8. [Web 控制台使用指南](./comfort-fan-final-package/docs/Web控制台使用指南.md)：完整参考实现，可选
9. [资源获取清单](./resources/README.md)

## 当前最终项目

项目：启航舒适风扇 v6

功能：

- SHT30 温湿度采集
- SH1106 OLED 本地显示
- OFF / QUIET / AUTO / ECO 模式
- MQTT 状态上报和命令控制
- MQTT 远程控制
- 简单 Web 前端或自定义前端控制
- 默认 Motor ON 低功率常转，`MOTOR_OFF` 可关闭常转但保留敏感脉冲反馈；噪音较大时可关闭常转

最终项目目录：

```text
comfort-fan-final-package/
```

说明：仓库中保留了教师验收、故障对照和历史归档用的成品资源，但学生主线默认要求自己编译固件、自己完成 MQTT 验证和前端实现。

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
├── comfort-fan-final-package/     # 最终项目源码、联调文档、脚本和参考实现
├── firmware/
│   └── final/                     # 教师验收和故障对照用固件，学生主线不依赖
├── resources/
│   ├── README.md                  # 难下载资源索引
│   ├── tools/                     # 本地缓存工具，例如 HiBurn
│   ├── course-materials/          # 课程指导书和课件
│   └── vendor-kits/               # Bossay 与启航套件资料
├── releases/                      # 历史交付压缩包
├── archive/                       # 历史临时文件和重复文件归档
└── templates/                     # 报告模板和简单前端模板
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

前端开发起点：

```text
templates/simple-dashboard/
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
- 不适合直接提交或可能更新的资源，在 [resources/README.md](./resources/README.md) 中写清官方下载地址、版本和用途。
- 真实 Wi-Fi 密码、`network_config.h`、`node_modules/`、OpenHarmony 编译输出不提交。
- 旧文件不直接删除，统一放入 `archive/` 追溯。

## 许可证与用途

本仓库用于课程学习、课程设计复现和教学归档。第三方工具、课件、数据手册和开发板资料请遵循其原始许可和学校课程要求。
