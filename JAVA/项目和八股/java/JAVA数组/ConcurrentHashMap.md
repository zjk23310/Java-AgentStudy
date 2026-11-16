---
summary: 分为1.7之前和1.7之后的两种实现，我都不是很理解...
tags:
  - Java/JUC
created: 2026-11-15
updated: 2026-11-15
掌握: no
复习时间: 2026-11-15
---

相关网址：
[ConcurrentHashMap详解 - DeepSeek](https://chat.deepseek.com/a/chat/s/ca7f2900-1f8d-4449-bd70-78eb2526af17)
[ConcurrentHashMap 源码分析 | JavaGuide](https://javaguide.cn/java/collection/concurrent-hash-map-source-code.html#_3-put-1)
上面的网站介绍了ConcurrentHashMap还有Segment

相关笔记：


---
ConcurrentHashMap

1.8之后
初始化
initTable
首先需要理解sizeCtl的完整语义
外层的循环需要判断tab是否为空，当不为空的时候其为终止条件。
进入循环之后根据sc=sizeCtl是否小于0判断是否有线程执行CAS成功，如果小于0，要使用Thread.yield方法
之后执行CAS，如果tab为空就初始化容量然后修改tab值和sc值。
put源码
按照冲突程度逐渐升级同步手段
这里源码就不展示了，可以去上面对应网址去看
如果tab为空的话会自动触发initTable，否则如果桶内为空则CAS放入且不加锁，成功就直接break返回
