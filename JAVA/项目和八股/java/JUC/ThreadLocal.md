---
summary: ThreadLocal的原理以及注意
tags:
  - Java/JUC
created: 2026-11-17
updated: 2026-11-17
掌握: unkown
复习时间: 2026-11-17
---
相关网址：
[ThreadLocal 详解 | JavaGuide](https://javaguide.cn/java/concurrent/threadlocal.html)
[ThreadLocal详解 - DeepSeek](https://chat.deepseek.com/a/chat/s/6ef1209a-432e-4232-8fd6-25e968e84490)
[Java并发常见面试题总结（下） | JavaGuide](https://javaguide.cn/java/concurrent/java-concurrent-questions-03.html)
相关笔记：


---
其提供了一种“线程封闭”的机制，让每个线程都拥有变量的独立副本，从根本上规避了共享资源的竞争问题。但其独特的底层设计，也使其成为内存泄漏和“数据串号”的高发区。

ThreadLocalMap的key是ThreadLocal。如果我在一个线程中创建了多个ThreadLocal变量，所使用的都是同一个ThreadLocalMap。这是因为ThreadLocal存数据时会调用getmap方法，getmap会从当前线程取出当前线程的ThreadLocalMap
ThreadLocal中需要在意的设计是key是弱引用，value是强引用

需要强调的就是ThreadLocal的内存泄露问题
ThreadLocalMap中的Entry的key是ThreadLocal并且是弱引用。弱引用在ThreadLocal实例不被引用的时候可能会被GC，但value是强引用，这时value是不能够被GC的，所以可能会造成内存泄露。那为什么不涉及成强引用呢？因为如果使用强引用，ThreadLocal即使因为方法结束等不被其他引用引用，但key是强引用始终持有，就会导致key、value都不会被GC，问题更加严重。虽然ThreadLocalMap有机制能够删去key为null的Entry，但还是简易在调用ThreadLocal的进程中不要忘记remove

详解里面的东西感觉好复杂，不过我觉得了解这些应该也差不多了。