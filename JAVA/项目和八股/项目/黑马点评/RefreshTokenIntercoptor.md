
#flashcards/Java面经/项目/黑马点评

```java
public class RefreshTokenInterceptor implements HandlerInterceptor {  
    private StringRedisTemplate stringRedisTemplate;  
    public RefreshTokenInterceptor(StringRedisTemplate stringRedisTemplate)//有参构造  
    {  
        this.stringRedisTemplate=stringRedisTemplate;  
    }
```
这部分为什么不使用@Resource注解来注入stringRedisTemplate