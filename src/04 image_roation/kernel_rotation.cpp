
#include "kernel_rotation.h"
#include "common/opencl-warpper/kernel.hpp"


Kernel_histogram::Kernel_histogram() {
    kernel_code = R( // ########################## begin of OpenCL C code ####################################################################
            kernel void histogram(global int *data, int numberOfData,
                                  global int *histogram) { // equivalent to "for(uint n=0u; n<N; n++) {", but executed in parallel
                int lid = get_local_id(0);
                int gid = get_global_id(0);

                const int HIST_BINS = 256;
                local int localHistogram_R[HIST_BINS];
                local int localHistogram_G[HIST_BINS];
                local int localHistogram_B[HIST_BINS];

                for (int i = lid; i < HIST_BINS; i += get_local_size(0)) {
                    localHistogram_R[i] = 0;
                    localHistogram_G[i] = 0;
                    localHistogram_B[i] = 0;
                }

                barrier(CLK_LOCAL_MEM_FENCE);

                for (int i = gid; i < numberOfData; i += get_global_size(0)) {
                    atomic_add(&localHistogram_R[data[i * 3]], 1);
                    atomic_add(&localHistogram_G[data[i * 3 + 1]], 1);
                    atomic_add(&localHistogram_B[data[i * 3 + 2]], 1);
                }
                barrier(CLK_LOCAL_MEM_FENCE);

                for (int i = lid; i < HIST_BINS; i += get_local_size(0)) {
                    atomic_add(&histogram[i], localHistogram_R[i]);
                    atomic_add(&histogram[i + HIST_BINS], localHistogram_G[i]);
                    atomic_add(&histogram[i + HIST_BINS * 2], localHistogram_B[i]);
                }

                //printf("( %d | %d) ", gid, lid);

//                if (lid == 0 && gid == 0) {
//                    printf(" %d \n", localHistogram_R[1]);
//                    printf(" %d \n", localHistogram_B[1]);
//                    printf(" %d \n", localHistogram_G[1]);
//                }

            }

    ); // ############################################################### end of OpenCL C code #####################################################################
}
