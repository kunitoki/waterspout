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

struct disable_fpu_denormals
{
    disable_fpu_denormals()
    {
        disable_floating_point_assertions;
    }

    ~disable_fpu_denormals()
    {
        enable_floating_point_assertions;
    }
};


//==============================================================================

//------------------------------------------------------------------------------

/**
 * FPU math clas that provides floating point operations as fallback
 */

class math_fpu : public math_interface_
{
public:

    //==========================================================================

    //--------------------------------------------------------------------------

    const char* name() const { return "FPU"; }


    //--------------------------------------------------------------------------

    math_fpu()
    {
        //assertfalse; // not implemented !
    }


    //==========================================================================

    //--------------------------------------------------------------------------

    void clear_buffer_int32(
        int32* src_buffer,
        uint32 size) const
    {
        clear_buffer_generic(src_buffer, size);
    }


    //--------------------------------------------------------------------------

    void set_buffer_int32(
        int32* src_buffer,
        uint32 size,
        int32 value) const
    {
        set_buffer_generic(src_buffer, size, value);
    }


    //--------------------------------------------------------------------------

    void scale_buffer_int32(
        int32* src_buffer,
        uint32 size,
        float gain) const
    {
        const disable_fpu_denormals disable_denormals;

        for (uint32 i = 0; i < size; ++i)
        {
          *src_buffer = int32(*src_buffer * gain);
          ++src_buffer;
        }
    }


    //--------------------------------------------------------------------------

    void copy_buffer_int32(
        int32* src_buffer,
        int32* dst_buffer,
        uint32 size) const
    {
        copy_buffer_generic(src_buffer, dst_buffer, size);
    }


    //--------------------------------------------------------------------------

    void add_buffers_int32(
        int32* src_buffer_a,
        int32* src_buffer_b,
        int32* dst_buffer,
        uint32 size) const
    {
        add_buffers_generic(src_buffer_a, src_buffer_b, dst_buffer, size);
    }


    //--------------------------------------------------------------------------

    void subtract_buffers_int32(
        int32* src_buffer_a,
        int32* src_buffer_b,
        int32* dst_buffer,
        uint32 size) const
    {
        subtract_buffers_generic(src_buffer_a, src_buffer_b, dst_buffer, size);
    }


    //--------------------------------------------------------------------------

    void multiply_buffers_int32(
        int32* src_buffer_a,
        int32* src_buffer_b,
        int32* dst_buffer,
        uint32 size) const
    {
        for (uint32 i = 0; i < size; ++i)
        {
          *dst_buffer++ = *src_buffer_a++ * *src_buffer_b++;
        }
    }


    //--------------------------------------------------------------------------

    void divide_buffers_int32(
        int32* src_buffer_a,
        int32* src_buffer_b,
        int32* dst_buffer,
        uint32 size) const
    {
        for (uint32 i = 0; i < size; ++i)
        {
          *dst_buffer++ = *src_buffer_a++ / *src_buffer_b++;
        }
    }


    //==========================================================================

    //--------------------------------------------------------------------------

    void clear_buffer_float(
        float* src_buffer,
        uint32 size) const
    {
        clear_buffer_generic(src_buffer, size);
    }


    //--------------------------------------------------------------------------

    void set_buffer_float(
        float* src_buffer,
        uint32 size,
        float value) const
    {
        set_buffer_generic(src_buffer, size, value);
    }


    //--------------------------------------------------------------------------

    void scale_buffer_float(
        float* src_buffer,
        uint32 size,
        float gain) const
    {
        const disable_fpu_denormals disable_denormals;

        for (uint32 i = 0; i < size; ++i)
        {
          *src_buffer = *src_buffer * gain;
          undenormalizef(*src_buffer);
          ++src_buffer;
        }
    }


    //--------------------------------------------------------------------------

    void copy_buffer_float(
        float* src_buffer,
        float* dst_buffer,
        uint32 size) const
    {
        for (uint32 i = 0; i < size; ++i)
        {
          *dst_buffer++ = *src_buffer++;
        }
    }


    //--------------------------------------------------------------------------

    void add_buffers_float(
        float* src_buffer_a,
        float* src_buffer_b,
        float* dst_buffer,
        uint32 size) const
    {
        for (uint32 i = 0; i < size; ++i)
        {
          *dst_buffer++ = *src_buffer_a++ + *src_buffer_b++;
        }
    }


    //--------------------------------------------------------------------------

    void subtract_buffers_float(
        float* src_buffer_a,
        float* src_buffer_b,
        float* dst_buffer,
        uint32 size) const
    {
        for (uint32 i = 0; i < size; ++i)
        {
          *dst_buffer++ = *src_buffer_a++ - *src_buffer_b++;
        }
    }


    //--------------------------------------------------------------------------

    void multiply_buffers_float(
        float* src_buffer_a,
        float* src_buffer_b,
        float* dst_buffer,
        uint32 size) const
    {
        const disable_fpu_denormals disable_denormals;

        for (uint32 i = 0; i < size; ++i)
        {
          *dst_buffer = *src_buffer_a++ * *src_buffer_b++;
          undenormalizef(*dst_buffer);
          ++dst_buffer;
        }
    }


    //--------------------------------------------------------------------------

    void divide_buffers_float(
        float* src_buffer_a,
        float* src_buffer_b,
        float* dst_buffer,
        uint32 size) const
    {
        const disable_fpu_denormals disable_denormals;

        for (uint32 i = 0; i < size; ++i)
        {
          *dst_buffer = *src_buffer_a++ / *src_buffer_b++;
          undenormalizef(*dst_buffer);
          ++dst_buffer;
        }
    }


private:

    //--------------------------------------------------------------------------

    template<typename T> void clear_buffer_generic(
        T* src_buffer,
        uint32 size) const
    {
        for (uint32 i = 0; i < size; ++i)
        {
          *src_buffer++ = static_cast<T>(0);
        }
    }

    //--------------------------------------------------------------------------

    template<typename T> void set_buffer_generic(
        T* src_buffer,
        uint32 size,
        T value) const
    {
        for (uint32 i = 0; i < size; ++i)
        {
          *src_buffer++ = value;
        }
    }


    //--------------------------------------------------------------------------

    template<typename T> void copy_buffer_generic(
        T* src_buffer,
        T* dst_buffer,
        uint32 size) const
    {
        for (uint32 i = 0; i < size; ++i)
        {
          *dst_buffer++ = *src_buffer++;
        }
    }


    //--------------------------------------------------------------------------

    template<typename T> void add_buffers_generic(
        T* src_buffer_a,
        T* src_buffer_b,
        T* dst_buffer,
        uint32 size) const
    {
        for (uint32 i = 0; i < size; ++i)
        {
          *dst_buffer++ = *src_buffer_a++ + *src_buffer_b++;
        }
    }


    //--------------------------------------------------------------------------

    template<typename T> void subtract_buffers_generic(
        T* src_buffer_a,
        T* src_buffer_b,
        T* dst_buffer,
        uint32 size) const
    {
        for (uint32 i = 0; i < size; ++i)
        {
          *dst_buffer++ = *src_buffer_a++ - *src_buffer_b++;
        }
    }

};


#endif
