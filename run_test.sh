#!/bin/bash
# BueOS Test Script
# 测试脚本 - 支持QEMU模拟器

# 检查QEMU是否安装
if ! command -v qemu-system-x86_64 &> /dev/null
then
    echo "QEMU未安装，请先安装QEMU"
    echo "Linux/Debian: sudo apt-get install qemu-system-x86"
    echo "Termux: pkg install qemu-system-x86_64"
    exit 1
fi

# 构建项目
echo "构建BueOS..."
make clean
make || exit 1

# 运行测试
echo "启动QEMU模拟器..."
qemu-system-x86_64 \
    -drive format=raw,file=bueos.img \
    -serial stdio \
    -m 256M \
    -no-shutdown \
    -no-reboot

echo "测试完成"
