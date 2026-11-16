```java
public class Student {

    static int count = 10;

    static {
        System.out.println("Student 类初始化");
    }

    int age = 18;

    public Student() {
        System.out.println("Student 对象初始化");
    }
}
```
static int count = 10：属于类初始化。
static { ... }：属于类初始化。
int age = 18：属于对象初始化。
Student() 构造方法：属于对象初始化过程。
类初始化只执行一次，但对象初始化每创建一个实例就执行一次。JVM 规定，使用 new 指令创建某个类的实例时，如果这个类尚未初始化，就必须先完成类初始

***所有实例被垃圾回收，并不代表类一定会卸载。**
对于由可回收的自定义类加载器加载的普通类，通常需要同时满足以下条件，才可能被卸载：
1. 该类的所有实例已经无法被访问。
2. 定义该类的类加载器已经无法被访问。
3. 没有其他可达引用阻止相关类元数据和类加载器被回收，例如外部持有的 Class 对象。
而且，即使满足条件，也只是具备卸载的可能性，JVM 不保证立即卸载。
例如：
Student s = new Student();
s = null;
System.gc();
这段代码不能保证 Student 类被卸载。
特别是普通 Java 应用中，由应用类加载器加载的类，通常会一直保留到 JVM 进程结束。

```java
class Parent {
    static {
        System.out.println("Parent 初始化");
    }
}

class Child extends Parent {
    static {
        System.out.println("Child 初始化");
    }
}

public class Main {
    public static void main(String[] args) {
        System.out.println(Child.class);
    }
}```
这段代码实际上不会触发类的初始化，因为Child.class 会获取对应的 Class 对象，但不会仅因为使用类字面量就触发类初始化。

 JVM 判断两个类是否相同，不仅看全限定名，还要看定义它们的类加载器。
这也是 Java 热部署、插件系统和应用服务器使用自定义类加载器的重要原因之一。