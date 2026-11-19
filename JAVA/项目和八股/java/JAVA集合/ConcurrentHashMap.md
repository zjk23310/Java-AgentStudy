---
summary: 分为1.7之前和1.7之后的两种实现，我都不是很理解...
tags:
  - Java/JUC
created: 2026-11-15
updated: 2026-11-18
掌握: unkown
复习时间: 2026-11-18
---

相关网址：
[ConcurrentHashMap详解 - DeepSeek](https://chat.deepseek.com/a/chat/s/ca7f2900-1f8d-4449-bd70-78eb2526af17)
[ConcurrentHashMap 源码分析 | JavaGuide](https://javaguide.cn/java/collection/concurrent-hash-map-source-code.html#_3-put-1)
上面的网站介绍了ConcurrentHashMap还有Segment

相关笔记：
[[HashMap]]

---
# ConcurrentHashMap
## 1.7
Concurrent既要线程安全又不能让所有读写串行化。它将一把大锁拆分成多把锁--分段锁
整体结构是使用两层哈希表
第一层是段数组，长度=并发度
第二层是每个段内部的小hashmap
首先需要了解这个有参构造函数的内部实现逻辑
初始化的参数有：初始化容量、加载因子、并行化等级。根据并行化等级会得到ssize,sshift和segmentMask
put方法
put方法首先会根据key计算哈希，并根据哈希获取对应的segment下标。如果对应的segment为空则会进行初始化，并返回已经初始化成功的segment，之后会向对应的segment插入相应的值
ensureSegment方法首先会根据segment的位置，却并是否当前segment位置是否为空，如果为空则获取0号segment里的hashEntry里的初始化长度，根据加载因子计算扩容阈值。后创建容量为cap的hashEntry数组。如果再次检查对应位置的segmnet为空则创建segment参数分别为lf,threshold,tab。并使用资源赋值。
最后讲解下segment段里的put操作
首先会获取ReentrantLocak独占锁，如果获取不到则scanAndLockForPut。之后计算要put的数据位置，CAS获取对应tab的first，然后遍历链表并尝试赋值（就是很像hashMap的put）。这里要看开头的HashEntry，如果为空的话会直接进行头插，如果不为空则要遍历（遍历是向next遍历，如果最后为null则肯定会得到对应的null)（还需要注意的是头插入的时候node获取到锁时是null，获取不到锁就要用scanAndLockForPut）
scan函数

get方法
计算得到key存放位置
遍历指定位置查找相同key的value

## 1.8之后
初始化
initTable
首先需要理解sizeCtl的完整语义
外层的循环需要判断tab是否为空，当不为空的时候其为终止条件。
进入循环之后根据sc=sizeCtl是否小于0判断是否有线程执行CAS成功，如果小于0，要使用Thread.yield方法
之后执行CAS，如果tab为空就初始化容量然后修改tab值和sc值。
Thread.yield方法作用是把 CPU 让给同优先级的其他线程，代价小得多，且大概率下一轮循环就能看到初始化完成。
这里的初始化其实逻辑还是比较简单的，只不过是做出了一些多线程保护，尽管可能并不会发生

put源码
按照**冲突程度逐渐升级同步手段**
这里源码就不展示了，可以去上面对应网址去看
变量**bincount**声明在循环**外**，却在**四个不同位置**被读取或修改，承担着“执行标记”“链表长度”“扩容检查参数”三种语义。`0` 表示没做有效操作（空桶插入或双重检查失败），正数表示链表遍历长度（供树化判断），固定 `2` 表示树操作已执行（阻止重复树化）；最后它作为 `addCount` 的 `check` 参数，以 `>= 0` 的语义告诉计数逻辑“这是新增，请检查扩容”。
put方法限制了key不为空
如果tab为空的话会自动触发initTable，否则如果桶内为空则CAS放入且不加锁.
如果tabAt不为空但fh为MOVED则代表是
若上述条件都不满足则进行上锁操作，上锁的对象是f(也就是桶的头)。上锁之后需要再次验证当前的tabAt是否仍然是f，因为可能会发生扩容迁移和树化。这个是否要根据fh判断是链表还是红黑树。这里改变红黑树或者链表的值这部分就是很经典的操作了，hashmap里面就有,可能多了个只展示选项。不过要格外注意bincount这个参数。

可能还需要注意的就是如何利用哈希作为身份标签，最后的spread操作会把符号位清零，导致正常的hash结果全是正数，这时就可以在存特殊类型节点的时候修改对应的hash值

问题：
1.为什么数组元素要用 `tabAt` 而不是直接 `tab[i]`？
`table` 是 `volatile Node[]`，但 volatile 只保证**引用**可见，不保证**数组元素**可见。`tabAt` 封装 `Unsafe.getObjectVolatile` 提供元素级 volatile 语义；`casTabAt` 封装 `compareAndSwapObject` 提供元素级原子写。

get方法
get方法也和hashmap的get方法基本没有区别都是使用hash值和value值，只不过获取头节点需要使用tabAt。`table` 数组引用是 volatile 的，但**数组元素不是**——Java 的 volatile 只保证引用本身的可见性，不保证数组内部元素的可见性，所以必须借助 `Unsafe` 的 volatile 语义读来看到最新桶头。


## 总结
concurrentHashMap在java1.8之后使用的是数据+链表\红黑树，初始化会获取table并校验是否为空，如果为空则根据sizeCtl决定后续操作，当sc<0则说明有进程正在或已经完成了初始化，否则就使用CAS操作设置sc并进行初始化操作初始化操作会创建一个数组并且sc会改为n的3/4作为后续的扩容时机
put操作时concurrenthashmap的重点。该方法会先判断key和value非空（这里需要注意，value也不能为空）。之后会获取table，如果为空就进行初始化，否则判断f所在位置是否为空（使用tabAt)，如果为空就使用CAS操作将对应的node放入桶内。当fh(f的哈希值)为MOVED时，进行helpTransfer，这里注意点在于fh。