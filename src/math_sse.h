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

#ifndef __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_SSE_H__
#define __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_SSE_H__


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Specific SSE math class elaborating on __m128 buffers
 */

class math_sse : public math_mmx
{
public:

    //--------------------------------------------------------------------------

    const char* name() const { return "SSE"; }


    //--------------------------------------------------------------------------

    enum SSEMathDefines
    {
        MIN_SSE_SIZE    = 4,
        MIN_SSE_SAMPLES = 32
    };


    //--------------------------------------------------------------------------

    math_sse()
    {
        //assertfalse; // not implemented !
    }


    //--------------------------------------------------------------------------

    void clear_buffer(
        float* srcBuffer,
        uint32_t size) const
    {
        if (size < MIN_SSE_SAMPLES)
        {
            math_mmx::clear_buffer(srcBuffer, size);
        }
        else
        {
            assert(size >= MIN_SSE_SIZE);

            const ptrdiff_t align_bytes = ((ptrdiff_t)srcBuffer & 0x0F);

            // Copy unaligned head
            switch (align_bytes >> 2)
            {
            case 1: --size; *srcBuffer++ = 0.0f;
            case 2: --size; *srcBuffer++ = 0.0f;
            case 3: --size; *srcBuffer++ = 0.0f;
            }

            // Clear with simd
            __m128* vectorBuffer = (__m128 *)srcBuffer;

            int vector_count = size >> 2;
            while (vector_count--)
            {
                *vectorBuffer++ = _mm_setzero_ps();
            }

            // Handle any unaligned leftovers
            srcBuffer = (float*)vectorBuffer;

            switch (size & 3)
            {
            case 3: *srcBuffer++ = 0.0f;
            case 2: *srcBuffer++ = 0.0f;
            case 1: *srcBuffer++ = 0.0f;
            }
        }
    }


    //--------------------------------------------------------------------------

    void scale_buffer(
        float* srcBuffer,
        uint32_t size,
        float gain) const
    {
        if (size < MIN_SSE_SAMPLES)
        {
            math_mmx::scale_buffer(srcBuffer, size, gain);
        }
        else
        {
            assert(size >= MIN_SSE_SIZE);

            const ptrdiff_t align_bytes = ((ptrdiff_t)srcBuffer & 0x0F);

            // Copy unaligned head
            switch (align_bytes >> 2)
            {
            case 1:
                --size;
                *srcBuffer *= gain;
                undernormalize(*srcBuffer);
                ++srcBuffer;

            case 2:
                --size;
                *srcBuffer *= gain;
                undernormalize(*srcBuffer);
                ++srcBuffer;

            case 3:
                --size;
                *srcBuffer *= gain;
                undernormalize(*srcBuffer);
                ++srcBuffer;
            }

            // Scale with simd
            const disable_sse_denormals disable_denormals;

            const __m128 vscale =_mm_set1_ps(gain);

            __m128* vectorBuffer = (__m128*)srcBuffer;

            int vector_count = size >> 2;
            while (vector_count--)
            {
                *vectorBuffer = _mm_mul_ps(*vectorBuffer, vscale);
                ++vectorBuffer;
            }

            // Handle any unaligned leftovers
            srcBuffer = (float*)vectorBuffer;

            switch (size & 3)
            {
            case 3:
                *srcBuffer *= gain;
                undernormalize(*srcBuffer);
                ++srcBuffer;

            case 2:
                *srcBuffer *= gain;
                undernormalize(*srcBuffer);
                ++srcBuffer;

            case 1:
                *srcBuffer *= gain;
                undernormalize(*srcBuffer);
                ++srcBuffer;
            }
        }
    }


    //--------------------------------------------------------------------------

    void copy_buffer(
        float* srcBuffer,
        float* dstBuffer,
        uint32_t size) const
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)srcBuffer & 0x0F);

        if (size < MIN_SSE_SAMPLES ||
              ((ptrdiff_t)dstBuffer & 0x0F) != align_bytes)
        {
            math_mmx::copy_buffer(srcBuffer, dstBuffer, size);
        } 
        else 
        { 
            assert(size >= MIN_SSE_SIZE);

            // Copy unaligned head
            switch (align_bytes >> 2)
            {
            case 1: --size; *dstBuffer++ = *srcBuffer++;
            case 2: --size; *dstBuffer++ = *srcBuffer++;
            case 3: --size; *dstBuffer++ = *srcBuffer++;
            }  

            // Copy with simd
            __m128* sourceVector = (__m128*)srcBuffer;
            __m128* destVector = (__m128*)dstBuffer;

            int vector_count = size >> 2;
            while (vector_count--)
            {
                *destVector = *sourceVector;

                ++destVector;
                ++sourceVector;
            }

            // Handle unaligned leftovers
            dstBuffer = (float*)destVector;
            srcBuffer = (float*)sourceVector;

            switch (size & 3)
            {
            case 3: *dstBuffer++ = *srcBuffer++;
            case 2: *dstBuffer++ = *srcBuffer++;
            case 1: *dstBuffer++ = *srcBuffer++;
            }
        }
    }


    //--------------------------------------------------------------------------

    void add_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size) const
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)dstBuffer & 0x0F);

        if (size < MIN_SSE_SAMPLES ||
            (align_bytes != ((ptrdiff_t)srcBufferA & 0x0F) ||
             align_bytes != ((ptrdiff_t)srcBufferB & 0x0F)))
        {
            math_mmx::add_buffers(srcBufferA, srcBufferB, dstBuffer, size);
        }
        else
        {
            assert(size >= MIN_SSE_SIZE);

            // Copy unaligned head
            switch (align_bytes >> 2)
            {
            case 1: --size; *dstBuffer++ = *srcBufferA++ + *srcBufferB++;
            case 2: --size; *dstBuffer++ = *srcBufferA++ + *srcBufferB++;
            case 3: --size; *dstBuffer++ = *srcBufferA++ + *srcBufferB++;
            }

            // Scale with simd
            __m128* vectorBufferA = (__m128*)srcBufferA;
            __m128* vectorBufferB = (__m128*)srcBufferB;
            __m128* vectorDstBuffer = (__m128*)dstBuffer;

            int vector_count = size >> 2;
            while (vector_count--)
            {
                *vectorDstBuffer++ =
                  _mm_add_ps(*vectorBufferA++, *vectorBufferB++);
            }

            // Handle any unaligned leftovers
            srcBufferA = (float*)vectorBufferA;
            srcBufferB = (float*)vectorBufferB;
            dstBuffer = (float*)vectorDstBuffer;

            switch (size & 3)
            {
            case 3: *dstBuffer++ = *srcBufferA++ + *srcBufferB++;
            case 2: *dstBuffer++ = *srcBufferA++ + *srcBufferB++;
            case 1: *dstBuffer++ = *srcBufferA++ + *srcBufferB++;
            }
        }
    }


    //--------------------------------------------------------------------------

    void subtract_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size) const
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)dstBuffer & 0x0F);

        if (size < MIN_SSE_SAMPLES ||
            (align_bytes != ((ptrdiff_t)srcBufferA & 0x0F) ||
             align_bytes != ((ptrdiff_t)srcBufferB & 0x0F)))
        {
            math_mmx::subtract_buffers(srcBufferA, srcBufferB, dstBuffer, size);
        }
        else
        {
            assert(size >= MIN_SSE_SIZE);

            // Copy unaligned head
            switch (align_bytes >> 2)
            {
            case 1: --size; *dstBuffer++ = *srcBufferA++ - *srcBufferB++;
            case 2: --size; *dstBuffer++ = *srcBufferA++ - *srcBufferB++;
            case 3: --size; *dstBuffer++ = *srcBufferA++ - *srcBufferB++;
            }

            // Scale with simd
            __m128* vectorBufferA = (__m128*)srcBufferA;
            __m128* vectorBufferB = (__m128*)srcBufferB;
            __m128* vectorDstBuffer = (__m128*)dstBuffer;

            int vector_count = size >> 2;
            while (vector_count--)
            {
                *vectorDstBuffer++ =
                  _mm_sub_ps(*vectorBufferA++, *vectorBufferB++);
            }

            // Handle any unaligned leftovers
            srcBufferA = (float*)vectorBufferA;
            srcBufferB = (float*)vectorBufferB;
            dstBuffer = (float*)vectorDstBuffer;

            switch (size & 3)
            {
            case 3: *dstBuffer++ = *srcBufferA++ - *srcBufferB++;
            case 2: *dstBuffer++ = *srcBufferA++ - *srcBufferB++;
            case 1: *dstBuffer++ = *srcBufferA++ - *srcBufferB++;
            }
        }
    }


    //--------------------------------------------------------------------------

    void multiply_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size) const
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)dstBuffer & 0x0F);

        if (size < MIN_SSE_SAMPLES ||
            (align_bytes != ((ptrdiff_t)srcBufferA & 0x0F) ||
             align_bytes != ((ptrdiff_t)srcBufferB & 0x0F)))
        {
            math_mmx::multiply_buffers(srcBufferA, srcBufferB, dstBuffer, size);
        }
        else
        {
            assert(size >= MIN_SSE_SIZE);

            // Copy unaligned head
            switch (align_bytes >> 2)
            {
            case 1:
                --size;
                *dstBuffer = *srcBufferA++ * *srcBufferB++;
                undernormalize(*dstBuffer);
                ++dstBuffer;
            case 2:
                --size;
                *dstBuffer = *srcBufferA++ * *srcBufferB++;
                undernormalize(*dstBuffer);
                ++dstBuffer;
            case 3:
                --size;
                *dstBuffer = *srcBufferA++ * *srcBufferB++;
                undernormalize(*dstBuffer);
                ++dstBuffer;
            }

            // Scale with simd
            const disable_sse_denormals disable_denormals;

            __m128* vectorBufferA = (__m128*)srcBufferA;
            __m128* vectorBufferB = (__m128*)srcBufferB;
            __m128* vectorDstBuffer = (__m128*)dstBuffer;

            int vector_count = size >> 2;
            while (vector_count--)
            {
                *vectorDstBuffer++ =
                  _mm_mul_ps(*vectorBufferA++, *vectorBufferB++);
            }

            // Handle any unaligned leftovers
            srcBufferA = (float*)vectorBufferA;
            srcBufferB = (float*)vectorBufferB;
            dstBuffer = (float*)vectorDstBuffer;

            switch (size & 3)
            {
            case 3:
                *dstBuffer = *srcBufferA++ * *srcBufferB++;
                undernormalize(*dstBuffer);
                ++dstBuffer;
            case 2:
                *dstBuffer = *srcBufferA++ * *srcBufferB++;
                undernormalize(*dstBuffer);
                ++dstBuffer;
            case 1:
                *dstBuffer = *srcBufferA++ * *srcBufferB++;
                undernormalize(*dstBuffer);
                ++dstBuffer;
            }
        }
    }


    //--------------------------------------------------------------------------

    void divide_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size) const
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)dstBuffer & 0x0F);

        if (size < MIN_SSE_SAMPLES ||
            (align_bytes != ((ptrdiff_t)srcBufferA & 0x0F) ||
             align_bytes != ((ptrdiff_t)srcBufferB & 0x0F)))
        {
            math_mmx::divide_buffers(srcBufferA, srcBufferB, dstBuffer, size);
        }
        else
        {
            assert(size >= MIN_SSE_SIZE);

            // Copy unaligned head
            switch (align_bytes >> 2)
            {
            case 1:
                --size;
                *dstBuffer = *srcBufferA++ / *srcBufferB++;
                undernormalize(*dstBuffer);
                ++dstBuffer;
            case 2:
                --size;
                *dstBuffer = *srcBufferA++ / *srcBufferB++;
                undernormalize(*dstBuffer);
                ++dstBuffer;
            case 3:
                --size;
                *dstBuffer = *srcBufferA++ / *srcBufferB++;
                undernormalize(*dstBuffer);
                ++dstBuffer;
            }

            // Scale with simd
            const disable_sse_denormals disable_denormals;

            __m128* vectorBufferA = (__m128*)srcBufferA;
            __m128* vectorBufferB = (__m128*)srcBufferB;
            __m128* vectorDstBuffer = (__m128*)dstBuffer;

            int vector_count = size >> 2;
            while (vector_count--)
            {
                *vectorDstBuffer++ =
                  _mm_div_ps(*vectorBufferA++, *vectorBufferB++);
            }

            // Handle any unaligned leftovers
            srcBufferA = (float*)vectorBufferA;
            srcBufferB = (float*)vectorBufferB;
            dstBuffer = (float*)vectorDstBuffer;

            switch (size & 3)
            {
            case 3:
                *dstBuffer = *srcBufferA++ / *srcBufferB++;
                undernormalize(*dstBuffer);
                ++dstBuffer;
            case 2:
                *dstBuffer = *srcBufferA++ / *srcBufferB++;
                undernormalize(*dstBuffer);
                ++dstBuffer;
            case 1:
                *dstBuffer = *srcBufferA++ / *srcBufferB++;
                undernormalize(*dstBuffer);
                ++dstBuffer;
            }
        }
    }

};


#endif
