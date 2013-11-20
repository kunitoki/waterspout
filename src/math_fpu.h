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

#ifndef __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_FPU_H__
#define __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_FPU_H__


//==============================================================================

//------------------------------------------------------------------------------

/**
 * FPU math clas that provides floating point operations as fallback
 */

class math_fpu : public math
{
public:

    //--------------------------------------------------------------------------

    const char* name() { return "FPU"; }


    //--------------------------------------------------------------------------

    math_fpu()
    {
        //assertfalse; // not implemented !
    }


    //--------------------------------------------------------------------------

    void clear_buffer(
        float* srcBuffer,
        uint32_t size)
    {
        for (int i = 0; i < size; ++i)
        {
          *srcBuffer++ = 0.0f;
        }
    }


    //--------------------------------------------------------------------------

    void scale_buffer(
        float* srcBuffer,
        uint32_t size,
        float gain)
    {
        for (int i = 0; i < size; ++i)
        {
          *srcBuffer++ *= gain;
          undernormalize(srcBuffer[i]);
        }
    }


    //--------------------------------------------------------------------------

    void copy_buffer(
        float* srcBuffer,
        float* dstBuffer,
        uint32_t size)
    {
        for (int i = 0; i < size; ++i)
        {
          *dstBuffer++ = *srcBuffer++;
        }
    }


    //--------------------------------------------------------------------------

    void add_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size)
    {
        for (int i = 0; i < size; ++i)
        {
          *dstBuffer++ = *srcBufferA++ + *srcBufferB++;
        }
    }


    //--------------------------------------------------------------------------

    void subtract_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size)
    {
        for (int i = 0; i < size; ++i)
        {
          *dstBuffer++ = *srcBufferA++ - *srcBufferB++;
        }
    }


    //--------------------------------------------------------------------------

    void multiply_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size)
    {
        for (int i = 0; i < size; ++i)
        {
          *dstBuffer++ = *srcBufferA++ * *srcBufferB++;
          undernormalize(dstBuffer[i]);
        }
    }


    //--------------------------------------------------------------------------

    void divide_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size)
    {
        for (int i = 0; i < size; ++i)
        {
          *dstBuffer++ = *srcBufferA++ / *srcBufferB++;
          undernormalize(dstBuffer[i]);
        }
    }

};


#endif
