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
#include <iostream>


//------------------------------------------------------------------------------

using namespace waterspout;


//------------------------------------------------------------------------------

namespace {

    void prepare_buffers(float* a, float* b, uint32_t size)
    {
        for (int i = 0; i < size; ++i)
        {
            a[i] = i / (float)size;
            b[i] = 0.0f;
        }
    }

    double measure_buffer_copy(math_factory& factory, float* srcBuffer, float* dstBuffer, uint32_t size)
    {
        timer t;
        factory->copy_buffer(srcBuffer, dstBuffer, size);
        return t.clock_elapsed();
    }


    void check_buffers(const char* method, float* a, float* b, uint32_t size)
    {
        for (int i = 0; i < size; ++i)
        {
            if (a[i] != b[i])
            {
                std::cout << method << ": Copy is invalid !" << std::endl;
            }
        }
    }

} // end namespace


//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    const uint32_t size = 1024 * 1024;
    float_buffer srcBuffer(size), dstBuffer(size);

    {
        math_factory fpu(FORCE_FPU);

        prepare_buffers(srcBuffer.data(), dstBuffer.data(), srcBuffer.size());

        double elapsed =
          measure_buffer_copy(fpu, srcBuffer.data(), dstBuffer.data(), srcBuffer.size());

        check_buffers(fpu.name(), srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        std::cout << fpu.name() << ": " << std::setprecision(32) <<  elapsed << " " << srcBuffer.size() << std::endl;
    }

    {
        math_factory sse(FORCE_SSE);

        prepare_buffers(srcBuffer.data(), dstBuffer.data(), srcBuffer.size());

        double elapsed =
          measure_buffer_copy(sse, srcBuffer.data(), dstBuffer.data(), srcBuffer.size());

        check_buffers(sse.name(), srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        std::cout << sse.name() << ": " << std::setprecision(32) << elapsed << " " << srcBuffer.size() << std::endl;
    }

    {
        math_factory avx(FORCE_AVX);

        prepare_buffers(srcBuffer.data(), dstBuffer.data(), srcBuffer.size());

        double elapsed =
          measure_buffer_copy(avx, srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        
        check_buffers(avx.name(), srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        std::cout << avx.name() << ": " << std::setprecision(32) << elapsed << " " << srcBuffer.size() << std::endl;
    }
    
    return 0;
}
