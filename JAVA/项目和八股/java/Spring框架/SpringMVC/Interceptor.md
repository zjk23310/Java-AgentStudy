---
title: Spring MVC Interceptor
aliases:
  - Spring 拦截器
  - HandlerInterceptor
tags:
status: learning
created: 2026-10-15
updated: 2026-10-15
related:
  - "[[Filter]]"
  - "[[AOP]]"
  - "[[异常处理]]"
---

# Spring MVC 拦截器（Interceptor）

> [!abstract] 一句话定义
> 拦截器是 Spring MVC 在请求进入 Controller 前后提供的扩展机制，通过 `HandlerInterceptor` 参与请求处理链，可用于登录校验、权限校验、日志记录、耗时统计和统一上下文处理。

## 1. 它在请求链中的位置

一次典型请求大致经过：

```text
客户端
  ↓
Servlet 容器
  ↓
Filter（Servlet 层）
  ↓
DispatcherServlet（Spring MVC 前端控制器）
  ↓
HandlerMapping 找到 Controller
  ↓
Interceptor.preHandle
  ↓
Controller / HandlerAdapter
  ↓
Interceptor.postHandle
  ↓
视图解析或消息转换
  ↓
Interceptor.afterCompletion
```

> [!important] 关键结论
> 拦截器不是 Servlet Filter，也不是 AOP。它位于 `DispatcherServlet` 内部，只有请求进入 Spring MVC 并匹配到 Handler 后才能参与处理。

## 2. 核心接口与生命周期

```java
public interface HandlerInterceptor {
    default boolean preHandle(
            HttpServletRequest request,
            HttpServletResponse response,
            Object handler) throws Exception {
        return true;
    }

    default void postHandle(
            HttpServletRequest request,
            HttpServletResponse response,
            Object handler,
            ModelAndView modelAndView) throws Exception {
    }

    default void afterCompletion(
            HttpServletRequest request,
            HttpServletResponse response,
            Object handler,
            Exception ex) throws Exception {
    }
}
```

### 2.1 `preHandle`

Controller 执行前调用，返回值决定请求是否继续：

- `true`：继续执行后续拦截器和 Controller；
- `false`：请求链中断，Controller 不会执行；通常需要在这里写回响应；
- 可用于登录认证、权限判断、限流前置检查、生成 Trace ID。

### 2.2 `postHandle`

Controller 执行完成后、视图渲染或响应体写出前调用。

- 只适合处理成功进入 Controller 后的逻辑；
- 如果 Controller 抛异常，默认不会进入该方法；
- `@ResponseBody` 或 `@RestController` 场景下，响应可能已经由消息转换器处理，不适合在这里修改响应体。

### 2.3 `afterCompletion`

整个请求完成后调用，包括异常场景。

- 用于清理 `ThreadLocal`；
- 记录最终耗时和异常；
- 释放请求级资源；
- 只有对应拦截器的 `preHandle` 返回 `true` 时，才会执行该拦截器的 `afterCompletion`。

## 3. 执行顺序

假设注册了 `A、B` 两个拦截器：

```text
A.preHandle
  B.preHandle
    Controller
  B.postHandle
A.postHandle

B.afterCompletion
A.afterCompletion
```

如果 `B.preHandle` 返回 `false`：

```text
A.preHandle
  B.preHandle -> false
A.afterCompletion
```

`A` 已经成功通过前置处理，所以会执行 `A.afterCompletion`；`B` 没有通过前置处理，因此不会执行自己的完成回调。

## 4. 最小实现

```java
package com.example.web;

import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.springframework.web.servlet.HandlerInterceptor;

public class LoginInterceptor implements HandlerInterceptor {
    @Override
    public boolean preHandle(HttpServletRequest request,
                             HttpServletResponse response,
                             Object handler) throws Exception {
        Object user = request.getSession().getAttribute("loginUser");
        if (user == null) {
            response.sendError(HttpServletResponse.SC_UNAUTHORIZED, "请先登录");
            return false;
        }
        return true;
    }
}
```

实际项目中通常不建议把用户信息放在 Session 作为唯一方案；Token/JWT 项目应从请求头解析 Token，再交给统一认证组件校验。

## 5. 注册拦截器

### 5.1 Spring Boot + Java Config

```java
@Configuration
public class WebMvcConfig implements WebMvcConfigurer {

    private final LoginInterceptor loginInterceptor;

    public WebMvcConfig(LoginInterceptor loginInterceptor) {
        this.loginInterceptor = loginInterceptor;
    }

    @Override
    public void addInterceptors(InterceptorRegistry registry) {
        registry.addInterceptor(loginInterceptor)
                .addPathPatterns("/**")
                .excludePathPatterns(
                        "/login",
                        "/logout",
                        "/error",
                        "/css/**",
                        "/js/**",
                        "/images/**",
                        "/swagger-ui/**",
                        "/v3/api-docs/**"
                );
    }
}
```

拦截器需要成为 Spring Bean。可以在类上添加 `@Component`，也可以在配置类中使用 `@Bean`：

```java
@Bean
public LoginInterceptor loginInterceptor() {
    return new LoginInterceptor();
}
```

### 5.2 注解方式（不推荐用于全局认证）

也可以通过自定义注解标记需要拦截的 Controller 方法，然后在 `preHandle` 中读取 `HandlerMethod`：

```java
if (handler instanceof HandlerMethod method
        && method.hasMethodAnnotation(NeedLogin.class)) {
    // 只对标注了 @NeedLogin 的接口执行校验
}
```

全局 URL 规则更直观时，优先使用 `addPathPatterns/excludePathPatterns`；规则分散在注解中时，适合权限模型明确且接口数量较多的项目。

## 6. 常见实战：登录校验与用户上下文

```java
@Component
public class TokenInterceptor implements HandlerInterceptor {

    @Override
    public boolean preHandle(HttpServletRequest request,
                             HttpServletResponse response,
                             Object handler) {
        String token = request.getHeader("Authorization");
        Long userId = tokenService.parseUserId(token);
        if (userId == null) {
            response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
            return false;
        }
        UserContext.setUserId(userId);
        return true;
    }

    @Override
    public void afterCompletion(HttpServletRequest request,
                                HttpServletResponse response,
                                Object handler,
                                Exception ex) {
        UserContext.remove(); // 必须清理 ThreadLocal
    }
}
```

> [!warning] ThreadLocal 必须清理
> Tomcat 使用线程池，线程会被复用。如果不在 `afterCompletion` 中 `remove()`，下一个请求可能读取到上一个请求的用户信息，造成严重的数据越权问题。

## 7. 路径匹配规则

```java
registry.addInterceptor(interceptor)
        .addPathPatterns("/api/**")
        .excludePathPatterns("/api/public/**", "/api/login");
```

常见模式：

| 模式 | 含义 |
| --- | --- |
| `/api/**` | 匹配 `/api` 下的所有路径 |
| `/user/*` | 匹配一层路径，如 `/user/1`，不匹配更深层路径 |
| `/**` | 匹配所有 MVC 请求 |
| `/**/login` | 根据 Spring MVC 路径匹配器匹配登录路径 |

配置 `/**` 时要排除登录、错误页、静态资源、健康检查和文档接口，否则可能出现“登录接口也被拦截”或静态资源全部 401 的问题。

## 8. 拦截器、Filter、AOP、Spring Security 的区别

| 机制 | 所在层次 | 能否拦截静态资源 | 适合场景 |
| --- | --- | --- | --- |
| Filter | Servlet 容器 | 可以 | 编码、CORS、请求/响应包装、全局日志 |
| Interceptor | Spring MVC | 通常只处理 MVC Handler | 登录校验、Controller 前后处理 |
| AOP | Spring Bean 方法调用 | 不关心 HTTP | 事务、审计、方法级日志、权限切面 |
| Spring Security | 安全过滤器链 | 通过 Filter 链处理 | 认证、授权、CSRF、Session、OAuth2 |

选择原则：

- 需要在 Servlet 层处理：Filter；
- 需要知道 Controller、路径或 HandlerMethod：Interceptor；
- 需要拦截任意 Spring Bean 方法：AOP；
- 正式的认证授权体系：优先 Spring Security，不要仅靠自写拦截器。

## 9. 常见误区与排错

> [!warning] 高频问题
>
> 1. **注册了但不生效**：确认配置类被 Spring 扫描，拦截器本身是 Bean，路径确实进入 Spring MVC。
> 2. **登录接口也被拦截**：检查 `excludePathPatterns`，同时确认实际请求路径是否包含上下文路径。
> 3. **跨域预检请求失败**：浏览器的 `OPTIONS` 请求可能先被拦截，应放行预检请求或统一配置 CORS。
> 4. **Controller 抛异常时 `postHandle` 不执行**：这是生命周期设计，异常清理放在 `afterCompletion`。
> 5. **Response 已提交后无法修改**：不要在 `postHandle` 中强行改已经写出的响应体。
> 6. **依赖注入为 null**：拦截器不能用 `new` 随意创建，应由 Spring 管理。
> 7. **只校验 URL 不等于完整授权**：同一资源还需要校验用户是否拥有对应的操作权限。

## 10. 推荐的认证拦截流程

```text
请求到达
  ↓
提取 Authorization / Cookie
  ↓
校验 Token 是否存在、格式是否正确、是否过期
  ↓
解析 userId，并加载必要的身份信息
  ↓
写入请求上下文
  ↓
放行 Controller
  ↓
afterCompletion 清理上下文
```

认证与授权最好分开：认证回答“你是谁”，授权回答“你能做什么”。拦截器可以完成简单认证，但复杂角色、资源、方法级权限建议交给 Spring Security。

## 11. 复习清单

- [ ] 能画出 Filter、Interceptor、Controller 的执行顺序
- [ ] 能说明 `preHandle/postHandle/afterCompletion` 的区别
- [ ] 能解释 `preHandle` 返回 `false` 后哪些回调还会执行
- [ ] 能完成 `WebMvcConfigurer#addInterceptors` 注册
- [ ] 能配置放行路径和静态资源
- [ ] 能解释为什么必须清理 ThreadLocal
- [ ] 能区分拦截器、过滤器、AOP、Spring Security
- [ ] 能处理 OPTIONS、异常和响应已提交问题

## 12. 延伸阅读

- [[Filter]]
- [[AOP]]
- [[异常处理]]
- [[线程池]]
