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

#ifndef __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_MMX_H__
#define __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_MATH_MMX_H__


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Specific MMX math class elaborating on __m64 buffers
 */

class math_mmx : public math_fpu
{
public:

    //--------------------------------------------------------------------------

    const char* name() const { return "MMX"; }


    //--------------------------------------------------------------------------

    enum SSEMathDefines
    {
        MMX_MIN_SIZE    = 2,
        MMX_MIN_SAMPLES = 16,
        MMX_ALIGN       = 0x0F
    };


    //--------------------------------------------------------------------------

    math_mmx()
    {
        //assertfalse; // not implemented !
    }


    //--------------------------------------------------------------------------

    void clear_buffer_int32(
        int32* src_buffer,
        uint32 size) const
    {
        if (size < MMX_MIN_SAMPLES)
        {
            math_fpu::clear_buffer_int32(src_buffer, size);
        }
        else
        {
            assert(size >= MMX_MIN_SIZE);

            const ptrdiff_t align_bytes = ((ptrdiff_t)src_buffer & MMX_ALIGN);

            // Copy unaligned head
            simd_unroll_head_2(
                --size;
                *src_buffer++ = int32(0);
            );

            // Clear with simd
            const __m64 vvalue = _mm_set_pi32(0, 0);
            __m64* vector_buffer = (__m64*)src_buffer;

            uint32 vector_count = size >> 1;
            while (vector_count--)
            {
                *vector_buffer = vvalue;

                ++vector_buffer;
            }

            // Handle any unaligned leftovers
            src_buffer = (int32*)vector_buffer;

            simd_unroll_tail_2(
                *src_buffer++ = int32(0);
            );

            // Reset floating point state
            _mm_empty();
        }
    }


    //--------------------------------------------------------------------------

    void set_buffer_int32(
        int32* src_buffer,
        uint32 size,
        int32 value) const
    {
        if (size < MMX_MIN_SAMPLES)
        {
            math_fpu::set_buffer_int32(src_buffer, size, value);
        }
        else
        {
            assert(size >= MMX_MIN_SIZE);

            const ptrdiff_t align_bytes = ((ptrdiff_t)src_buffer & MMX_ALIGN);

            // Copy unaligned head
            simd_unroll_head_2(
                --size;
                *src_buffer++ = value;
            );

            // Clear with simd
            const __m64 vvalue = _mm_set_pi32(value, value);
            __m64* vector_buffer = (__m64*)src_buffer;

            uint32 vector_count = size >> 1;
            while (vector_count--)
            {
                *vector_buffer = vvalue;

                ++vector_buffer;
            }

            // Handle any unaligned leftovers
            src_buffer = (int32*)vector_buffer;

            simd_unroll_tail_2(
                *src_buffer++ = value;
            );

            // Reset floating point state
            _mm_empty();
        }
    }


    //--------------------------------------------------------------------------

    void copy_buffer_int32(
        int32* src_buffer,
        int32* dst_buffer,
        uint32 size) const
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)src_buffer & MMX_ALIGN);

        if (size < MMX_MIN_SAMPLES ||
              ((ptrdiff_t)dst_buffer & MMX_ALIGN) != align_bytes)
        {
            math_fpu::copy_buffer_int32(src_buffer, dst_buffer, size);
        }
        else
        {
            assert(size >= MMX_MIN_SIZE);

            // Copy unaligned head
            simd_unroll_head_2(
                --size;
                *dst_buffer++ = *src_buffer++;
            );

            // Copy with simd
            __m64* source_vector = (__m64*)src_buffer;
            __m64* dest_vector = (__m64*)dst_buffer;

            uint32 vector_count = size >> 1;
            while (vector_count--)
            {
                *dest_vector = *source_vector;

                ++dest_vector;
                ++source_vector;
            }

            // Handle unaligned leftovers
            src_buffer = (int32*)source_vector;
            dst_buffer = (int32*)dest_vector;

            simd_unroll_tail_2(
                *dst_buffer++ = *src_buffer++;
            );

            // Reset floating point state
            _mm_empty();
        }
    }


    //--------------------------------------------------------------------------

    void add_buffers_int32(
        int32* src_buffer_a,
        int32* src_buffer_b,
        int32* dst_buffer,
        uint32 size) const
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)dst_buffer & MMX_ALIGN);

        if (size < MMX_MIN_SAMPLES ||
            (align_bytes != ((ptrdiff_t)src_buffer_a & MMX_ALIGN) ||
             align_bytes != ((ptrdiff_t)src_buffer_b & MMX_ALIGN)))
        {
            math_fpu::add_buffers_int32(src_buffer_a, src_buffer_b, dst_buffer, size);
        }
        else
        {
            assert(size >= MMX_MIN_SIZE);

            // Copy unaligned head
            simd_unroll_head_2(
                --size;
                *dst_buffer++ = *src_buffer_a++ + *src_buffer_b++;
            );

            // Scale with simd
            __m64* vector_buffer_a = (__m64*)src_buffer_a;
            __m64* vector_buffer_b = (__m64*)src_buffer_b;
            __m64* vector_dst_buffer = (__m64*)dst_buffer;

            uint32 vector_count = size >> 1;
            while (vector_count--)
            {
                *vector_dst_buffer =
                  _mm_add_pi32(*vector_buffer_a, *vector_buffer_b);

                ++vector_buffer_a;
                ++vector_buffer_b;
                ++vector_dst_buffer;
            }

            // Handle any unaligned leftovers
            src_buffer_a = (int32*)vector_buffer_a;
            src_buffer_b = (int32*)vector_buffer_b;
            dst_buffer = (int32*)vector_dst_buffer;

            simd_unroll_tail_2(
                *dst_buffer++ = *src_buffer_a++ + *src_buffer_b++;
            );

            // Reset floating point state
            _mm_empty();
        }
    }


    //--------------------------------------------------------------------------

    void subtract_buffers_int32(
        int32* src_buffer_a,
        int32* src_buffer_b,
        int32* dst_buffer,
        uint32 size) const
    {
        const ptrdiff_t align_bytes = ((ptrdiff_t)dst_buffer & MMX_ALIGN);

        if (size < MMX_MIN_SAMPLES ||
            (align_bytes != ((ptrdiff_t)src_buffer_a & MMX_ALIGN) ||
             align_bytes != ((ptrdiff_t)src_buffer_b & MMX_ALIGN)))
        {
            math_fpu::subtract_buffers_int32(src_buffer_a, src_buffer_b, dst_buffer, size);
        }
        else
        {
            assert(size >= MMX_MIN_SIZE);

            // Copy unaligned head
            simd_unroll_head_2(
                --size;
                *dst_buffer++ = *src_buffer_a++ - *src_buffer_b++;
            );

            // Scale with simd
            __m64* vector_buffer_a = (__m64*)src_buffer_a;
            __m64* vector_buffer_b = (__m64*)src_buffer_b;
            __m64* vector_dst_buffer = (__m64*)dst_buffer;

            uint32 vector_count = size >> 1;
            while (vector_count--)
            {
                *vector_dst_buffer =
                  _mm_sub_pi32(*vector_buffer_a, *vector_buffer_b);

                ++vector_buffer_a;
                ++vector_buffer_b;
                ++vector_dst_buffer;
            }

            // Handle any unaligned leftovers
            src_buffer_a = (int32*)vector_buffer_a;
            src_buffer_b = (int32*)vector_buffer_b;
            dst_buffer = (int32*)vector_dst_buffer;

            simd_unroll_tail_2(
                *dst_buffer++ = *src_buffer_a++ - *src_buffer_b++;
            );

            // Reset floating point state
            _mm_empty();
        }
    }

};


#endif
