# Offline-AWTRIX-Clock-by-Arduino
Since there might be quite a lot problems when u build ur own host and connect it to esp8266, I figure out a solution to make the AWTRIX run its core function(time and date)without host and wifi, which is more conveninent for daily use under different internet circumstance. 

Also, the original host is writen by Java, which might be a little difficult for secondary development, so I make this happen on arduino paltform using esp8266. It' s totally fine with arduino nano etc., but considering the follwing functions(eg. bluetooth speaker, temperature, alarm, gif animation rtc.)I may attach to it, it is strongly recommend to use esp8266 / esp32/ arduino rp2040 etc. which has much larger flash than arduino nano/uno to run this project.
