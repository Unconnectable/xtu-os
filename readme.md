# 🧑‍💻 操作系统课程实验项目

## 📚 实验目录

### - [Lab1: 进程管理](reports/lab1/README.md)

### - [Lab2: 死锁避免](reports/lab2/README.md)

### - [Lab3: 内存动态分区分配](reports/lab3/README.md)

### - [Lab4: 虚拟内存](reports/lab4/README.md)

## ⚙️ 构建要求

- `CMake` 版本: 3.10 或以上
- `C++` 标准`: C++17
- **编译器支持**:
  - `GCC`(建议版本 >= 8)
  - `Clang`(建议版本 >= 9)
  - `MSVC`(Windows 平台)

---

## 🛠️ 如何使用命令行构建

```sh
# 创建构建目录并进入
mkdir build && cd build
# 使用默认编译器配置项目
cmake ..
# 多线程构建
make -j$(nproc)
```

## 🧪 测试说明

### 项目中已集成 Google Test 单元测试框架.你可以运行以下命令执行测试:

```sh
cd build
./test/banker_test
./test/MemoryAlloc_test
```

> ⚠️ 注意:部分测试用例可能被标记为 DISABLED\_,请在对应 .cpp 文件中移除前缀以启用它们.

## 🤝 贡献指南

### 欢迎各位同学提交 PR 来完善本项目！

你可以参与的方向包括但不限于:

- ✅ 补充更多测试用例(边界测试、异常输入等)
- 🐞 修复发现的 bug(如内存泄漏、逻辑错误)
- 💡 提交新的实验实现(如文件系统、调度算法等)
- 📝 编写更详细的实验报告或补充注释说明
- ❤️ 致谢

感谢你的关注与贡献！
