@echo off
REM BueOS Windows Test Script
REM 测试脚本 - 支持Hyper-V

REM 检查Hyper-V是否启用
powershell -command "if ((Get-WindowsOptionalFeature -Online -FeatureName Microsoft-Hyper-V).State -ne 'Enabled') { exit 1 }"
if %errorlevel% neq 0 (
    echo Hyper-V未启用，请先启用Hyper-V
    echo 1. 以管理员身份运行PowerShell
    echo 2. 执行: Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Hyper-V -All
    exit /b 1
)

REM 构建项目
echo 构建BueOS...
make clean
make || exit /b 1

REM 创建虚拟机
echo 创建Hyper-V虚拟机...
set VM_NAME=BueOS_Test
set VHD_PATH=%cd%\bueos.vhdx
set IMG_PATH=%cd%\bueos.img

REM 转换镜像格式
echo 转换镜像格式...
qemu-img convert -f raw -O vhdx %IMG_PATH% %VHD_PATH%

REM 设置并启动虚拟机
powershell -command "
    New-VM -Name '%VM_NAME%' -MemoryStartupBytes 256MB -Generation 1 -NoVHD
    Add-VMHardDiskDrive -VMName '%VM_NAME%' -Path '%VHD_PATH%'
    Start-VM -Name '%VM_NAME%'
"

echo 测试虚拟机已启动，请使用Hyper-V管理器查看
