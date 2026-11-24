---
summary:
tags:
created: 2026-11-24
updated: 2026-11-24
掌握: unkown
复习时间: 2026-11-24
---
相关网址：
[类加载器详解（重点） | JavaGuide](https://javaguide.cn/java/jvm/classloader.html#%E7%B1%BB%E5%8A%A0%E8%BD%BD%E5%99%A8)
[Java类加载详解 - DeepSeek](https://chat.deepseek.com/a/chat/s/ac28b944-9ea5-42e7-adc5-1646feb1aedf)这个聊天记录涉及一些问题可以深入询问AI
相关笔记：


---
# 类加载器
其赋予java类可以被动态加载到JVM中并执行的能力
简单来说类加载器的主要作用是动态加载java类的字节码到JVM中。字节码可以是java源程序经过javac编译得到，也可以是通过工具动态生成或网络下载得到的
java的加载类除了加载类还能够加载java所需要使用的资源（文本、图像、配置文件、视频等文件资源）
## 类加载器加载规则
JVM启动并不会一次性加载所有类，根据需要动态加载。大部分类只有在具体用到的时候才会被加载

## 类加载器总结
**启动类加载器**（Bootstrap ClassLoader）
通常由 C/C++ 实现，加载 java.base 等核心模块中的类
**平台类加载器**（Platform ClassLoader）加载部分 Java 平台模块中的类
ExtensionClassLoader扩展类记载器，负责加载`%JRE_HOME%/lib/ext`目录下的jar包和类
JDK9起，原来的**扩展类加载器已经被平台类加载器取代**
**应用类加载器**（Application ClassLoader）
通常加载应用 Classpath 中的所有jar包和类，面向用户。
表示类加载器的委派关系，不是 Java 的继承关系。

例如：
public class Student {
}
通常由应用类加载器加载。
而：
java.lang.String
java.lang.Object
通常由启动类加载器加载。
启动类加载器是虚拟机内置的，平台类加载器、应用类加载器以及通常的自定义类加载器都是 `ClassLoader` 的实例。
ClassLoader可通过getParent获取其父ClassLoader，如果获取到的ClassLoader为null则该类的父类加载器为BoostrapClassLoader，这是因为启动类加载器是JVM内置类加载器，API约定通常用Null表示。
```java
public class PrintClassLoaderTree {

    public static void main(String[] args) {

        ClassLoader classLoader = PrintClassLoaderTree.class.getClassLoader();

        StringBuilder split = new StringBuilder("|--");
        boolean needContinue = true;
        while (needContinue){
            System.out.println(split.toString() + classLoader);
            if(classLoader == null){
                needContinue = false;
            }else{
                classLoader = classLoader.getParent();
                split.insert(0, "\t");
            }
        }
    }

}
```
这里的代码就展示了获取当前类加载器并不断向上追溯的过程

## 自定义类加载器
除了启动类加载器，其他类加载器均有java实现且全部继承自java.lang.ClassLoader，若要实现自己的类加载器则需要继承ClassLoader抽象类
**不能直接继承 HotSpot 的 Bootstrap ClassLoader**。
因为它不是一个普通的 Java 类，不存在供你这样使用的 Java 类定义：
class MyLoader extends BootstrapClassLoader {
}
但是，**你可以继承 Java 提供的 ClassLoader 类，实现自定义类加载器**：
public class MyClassLoader extends ClassLoader {

public MyClassLoader() {
super();
}
}
这里继承的是 java.lang.ClassLoader，而不是 Bootstrap ClassLoader。
有意思的是，java.lang.ClassLoader 本身通常就是由启动类加载器加载的。
因此，你的自定义类加载器同样可以继承一个由启动类加载器加载的 Java 类。
其有两个关键方法：
- `protected Class loadClass(String name, boolean resolve)`：加载指定二进制名称的类，实现了双亲委派机制。`name` 为类的二进制名称，`resolve` 如果为 true，在加载时调用 `resolveClass(Class<?> c)` 方法解析该类。
- `protected Class findClass(String name)`：根据类的二进制名称查找类，默认实现直接抛出 `ClassNotFoundException`。

如果不想打破双亲委派机制就直接重写findClass如果要打破则重写loadClass方法
# 双亲委派机制
Classloader类使用委托模型来搜索类和资源
双亲委派模型要求除了顶层的启动类加载器外，其余所有的类加载器都有对应的父类加载器。
classLoader在视图亲自查找类和资源之前会将搜索类或资源的委托委托给父类加载器

类加载器的父子关系一般不是通过继承实现的，而是通过组合实现的，代码基本如下
```
public abstract class ClassLoader {
  ...
  // 组合
  private final ClassLoader parent;
  protected ClassLoader(ClassLoader parent) {
       this(checkCreateClassLoader(), parent);
  }
  ...
}
```
## 双亲委派机制的执行流程
该机制主要集中在loadClass函数中
执行流程如下。源码则不贴出了，不过我觉得可以去了解一下的
1.类记载时首先判断类是否被加载过，已经被记载的类会直接被返回，负责尝试加载
2.类加载器进行类加载首先会把请求委派给父类加载器。这样所有的请求最终都会传送到顶层的启动类加载器
3.只有父加载器反馈自己无法完成请求子加载器才能自己尝试加载
4.如果子类加载器也无法加载这个类会抛出异常

**JVM 判定两个 Java 类是否相同的具体规则**：JVM 不仅要看类的全名是否相同，还要看加载此类的类加载器是否一样。只有两者都相同的情况，才认为两个类是相同的。即使两个类来源于同一个 `Class` 文件，被同一个虚拟机加载，只要加载它们的类加载器不同，那这两个类就必定不相同
## 双亲委派机制的优点
父加载器优先有助于
1.让同一委派链中的代码复用父加载器已经定义的类型
比如 AppClassLoader 要加载 `java.lang.String`，它会一路委派到 Bootstrap，最后由 Bootstrap 定义。这样整个委派链里用的都是同一个 `String` 类型。
2.降低应用代码冒充平台API的风险
双亲委派机制不能够阻止两个相互独立的类加载器分别定义同名类。
如果我重写loadClass类，绕过双亲委派机制，是不是就可以加载一些类比如"java.lang.String"了呢？并不能，因为java具备更底层的安全机制保护核心类库
## 打破双亲委派模型方法
自定义加载器，需要继承ClassLoader，如果不想打破双亲委派模型可以重写findClass方法。如果需要重写loadClass方法，可以改变传统双亲委派模型的执行流程