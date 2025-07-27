#!/usr/bin/env python3
# BueOS Build Environment Setup
# BueOS构建环境配置脚本
# 支持Linux和Windows(Termux)环境

import os
import sys
import subprocess
import platform

# 所需工具列表
REQUIRED_TOOLS = {
    'nasm': 'NASM assembler',
    'clang': 'Clang compiler',
    'ld': 'GNU linker',
    'objcopy': 'GNU objcopy'
}

def check_tools():
    """检查是否安装了必要的工具"""
    missing_tools = []
    for tool, desc in REQUIRED_TOOLS.items():
        try:
            subprocess.run([tool, '--version'], 
                          stdout=subprocess.PIPE, 
                          stderr=subprocess.PIPE)
        except FileNotFoundError:
            missing_tools.append((tool, desc))
    
    return missing_tools

def install_tools(missing_tools):
    """安装缺失的工具"""
    system = platform.system().lower()
    success = True
    
    print("\n安装缺失的构建工具:")
    for tool, desc in missing_tools:
        print(f"- {desc} ({tool})")
        
        try:
            if system == 'linux':
                if os.path.exists('/data/data/com.termux/files/usr/bin/pkg'):
                    # Termux环境
                    subprocess.run(['pkg', 'install', '-y', tool], check=True)
                else:
                    # 普通Linux
                    # 先尝试修复损坏的依赖
                    subprocess.run(['sudo', 'apt', '--fix-broken', 'install', '-y'], 
                                 stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    # 更新软件包列表
                    subprocess.run(['sudo', 'apt-get', 'update'], 
                                 stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    # 安装工具
                    subprocess.run(['sudo', 'apt-get', 'install', '-y', tool], check=True)
            elif system == 'windows':
                print(f"请在Windows上手动安装 {tool}")
                success = False
            else:
                print(f"不支持的系统: {system}")
                success = False
                
            print(f"✓ 成功安装: {tool}")
                
        except subprocess.CalledProcessError as e:
            print(f"✗ 安装失败: {tool}")
            print(f"错误详情: {e.stderr.decode().strip() if e.stderr else str(e)}")
            
            if system == 'linux' and not os.path.exists('/data/data/com.termux/files/usr/bin/pkg'):
                print("尝试使用aptitude作为备选方案...")
                try:
                    subprocess.run(['sudo', 'apt-get', 'install', '-y', 'aptitude'], 
                                 stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    subprocess.run(['sudo', 'aptitude', 'install', '-y', tool], check=True)
                    print(f"✓ 使用aptitude成功安装: {tool}")
                    continue
                except:
                    pass
                
            print(f"请手动运行安装命令: sudo apt-get install {tool}")
            success = False
    
    return success

def main():
    print("=== BueOS 构建环境检查 ===")
    
    # 检查工具
    missing = check_tools()
    if missing:
        print("\n缺少必要的构建工具:")
        for tool, desc in missing:
            print(f"- {desc} ({tool})")
        
        if not install_tools(missing):
            sys.exit(1)
    else:
        print("\n所有必要的构建工具已安装")
    
    print("\n环境准备完成，可以开始构建BueOS")

if __name__ == '__main__':
    main()
