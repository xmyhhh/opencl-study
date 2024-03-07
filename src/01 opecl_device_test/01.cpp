//
// Created by xmyci on 05/03/2024.
//
#include <iostream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <exception>

#include <CL/opencl.hpp>

void device_test() {
    //get all platforms (drivers)
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if (all_platforms.size() == 0) {
        std::cout << "No platforms found. Check OpenCL installation!\n";
        exit(1);
    }

    for (auto platform: all_platforms) {
        std::cout << "Platform: " << platform.getInfo<CL_PLATFORM_NAME>() << " version " << platform.getInfo<CL_PLATFORM_VERSION>() << std::endl;
        std::cout << "  Vendor: " << platform.getInfo<CL_PLATFORM_VENDOR>() << std::endl;
        std::cout << "  Extensions: " << platform.getInfo<CL_PLATFORM_EXTENSIONS>() << std::endl;

        std::vector<cl::Device> all_devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
        if (all_devices.size() == 0) {
            std::cout << " No devices found for this platform. Check OpenCL installation!\n";
        }
        for (cl::Device &device: all_devices) {
            std::cout << "  Device: " << device.getInfo<CL_DEVICE_NAME>() << "\n";
            cl_device_type dev_type = device.getInfo<CL_DEVICE_TYPE>();
            std::cout << "    Type: ";
            switch (dev_type) {
                case CL_DEVICE_TYPE_CPU:
                    std::cout << "CPU";
                    break;
                case CL_DEVICE_TYPE_GPU:
                    std::cout << "GPU";
                    break;
                case CL_DEVICE_TYPE_ACCELERATOR:
                    std::cout << "Accelerator";
                    break;
                case CL_DEVICE_TYPE_DEFAULT:
                    std::cout << "Default";
                    break;
            }
            std::cout << std::endl;
            std::cout << "    Version: " << device.getInfo<CL_DEVICE_VERSION>() << std::endl;
            std::cout << "    Global memory: " << device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << " bytes" << std::endl;
            std::cout << "    Local memory: " << device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << " bytes" << std::endl;
            std::cout << "    Max compute units: " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
            std::cout << "    Max work group size: " << device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << std::endl;
        }
    }
}