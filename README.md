# Introduction

用来将 FBX 模型拆分成小模型的工具（基于带有网格的 FBX 节点）.

编译需要用到fbxsdk(libfbxsdk.so),可从官网下载 https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2019-2.
修复版本.
使用前需要根据官网配置C++链接库.
速度一般，3万个零件需要1.5小时.
# Usage

```sh
FBX-SPLIT v0.1
Usage: ./fbx-split [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -i,--input TEXT:FILE        输入的FBX模型文件
  -o,--output TEXT            输出模型文件的目录
```
