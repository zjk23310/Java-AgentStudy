---
summary:
tags:
created: 2026-11-21
updated: 2026-11-21
掌握: unkown
复习时间: 2026-11-21
---
相关网址：
[ArrayBlockingQueue 源码分析 | JavaGuide](https://javaguide.cn/java/collection/arrayblockingqueue-source-code.html)
[队列出队顺序非公平 - DeepSeek](https://chat.deepseek.com/a/chat/s/5479c979-ea96-4b98-8570-400e49c62412)
相关笔记：
[[AQS与CLH]]

---
阻塞队列又阻塞读取也有非阻塞读取，除此之外阻塞队列还有drainTo方法，一次性将队列中所有元素存放到列表，成功会返回转义的元素数，反之返回0
# 源码
通过继承 `AbstractQueue` 获得队列所有的操作模板，其实现的入队和出队操作的整体框架。然后 `ArrayBlockingQueue` 通过实现 `BlockingQueue` 获取到阻塞队列的常见操作并将这些操作实现，填充到 `AbstractQueue` 模板方法的细节中，由此 `ArrayBlockingQueue` 成为一个完整的阻塞队列。
## 阻塞队列的初始化
阻塞队列的初始化有三种，其中重要的是
```java
// capacity 表示队列初始容量，fair 表示 锁的公平性
public ArrayBlockingQueue(int capacity, boolean fair) {
  //如果设置的队列大小小于0，则直接抛出IllegalArgumentException
  if (capacity <= 0)
      throw new IllegalArgumentException();
  //初始化一个数组用于存放队列的元素
  this.items = new Object[capacity];
  //创建阻塞队列流程控制的锁
  lock = new ReentrantLock(fair);
  //用lock锁创建两个条件控制队列生产和消费
  notEmpty = lock.newCondition();
  notFull =  lock.newCondition();
}
```

这里的notEmpty和notFull在AQS中也有提及[[AQS与CLH]],另外的构造方法都基于该方法，如只有第一个参数表示默认非公平的阻塞队列和加上Collection参数，将集合元素直接加入到阻塞队列的初始化方法

非常强烈推荐阅读[ArrayBlockingQueue 源码分析 | JavaGuide](https://javaguide.cn/java/collection/arrayblockingqueue-source-code.html#%E6%95%B4%E4%BD%93%E8%AE%BE%E8%AE%A1)讲的还是很好的，可能是这个数据结构比较简单。但是里面有关阻塞、非阻塞还有各种方法讲的都比较全

# 相关面试题
1.ArrayBlockingQueue是什么？特点是什么
2.ArrayBlockingQueue和LinkedBlockingQueue有什么区别
3.ArrayBlockingQueue和ConcurrentLinkedQueue有什么区别
4.ArrayBlockingQueue实现原理有哪些