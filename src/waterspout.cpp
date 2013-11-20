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
 * @brief The CpuFeatures enum
 *
 * Reference:
 * http://datasheets.chipdb.org/Intel/x86/CPUID/24161821.pdf
 * http://www.flounder.com/cpuid_explorer2.htm
 */
enum CpuFeatures
{
  FPU   = 1<< 0, // Floating-Point Unit on-chip
  MMX   = 1<<23, // MultiMedia eXtension
  SSE   = 1<<25, // Streaming SIMD Extension 1
  SSE2  = 1<<26  // Streaming SIMD Extension 2
};

/**
 * @brief The CpuExtendedFeatures enum
 *
 * Reference:
 * http://datasheets.chipdb.org/Intel/x86/CPUID/24161821.pdf
 * http://www.flounder.com/cpuid_explorer2.htm
 */
enum CpuExtendedFeatures
{
  SSE3  = 1<< 0, // Streaming SIMD Extension 3
  SSE4A = 1<< 6, // SSE4A (only for AMD)
  SSSE3 = 1<< 9, // SSSE3
  SSE41 = 1<<19, // SSE41
  SSE42 = 1<<20, // SSE42
  AVX   = 1<<28  // AVX
};

/**
 * @brief The CpuEndianess enum
 */
enum CpuEndianess
{
  ENDIAN_UNKNOWN     = 0,
  ENDIAN_BIG         = 1,
  ENDIAN_LITTLE      = 2,
  ENDIAN_BIG_WORD    = 3, // Middle-endian, Honeywell 316 style
  ENDIAN_LITTLE_WORD = 4  // Middle-endian, PDP-11 style
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

uint32_t cpu_features()
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

uint32_t cpu_extended_features()
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

void cpu_processor_name(char* name)
{
  name[12] = 0;
  uint32_t max_op;
  cpuid(0, max_op, (uint32_t&)name[0], (uint32_t&)name[8], (uint32_t&)name[4]);
}

//------------------------------------------------------------------------------

/**
 * Retrieve the processor endianess.
 */

uint32_t cpu_endianness()
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
  
    static const uint32_t caps = cpu_features();

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

#include "math_fpu.h"

#if defined(WATERSPOUT_SIMD_MMX)
    #include "math_mmx.h"
#endif

#if defined(WATERSPOUT_SIMD_SSE)
    #include "math_sse.h"
#endif

#if defined(WATERSPOUT_SIMD_SSE2)
    #include "math_sse2.h"
#endif

#if defined(WATERSPOUT_SIMD_SSE3)
    #include "math_sse3.h"
#endif

#if defined(WATERSPOUT_SIMD_SSSE3)
    #include "math_ssse3.h"
#endif

#if defined(WATERSPOUT_SIMD_SSE41)
    #include "math_sse41.h"
#endif

#if defined(WATERSPOUT_SIMD_SSE42)
    #include "math_sse42.h"
#endif

#if defined(WATERSPOUT_SIMD_AVX)
    #include "math_avx.h"
#endif

#if defined(WATERSPOUT_SIMD_NEON)
    #include "math_neon.h"
#endif


//==============================================================================

//------------------------------------------------------------------------------

math_factory::math_factory(int flags, bool fallback)
  : _math(NULL)
{
    if (! fallback)
    {
#if ! defined(WATERSPOUT_SIMD_AVX)
        if (flags == FORCE_AVX) throw 1;
#endif
#if ! defined(WATERSPOUT_SIMD_SSE42)
        if (flags == FORCE_SSE42) throw 1;
#endif
#if ! defined(WATERSPOUT_SIMD_SSE41)
        if (flags == FORCE_SSE41) throw 1;
#endif
#if ! defined(WATERSPOUT_SIMD_SSSE3)
        if (flags == FORCE_SSSE3) throw 1;
#endif
#if ! defined(WATERSPOUT_SIMD_SSE3)
        if (flags == FORCE_SSE3) throw 1;
#endif
#if ! defined(WATERSPOUT_SIMD_SSE2)
        if (flags == FORCE_SSE2) throw 1;
#endif
#if ! defined(WATERSPOUT_SIMD_SSE)
        if (flags == FORCE_SSE) throw 1;
#endif
#if ! defined(WATERSPOUT_SIMD_MMX)
        if (flags == FORCE_MMX) throw 1;
#endif
    }
    
#if defined(WATERSPOUT_SYSTEM_ANDROID)
    if ((android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM &&
         (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0) ||
         (flags == FORCE_NEON))
    {
        _math = new math_neon;
    }

#else
    {
        static uint32_t features = cpu_features();
        static uint32_t features_ext = cpu_extended_features();

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
            && flags != FORCE_SSE
            && flags != FORCE_MMX
            && flags != FORCE_FPU)
        {
            _math = new math_avx;
        }
    #endif

    #if defined(WATERSPOUT_SIMD_SSE42)
        else if ((features_ext & SSE42)
            && flags != FORCE_SSE41
            && flags != FORCE_SSSE3
            && flags != FORCE_SSE3
            && flags != FORCE_SSE2
            && flags != FORCE_SSE
            && flags != FORCE_MMX
            && flags != FORCE_FPU)
        {
            _math = new math_sse42;
        }
    #endif

    #if defined(WATERSPOUT_SIMD_SSE41)
        else if ((features_ext & SSE41)
            && flags != FORCE_SSSE3
            && flags != FORCE_SSE3
            && flags != FORCE_SSE2
            && flags != FORCE_SSE
            && flags != FORCE_MMX
            && flags != FORCE_FPU)
        {
            _math = new math_sse41;
        }
    #endif

    #if defined(WATERSPOUT_SIMD_SSSE3)
        else if ((features_ext & SSSE3)
            && flags != FORCE_SSE3
            && flags != FORCE_SSE2
            && flags != FORCE_SSE
            && flags != FORCE_MMX
            && flags != FORCE_FPU)
        {
            _math = new math_ssse3;
        }
    #endif

    #if defined(WATERSPOUT_SIMD_SSE3)
        else if ((features_ext & SSE3)
            && flags != FORCE_SSE2
            && flags != FORCE_SSE
            && flags != FORCE_MMX
            && flags != FORCE_FPU)
        {
            _math = new math_sse3;
        }
    #endif

    #if defined(WATERSPOUT_SIMD_SSE2)
        else if ((features & SSE2)
            && flags != FORCE_SSE
            && flags != FORCE_MMX
            && flags != FORCE_FPU)
        {
            _math = new math_sse2;
        }
    #endif

    #if defined(WATERSPOUT_SIMD_SSE)
        else if ((features & SSE)
            && flags != FORCE_MMX
            && flags != FORCE_FPU)
        {
            _math = new math_sse;
        }
    #endif

    #if defined(WATERSPOUT_SIMD_MMX)
        else if ((features & MMX)
            && flags != FORCE_FPU)
        {
            _math = new math_mmx;
        }
    #endif

        else // if ((features & FPU) || flags == FORCE_FPU)
        {
            _math = new math_fpu;
        }
    }
#endif


#if defined(WATERSPOUT_DEBUG)
    char procname[13];
    cpu_processor_name(procname);
    std::cout << "Processor name: " << procname << std::endl;

    std::cout << "Processor endianess: "
      << (cpu_endianness() == ENDIAN_BIG ? "bigendian" : "littlendian") << std::endl;

    std::cout << "Processor features:" << std::endl;
    std::cout << "  FPU   = " << std::boolalpha << (bool)(cpu_features() & FPU  ) << std::endl;
    #if defined(WATERSPOUT_SIMD_MMX)
        std::cout << "  MMX   = " << std::boolalpha << (bool)(cpu_features() & MMX  ) << std::endl;
    #endif
    #if defined(WATERSPOUT_SIMD_SSE)
        std::cout << "  SSE   = " << std::boolalpha << (bool)(cpu_features() & SSE  ) << std::endl;
    #endif
    #if defined(WATERSPOUT_SIMD_SSE2)
        std::cout << "  SSE2  = " << std::boolalpha << (bool)(cpu_features() & SSE2 ) << std::endl;
    #endif
    #if defined(WATERSPOUT_SIMD_SSE3)
        std::cout << "  SSE3  = " << std::boolalpha << (bool)(cpu_extended_features() & SSE3) << std::endl;
    #endif
    #if defined(WATERSPOUT_SIMD_SSSE3)
        std::cout << "  SSSE3 = " << std::boolalpha << (bool)(cpu_extended_features() & SSSE3 ) << std::endl;
    #endif
    #if defined(WATERSPOUT_SIMD_SSE41)
        std::cout << "  SSE41 = " << std::boolalpha << (bool)(cpu_extended_features() & SSE41 ) << std::endl;
    #endif
    #if defined(WATERSPOUT_SIMD_SSE42)
        std::cout << "  SSE42 = " << std::boolalpha << (bool)(cpu_extended_features() & SSE42 ) << std::endl;
    #endif
    #if defined(WATERSPOUT_SIMD_SSE4A)
        std::cout << "  SSE4A = " << std::boolalpha << (bool)(cpu_extended_features() & SSE4A ) << std::endl;
    #endif
    #if defined(WATERSPOUT_SIMD_AVX)
        std::cout << "  AVX   = " << std::boolalpha << (bool)(cpu_extended_features() & AVX ) << std::endl;
    #endif

    if (_math != NULL)
    {
        std::cout << "Enabled " << _math->name() << std::endl;
    }
#endif
}


//------------------------------------------------------------------------------

math_factory::~math_factory()
{
    if (_math != NULL)
    {
        delete _math;
    }
}


//------------------------------------------------------------------------------

const char* math_factory::name()
{
    if (_math != NULL)
    {
        return _math->name();
    }

    return "";
}


} // end namespace
