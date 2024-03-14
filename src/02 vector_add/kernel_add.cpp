//
// Created by xmyci on 14/03/2024.
//
#include "kernel_add.h"
#include "common/opencl-warpper/kernel.hpp"

Kernel_add::Kernel_add() {
    kernel_code = R( // ########################## begin of OpenCL C code ####################################################################
            kernel void add(global float *A, global float *B, global float *C) { // equivalent to "for(uint n=0u; n<N; n++) {", but executed in parallel
                const uint n = get_global_id(0);
                C[n] = A[n] + B[n];
            }
    ); // ############################################################### end of OpenCL C code #####################################################################

}
