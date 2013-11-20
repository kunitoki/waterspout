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

class timer : private noncopyable
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

protected:
    double clock_start_, clock_end_;
    clock_t cpu_start_, cpu_end_;
    bool stopped_;
};


//------------------------------------------------------------------------------

namespace {


static const uint32_t num_runs = 1;


//------------------------------------------------------------------------------

void check_buffer_is_value(float* buffer, uint32_t size, float value)
{
    for (int i = 0; i < size; ++i)
    {
        if (buffer[i] != value)
        {
            std::clog << "Errors at index "
                      << i << " (" << buffer[i] << "!=" << value << ")" << std::endl;

            throw std::runtime_error("Buffer is not a specific value !");
        }
    }
}

void check_buffer_is_zero(float* buffer, uint32_t size)
{
    check_buffer_is_value(buffer, size, 0.0f);
}

void check_buffers_are_equal(float* a, float* b, uint32_t size)
{
    for (int i = 0; i < size; ++i)
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
/*
double measure_buffer_clear(const math& factory, float* srcBuffer, uint32_t size)
{
    timer t;
    for (int i = 0; i < num_runs; ++i)
        factory->clear_buffer(srcBuffer, size);
    return t.clock_elapsed() / (double)num_runs;
}

double measure_buffer_set(const math& factory, float* srcBuffer, uint32_t size, float value)
{
    timer t;
    for (int i = 0; i < num_runs; ++i)
        factory->set_buffer(srcBuffer, size, value);
    return t.clock_elapsed() / (double)num_runs;
}

double measure_buffer_scale(const math& factory, float* srcBuffer, uint32_t size, float gain)
{
    timer t;
    for (int i = 0; i < num_runs; ++i)
        factory->scale_buffer(srcBuffer, size, gain);
    return t.clock_elapsed() / (double)num_runs;
}

double measure_buffer_copy(const math& factory, float* srcBuffer, float* dstBuffer, uint32_t size)
{
    timer t;
    for (int i = 0; i < num_runs; ++i)
        factory->copy_buffer(srcBuffer, dstBuffer, size);
    return t.clock_elapsed() / (double)num_runs;
}

double measure_buffers_add(const math& factory, float* srcBuffer, float* dstBuffer, uint32_t size)
{
    timer t;
    for (int i = 0; i < num_runs; ++i)
        factory->add_buffers(srcBuffer, srcBuffer, dstBuffer, size);
    return t.clock_elapsed() / (double)num_runs;
}

double measure_buffers_subtract(const math& factory, float* srcBuffer, float* dstBuffer, uint32_t size)
{
    timer t;
    for (int i = 0; i < num_runs; ++i)
        factory->subtract_buffers(srcBuffer, srcBuffer, dstBuffer, size);
    return t.clock_elapsed() / (double)num_runs;
}

double measure_buffers_multiply(const math& factory, float* srcBuffer, float* dstBuffer, uint32_t size)
{
    timer t;
    for (int i = 0; i < num_runs; ++i)
        factory->multiply_buffers(srcBuffer, srcBuffer, dstBuffer, size);
    return t.clock_elapsed() / (double)num_runs;
}

double measure_buffers_divide(const math& factory, float* srcBuffer, float* dstBuffer, uint32_t size)
{
    timer t;
    for (int i = 0; i < num_runs; ++i)
        factory->divide_buffers(srcBuffer, srcBuffer, dstBuffer, size);
    return t.clock_elapsed() / (double)num_runs;
}


//------------------------------------------------------------------------------

void print_elapsed(const math& factory, const char* function, double elapsed)
{
    std::cout
        << factory.name() << "(" << function << "): "
        << std::setprecision(32) << elapsed << " ms" << std::endl;
}

#if 0
double elapsed = 0.0;

prepare_buffers(srcBuffer.data(), dstBuffer.data(), srcBuffer.size());

elapsed =
  measure_buffer_clear(factory, srcBuffer.data(), srcBuffer.size());
print_elapsed(factory, "clear_buffer", elapsed);

elapsed =
  measure_buffer_set(factory, srcBuffer.data(), srcBuffer.size(), 0.5f);
print_elapsed(factory, "set_buffer", elapsed);

elapsed =
  measure_buffer_scale(factory, srcBuffer.data(), srcBuffer.size(), 0.5f);
print_elapsed(factory, "scale_buffer", elapsed);

elapsed =
  measure_buffer_copy(factory, srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
print_elapsed(factory, "copy_buffer", elapsed);

elapsed =
  measure_buffers_add(factory, srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
print_elapsed(factory, "add_buffers", elapsed);

elapsed =
  measure_buffers_subtract(factory, srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
print_elapsed(factory, "subtract_buffers", elapsed);

elapsed =
  measure_buffers_multiply(factory, srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
print_elapsed(factory, "multiply_buffers", elapsed);

elapsed =
  measure_buffers_divide(factory, srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
print_elapsed(factory, "divide_buffers", elapsed);

std::cout << std::endl;
#endif

*/


//------------------------------------------------------------------------------

void run_unit_tests(int flags)
{
    const uint32_t size = 4096;

    math fpu(FORCE_FPU);
    math simd(flags);

    float_buffer src_buffer_a1(size);
    float_buffer src_buffer_a2(size);
    float_buffer src_buffer_b1(size);
    float_buffer src_buffer_b2(size);
    float_buffer dst_buffer_1(size);
    float_buffer dst_buffer_2(size);

    std::clog << simd.name() << ": clear_buffer" << std::endl;
    simd->clear_buffer(src_buffer_a1.data(), size);
    fpu->clear_buffer(src_buffer_a2.data(), size);
    check_buffers_are_equal(src_buffer_a1.data(), src_buffer_a2.data(), size);

    std::clog << simd.name() << ": set_buffer" << std::endl;
    simd->set_buffer(src_buffer_a1.data(), size, 0.5f);
    fpu->set_buffer(src_buffer_a2.data(), size, 0.5f);
    check_buffers_are_equal(src_buffer_a1.data(), src_buffer_a2.data(), size);

    std::clog << simd.name() << ": scale_buffer" << std::endl;
    simd->scale_buffer(src_buffer_a1.data(), size, 2.0f);
    fpu->scale_buffer(src_buffer_a2.data(), size, 2.0f);
    check_buffers_are_equal(src_buffer_a1.data(), src_buffer_a2.data(), size);

    std::clog << simd.name() << ": copy_buffer" << std::endl;
    simd->copy_buffer(src_buffer_a1.data(), dst_buffer_1.data(), size);
    fpu->copy_buffer(src_buffer_a2.data(), dst_buffer_2.data(), size);
    check_buffers_are_equal(dst_buffer_1.data(), dst_buffer_2.data(), size);

    std::clog << simd.name() << ": add_buffers" << std::endl;
    simd->add_buffers(src_buffer_a1.data(), src_buffer_b1.data(), dst_buffer_1.data(), size);
    fpu->add_buffers(src_buffer_a2.data(), src_buffer_b2.data(), dst_buffer_2.data(), size);
    check_buffers_are_equal(dst_buffer_1.data(), dst_buffer_2.data(), size);

    std::clog << simd.name() << ": subtract_buffers" << std::endl;
    simd->subtract_buffers(src_buffer_a1.data(), src_buffer_b1.data(), dst_buffer_1.data(), size);
    fpu->subtract_buffers(src_buffer_a2.data(), src_buffer_b2.data(), dst_buffer_2.data(), size);
    check_buffers_are_equal(dst_buffer_1.data(), dst_buffer_2.data(), size);

    std::clog << simd.name() << ": multiply_buffers" << std::endl;
    simd->multiply_buffers(src_buffer_a1.data(), src_buffer_b1.data(), dst_buffer_1.data(), size);
    fpu->multiply_buffers(src_buffer_a2.data(), src_buffer_b2.data(), dst_buffer_2.data(), size);
    check_buffers_are_equal(dst_buffer_1.data(), dst_buffer_2.data(), size);

    std::clog << simd.name() << ": divide_buffers" << std::endl;
    simd->set_buffer(src_buffer_b1.data(), size, 2.0f);
    fpu->set_buffer(src_buffer_b2.data(), size, 2.0f);
    simd->divide_buffers(src_buffer_a1.data(), src_buffer_b1.data(), dst_buffer_1.data(), size);
    fpu->divide_buffers(src_buffer_a2.data(), src_buffer_b2.data(), dst_buffer_2.data(), size);
    check_buffers_are_equal(dst_buffer_1.data(), dst_buffer_2.data(), size);
}


} // end namespace


//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    //run_unit_tests(FORCE_MMX);
    run_unit_tests(FORCE_SSE);
    //run_unit_tests(FORCE_SSE2);
    //run_unit_tests(FORCE_SSE3);
    //run_unit_tests(FORCE_SSSE3);
    //run_unit_tests(FORCE_SSE41);
    //run_unit_tests(FORCE_SSE42);
    //run_unit_tests(FORCE_AVX);

    return 0;
}
