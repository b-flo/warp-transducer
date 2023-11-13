#pragma once

#ifdef __CUDACC__
// changed from '__host__ __device__' to '__device__' to call cuda math lib functions for half
    #define HOSTDEVICE __device__
#else
    #define HOSTDEVICE
#endif