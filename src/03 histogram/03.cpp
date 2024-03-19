//
// Created by xmyci on 05/03/2024.
//
#define CL_HPP_ENABLE_EXCEPTIONS

#include <iostream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <exception>

#include <CL/opencl.hpp>
#include <omp.h>
#include <tbb/tbb.h>
#include <atomic>
#include <mutex>
#include <random>
#include "common/timer.h"
#include "kernel_histogram.h"
#include "common/opencl-warpper/opencl.hpp"

void histogram() {
    //read ppm
    struct RGB {
        int r, g, b;

        int operator[](const int index) const {
            if (index == 0)
                return r;
            if (index == 1)
                return g;
            if (index == 2)
                return b;
            return -1;
        }

    };
    struct PPM {
        PPM() {
            width = 0;
            height = 0;
        }

        size_t pixels() const {
            return width * height;
        }

        size_t width;
        size_t height;
        std::vector<RGB> data;

        void random_generate(int size) {
            width = size;
            height = size;
            auto l = pixels();
            data.resize(l);
            std::random_device rd; // obtain a random number from hardware
            std::mt19937 gen(rd()); // seed the generator
            std::uniform_int_distribution<> distr(0, 255); // define the range

            tbb::parallel_for(tbb::blocked_range<size_t>(0, pixels()),
                              [&](const tbb::blocked_range<size_t> &r) {
                                  for (size_t i = r.begin(); i != r.end(); ++i) {
                                      data[i] = {distr(gen), distr(gen), distr(gen)};
                                      //data[i] = {1, 2, 3};
                                  }
                              }
            );
//            for (int i = 0; i < pixels(); i++) {
//                data[i] = {distr(gen), distr(gen), distr(gen)};
//            }
        }

    };

    struct Histogram {
        std::array<std::array<int, 256>, 3> value;

        Histogram() {
            for (auto &element: value) {
                for (auto &element_element: element) {
                    element_element = 0;
                }
            }
        }

        void print() {
            std::cout << "-----begin-----" << std::endl;
            for (auto &element: value) {
                std::cout << "----------" << std::endl;
                for (auto &element_element: element) {
                    std::cout << element_element << "  |  ";
                }
                std::cout << std::endl;
            }
            std::cout << "-----end-----" << std::endl;
        }


        bool operator==(const Histogram target) {
            bool res = true;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 255; j++) {
                    if (value[i][j] != target.value[i][j]) {
                        res = false;
                        break;
                    }
                }
            }

            return res;
        }
    };

    auto read_line = [](char *string, FILE *infile, int *linenumber, bool skip_sapce_and_tab = true) {
        char *result;

        // Search for a non-empty line.
        do {
            result = fgets(string, 2048, infile);
            if (linenumber) (*linenumber)++;
            if (result == (char *) NULL) {
                return (char *) NULL;
            }
            // Skip white spaces.
            if (skip_sapce_and_tab)
                while ((*result == ' ') || (*result == '\t')) result++;
            // If it's end of line, read another line and try again.
        } while ((*result == '\0') || (*result == '\r') || (*result == '\n'));
        return result;
    };

    auto load_ppm = [read_line](std::string path, PPM &ppm) {

        FILE *fp = fopen(path.c_str(), "r");
        if (fp == (FILE *) NULL) {
            //printf("File I/O Error:  Cannot create file %s.\n", vtk_file_path);
            return false;
        }

        char buffer[2048];
        char *bufferp;
        int line_count = 0;
        int index = 0;
        while ((bufferp = read_line(buffer, fp, &line_count)) != NULL) {
            if (ppm.width == 0) {
                read_line(buffer, fp, &line_count); //p3
                sscanf(bufferp, "%d %d", &ppm.width, &ppm.height);
                ppm.data.resize(ppm.width * ppm.height);
                read_line(buffer, fp, &line_count); //255
            } else {
                sscanf(bufferp, "%d %d %d", &ppm.data[index].r, &ppm.data[index].g, &ppm.data[index].b);
                index++;
            }
        }
    };


    auto histogram_cpu = [](const PPM &ppm) {
        Histogram his;
        __int64 begin = GetTickCount();
        for (int i = 0; i < ppm.pixels(); i++) {
            for (int j = 0; j < 3; j++) {
                his.value[j][ppm.data[i][j]]++;
            }
        }
        auto end = GetTickCount() - begin;
        std::cout << "\rDone.      " + std::to_string(end / 1000.0) + "           \n";
        return his;
    };

    auto histogram_cpu_tbb = [](const PPM &ppm) {
        Histogram his;
        std::mutex lock;

        __int64 begin = GetTickCount();
        tbb::parallel_for(tbb::blocked_range<size_t>(0, ppm.pixels()),
                          [&](const tbb::blocked_range<size_t> &r) {
                              for (size_t i = r.begin(); i != r.end(); ++i) {
                                  for (int j = 0; j < 3; j++) {
                                      lock.lock();
                                      his.value[j][ppm.data[i][j]]++;
                                      lock.unlock();
                                  }
                              }
                          }
        );
        auto end = GetTickCount() - begin;
        std::cout << "\rDone.      " + std::to_string(end / 1000.0) + "           \n";
        return his;
    };

    auto histogram_cpu_tbb_local = [](const PPM &ppm) {
        Histogram his;
        std::mutex lock;
        __int64 begin = GetTickCount();
        tbb::parallel_for(tbb::blocked_range<size_t>(0, ppm.pixels(), 100),
                          [&](const tbb::blocked_range<size_t> &r) {
                              Histogram local;
                              for (size_t i = r.begin(); i != r.end(); ++i) {
                                  for (int j = 0; j < 3; j++) {
                                      local.value[j][ppm.data[i][j]]++;
                                  }
                              }
                              lock.lock();
                              for (int i = 0; i < 256; ++i) {
                                  for (int j = 0; j < 3; j++) {
                                      his.value[j][i] += local.value[j][i];
                                  }
                              }
                              lock.unlock();
                          }
        );
        auto end = GetTickCount() - begin;
        std::cout << "\rDone.      " + std::to_string(end / 1000.0) + "           \n";
        return his;
    };

    auto histogram_opencl = [](const PPM &ppm, int device_id) {
        __int64 begin = GetTickCount();


//        vector<Device_Info> devices; // get all devices of all platforms
//        vector<cl::Platform> cl_platforms; // get all platforms (drivers)
//        cl::Platform::get(&cl_platforms);
//        uint id = 0u;
//        for (uint i = 0u; i < (uint) cl_platforms.size(); i++) {
//            vector<cl::Device> cl_devices;
//            cl_platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &cl_devices);
//            //cl::Context cl_context(cl_devices); // same cl::Context for all devices (allocates extra VRAM on all other unused Nvidia GPUs)
//            for (uint j = 0u; j < (uint) cl_devices.size(); j++) {
//                cl::Context cl_context(cl_devices[j]); // separate cl::Context for each device
//                devices.push_back(Device_Info(cl_devices[j], cl_context, id++));
//            }
//        }




        Histogram his;
        //get all platforms (drivers)
        std::vector<cl::Platform> all_platforms;
        cl::Platform::get(&all_platforms);
        if (all_platforms.size() == 0) {
            std::cout << " No platforms found. Check OpenCL installation!\n";
            exit(1);
        }
        for (int i = 0; i < all_platforms.size(); i++) {
            std::cout << "found platform: " << all_platforms[i].getInfo<CL_PLATFORM_NAME>() << "\n";
        }

        cl::Platform default_platform = all_platforms[device_id];
        std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

        //get default device of the default platform
        std::vector<cl::Device> all_devices;
        default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
        if (all_devices.size() == 0) {
            std::cout << " No devices found. Check OpenCL installation!\n";
            exit(1);
        }
        cl::Device default_device = all_devices[0];
        std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";

        cl::Context context({default_device});

        cl::Program::Sources sources;
        // kernel calculates for each element C=A+B

        Kernel_histogram k;
        sources.push_back(k.get_kernel_code());

        cl::Program program(context, sources);
        try {
            program.build({default_device});
        }
        catch (cl::Error err) {
            std::cout << " Error building: " <<
                      program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
            exit(1);
        }

        //create queue to which we will push commands for the device.
        cl::CommandQueue queue(context, default_device);

        const int imageElements = ppm.pixels();
        // create buffers on the device
        cl::Buffer buffer_image(context, CL_MEM_READ_WRITE, sizeof(int) * ppm.pixels() * 3);
        cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, sizeof(int));
        cl::Buffer buffer_histogram(context, CL_MEM_READ_WRITE, sizeof(int) * 256 * 3);

        //write image to the device
        int zero = 0;
        auto aa = sizeof(int) * ppm.pixels() * 3;
        queue.enqueueWriteBuffer(buffer_image, CL_TRUE, 0, sizeof(int) * ppm.pixels() * 3, ppm.data.data());
        queue.enqueueFillBuffer(buffer_histogram, zero, 0, sizeof(int) * 256 * 3, NULL, NULL);

        cl::Kernel kernel(program, "histogram");
        kernel.setArg(0, buffer_image);
        kernel.setArg(1, sizeof(int), &imageElements);
        kernel.setArg(2, buffer_histogram);

        queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1024), cl::NDRange(64));

//        int C[256 * 3];
//      /read result C from the device to array C
        queue.enqueueReadBuffer(buffer_histogram, CL_TRUE, 0, sizeof(int) * 256 * 3, his.value.data());
        queue.finish();

        auto end = GetTickCount() - begin;
        std::cout << "\rDone.      " + std::to_string(end / 1000.0) + "           \n";
        return his;
    };


    PPM ppm;
    //load_ppm("./example.ppm", ppm);
    ppm.random_generate(15000);

//    auto his1 = histogram_cpu(ppm);

    //auto his2 = histogram_cpu_tbb(ppm);

    auto his3 = histogram_cpu_tbb_local(ppm);

    auto his4 = histogram_opencl(ppm, 0); //nv gpu

    auto his5 = histogram_opencl(ppm, 1); //intel gpu

    auto his6 = histogram_opencl(ppm, 2);//cpu

    std::cout << std::endl;
    //std::cout << "his1 == his2: " << std::to_string(his1 == his2) << std::endl;
    //std::cout << "his1 == his3: " << std::to_string(his1 == his3) << std::endl;
    std::cout << "his1 == his4: " << std::to_string(his3 == his4) << std::endl;

    std::cout << "his4 == his5 == his6: " << std::to_string(his4 == his5 && his5 == his6) << std::endl;

}