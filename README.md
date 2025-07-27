# 简介

**BueOS是一款开源的操作系统内核，基于GNU_v2许可证发布。BueOS是有趣的轻量操作系统内核，拥有极高的自由度(比如你可以在root权限下干掉引导、操作系统)**

# 特性

- 轻量级内核，占用资源少，启动速度快
- 基于C语言编写，易于移植到其他平台


# 构建与测试

# 构建

# 构建与测试

## 环境要求
- NASM 2.15+
- Clang 12.0+
- GNU ld 2.36+
- QEMU 6.0+ (用于测试)
- Python 3.6+ (用于环境配置)

## 构建步骤
```bash
# 1. 安装依赖工具
python3 Build.py

# 2. 构建系统 (使用make)
make

# 3. 运行测试 (可选)
make test
```

## 测试方法
### Linux/QEMU
```bash
# 使用QEMU运行
qemu-system-i386 -drive file=bueos.img,format=raw
```

### Windows/Hyper-V
```cmd
:: 使用Hyper-V运行
.\test\run_test.bat
```

### Android/Termux
```bash
# 安装QEMU
pkg install qemu-system-x86_64

# 运行测试
./test/run_test.sh
```

# 开发进度

- [x] **`Bootloader` - 完成基本的引导程序，可以引导内核**

- [x] **`GDT/IDT` - 完成GDT/IDT的设置，包括设置GDT、设置TSS、设置IDT、设置中断门、设置系统调用门**

- [x] **`Interrupts` - 完成基本的中断处理，包括中断向量表、中断描述符表、中断处理程序、中断屏蔽、中断嵌套等功能**

- [ ] **`Scheduler` - 完成基本的调度器，包括进程调度、时间片轮转、优先级调度等功能**

- [ ] **`Memory Management` - 完成基本的内存管理，包括页表管理、虚拟内存管理、物理内存管理**

- [ ] **`Process Management` - 完成基本的进程管理，包括进程调度、进程创建、进程终止、进程切换**

- [ ] **`File System` - 完成基本的文件系统，包括文件读写、目录管理、文件系统格式化**

- [ ] **`Device Driver` - 完成基本的设备驱动，包括串口、键盘、鼠标、网络等设备驱动**

- [ ] **`Shell` - 完成基本的命令行接口，包括命令输入、命令执行、命令提示符**

- [ ] **`Userland` - 完成基本的用户态程序，包括进程管理、文件系统、网络等程序**

- [ ] **`GUI` - 完成基本的图形用户界面，包括窗口管理、图形渲染、鼠标、键盘等交互**

- [ ] **`Kernel Debugging` - 完成基本的内核调试功能，包括断点、单步调试、监视点、日志记录等功能**

- [ ] **`Kernel Testing` - 完成基本的内核测试功能，包括单元测试、集成测试、系统测试等功能**

- [ ] **`Documentation` - 完成基本的文档，包括内核设计文档、用户手册、开发指南等文档**

# 许可证

**BueOS使用GNU_v2许可证**