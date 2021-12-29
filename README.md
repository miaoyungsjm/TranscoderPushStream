For documentation and instructions for building this software,
see <http://www.live555.com/liveMedia/>


## 编译
### Linux Platform
```
./genMakefiles linux
make

编译结果：
mediaServer/live555MediaServer
```

### Andorid Platform
```
Android.mk

编译结果：
/system/lib/liblive556.so
/system/bin/live555MediaServer
```

## 调试
注意：  
live555MediaServer 可执行文件 和 ts 文件需要最好放在同一路径  
它会以程序执行的环境路径作为遍历文件的根路径  
```
开启服务：
./system/bin/live555MediaServer

播放器测试 RTSP 播放：
rtsp://192.168.1.99/system/bin/xxx.ts
```

## 开发
缺陷：  
system/bin/xxx.live 文件必须真实存在，即使程序功能实现没用文件操作  
```
线程开启服务：
void* liveServer(void *arg)

线程进行数据填充 Ts Stream buffer：
void SubmitLiveTsData(void* data, int size)

播放器测试 RTSP 播放：
rtsp://192.168.1.99/system/bin/xxx.live
```