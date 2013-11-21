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

void run_unit_tests(int flags)
{
    const uint32 s = 4096;

    math fpu(FORCE_FPU);
    math simd(flags);


    // integer math

    int32_buffer src_buffer_int32_a1(s);
    int32_buffer src_buffer_int32_a2(s);
    int32_buffer src_buffer_int32_b1(s);
    int32_buffer src_buffer_int32_b2(s);
    int32_buffer dst_buffer_int32_1(s);
    int32_buffer dst_buffer_int32_2(s);

    std::clog << simd.name() << ": clear_buffer_in32" << std::endl;
    simd->clear_buffer_int32(src_buffer_int32_a1.data(), s);
    fpu->clear_buffer_int32(src_buffer_int32_a2.data(), s);
    check_buffers_are_equal(src_buffer_int32_a1.data(), src_buffer_int32_a2.data(), s);

    std::clog << simd.name() << ": set_buffer_int32" << std::endl;
    simd->set_buffer_int32(src_buffer_int32_a1.data(), s, 100);
    fpu->set_buffer_int32(src_buffer_int32_a2.data(), s, 100);
    simd->set_buffer_int32(src_buffer_int32_b1.data(), s, 1000);
    fpu->set_buffer_int32(src_buffer_int32_b2.data(), s, 1000);
    check_buffers_are_equal(src_buffer_int32_a1.data(), src_buffer_int32_a2.data(), s);
    check_buffers_are_equal(src_buffer_int32_b1.data(), src_buffer_int32_b2.data(), s);

    std::clog << simd.name() << ": copy_buffer_int32" << std::endl;
    simd->copy_buffer_int32(src_buffer_int32_a1.data(), dst_buffer_int32_1.data(), s);
    fpu->copy_buffer_int32(src_buffer_int32_a2.data(), dst_buffer_int32_2.data(), s);
    check_buffers_are_equal(dst_buffer_int32_1.data(), dst_buffer_int32_2.data(), s);

    std::clog << simd.name() << ": add_buffers_int32" << std::endl;
    simd->add_buffers_int32(src_buffer_int32_a1.data(), src_buffer_int32_b1.data(), dst_buffer_int32_1.data(), s);
    fpu->add_buffers_int32(src_buffer_int32_a2.data(), src_buffer_int32_b2.data(), dst_buffer_int32_2.data(), s);
    check_buffers_are_equal(dst_buffer_int32_1.data(), dst_buffer_int32_2.data(), s);

    std::clog << simd.name() << ": subtract_buffers_int32" << std::endl;
    simd->subtract_buffers_int32(src_buffer_int32_a1.data(), src_buffer_int32_b1.data(), dst_buffer_int32_1.data(), s);
    fpu->subtract_buffers_int32(src_buffer_int32_a2.data(), src_buffer_int32_b2.data(), dst_buffer_int32_2.data(), s);
    check_buffers_are_equal(dst_buffer_int32_1.data(), dst_buffer_int32_2.data(), s);


    // floating point math

    float_buffer src_buffer_a1(s);
    float_buffer src_buffer_a2(s);
    float_buffer src_buffer_b1(s);
    float_buffer src_buffer_b2(s);
    float_buffer dst_buffer_1(s);
    float_buffer dst_buffer_2(s);

    std::clog << simd.name() << ": clear_buffer_float" << std::endl;
    simd->clear_buffer_float(src_buffer_a1.data(), s);
    fpu->clear_buffer_float(src_buffer_a2.data(), s);
    check_buffers_are_equal(src_buffer_a1.data(), src_buffer_a2.data(), s);

    std::clog << simd.name() << ": set_buffer_float" << std::endl;
    simd->set_buffer_float(src_buffer_a1.data(), s, 0.5f);
    fpu->set_buffer_float(src_buffer_a2.data(), s, 0.5f);
    simd->set_buffer_float(src_buffer_b1.data(), s, 500.0f);
    fpu->set_buffer_float(src_buffer_b2.data(), s, 500.0f);
    check_buffers_are_equal(src_buffer_a1.data(), src_buffer_a2.data(), s);
    check_buffers_are_equal(src_buffer_b1.data(), src_buffer_b2.data(), s);

    std::clog << simd.name() << ": scale_buffer_float" << std::endl;
    simd->scale_buffer_float(src_buffer_a1.data(), s, 2.0f);
    fpu->scale_buffer_float(src_buffer_a2.data(), s, 2.0f);
    check_buffers_are_equal(src_buffer_a1.data(), src_buffer_a2.data(), s);

    std::clog << simd.name() << ": copy_buffer_float" << std::endl;
    simd->copy_buffer_float(src_buffer_a1.data(), dst_buffer_1.data(), s);
    fpu->copy_buffer_float(src_buffer_a2.data(), dst_buffer_2.data(), s);
    check_buffers_are_equal(dst_buffer_1.data(), dst_buffer_2.data(), s);

    std::clog << simd.name() << ": add_buffers_float" << std::endl;
    simd->add_buffers_float(src_buffer_a1.data(), src_buffer_b1.data(), dst_buffer_1.data(), s);
    fpu->add_buffers_float(src_buffer_a2.data(), src_buffer_b2.data(), dst_buffer_2.data(), s);
    check_buffers_are_equal(dst_buffer_1.data(), dst_buffer_2.data(), s);

    std::clog << simd.name() << ": subtract_buffers_float" << std::endl;
    simd->subtract_buffers_float(src_buffer_a1.data(), src_buffer_b1.data(), dst_buffer_1.data(), s);
    fpu->subtract_buffers_float(src_buffer_a2.data(), src_buffer_b2.data(), dst_buffer_2.data(), s);
    check_buffers_are_equal(dst_buffer_1.data(), dst_buffer_2.data(), s);

    std::clog << simd.name() << ": multiply_buffers_float" << std::endl;
    simd->multiply_buffers_float(src_buffer_a1.data(), src_buffer_b1.data(), dst_buffer_1.data(), s);
    fpu->multiply_buffers_float(src_buffer_a2.data(), src_buffer_b2.data(), dst_buffer_2.data(), s);
    check_buffers_are_equal(dst_buffer_1.data(), dst_buffer_2.data(), s);

    std::clog << simd.name() << ": divide_buffers_float" << std::endl;
    simd->set_buffer_float(src_buffer_b1.data(), s, 2.0f);
    fpu->set_buffer_float(src_buffer_b2.data(), s, 2.0f);
    simd->divide_buffers_float(src_buffer_a1.data(), src_buffer_b1.data(), dst_buffer_1.data(), s);
    fpu->divide_buffers_float(src_buffer_a2.data(), src_buffer_b2.data(), dst_buffer_2.data(), s);
    check_buffers_are_equal(dst_buffer_1.data(), dst_buffer_2.data(), s);

    std::clog << std::endl;
}


} // end namespace


//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    run_unit_tests(FORCE_MMX);
    run_unit_tests(FORCE_SSE);
    run_unit_tests(FORCE_SSE2);
    //run_unit_tests(FORCE_SSE3);
    //run_unit_tests(FORCE_SSSE3);
    //run_unit_tests(FORCE_SSE41);
    //run_unit_tests(FORCE_SSE42);
    //run_unit_tests(FORCE_AVX);

    return 0;
}
