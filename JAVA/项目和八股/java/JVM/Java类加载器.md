**启动类加载器**（Bootstrap ClassLoader）
通常由 C/C++ 实现，加载 java.base 等核心模块中的类

**平台类加载器**（Platform ClassLoader）
加载部分 Java 平台模块中的类
**应用类加载器**（Application ClassLoader）
通常加载应用 Classpath 中的类
表示类加载器的委派关系，不是 Java 的继承关系。

例如：
public class Student {
}
通常由应用类加载器加载。
而：
java.lang.String
java.lang.Object
通常由启动类加载器加载。

不能直接继承 HotSpot 的 Bootstrap ClassLoader。
因为它不是一个普通的 Java 类，不存在供你这样使用的 Java 类定义：
class MyLoader extends BootstrapClassLoader {
}

但是，你可以继承 Java 提供的 ClassLoader 类，实现自定义类加载器：
public class MyClassLoader extends ClassLoader {

public MyClassLoader() {
super();
}
}
这里继承的是 java.lang.ClassLoader，而不是 Bootstrap ClassLoader。
有意思的是，java.lang.ClassLoader 本身通常就是由启动类加载器加载的。
因此，你的自定义类加载器同样可以继承一个由启动类加载器加载的 Java 类。
