//
// Created by xmyci on 13/03/2024.
//
#include "timer.h"
#include "tbb/compat/condition_variable"
#include <string>
#include <iostream>

void run_funtion_and_get_runtime(run_funtion p) {
    __int64 begin = GetTickCount();
    p();
    auto end = GetTickCount() - begin;
    std::cout << "\rDone.      " + std::to_string(end / 1000.0) + "           \n";
}
