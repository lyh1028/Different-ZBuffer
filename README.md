# Different-ZBuffer
CG Assignment of zju. C++ Implementation of Z-buffer, Scanline Z-buffer, Hierarchical Z-buffer with and without Octree. Also including a toy soft renderer with glm, SDL2 and tinyobjloader.

# What I did:

- 构建了一个完整的渲染管线流程用于显示3维Obj模型，包括Vertex Porcessing， Triangle Processing， Rasterization， Fragment Processing， FramBuffer Operations五个阶段。
- 实现了一个简易窗口界面，可以用鼠标滚轮缩放、左右拖拽鼠标旋转画面。
- 采用了背面剔除和顶点剔除的加速。
- 使用双缓冲避免掉帧。
- 分别实现了ZBuffer，Scanline Z-buffer， Hierarchical Z-buffer，Hierarchical Z-buffer with Octree.
- 实现了朗伯特反射模型，并加入了环境光。

# How to build:

- project framework:
```cpp
├─build ----------------------> 二进制文件目录，存放项目工程文件、中间文件、编译结果
│ ├─Debug -------------------> Vs在debug模式下的编译结果存放目录
│ ├─model -------------------> 模型数据文件目录，存放待渲染的模型文件
│ └─Release -----------------> Vs在release模式下的编译结果存放目录
├─include --------------------> 项目依赖的第三方库头文件目录
│ ├─glm ---------------------> 数学库头文件目录
│ └─SDL2 --------------------> 窗口库头文件目录
├─libs -----------------------> 项目依赖的动态链接库文件目录
└─src ------------------------> 项目的源代码文件目录（存放项目的.h文件和.cpp文件）
```
## envrionment configuration
- cmake-gui of cmake version 3.21.2 
- Visual Studio 2017 or 2019
- Other version may also work, but I don't try.
Operate as images below:
![image](https://github.com/lyh1028/Different-ZBuffer/assets/55468294/8f064682-bdf3-4d90-b267-b2d74fa530b1)
![image](https://github.com/lyh1028/Different-ZBuffer/assets/55468294/ccc8080c-b668-45d8-90b9-e9ac776cc8dc)
![image](https://github.com/lyh1028/Different-ZBuffer/assets/55468294/dd70baff-8d1d-426f-88ea-39eff90a9cd1)

After generating, go to directory /build and open CGAssignment3.sln, set CGAssignment3 as start project(设为启动项目)
Then just run!
Release mode will run much more faster.
