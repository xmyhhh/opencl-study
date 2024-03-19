//
// Created by xmyci on 14/03/2024.
//

#ifndef OPENCL_KERNEL_WARPPER_H
#define OPENCL_KERNEL_WARPPER_H

#include <iostream>

class kernel_warpper {
protected:
    std::string kernel_code;
public:
    std::string get_kernel_code(bool debug = false);
};


#endif //OPENCL_KERNEL_WARPPER_H
