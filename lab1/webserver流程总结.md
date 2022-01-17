### 基本流程

基于winsock

* WSAStartup() 初始化
* socket() 创建一个指定类型和地址族的socket，用于监听
* bind() 把监听socket和主机ip和端口进行绑定
* ioctlsocket() 设置阻塞、非阻塞模式

之后进入循环监听、处理模式

* 