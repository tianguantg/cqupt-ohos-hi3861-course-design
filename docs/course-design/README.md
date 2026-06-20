# 从零开始的主线步骤

这个目录只放我认为从零做到验收最需要看的步骤。课件、厂商资料和历史文件放在 `resources/` 或 `archive/`，避免主线太乱。

建议阅读顺序：

1. [00_一页路线图.md](./00_一页路线图.md)
2. [01_通用配置步骤.md](./01_通用配置步骤.md)：第一次实操主线，从准备硬件开始
3. [最终项目包 README](../../comfort-fan-final-package/README.md)
4. [最终项目联调指南](../../comfort-fan-final-package/docs/最终项目联调指南.md)
5. [编译与烧录指南](../../comfort-fan-final-package/docs/编译烧录指南.md)
6. [MQTT 测试指南](../../comfort-fan-final-package/docs/MQTT测试指南.md)
7. [简单前端模板](../../templates/simple-dashboard/README.md)
8. [Web 控制台使用指南](../../comfort-fan-final-package/docs/Web控制台使用指南.md)：完整参考实现，可选
9. [资源获取清单](../../resources/README.md)

获取仓库时默认使用 Gitee：

```bash
git clone --depth 1 https://gitee.com/tianguantg/cqupt-ohos-hi3861-course-design.git
```

按这个顺序执行，基本可以从环境搭建、首次编译、烧录串口，一直走到最终项目验收。
