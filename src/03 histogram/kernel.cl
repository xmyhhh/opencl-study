MSTRINGIFY(

kernel void simple_add( global int* A, global int* B, global int* C ) {
    const int idx = get_global_id(0);
    C[idx] = A[idx] + B[idx] * 2;
}

);