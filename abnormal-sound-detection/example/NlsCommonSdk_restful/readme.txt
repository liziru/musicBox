SDK压缩包说明:

- CMakeLists.txt demo工程的CMakeList文件
- build 编译目录
- demo 包含tokenDemo.cpp，fileTransDemo.cpp，restfulAsrDemo.cpp
- include 包含sdk头文件，以及部分第三方头文件。各文件描述见下表

| 文件名  | 描述          |
| --------| ------------  |
| openssl | linux下使用   |
| curl    | curl          |
| uuid    | linux下使用   |
| json    | jsoncpp       |
| Global.h | 全局声明头文件 |
| Token.h | token sdk |
| FileTrans.h | 文件转写 sdk |

- lib
  包含sdk，以及第三方依赖库。其中根据平台/功能不同，可以选择linux版本libnlsCommonSdk.so（glibc:2.5及以上, Gcc4, Gcc5）、windows版本nlsCommonSdk.dll（VS2013、VS2015）
- readme.txt SDK说明
- release.log 更新记录
- version 版本号
- build.sh demo编译脚本

注意：
1.linux环境下，运行环境最低要求：Glibc 2.5及以上， Gcc4、Gcc5.
2. token值有效期默认24小时，超时请重新获取，否则会导致请求识别 

依赖库：
1: openssl-1.0.2j
2: libuuid-1.0.3
3: curl-7.60.0
4: jsoncpp-0.10.6

Linux下demo编译过程:

1: 请确认本地系统以安装Cmake，最低版本2.4
2: 执行[./build.sh]编译demo
3: 编译完毕，进入demo目录，执行[./tokenDemo]


