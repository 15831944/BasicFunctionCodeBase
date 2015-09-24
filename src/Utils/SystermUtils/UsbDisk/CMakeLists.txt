list(APPEND porting_b200_SRCS
    UsbDisk/UDiskAssertions.cpp
    UsbDisk/UDiskConfig.cpp 
    UsbDisk/UDiskDetect.cpp
    UsbDisk/UDiskJsCall.cpp
    UsbDisk/UDiskQuickInstall.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/EC2108_C27/ipstb/src/app/Log
    ${PROJECT_SOURCE_DIR}/EC2108_C27/ipstb/src/app/Upgrade
    ${PROJECT_SOURCE_DIR}/EC2108_C27/ipstb/src/toos
)

