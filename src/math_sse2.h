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

#ifndef __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_SSE2_H__
#define __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_SSE2_H__


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Specific SSE2 math class elaborating on __m128d buffers
 */

class math_sse2 : public math_sse
{
public:

    //--------------------------------------------------------------------------

    const char* name() const { return "SSE2"; }


    //--------------------------------------------------------------------------

    enum SSEMathDefines
    {
        SSE2_MIN_SIZE    = 4,
        SSE2_MIN_SAMPLES = 32,
        SSE2_ALIGN       = 0x0F
    };

    //--------------------------------------------------------------------------

    math_sse2()
    {
        //assertfalse; // not implemented !
    }


    //--------------------------------------------------------------------------

    void clear_buffer_int32(
        int32* src_buffer,
        uint32 size) const
    {
        if (size < SSE2_MIN_SAMPLES)
        {
            math_sse::clear_buffer_int32(src_buffer, size);
        }
        else
        {
            assert(size >= SSE2_MIN_SIZE);

            const ptrdiff_t align_bytes =
                ((ptrdiff_t)src_buffer & SSE2_ALIGN);

            // Copy unaligned head
            simd_unroll_head_4(
                --size;
                *src_buffer++ = 0;
            );

            // Clear with simd
            const __m128i vvalue = _mm_setzero_si128();
            __m128i* vector_buffer = (__m128i*)src_buffer;

            uint32 vector_count = size >> 2;
            while (vector_count--)
            {
                *vector_buffer = vvalue;

                ++vector_buffer;
            }

            // Handle any unaligned leftovers
            src_buffer = (int32*)vector_buffer;

            simd_unroll_tail_4(
                *src_buffer++ = 0;
            );
        }
    }


    //--------------------------------------------------------------------------

    void set_buffer_int32(
        int32* src_buffer,
        uint32 size,
        int32 value) const
    {
        if (size < SSE2_MIN_SAMPLES)
        {
            math_sse::set_buffer_int32(src_buffer, size, value);
        }
        else
        {
            assert(size >= SSE2_MIN_SIZE);

            const ptrdiff_t align_bytes =
                ((ptrdiff_t)src_buffer & SSE2_ALIGN);

            // Copy unaligned head
            simd_unroll_head_4(
                --size;
                *src_buffer++ = value;
            );

            // Clear with simd
            const __m128i vvalue = _mm_set1_epi32(value);
            __m128i* vector_buffer = (__m128i*)src_buffer;

            uint32 vector_count = size >> 2;
            while (vector_count--)
            {
                *vector_buffer = vvalue;

                ++vector_buffer;
            }

            // Handle any unaligned leftovers
            src_buffer = (int32*)vector_buffer;

            simd_unroll_tail_4(
                *src_buffer++ = value;
            );
        }
    }


    //--------------------------------------------------------------------------

    void copy_buffer_int32(
        int32* src_buffer,
        int32* dst_buffer,
        uint32 size) const
    {
        const ptrdiff_t align_bytes =
            ((ptrdiff_t)src_buffer & SSE2_ALIGN);

        if (size < SSE2_MIN_SAMPLES ||
            ((ptrdiff_t)dst_buffer & SSE2_ALIGN) != align_bytes)
        {
            math_sse::copy_buffer_int32(src_buffer, dst_buffer, size);
        }
        else
        {
            assert(size >= SSE2_MIN_SIZE);

            // Copy unaligned head
            simd_unroll_head_4(
                --size;
                *dst_buffer++ = *src_buffer++;
            );

            // Scale with simd
            __m128i* source_vector = (__m128i*)src_buffer;
            __m128i* dest_vector = (__m128i*)dst_buffer;

            uint32 vector_count = size >> 2;
            while (vector_count--)
            {
                *dest_vector = *source_vector;

                ++dest_vector;
                ++source_vector;
            }

            // Handle any unaligned leftovers
            src_buffer = (int32*)source_vector;
            dst_buffer = (int32*)dest_vector;

            simd_unroll_tail_4(
                *dst_buffer++ = *src_buffer++;
            );
        }
    }


    //--------------------------------------------------------------------------

    void add_buffers_int32(
        int32* src_buffer_a,
        int32* src_buffer_b,
        int32* dst_buffer,
        uint32 size) const
    {
        const ptrdiff_t align_bytes =
            ((ptrdiff_t)dst_buffer & SSE2_ALIGN);

        if (size < SSE2_MIN_SAMPLES ||
            (align_bytes != ((ptrdiff_t)src_buffer_a & SSE2_ALIGN) ||
             align_bytes != ((ptrdiff_t)src_buffer_b & SSE2_ALIGN)))
        {
            math_sse::add_buffers_int32(src_buffer_a, src_buffer_b, dst_buffer, size);
        }
        else
        {
            assert(size >= SSE2_MIN_SIZE);

            // Copy unaligned head
            simd_unroll_head_4(
                --size;
                *dst_buffer++ = *src_buffer_a++ + *src_buffer_b++;
            );

            // Scale with simd
            __m128i* vector_buffer_a = (__m128i*)src_buffer_a;
            __m128i* vector_buffer_b = (__m128i*)src_buffer_b;
            __m128i* vector_dst_buffer = (__m128i*)dst_buffer;

            uint32 vector_count = size >> 2;
            while (vector_count--)
            {
                *vector_dst_buffer =
                  _mm_add_epi32(*vector_buffer_a, *vector_buffer_b);

                ++vector_buffer_a;
                ++vector_buffer_b;
                ++vector_dst_buffer;
            }

            // Handle any unaligned leftovers
            src_buffer_a = (int32*)vector_buffer_a;
            src_buffer_b = (int32*)vector_buffer_b;
            dst_buffer = (int32*)vector_dst_buffer;

            simd_unroll_tail_4(
                *dst_buffer++ = *src_buffer_a++ + *src_buffer_b++;
            );
        }
    }


    //--------------------------------------------------------------------------

    void subtract_buffers_int32(
        int32* src_buffer_a,
        int32* src_buffer_b,
        int32* dst_buffer,
        uint32 size) const
    {
        const ptrdiff_t align_bytes =
            ((ptrdiff_t)dst_buffer & SSE2_ALIGN);

        if (size < SSE2_MIN_SAMPLES ||
            (align_bytes != ((ptrdiff_t)src_buffer_a & SSE2_ALIGN) ||
             align_bytes != ((ptrdiff_t)src_buffer_b & SSE2_ALIGN)))
        {
            math_sse::subtract_buffers_int32(src_buffer_a, src_buffer_b, dst_buffer, size);
        }
        else
        {
            assert(size >= SSE2_MIN_SIZE);

            // Copy unaligned head
            simd_unroll_head_4(
                --size;
                *dst_buffer++ = *src_buffer_a++ - *src_buffer_b++;
            );

            // Scale with simd
            __m128i* vector_buffer_a = (__m128i*)src_buffer_a;
            __m128i* vector_buffer_b = (__m128i*)src_buffer_b;
            __m128i* vector_dst_buffer = (__m128i*)dst_buffer;

            uint32 vector_count = size >> 2;
            while (vector_count--)
            {
                *vector_dst_buffer =
                  _mm_sub_epi32(*vector_buffer_a, *vector_buffer_b);

                ++vector_buffer_a;
                ++vector_buffer_b;
                ++vector_dst_buffer;
            }

            // Handle any unaligned leftovers
            src_buffer_a = (int32*)vector_buffer_a;
            src_buffer_b = (int32*)vector_buffer_b;
            dst_buffer = (int32*)vector_dst_buffer;

            simd_unroll_tail_4(
                *dst_buffer++ = *src_buffer_a++ - *src_buffer_b++;
            );
        }
    }


};


#endif
