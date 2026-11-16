# run和start
run() 方法不会创建一个新的线程，只会在当前线程中执行run方法的代码。
是线程的执行体，包含线程要执行的代码。
当直接调用run方法时，代码直接在当前线程中作为普通方法执行，不会创建新的线程。
run方法也可以被子类重写，实现特定任务

start() 方法可以启动一个新线程，能够实现多线程执行。
会启动一个新的线程，会在新线程中执行run方法代码。
start方法会为线程分配系统资源，并将线程置于就绪状态，当调度器选择该线程时，会执行run方法中的代码。
start方法只能调用一次，如果再次调用，将抛出IllegalThreadStateExeception异常。
# 线程状态
public enum State {

    NEW,

    RUNNABLE,

    BLOCKED,

    WAITING,

    TIMED_WAITING,

    TERMINATED
}
1. NEW
对象创建但是没有启动
Thread t = new Thread();
创建 Thread 对象 ≠ 创建操作系统线程。
真正创建线程是在 start() 时。
2. RUNNABLE
   可运行状态
   t.start();
   Java 把就绪状态Ready运行状态Running合并成了 RUNNABLE。
3. BLOCKED
    （阻塞状态）竞争同步锁失败，等待锁释放。
4. WAITING 
    无限等待
Object obj = new Object();
synchronized(obj){
    obj.wait();
}
需要notify或者notifyAll来唤醒
5. TIMED_WAITING
    计时等待
Thread.sleep(1000);或obj.wait(1000);到时间自动会醒来
6. TERMINATED
    终止状态
在run方法执行完成后会进入终止状态