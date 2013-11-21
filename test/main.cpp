/*
 * waterspout
 *
 *   - simd abstraction library for audio/image manipulation -
 *
 * Copyright (c) 2013 Lucio Asnaghi
 *
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <waterspout.h>

#include <ctime>
#include <stdexcept>
#include <iostream>
#include <iomanip>


using namespace waterspout;


//==============================================================================

//------------------------------------------------------------------------------

/**
 * The timer class to measure elapsed time and benchmarking
 */

class timer
{
public:
    timer()
    {
        restart();
    }

    virtual ~timer()
    {
    }

    void restart()
    {
        stopped_ = false;
        clock_start_ = time_now();
        cpu_start_ = clock();
    }

    virtual void stop()
    {
        stopped_ = true;
        cpu_end_ = clock();
        clock_end_ = time_now();
    }

    double cpu_elapsed()
    {
        // return elapsed CPU time in ms
        if (! stopped_)
        {
            stop();
        }

        return ((double)(cpu_end_ - cpu_start_)) / CLOCKS_PER_SEC * 1000.0;
    }

    double clock_elapsed()
    {
        // return elapsed wall clock time in ms
        if (! stopped_)
        {
            stop();
        }

        return (clock_end_ - clock_start_) * 1000.0;
    }

    forcedinline double time_now()
    {
#if defined(WATERSPOUT_COMPILER_MSVC)
        LARGE_INTEGER t, f;
        QueryPerformanceCounter(&t);
        QueryPerformanceFrequency(&f);
        return double(t.QuadPart) / double(f.QuadPart);
#else
        struct timeval t;
        struct timezone tzp;
        gettimeofday(&t, &tzp);
        return t.tv_sec + t.tv_usec * 1e-6;
#endif
    }

protected:
    double clock_start_, clock_end_;
    clock_t cpu_start_, cpu_end_;
    bool stopped_;
};


//------------------------------------------------------------------------------

namespace {


//------------------------------------------------------------------------------

template<typename T>
void check_buffer_is_value(T* buffer, uint32 size, T value)
{
    for (uint32 i = 0; i < size; ++i)
    {
        if (buffer[i] != value)
        {
            std::clog << "Errors at index "
                      << i << " (" << buffer[i] << "!=" << value << ")" << std::endl;

            throw std::runtime_error("Buffer is not a specific value !");
        }
    }
}

template<typename T>
void check_buffer_is_zero(T* buffer, uint32 size)
{
    check_buffer_is_value(buffer, size, static_cast<T>(0));
}

template<typename T>
void check_buffers_are_equal(T* a, T* b, uint32 size)
{
    for (uint32 i = 0; i < size; ++i)
    {
        if (a[i] != b[i])
        {
            std::clog << "Errors at index "
                      << i << " (" << a[i] << "!=" << b[i] << ")" << std::endl;

            throw std::runtime_error("Buffers are not equal !");
        }
    }
}


//------------------------------------------------------------------------------

#define run_typed_test_by_flag(datatype) \
    { \
    datatype## _buffer src_buffer_a1(s); \
    datatype## _buffer src_buffer_a2(s); \
    datatype## _buffer src_buffer_b1(s); \
    datatype## _buffer src_buffer_b2(s); \
    datatype## _buffer dst_buffer_1(s); \
    datatype## _buffer dst_buffer_2(s); \
    \
    std::clog << " - clear_buffer_" << #datatype << std::endl; \
    simd->clear_buffer_ ##datatype (src_buffer_a1.data(), s); \
    fpu->clear_buffer_ ##datatype (src_buffer_a2.data(), s); \
    check_buffers_are_equal(src_buffer_a1.data(), src_buffer_a2.data(), s); \
    \
    std::clog << " - set_buffer_" << #datatype << std::endl; \
    simd->set_buffer_ ##datatype (src_buffer_a1.data(), s, (datatype)1); \
    fpu->set_buffer_ ##datatype (src_buffer_a2.data(), s, (datatype)1); \
    simd->set_buffer_ ##datatype (src_buffer_b1.data(), s, (datatype)500); \
    fpu->set_buffer_ ##datatype (src_buffer_b2.data(), s, (datatype)500); \
    check_buffers_are_equal(src_buffer_a1.data(), src_buffer_a2.data(), s); \
    check_buffers_are_equal(src_buffer_b1.data(), src_buffer_b2.data(), s); \
    \
    std::clog << " - scale_buffer_" << #datatype << std::endl; \
    simd->scale_buffer_ ##datatype (src_buffer_a1.data(), s, 2.0f); \
    fpu->scale_buffer_ ##datatype (src_buffer_a2.data(), s, 2.0f); \
    check_buffers_are_equal(src_buffer_a1.data(), src_buffer_a2.data(), s); \
    \
    std::clog << " - copy_buffer_" << #datatype << std::endl; \
    simd->copy_buffer_ ##datatype (src_buffer_a1.data(), dst_buffer_1.data(), s); \
    fpu->copy_buffer_ ##datatype (src_buffer_a2.data(), dst_buffer_2.data(), s); \
    check_buffers_are_equal(dst_buffer_1.data(), dst_buffer_2.data(), s); \
    \
    std::clog << " - add_buffers_" << #datatype << std::endl; \
    simd->add_buffers_ ##datatype (src_buffer_a1.data(), src_buffer_b1.data(), dst_buffer_1.data(), s); \
    fpu->add_buffers_ ##datatype (src_buffer_a2.data(), src_buffer_b2.data(), dst_buffer_2.data(), s); \
    check_buffers_are_equal(dst_buffer_1.data(), dst_buffer_2.data(), s); \
    \
    std::clog << " - subtract_buffers_" << #datatype << std::endl; \
    simd->subtract_buffers_ ##datatype (src_buffer_a1.data(), src_buffer_b1.data(), dst_buffer_1.data(), s); \
    fpu->subtract_buffers_ ##datatype (src_buffer_a2.data(), src_buffer_b2.data(), dst_buffer_2.data(), s); \
    check_buffers_are_equal(dst_buffer_1.data(), dst_buffer_2.data(), s); \
    \
    std::clog << " - multiply_buffers_" << #datatype << std::endl; \
    simd->multiply_buffers_ ##datatype (src_buffer_a1.data(), src_buffer_b1.data(), dst_buffer_1.data(), s); \
    fpu->multiply_buffers_ ##datatype (src_buffer_a2.data(), src_buffer_b2.data(), dst_buffer_2.data(), s); \
    check_buffers_are_equal(dst_buffer_1.data(), dst_buffer_2.data(), s); \
    \
    std::clog << " - divide_buffers_" << #datatype << std::endl; \
    simd->set_buffer_ ##datatype (src_buffer_b1.data(), s, (datatype)2); \
    fpu->set_buffer_ ##datatype (src_buffer_b2.data(), s, (datatype)2); \
    simd->divide_buffers_ ##datatype (src_buffer_a1.data(), src_buffer_b1.data(), dst_buffer_1.data(), s); \
    fpu->divide_buffers_ ##datatype (src_buffer_a2.data(), src_buffer_b2.data(), dst_buffer_2.data(), s); \
    check_buffers_are_equal(dst_buffer_1.data(), dst_buffer_2.data(), s); \
    \
    std::clog << std::endl; \
    }


#define run_all_tests_by_flag(flags) \
    { \
    const uint32 s = 8192; \
    math fpu(FORCE_FPU); \
    math simd(flags); \
    std::clog << simd.name() << ": " << std::endl; \
    run_typed_test_by_flag(int8); \
    run_typed_test_by_flag(uint8); \
    run_typed_test_by_flag(int16); \
    run_typed_test_by_flag(uint16); \
    run_typed_test_by_flag(int32); \
    run_typed_test_by_flag(uint32); \
    run_typed_test_by_flag(int64); \
    run_typed_test_by_flag(uint64); \
    run_typed_test_by_flag(float); \
    run_typed_test_by_flag(double); \
    }

#define run_all_tests \
    run_all_tests_by_flag(FORCE_MMX); \
    run_all_tests_by_flag(FORCE_SSE); \
    run_all_tests_by_flag(FORCE_SSE2); \
    run_all_tests_by_flag(FORCE_SSE3); \
    run_all_tests_by_flag(FORCE_SSSE3); \
    run_all_tests_by_flag(FORCE_SSE41); \
    run_all_tests_by_flag(FORCE_SSE42); \
    run_all_tests_by_flag(FORCE_AVX);

} // end namespace


//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    unused(argc);
    unused(argv);

    run_all_tests

    return 0;
}
