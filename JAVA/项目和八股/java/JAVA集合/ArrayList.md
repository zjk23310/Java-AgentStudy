---
summary:
tags:
created: 2026-11-20
updated: 2026-11-20
掌握:
复习时间: 2026-11-20
---
相关网址：
[ArrayList 源码分析 | JavaGuide](https://javaguide.cn/java/collection/arraylist-source-code.html)
相关笔记：


---
ArrayList相当于**动态数组**，其容量可以实现动态增长
添加大量元素前可以使用ensureCapacity增加实例容量
# 实现接口
其实现了list、RandomAccess、Cloneable、Serializable接口

ArrayList可以存储任何类型的对象，包含Null,但是并不建议
# 与LinkedList区别
1.是否保证线程安全，
2.底层数据结构
3.插入与删除是否受元素位置影响
4.是否支持快速随机访问
5.内存空间占用

# 源码
## 初始化
