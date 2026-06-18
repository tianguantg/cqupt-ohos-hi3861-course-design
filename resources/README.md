# 资源获取清单

本目录用于保存课程设计中难找、难下载或容易下错版本的资料。优先使用本目录已缓存资源；没有缓存的资源按官方链接下载。

## 推荐获取仓库

默认使用 Gitee，适合国内学生：

```bash
git clone --depth 1 https://gitee.com/tianguantg/cqupt-ohos-hi3861-course-design.git
```

备用镜像：

```text
GitHub：https://github.com/tianguantg/cqupt-ohos-hi3861-course-design.git
GitCode：https://gitcode.com/tianguantg/cqupt-ohos-hi3861-course-design.git
```

## 本地已缓存资源

```text
resources/tools/HiBurn.exe
resources/course-materials/
resources/vendor-kits/bossay开发板/
resources/vendor-kits/启航KP_IOT智能开发套件/
```

重点资源：

| 资源 | 本地路径 | 用途 |
|------|----------|------|
| HiBurn | `resources/tools/HiBurn.exe` | Hi3861 固件烧录 |
| 课程指导书 | `resources/course-materials/` | 课程要求和报告依据 |
| Bossay 资料 | `resources/vendor-kits/bossay开发板/` | 备用教学资料 |
| 启航 KP-IOT 资料 | `resources/vendor-kits/启航KP_IOT智能开发套件/` | qihang 适配层、示例、数据手册 |
| `code-master.zip` | `resources/vendor-kits/启航KP_IOT智能开发套件/code-master.zip` | 启航板卡适配层 |
| OLED 示例 | `resources/vendor-kits/启航KP_IOT智能开发套件/022_OH_KP_OLED_SAMPLE.zip` | OLED 模块参考 |
| SHT30 示例 | `resources/vendor-kits/启航KP_IOT智能开发套件/023_OH_KP_SHT30_Agriculture.zip` | 温湿度模块参考 |
| SHT30 数据手册 | `resources/vendor-kits/启航KP_IOT智能开发套件/附件--SHT30-DataSheet.PDF` | 传感器协议参考 |

校验文件：

```text
resources/checksums/SHA256SUMS.txt
```

## 官方下载地址

这些链接用于从零搭建环境。若链接变化，优先搜索资源名称和版本号。

| 资源 | 推荐版本/用途 | 地址 |
|------|---------------|------|
| Ubuntu ISO | Ubuntu 20.04.6 Desktop amd64 | `https://releases.ubuntu.com/20.04/` |
| Ubuntu 清华镜像 | 国内下载更快 | `https://mirrors.tuna.tsinghua.edu.cn/ubuntu-releases/20.04/` |
| VMware Workstation | 虚拟机软件 | `https://knowledge.broadcom.com/external/article/344595/downloading-vmware-workstation-pro.html` |
| OpenHarmony manifest | OpenHarmony 3.2 Release 源码同步 | `https://gitee.com/openharmony/manifest.git` |
| Repo 工具 | 多仓库同步工具 | `https://github.com/GerritCodeReview/git-repo` |
| Repo 启动脚本 | 下载到 `~/.local/bin/repo` | `https://raw.githubusercontent.com/GerritCodeReview/git-repo/main/repo` |
| RISC-V GCC | Hi3861 GCC 7.3.0 | `https://repo.huaweicloud.com/harmonyos/compiler/gcc_riscv32/7.3.0/linux/gcc_riscv32-linux-7.3.0.tar.gz` |
| Mosquitto | Windows MQTT Broker | `https://mosquitto.org/download/` |
| Node.js | 自定义前端或完整参考 dashboard 运行环境 | `https://nodejs.org/en/download` |
| CH340 / CH341 驱动 | Type-C 串口识别 | `https://www.wch-ic.com/downloads/CH341SER_EXE.html` |
| Xshell | 串口日志工具 | `https://www.netsarang.com/en/xshell/` |

## OpenHarmony 关键版本

本项目验证组合：

```text
OpenHarmony-v3.2-Release
qihang / issedu
Hi3861
LiteOS-M
BearPi HAL 固定提交：aa0a2f91e2ec27ea641db7c4748094b817d18599
RISC-V GCC 7.3.0
```

## 不建议放入 Git 的内容

```text
真实 network_config.h
Wi-Fi 密码
个人热点密码
OpenHarmony 完整源码
OpenHarmony out/ 编译输出
node_modules/
临时日志
虚拟机镜像
```

## 下载失败时的建议

```text
repo sync 失败：降低并发到 -j2 或 -j1 --fail-fast
Git LFS 失败：重新执行 repo forall -c 'git lfs pull'
gcc_riscv32 下载慢：优先使用华为云链接或已缓存压缩包
Mosquitto 无法连接：检查 Windows 防火墙 1883 端口
自定义前端或参考 dashboard 打不开：检查端口和 Node.js 是否安装
开发板没有 COM 口：安装 CH340/CH341 驱动，换 Type-C 数据线
```
