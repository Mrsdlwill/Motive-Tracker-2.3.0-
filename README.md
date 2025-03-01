# Motive-Tracker-2.3.0-
自己做的vs+qt 小项目 ，可以连接Motive、显示动捕数据和导出csv文件，OptiTrack的动捕数据
        （OptiTrack提供了NatNet SDK下载，https://www.optitrack.com/support/downloads/developer-tools.html）

**2. 代码结构**
ceshi1218
├── include           # 头文件存放目录
│   ├── NatNetCAPI.h
│   │── NatNetClient.h
│   ├── NatNetRequests.h
│   ├── NatNetRepeater.h
│   ├── NatNetTypes.h
│   ├── lib            # 库文件存放目录
│   │   ├── NatNetLib.dll    # natnet 的 API 库
│   │   ├── NatNetLib.lib    # natnet 的 API 库│
├── src
│   ├── main.c           # 主函数
├── head
│   ├── main.c           # 主函数
├── readme.md            # 为示例工程提供详细的文档
├── 界面.png              # 运行界面
