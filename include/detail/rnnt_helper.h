#pragma once

#include <limits>
#include <algorithm>
#include <cmath>

#include "hostdevice.h"
#include <cuda_fp16.h>

namespace rnnt_helper {

static const float threshold = 1e-1;

template<typename T>
inline HOSTDEVICE T neg_inf() { return -T(INFINITY); }

template<typename T>
inline HOSTDEVICE T log_sum_exp(T a, T b) {
    if (a == neg_inf<T>()) return b;
    if (b == neg_inf<T>()) return a;
    if (a > b)
        return log1p(exp(b-a)) + a;
    else
        return log1p(exp(a-b)) + b;
}

inline int div_up(int x, int y) {
    return (x + y - 1) / y;
}
template<>
inline HOSTDEVICE half log_sum_exp(half a, half b)
{
if (__hisinf(a) == -1) return b;
if (__hisinf(b) == -1) return a;
if (__hgt(a, b))
return __hadd(hlog(__hadd((half)1, hexp(__hsub(b, a)))), a);
else
return __hadd(hlog(__hadd((half)1, hexp(__hsub(a, b)))), b);
}

template <typename Arg, typename Res = Arg> struct maximum {
    HOSTDEVICE
    Res operator()(const Arg& x, const Arg& y) const {
        return x < y ? y : x;
    }
};
template<> struct maximum<half, half> {
    HOSTDEVICE
    half operator()(const half& x, const half& y) const {
        return __hlt(x, y) ? y : x;
    }
};

template <typename Arg, typename Res = Arg> struct add {
    HOSTDEVICE
    Res operator()(const Arg& x, const Arg& y) const {
        return x + y;
    }
};

template<> struct add<half, half> {
    HOSTDEVICE
    half operator()(const half& x, const half& y) {
        return __hadd(x, y);
    }
};

template <typename Arg, typename Res = Arg> struct identity {
    HOSTDEVICE Res operator()(const Arg& x) const {return Res(x);}
};

template <typename Arg, typename Res = Arg> struct negate {
    HOSTDEVICE Res operator()(const Arg& x) const {return Res(-x);}
};

template <typename Arg, typename Res = Arg> struct exponential {
    HOSTDEVICE Res operator()(const Arg& x) const {return std::exp(x);}
};

template<> struct exponential<half, half>
{
    HOSTDEVICE half operator()(const half& x) const {
        return hexp(x);
    }
};

template<typename Arg1, typename Arg2 = Arg1, typename Res=Arg1>
struct log_plus {
    typedef Res result_type;
    HOSTDEVICE
    Res operator()(const Arg1& p1, const Arg2& p2) {
        if (p1 == neg_inf<Arg1>())
            return p2;
        if (p2 == neg_inf<Arg2>())
            return p1;
        Res result = log1p(exp(-fabs(p1 - p2))) + maximum<Res>()(p1, p2);
        return result;
    }
};

}