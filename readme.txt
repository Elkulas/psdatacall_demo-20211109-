
# 使用makefile形式

1.将lib下所有的库拷贝到psdatacall_demo下，包括HCNetSDKCom文件夹

2.Device.ini中填入相关的设备信息

3.在终端输入make命令即可生成getpsdata，使用./getpsdata即可执行

# 使用CMake形式（当前）

1. 需要链接OpenCV，注意链接OpenCV库的路径以及版本，修改CMakeLists中的opencv dir

2. 修改device.ini，按照cmake的形式编译，可执行文件位于bin
  