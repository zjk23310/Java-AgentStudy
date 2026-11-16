# 垃圾回收根节点
创建对象时，JVM 并不会简单地把每个新对象都添加到一个叫 GC Roots 的集合中。GC Roots 是垃圾回收器在进行可达性分析时，根据线程栈、静态字段、JNI 引用等运行时信息识别出来的根引用。
## 哪些引用可作为GCRoots
1. 方法中仍然活跃的局部变量引用
public void test() {
    Student s = new Student();
}
2. 类的静态字段引用
class Student {
    static Object obj = new Object();
}
两个对象相互引用，但是只要在GCRoots中不可达就可以被回收
class Node {
    Node next;
}

public class Main {
    public static void main(String[] args) {
        Node a = new Node();
        Node b = new Node();

        a.next = b;
        b.next = a;

        a = null;
        b = null;
    }
}