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
#include "kernel_rotation.h"
#include "common/opencl-warpper/opencl.hpp"

void rotation() {
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
    //x' = cos θ (x − x0) + sin θ (y − y0)
    //y' = − sin θ (x − x0) + cos θ (y − y0)
    auto rotation_cpu = [](const PPM &ppm, double theta) {
        PPM res;
        __int64 begin = GetTickCount();

        auto end = GetTickCount() - begin;
        std::cout << "\rDone.      " + std::to_string(end / 1000.0) + "           \n";
        return res;
    };

    PPM ppm;

    ppm.random_generate(15000);

    auto res1 = rotation_cpu(ppm, 45);


}