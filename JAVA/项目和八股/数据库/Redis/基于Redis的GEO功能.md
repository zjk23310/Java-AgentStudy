

数据的导入
```java
@Test  
void loadShopDate()  
{  
    List<Shop> shops = shopService.list();  
    Map<Long, List<Shop>> map = shops.stream().collect(Collectors.groupingBy(Shop::getTypeId));  
    //分批写入  
    for(Map.Entry<Long,List<Shop>> entry: map.entrySet()){  
        Long typeId=entry.getKey();  
        String key="shop:geo:"+typeId;  
        List<Shop> value=entry.getValue();  
        List<RedisGeoCommands.GeoLocation<String>> locations=new ArrayList<>(value.size());  
        //写入  
        for (Shop shop : value) {  
            locations.add(new RedisGeoCommands.GeoLocation<>(shop.getId().toString(),new Point(shop.getX(),shop.getY())));  
        }  
        stringRedisTemplate.opsForGeo().add(key,locations);  
    }  
}
```