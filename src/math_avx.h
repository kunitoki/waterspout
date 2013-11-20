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

#ifndef __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_AVX_H__
#define __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_AVX_H__

#include <immintrin.h> // AVX


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Specific AVX math class elaborating on __m256 buffers
 */

class math_avx : public math_sse42
{
public:

    //--------------------------------------------------------------------------

    const char* name() const { return "AVX"; }


    //--------------------------------------------------------------------------

    enum AVXMathDefines
    {
        MIN_AVX_SIZE    = 8,
        MIN_AVX_SAMPLES = 32
    };


    //--------------------------------------------------------------------------

    math_avx()
    {
        //assertfalse; // not implemented !
    }

    
    //--------------------------------------------------------------------------

    void copy_buffer(
        float* src_buffer,
        float* dst_buffer,
        uint32_t size) const
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)src_buffer & 0x1F);

        if (size < MIN_AVX_SAMPLES ||
              ((ptrdiff_t)dst_buffer & 0x1F) != align_bytes)
        {
            math_sse42::copy_buffer(src_buffer, dst_buffer, size);
        } 
        else 
        { 
            assert(size >= MIN_AVX_SIZE);

            // Copy unaligned head
            switch (align_bytes >> 3)
            {
            case 1: --size; *dst_buffer++ = *src_buffer++;
            case 2: --size; *dst_buffer++ = *src_buffer++;
            case 3: --size; *dst_buffer++ = *src_buffer++;
            case 4: --size; *dst_buffer++ = *src_buffer++;
            case 5: --size; *dst_buffer++ = *src_buffer++;
            case 6: --size; *dst_buffer++ = *src_buffer++;
            case 7: --size; *dst_buffer++ = *src_buffer++;
            }  

            // Copy with simd
            __m256* source_vector = (__m256*)src_buffer;
            __m256* dest_vector = (__m256*)dst_buffer;

            int vector_count = size >> 3;
            while (vector_count--)
            {
                *dest_vector = *source_vector;

                ++dest_vector;
                ++source_vector;
            }

            // Handle unaligned leftovers
            src_buffer = (float*)source_vector;
            dst_buffer = (float*)dest_vector;

            switch (size & 7)
            {
            case 7: *dst_buffer++ = *src_buffer++;
            case 6: *dst_buffer++ = *src_buffer++;
            case 5: *dst_buffer++ = *src_buffer++;
            case 4: *dst_buffer++ = *src_buffer++;
            case 3: *dst_buffer++ = *src_buffer++;
            case 2: *dst_buffer++ = *src_buffer++;
            case 1: *dst_buffer++ = *src_buffer++;
            }  
        }
    }

};


#endif
