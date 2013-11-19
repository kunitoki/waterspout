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

#include <waterspout.h>


namespace waterspout {


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Reference:
 * http://datasheets.chipdb.org/Intel/x86/CPUID/24161821.pdf
 * http://www.flounder.com/cpuid_explorer2.htm
 */
enum CpuidFeatures
{
  FPU   = 1<< 0, // Floating-Point Unit on-chip
  MMX   = 1<<23, // MultiMedia eXtension
  SSE   = 1<<25, // Streaming SIMD Extension 1
  SSE2  = 1<<26, // Streaming SIMD Extension 2
};


/**
 * Reference:
 * http://datasheets.chipdb.org/Intel/x86/CPUID/24161821.pdf
 * http://www.flounder.com/cpuid_explorer2.htm
 */
enum CpuidExtendedFeatures
{
  SSE3  = 1<< 0, // Streaming SIMD Extension 3
  SSSE3 = 1<< 9, // SSSE3
  SSE41 = 1<<19, // SSE41
  SSE42 = 1<<20, // SSE42
  AVX   = 1<<28, // AVX
};


//------------------------------------------------------------------------------

/**
 * Calls cpuid with op and store results of eax,ebx,ecx,edx
 * \param op cpuid function (eax input)
 * \param eax content of eax after the call to cpuid
 * \param ebx content of ebx after the call to cpuid
 * \param ecx content of ecx after the call to cpuid
 * \param edx content of edx after the call to cpuid
 */

void cpuid(uint32_t op, uint32_t& eax, uint32_t& ebx, uint32_t& ecx, uint32_t& edx)
{
#if defined(WATERSPOUT_COMPILER_GCC) || defined(WATERSPOUT_COMPILER_MINGW)
    // GCC/MINGW provides a __get_cpuid function
    __get_cpuid(op, &eax, &ebx, &ecx, &edx);

#elif defined(WATERSPOUT_COMPILER_MSVC)
    // MSVC provides a __cpuid function
    int regs[4];
    __cpuid(regs, op);
    eax = (uint32_t)regs[0];
    ebx = (uint32_t)regs[1];
    ecx = (uint32_t)regs[2];
    edx = (uint32_t)regs[3];

#endif
}


//------------------------------------------------------------------------------

/**
 * This will retrieve the CPU features available
 * \return The content of the edx register containing available features
 */

uint32_t cpuid_features()
{
  uint32_t eax, ebx, ecx, edx;
  cpuid(1, eax, ebx, ecx, edx);
  return edx;
}


//------------------------------------------------------------------------------

/**
 * This will retrieve the extended CPU features available
 * \return The content of the ecx register containing available extended features
 */

uint32_t cpuid_extended_features()
{
  uint32_t eax, ebx, ecx, edx;
  cpuid(1, eax, ebx, ecx, edx);
  return ecx;
}


//------------------------------------------------------------------------------

/**
 * Retrieve the processor name.
 * \param name Preallocated string containing at least room for 13 characters. Will
 *             contain the name of the processor.
 */

void cpuid_procname(char* name)
{
  name[12] = 0;
  uint32_t max_op;
  cpuid(0, max_op, (uint32_t&)name[0], (uint32_t&)name[8], (uint32_t&)name[4]);
}


//==============================================================================

//------------------------------------------------------------------------------

enum CpuEndianess
{
  ENDIAN_UNKNOWN,
  ENDIAN_BIG,
  ENDIAN_LITTLE,
  ENDIAN_BIG_WORD,   // Middle-endian, Honeywell 316 style
  ENDIAN_LITTLE_WORD // Middle-endian, PDP-11 style
};


//------------------------------------------------------------------------------

int endianness()
{
  uint32_t value;
  uint8_t* buffer = (uint8_t*)&value;

  buffer[0] = 0x00;
  buffer[1] = 0x01;
  buffer[2] = 0x02;
  buffer[3] = 0x03;

  switch (value)
  {
  case uint32_t(0x00010203): return ENDIAN_BIG;
  case uint32_t(0x03020100): return ENDIAN_LITTLE;
  case uint32_t(0x02030001): return ENDIAN_BIG_WORD;
  case uint32_t(0x01000302): return ENDIAN_LITTLE_WORD;
  default:                   return ENDIAN_UNKNOWN;
  }
}


//==============================================================================

//------------------------------------------------------------------------------

struct disable_sse_denormals
{
  disable_sse_denormals()
  {
    disable_floating_point_assertions

    _old_mxcsr = _mm_getcsr();
  
    static const uint32_t caps = cpuid_features();

    if (caps & SSE2)
    {
      if ((_old_mxcsr & 0x8040) == 0) // set DAZ and FZ bits...
      {
        _mm_setcsr(_old_mxcsr | 0x8040);
      }
    }
    else
    {
      assert(caps & SSE); // Expected at least sse 1

      if ((_old_mxcsr & 0x8000) == 0) // set DAZ bit...
      {
        _mm_setcsr(_old_mxcsr | 0x8000);
      }
    }
  }
  
  ~disable_sse_denormals()
  {
    if (_old_mxcsr != 0)
    {
      _mm_setcsr(_old_mxcsr);
    }

    enable_floating_point_assertions
  }

private:
  int _old_mxcsr;
};


//==============================================================================

//------------------------------------------------------------------------------

/**
 * FPU math clas that provides floating point operations as fallback
 */

class math_fpu : public math
{
public:

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

    void scale_buffer(
        float* srcBuffer,
        uint32_t size,
        float gain)
    {
        for (int i = 0; i < size; ++i)
        {
          *srcBuffer++ *= gain;
        }
    }

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

    void multiply_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size)
    {
        for (int i = 0; i < size; ++i)
        {
          *dstBuffer++ = *srcBufferA++ * *srcBufferB++;
        }
    }

    void divide_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size)
    {
        for (int i = 0; i < size; ++i)
        {
          *dstBuffer++ = *srcBufferA++ / *srcBufferB++;
        }
    }
};


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Specific MMX math class elaborating on __m64 buffers
 */

#if defined(WATERSPOUT_SIMD_MMX)

class math_mmx : public math_fpu
{
public:
    math_mmx()
    {
        assertfalse; // not implemented !
    }
};

#endif


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Specific SSE/SSE2 math class elaborating on __m128 buffers
 */

#if defined(WATERSPOUT_SIMD_SSE) && defined(WATERSPOUT_SIMD_SSE2)

class math_sse2 : public math_fpu
{
public:

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


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Specific SSE3 math class
 */

#if defined(WATERSPOUT_SIMD_SSE3)

class math_sse3 : public math_fpu
{
public:
    math_sse3()
    {
        assertfalse; // not implemented !
    }
};

#endif


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Specific SSSE3 math class
 */

#if defined(WATERSPOUT_SIMD_SSSE3)

class math_ssse3 : public math_fpu
{
public:
    math_ssse3()
    {
        assertfalse; // not implemented !
    }
};

#endif


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Specific SSE41 math class
 */

#if defined(WATERSPOUT_SIMD_SSE41)

class math_sse41 : public math_fpu
{
public:
    math_sse41()
    {
        assertfalse; // not implemented !
    }
};

#endif


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Specific SSE42 math class
 */

#if defined(WATERSPOUT_SIMD_SSE42)

class math_sse42 : public math_fpu
{
public:
    math_sse42()
    {
        assertfalse; // not implemented !
    }
};

#endif


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Specific AVX math class
 */

#if defined(WATERSPOUT_SIMD_AVX)

class math_avx : public math_fpu
{
public:

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
            assert(size >= 8);

            // Copy unaligned head
            switch (align_bytes >> 3)
            {
            case 1: --size; *dstBuffer++ = *srcBuffer++;
            case 2: --size; *dstBuffer++ = *srcBuffer++;
            case 3: --size; *dstBuffer++ = *srcBuffer++;
            case 4: --size; *dstBuffer++ = *srcBuffer++;
            case 5: --size; *dstBuffer++ = *srcBuffer++;
            case 6: --size; *dstBuffer++ = *srcBuffer++;
            case 7: --size; *dstBuffer++ = *srcBuffer++;
            }  

            // Copy with simd
            __m256* sourceVector = (__m256*)srcBuffer;
            __m256* destVector = (__m256*)dstBuffer;

            int vector_count = size >> 3;
            while (vector_count--)
            {
                *destVector = *sourceVector;

                ++destVector;
                ++sourceVector;
            }

            // Handle unaligned leftovers
            dstBuffer = (float*)destVector;
            srcBuffer = (float*)sourceVector;

            switch (size & 7)
            {
            case 7: *dstBuffer++ = *srcBuffer++;
            case 6: *dstBuffer++ = *srcBuffer++;
            case 5: *dstBuffer++ = *srcBuffer++;
            case 4: *dstBuffer++ = *srcBuffer++;
            case 3: *dstBuffer++ = *srcBuffer++;
            case 2: *dstBuffer++ = *srcBuffer++;
            case 1: *dstBuffer++ = *srcBuffer++;
            }  
        }
    }

};

#endif


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Specific NEON math class
 */

#if defined(WATERSPOUT_SIMD_NEON)

class math_neon : public math_fpu
{
public:
    math_neon()
    {
        assertfalse; // not implemented !
    }
};

#endif


//==============================================================================

//------------------------------------------------------------------------------

math_factory::math_factory(int flags)
  : _math(NULL)
{
#if defined(WATERSPOUT_DEBUG)
    char procname[13];
    cpuid_procname(procname);
    std::cout << "Processor name: " << procname << std::endl;

    std::cout << "Processor endianess: " << (endianness() == ENDIAN_BIG ? "bigendian" : "littlendian") << std::endl;

    std::cout << "Processor features:" << std::endl;
    std::cout << "  FPU   = " << std::boolalpha << (bool)(cpuid_features() & FPU  ) << std::endl;
    std::cout << "  MMX   = " << std::boolalpha << (bool)(cpuid_features() & MMX  ) << std::endl;
    std::cout << "  SSE   = " << std::boolalpha << (bool)(cpuid_features() & SSE  ) << std::endl;
    std::cout << "  SSE2  = " << std::boolalpha << (bool)(cpuid_features() & SSE2 ) << std::endl;
    std::cout << "  SSE3  = " << std::boolalpha << (bool)(cpuid_extended_features() & SSE3) << std::endl;
    std::cout << "  SSSE3 = " << std::boolalpha << (bool)(cpuid_extended_features() & SSSE3 ) << std::endl;
    std::cout << "  SSE41 = " << std::boolalpha << (bool)(cpuid_extended_features() & SSE41 ) << std::endl;
    std::cout << "  SSE42 = " << std::boolalpha << (bool)(cpuid_extended_features() & SSE42 ) << std::endl;
    std::cout << "  AVX   = " << std::boolalpha << (bool)(cpuid_extended_features() & AVX ) << std::endl;
#endif

#if defined(WATERSPOUT_SYSTEM_ANDROID)
    if ((android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM &&
         (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0) || flags == FORCE_NEON)
    {
        #if defined(WATERSPOUT_DEBUG)
            std::cout << "Enabled NEON" << std::endl;
        #endif

        _math = new math_neon;

        return;
    }
#endif

    uint32_t features = cpuid_features();
    uint32_t features_ext = cpuid_extended_features();

    if (0)
    {
        // placeholder do nothing !
    }
    
#if defined(WATERSPOUT_SIMD_AVX)
    else if ((features_ext & AVX)
        && flags != FORCE_SSE42
        && flags != FORCE_SSE41
        && flags != FORCE_SSSE3
        && flags != FORCE_SSE3
        && flags != FORCE_SSE2
        && flags != FORCE_MMX
        && flags != FORCE_FPU)
    {
        #if defined(WATERSPOUT_DEBUG)
            std::cout << "Enabled AVX" << std::endl;
        #endif

        _math = new math_avx;
    }
#endif

#if defined(WATERSPOUT_SIMD_SSE42)
    else if ((features_ext & SSE42)
        && flags != FORCE_SSE41
        && flags != FORCE_SSSE3
        && flags != FORCE_SSE3
        && flags != FORCE_SSE2
        && flags != FORCE_MMX
        && flags != FORCE_FPU)
    {
        #if defined(WATERSPOUT_DEBUG)
            std::cout << "Enabled SSE42" << std::endl;
        #endif

        _math = new math_sse42;
    }
#endif

#if defined(WATERSPOUT_SIMD_SSE41)
    else if ((features_ext & SSE41)
        && flags != FORCE_SSSE3
        && flags != FORCE_SSE3
        && flags != FORCE_SSE2
        && flags != FORCE_MMX
        && flags != FORCE_FPU)
    {
        #if defined(WATERSPOUT_DEBUG)
            std::cout << "Enabled SSE41" << std::endl;
        #endif

        _math = new math_sse41;
    }
#endif

#if defined(WATERSPOUT_SIMD_SSSE3)
    else if ((features_ext & SSSE3)
        && flags != FORCE_SSE3
        && flags != FORCE_SSE2
        && flags != FORCE_MMX
        && flags != FORCE_FPU)
    {
        #if defined(WATERSPOUT_DEBUG)
            std::cout << "Enabled SSSE3" << std::endl;
        #endif

        _math = new math_ssse3;
    }
#endif

#if defined(WATERSPOUT_SIMD_SSE3)
    else if ((features_ext & SSE3)
        && flags != FORCE_SSE2
        && flags != FORCE_MMX
        && flags != FORCE_FPU)
    {
        #if defined(WATERSPOUT_DEBUG)
            std::cout << "Enabled SSE3" << std::endl;
        #endif

        _math = new math_sse3;
    }
#endif

#if defined(WATERSPOUT_SIMD_SSE) && defined(WATERSPOUT_SIMD_SSE2)
    else if ((features & SSE) && (features & SSE2)
        && flags != FORCE_MMX
        && flags != FORCE_FPU)
    {
        #if defined(WATERSPOUT_DEBUG)
            std::cout << "Enabled SSE/SSE2" << std::endl;
        #endif
        
        _math = new math_sse2;
    }
#endif

#if defined(WATERSPOUT_SIMD_MMX)
    else if ((features & MMX)
        && flags != FORCE_FPU)
    {
        #if defined(WATERSPOUT_DEBUG)
            std::cout << "Enabled MMX" << std::endl;
        #endif

        _math = new math_mmx;
    }
#endif

    else // if (features & FPU)
    {
        #if defined(WATERSPOUT_DEBUG)
            std::cout << "Enabled FPU" << std::endl;
        #endif

        _math = new math_fpu;
    }
}


//------------------------------------------------------------------------------

math_factory::~math_factory()
{
    if (_math != NULL)
    {
        delete _math;
    }
}


} // end namespace
