---
summary: 该模块主要用于实体和数据传输对象的转换
tags:
created: 2026-11-21
updated: 2026-11-21
---
相关网址：
[MapStruct 实体映射最佳实践](https://www.yuque.com/snailclimb/itdq8h/ewytsrnw5rihf1m9)
[MapStruct实体映射详解 - DeepSeek](https://chat.deepseek.com/a/chat/s/2dec1202-a56b-4ae7-82dc-63dc6a603595)
相关笔记：


---
# 简介
**MapStruct** 是一个面向 Java Bean 映射的 **编译时代码生成器**。它遵循“约定优于配置”的理念：在编译期读取 `@Mapper`、`@Mapping` 等注解与方法签名，生成明确、可读的映射实现代码；与依赖反射的方案不同，MapStruct 的映射逻辑在运行时就是普通的 Java 方法调用与属性赋值。