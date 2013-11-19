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
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
    math_sse()
    {
        //assertfalse; // not implemented !
    }

    const char* name() { return "SSE"; }

    void copy_buffer(float* srcBuffer, float* dstBuffer, uint32_t size)
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)srcBuffer & 0x0F);

        if (size < WATERSPOUT_MIN_SSE_SAMPLES ||
              ((ptrdiff_t)dstBuffer & 0x0F) != align_bytes)
        {
            for (int i = 0; i < size; ++i)
            {
              dstBuffer[i] = srcBuffer[i];
            }
        } 
        else 
        { 
            assert(size >= 4);

            // Copy unaligned head
            switch (align_bytes >> 2)
            {
            case 1:
                --size;
                *dstBuffer++ = *srcBuffer++;

            case 2:
                --size;
                *dstBuffer++ = *srcBuffer++;

            case 3:
                --size;
                *dstBuffer++ = *srcBuffer++;
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
            case 3:
                *dstBuffer++ = *srcBuffer++;

            case 2:
                *dstBuffer++ = *srcBuffer++;

            case 1:
                *dstBuffer++ = *srcBuffer++;
            }  
        }
    }

    void scale_buffer(float* srcBuffer, uint32_t size, float gain)
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)srcBuffer & 0x0F);

        if (size < WATERSPOUT_MIN_SSE_SAMPLES)
        {
            for (int i = 0; i < size; ++i)
            {
                srcBuffer[i] *= gain;
                undernormalize(srcBuffer[i]);
            }
        } 
        else
        {
            assert(size >= 4);
      
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

};


#endif
