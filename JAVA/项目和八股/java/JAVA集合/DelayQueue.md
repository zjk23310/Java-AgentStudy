---
summary:
tags:
created: 2026-11-21
updated: 2026-11-21
掌握:
复习时间: 2026-11-21
---
相关网址：
[DelayQueue 源码分析 | JavaGuide](https://javaguide.cn/java/collection/delayqueue-source-code.html)
[DelayQueue与AQS详解 - DeepSeek](https://chat.deepseek.com/a/chat/s/9aff04b1-e320-4d6f-8e30-894aac309310)
相关笔记：
[[PriorityQueue]]
[[AQS与CLH]]

---
它是JUC包提供的延迟队列，用于实现延时任务。是BlockingQueue的一种，底层基于PriorityQueue实现的无界队列
`DelayQueue` 中存放的元素必须实现 `Delayed` 接口，并且需要重写 `getDelay()` 方法（计算是否到期）。

# 源码分析
## 接口继承情况
`DelayQueue` 继承了 `AbstractQueue` 类，实现了 `BlockingQueue` 、Delayed接口。
## DelayQueue的核心成员变量
```java
//可重入锁，实现线程安全的关键
private final transient ReentrantLock lock = new ReentrantLock();
//延迟队列底层存储数据的集合,确保元素按照到期时间升序排列
private final PriorityQueue<E> q = new PriorityQueue<E>();

//指向准备执行优先级最高的线程
private Thread leader = null;
//实现多线程之间等待唤醒的交互
private final Condition available = lock.newCondition();
```
## 构造方法
延迟队列的构造方法很简单，因为所有成员变量在类加载时已经初始完成，所以默认构造方法什么也没有做，还有传入Collection对象的构造函数，其调用addAll()
## 添加元素、获取元素、查看元素
这些方法在贴出的网站有，可以直接阅读
这里面也用到了AQS，AQS在JUC中还是很重要的，但是目前我依旧没有完全理解