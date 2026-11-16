# Java 引用

Java 中的引用描述了对象与 GC Roots 之间的可达关系。垃圾收集器会根据引用的强弱，决定对象是否可以回收。

## 一、引用类型总览

| 引用类型 | 对象是否会被回收 | `get()` 返回值 | 典型用途 |
|---|---|---|---|
| 强引用（Strong Reference） | 只要仍然强可达，就不会被回收 | 对象本身 | 普通对象引用 |
| 软引用（Soft Reference） | 内存不足时，可能被回收 | 对象仍存活时返回对象，否则返回 `null` | 内存敏感的缓存 |
| 弱引用（Weak Reference） | 下一次 GC 发现对象只剩弱引用时，通常就会被回收 | 对象仍存活时返回对象，否则返回 `null` | `WeakHashMap`、规范化映射 |
| 虚引用（Phantom Reference） | 不影响对象回收；对象被回收后才会入队 | 始终返回 `null` | 跟踪对象回收、清理堆外资源 |

> 软引用和弱引用都不保证对象一定存活；它们只能表达“对象可以被回收”，不能用来保证业务数据可用。

## 二、强引用

```java
Object obj = new Object();
```

只要 `obj` 仍然指向对象，该对象就是强可达的，垃圾收集器不会主动回收它。将引用置为 `null`，或让引用离开作用域后，对象才可能变成不可达对象：

```java
obj = null;
```

绝大多数 Java 代码使用的都是强引用。强引用本身没有额外的包装类，也不能通过显式调用 GC 强制回收对象。

## 三、软引用 `SoftReference`

软引用适合表示“有价值但可以丢弃”的缓存数据：

```java
Map<Key, SoftReference<Value>> cache = new HashMap<>();
cache.put(key, new SoftReference<>(value));

Value cached = cache.get(key).get();
if (cached == null) {
    // 缓存已被回收，需要重新加载
}
```

当 JVM 判断内存不足时，可能清除软引用，再回收对象。具体回收时机受 JVM 实现、堆大小和 GC 压力影响，因此不应依赖软引用实现严格的缓存淘汰策略。需要可预测的缓存容量和淘汰行为时，应使用显式缓存策略，例如基于大小或时间的淘汰。

## 四、弱引用 `WeakReference`

弱引用不会阻止对象被回收：

```java
WeakReference<Object> reference = new WeakReference<>(new Object());
Object object = reference.get();
```

如果对象不再被强引用或其他更强的引用持有，GC 会清除弱引用，并使 `get()` 返回 `null`。弱引用常用于不希望影响对象生命周期的关联关系。

### `WeakHashMap`

`WeakHashMap` 的键使用弱引用保存。键对象没有其他强引用后，GC 可以回收该键及其对应的映射项：

```java
Map<Object, String> map = new WeakHashMap<>();
Object key = new Object();
map.put(key, "value");

key = null;
// 经过 GC 后，对应的映射项可能自动消失
```

注意：如果 value 直接或间接持有 key，仍可能形成强引用链，使键无法被回收。

## 五、虚引用 `PhantomReference`

虚引用必须配合 `ReferenceQueue` 使用，并且引用对象的 `get()` 永远返回 `null`：

```java
ReferenceQueue<Object> queue = new ReferenceQueue<>();
PhantomReference<Object> reference =
        new PhantomReference<>(object, queue);
```

当对象即将被回收时，JVM 会把相关虚引用加入引用队列。程序可以监听队列，得知对象已经进入回收阶段，并执行登记过的清理动作。

虚引用不能用于恢复对象，也不能通过它访问对象内容。清理重要资源时，优先使用 `try-with-resources` 和 `AutoCloseable`；虚引用更适合作为额外的回收跟踪机制，而不是常规资源管理方案。

## 六、引用队列 `ReferenceQueue`

软引用、弱引用和虚引用都可以注册到引用队列。当 JVM 清除或处理这些引用后，会把引用对象放入队列，程序可以集中处理已失效的关联关系：

```java
ReferenceQueue<Object> queue = new ReferenceQueue<>();
WeakReference<Object> reference = new WeakReference<>(object, queue);

Reference<?> cleared;
while ((cleared = queue.poll()) != null) {
    // 删除额外的数据结构中的记录
}
```

## 七、可达性强弱

对象的可达性大致可以理解为：

1. **强可达**：从 GC Roots 出发，通过强引用可以到达。
2. **软可达**：不是强可达，但可以通过软引用到达。
3. **弱可达**：不是强可达或软可达，但可以通过弱引用到达。
4. **虚可达**：不是强、软、弱可达，但被虚引用关联。
5. **不可达**：不再被任何引用链关联，可以回收。

引用强度越弱，对象生命周期越不受程序控制。实际回收仍由垃圾收集器和 JVM 实现决定。

## 八、常见误区

- `System.gc()` 只是建议 JVM 进行 GC，不能保证立即执行。
- `SoftReference` 不等于可靠缓存，缓存命中率和回收时机都不可预测。
- `WeakReference.get()` 可能在两次访问之间变成 `null`，使用时应先保存到局部强引用中。
- `WeakHashMap` 不是线程安全容器，并且不适合承载必须长期存在的数据。
- 虚引用的 `get()` 始终为 `null`，不能用来读取对象。
- 管理文件、连接等外部资源时，应使用 `try-with-resources`，不要依赖 GC。

## 九、如何选择

| 需求 | 建议 |
|---|---|
| 对象必须一直存活 | 强引用 |
| 对象可作为内存紧张时的缓存 | 软引用，但优先考虑显式缓存淘汰策略 |
| 不希望关联关系延长对象生命周期 | 弱引用 |
| 只需要知道对象何时进入回收阶段 | 虚引用 + `ReferenceQueue` |
| 管理文件、连接等外部资源 | `try-with-resources`，不要依赖 GC |

