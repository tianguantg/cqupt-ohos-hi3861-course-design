# 常见问题（FAQ）

## 关于 PPT / DOCX / PDF / ZIP 文件

### Q: 为什么仓库里有 .pptx / .docx / .pdf / .zip 这类二进制文件？

这些是配套的教学课件（PPT）、实验指导手册（DOCX）、硬件数据手册/原理图（PDF）和示例代码包（ZIP）。它们在本地开发时非常重要：

| 文件类型 | 用途 |
|----------|------|
| `.pptx` | 课堂教学课件，包含详细步骤截图 |
| `.docx` | 实验手册、开发指南，含代码说明和操作步骤 |
| `.pdf` | 芯片数据手册、电路原理图，硬件开发必备参考 |
| `.zip` | 示例代码压缩包，方便快速获取独立项目 |

### Q: GitHub 上无法预览 PPT/DOCX/PDF 怎么办？

- **PDF**：GitHub 原生支持 PDF 预览，直接点击文件即可
- **PPT/DOCX**：GitHub 不支持预览，请下载到本地后用 Microsoft Office 或 WPS 打开
- 如果文件未来超过 GitHub 单文件限制（100MB）或仓库体积明显增长，建议维护者再启用 Git LFS 或使用 Releases 发布资料包

### Q: ZIP 文件如何查看？

- GitHub 网页端可直接下载 ZIP 文件
- 下载到本地后解压，即可查看源码
- 解压后请参考内含的 README 或配套代码说明

### Q: 为什么不把 PPT/DOCX 转成 Markdown 格式？

部分课件包含大量截图、流程图和动画效果，转换为纯文本会丢失关键视觉信息。对于纯代码说明类内容（如 `022_OH_KP_OLED_SAMPLE/README.md`），已使用 Markdown 格式。

### Q: 这些大文件会让仓库体积过大吗？

当前 `.gitattributes` 已将 `.pptx`、`.docx`、`.pdf`、`.zip` 等文件标记为二进制，避免 GitHub 尝试按文本 diff 展示。若后续资料体积明显增长，建议再评估是否启用 Git LFS 或改用 GitHub Releases 发布整包资料。

## 关于环境搭建

### Q: 需要安装哪些软件？

1. **DevEco Device Tool**：华为官方 OpenHarmony 设备开发工具（含编译工具链）
2. **Visual Studio Code**：推荐代码编辑器
3. **CH340 串口驱动**：用于烧录与串口调试
4. **Git**：用于克隆或更新仓库；如果以后启用 Git LFS，再按仓库说明安装 Git LFS

### Q: 编译报错怎么办？

1. 确认 DevEco Device Tool 版本与开发板匹配
2. 检查 `SConscript` 和 `BUILD.gn` 配置文件中路径是否正确
3. 确保使用的 Hi3861 SDK 版本与代码兼容
4. 参考 `resources/vendor-kits/bossay开发板` 中的环境搭建 PPT 逐步排查

### Q: 烧录失败或串口无输出？

- 检查 USB 转串口驱动是否安装（CH340）
- 确认串口号和波特率（通常 115200）
- 确保开发板已正确供电
- 尝试更换 USB 线缆或 USB 端口

## 关于代码

### Q: wifiiot_app 中的模块如何单独编译？

每个模块目录下有独立的 `SConscript` 文件。使用 SCons 构建系统时，修改顶层 `app.json` 或构建配置，指定需要编译的模块即可。

### Q: 可以在非 Hi3861 平台上运行这些代码吗？

代码中的 `wifiiot_*.h` 接口是 OpenHarmony Hi3861 平台的硬件抽象层 API。迁移到其他平台需要适配对应的 HAL 接口。

### Q: code-master.zip 里的代码是做什么的？

`code-master.zip` 是一个基础的 HelloWorld 项目，用于验证编译环境是否搭建成功。建议先使用此项目跑通"编译→烧录→运行"全流程，再导入其他模块代码。

## 关于学习

### Q: 零基础如何开始？

建议按照 [docs/learning-path.md](./docs/learning-path.md) 中的路径，从环境搭建 → HelloWorld → 基础外设 → 传感器模块循序渐进。

### Q: 遇到问题如何求助？

1. 查阅本 FAQ 和相关文档
2. 搜索 [OpenHarmony 官方文档](https://docs.openharmony.cn/)
3. 在 GitHub Issues 中提交问题（请附带错误日志和开发环境信息）
