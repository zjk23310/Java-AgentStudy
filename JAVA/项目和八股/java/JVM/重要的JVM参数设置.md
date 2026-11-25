---
summary:
tags:
created: 2026-11-25
updated: 2026-11-25
掌握:
复习时间: 2026-11-25
---
相关网址：
[最重要的JVM参数总结 | JavaGuide](https://javaguide.cn/java/jvm/jvm-parameters-intro.html#%E5%A0%86%E5%86%85%E5%AD%98%E7%9B%B8%E5%85%B3)
相关笔记：


---
# 堆内存相关
## 设置堆内存大小
可以设置初始和最大堆内存大小，这是性能调优常见实践，两者设置为相同值可以减少运行时调整堆容量的开销
```
-Xms<heap size>[unit]  # 设置 JVM 初始堆大小
-Xmx<heap size>[unit]  # 设置 JVM 最大堆大小
```
heap size是大小，unit是单位
```
-Xms4G -Xmx4G
```
初始堆和最大堆大小都是4g
## 设置新生代内存大小
### 通过XX:NewSize和XX:MaxNewSize指定
分别设置初始大小和最大大小。下面给出具体的代码示例
```
-XX:NewSize=512m -XX:MaxNewSize=1024m
```
### 通过\-Xmn\<young size>\[unit]指定
这个和上面的设置堆内存大小差不多
这里需要注意的一点是：要尽量让新创建的对象进入新生代而不是老年代，因为MinorGC的成本要远远低于FullGC
此外，还可以
-XX:NewRatio=\<int> 参数来设置**老年代与整个新生代（包括 Eden 和两个 Survivor 区）的内存大小比例**。
```
-XX:NewRatio=2
```
## 设置永久代/元空间大小
1.8之前永久代没有被彻底移除时可以通过下面参数来调节方法区大小
```
-XX:PermSize=N #方法区 (永久代) 初始大小
-XX:MaxPermSize=N #方法区 (永久代) 最大大小,超过这个值将会抛出 OutOfMemoryError 异常:java.lang.OutOfMemoryError: PermGen
```
1.8方法去被彻底移除了，取而代之的是元空间，元空间使用的是本地内存
下面是常见参数
```
-XX:MetaspaceSize=N #设置 Metaspace 的初始大小（是一个常见的误区，后面会解释）
-XX:MaxMetaspaceSize=N #设置 Metaspace 的最大大小
```
无论 `-XX:MetaspaceSize` 配置什么值，对于 64 位 JVM，元空间的初始容量通常是一个固定的较小值
**扩容与元数据GC**：当 Metaspace 的已提交空间达到 `-XX:MetaspaceSize` 对应的高水位阈值时，JVM 会触发一次垃圾回收以尝试卸载类、释放类元数据。在此之后，JVM 会根据释放效果动态调高或调低下一次触发 GC 的阈值；具体采用哪类回收周期取决于垃圾收集器和 JDK 版本，不能一概称为 Full GC
-XX:MaxMetaspace的作用，如果不显式地设置`-XX:MaxMetaspaceSize`，元空间默认没有固定上限，持续增长的类元数据可能消耗大量本地内存。是否设置该参数以及设置多大，应结合类元数据使用情况和进程的本地内存预算决定。上限过小会增加元数据 GC 的频率，也可能提前触发 `OutOfMemoryError: Metaspace`，因此不存在适用于所有应用的推荐值。
# 垃圾收集器相关
## 选择合适地垃圾回收器
具体文章可以查看[[垃圾回收器]]这部分
## JDK17ZGC参数
## GC日志记录
生产环境下或进行GC问题排查，务必开启GC日志记录。
# 处理OOM
JVM提供了以下参数，这些参数将堆内存转储到一个物理文件中，以后可以用来查找泄露
```
# Java 堆耗尽并抛出 OOME 时生成堆转储文件
-XX:+HeapDumpOnOutOfMemoryError

# 指定堆转储文件或目录。若指定目录，JVM 会使用默认名称 java_pid<pid>.hprof
-XX:HeapDumpPath=/data/dumps/

# (可选) 在发生 OOM 时执行指定的命令或脚本
# 例如，发送告警通知或尝试重启服务（需谨慎使用）
# -XX:OnOutOfMemoryError="<command> <args>"
# 示例：-XX:OnOutOfMemoryError="sh /path/to/notify.sh"

# (可选) 启用 GC 开销限制检查
# 如果 GC 时间占总时间比例过高（默认 98%）且回收效果甚微（默认小于 2% 堆内存），
# 会提前抛出 OOM，防止应用长时间卡死在 GC 中。
-XX:+UseGCOverheadLimit
```
# 其他参数
