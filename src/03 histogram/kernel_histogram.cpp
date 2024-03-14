
#include "kernel_histogram.h"
#include "common/opencl-warpper/kernel.hpp"


Kernel_histogram::Kernel_histogram() {
    kernel_code = R( // ########################## begin of OpenCL C code ####################################################################

            kernel void histogram(global int *data, int numberOfData, global int *histogram) { // equivalent to "for(uint n=0u; n<N; n++) {", but executed in parallel
                const int HIST_BINS = 256;
                local int localHistogram[HIST_BINS];
//                int lid = get_local_id(0);
//                int gid = get_global_id(0);
//                for (int i = lid; i < HIST_BINS; i += get_local_size(0)) {
//                    localHistogram[i] = 0;
//                }
//                barrier(CLK_LOCAL_MEM_FENCE);
//                for (int i = gid; i < numberOfData; i += get_global_size(0)) {
//                    atomic_add(&localHistogram[data[i]], 1);
//                }
//                barrier(CLK_LOCAL_MEM_FENCE);
//
//                for (int i = lid; i < HIST_BINS; i += get_local_size(0)) {
//                    atomic_add(&histogram[i], localHistogram[data[i]]);
//                }
            }
    ); // ############################################################### end of OpenCL C code #####################################################################

}
