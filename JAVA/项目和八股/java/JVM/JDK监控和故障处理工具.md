---
summary:
tags:
created:
updated:
掌握:
复习时间:
---
相关网址：

相关笔记：
[[重要的JVM参数设置]]

---
# JDK命令行工具
## jps
`jps`(JVM Process Status) 命令类似 UNIX 的 `ps` 命令，用于列出当前用户有权访问且能被该工具发现的 JVM 进程，并不保证枚举系统中的所有 Java 进程。
jps显示**虚拟机执行主类名称**以及这些进程的**主机虚拟机唯一ID**
jps-q只输出进程的本地虚拟机唯一ID
jps -l输出主类的全名，如果进程执行的是Jar包输出Jar路径
jps -v 输出虚拟机启动时JVM参数
jps -m输出传递给Java进程main函数的参数
## jstat监视虚拟机各种运行状态信息
jstat（JVM Statistics Monitoring Tool） 使用于监视虚拟机**各种运行状态信息**的命令行工具。 它可以显示本地或者远程（需要远程主机提供 RMI 支持）虚拟机进程中的类信息、内存、垃圾收集、JIT 编译等运行数据，在没有 GUI，只提供了纯文本控制台环境的服务器上，它将是运行期间定位虚拟机性能问题的首选工具。
命令的使用格式如下
```
jstat -<option> [-t] [-h<lines>] <vmid> [<interval> [<count>]]
```
`jstat -gc -h3 31736 1000 10` 表示分析进程 id 为 31736 的 gc 情况，每隔 1000ms 打印一次记录，打印 10 次停止，每 3 行后打印指标头部。
常见的option如下
```
- `jstat -class vmid`：显示 ClassLoader 的相关信息；
- `jstat -compiler vmid`：显示 JIT 编译的相关信息；
- `jstat -gc vmid`：显示与 GC 相关的堆信息；
- `jstat -gccapacity vmid`：显示各个代的容量及使用情况；
- `jstat -gcnew vmid`：显示新生代信息；
- `jstat -gcnewcapacity vmid`：显示新生代大小与使用情况；
- `jstat -gcold vmid`：显示老年代的行为统计；在现代 JDK 中，输出还包含元空间和压缩类空间等统计信息；
- `jstat -gcoldcapacity vmid`：显示老年代的大小；
- `jstat -gcpermcapacity vmid`：显示永久代大小，从 JDK 8 开始该选项不存在了；现代 JDK 可使用 `-gcmetacapacity` 查看元空间容量统计；
- `jstat -gcutil vmid`：显示垃圾收集信息；

另外，加上 `-t` 参数可以在输出信息上加一个 Timestamp 列，显示程序的运行时间
```
## jinfo
实时查看调整虚拟机各项参数
`jinfo vmid` :输出当前 jvm 进程的全部参数和系统属性（第一部分是系统的属性，第二部分是 JVM 的参数）。
`jinfo -flag name vmid` :输出对应名称的参数的具体值。比如输出 `MaxHeapSize`。下面查看和动态调整 `PrintGC` 的示例针对 JDK 8；JDK 9 及之后的 GC 日志应使用统一日志参数 `-Xlog`，并可通过 `jcmd VM.log` 在运行时调整。
```
C:\Users\SnailClimb>jinfo  -flag MaxHeapSize 17340
-XX:MaxHeapSize=2124414976
C:\Users\SnailClimb>jinfo  -flag PrintGC 17340
-XX:-PrintGC
```
`jinfo` 可以在不重启虚拟机的情况下动态修改一部分标记为可管理的 JVM 参数，并非所有参数都支持运行时修改。
```
C:\Users\SnailClimb>jinfo  -flag  PrintGC 17340
-XX:-PrintGC

C:\Users\SnailClimb>jinfo  -flag  +PrintGC 17340

C:\Users\SnailClimb>jinfo  -flag  PrintGC 17340
-XX:+PrintGC
```
## jmap
jmap用于生成堆转储快照。如果不适用jmap命令，要想获取Java堆转储，可以使用-XX:+HeapDumpOnOutOfMemoryError参数，让虚拟机在OOM异常后自动生成dump文件。
### jthat 分析heapdump文件
jthat用于分析heapdump，会建立一个http/html服务器，让用户在服务器上查看分析结果

## jstack
用于生成虚拟机当前时刻的线程快照。线程快照是当前虚拟机内每一条线程正在执行的方法堆栈的集合
生成线程快照的目的主要是定位线程长时间出现停顿的原因，如线程间死锁、死循环、请求外部资源导致的长时间等待等都是导致线程长时间停顿的原因。

# JDK可视化工具
