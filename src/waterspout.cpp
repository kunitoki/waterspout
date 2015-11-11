/*
 * waterspout
 *
 *   - simd abstraction library for audio/image manipulation -
 *
 * Copyright (c) 2015 Lucio Asnaghi
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

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <stdexcept>

#include <memory>
#include <iostream>
#include <sstream>
#include <ostream>
#include <fstream>

#include <string>
#include <map>


#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4146 4127)
#endif


#if defined(WATERSPOUT_COMPILER_GCC) || defined(WATERSPOUT_COMPILER_MINGW) || defined(WATERSPOUT_COMPILER_CLANG)
    #include <cpuid.h>
#endif


#if defined(WATERSPOUT_SIMD_MMX)
    #include <mmintrin.h>  // MMX
#endif

#if defined(WATERSPOUT_SIMD_SSE)
    #include <xmmintrin.h> // SSE
#endif

#if defined(WATERSPOUT_SIMD_SSE2)
    #include <emmintrin.h> // SSE2
#endif

#if defined(WATERSPOUT_SIMD_SSE3)
    #include <pmmintrin.h> // SSE3
#endif

#if defined(WATERSPOUT_SIMD_SSSE3)
    #include <tmmintrin.h> // SSSE3
#endif

#if defined(WATERSPOUT_SIMD_SSE41)
    #include <smmintrin.h> // SSE4.1
#endif

#if defined(WATERSPOUT_SIMD_SSE42)
    #include <nmmintrin.h> // SSE4.2
#endif

#if defined(WATERSPOUT_SIMD_AVX) || defined(WATERSPOUT_SIMD_AVX2)
    #include <immintrin.h> // AVX and AVX2
#endif

#if defined(WATERSPOUT_SIMD_NEON)
    // which include ?
#endif


namespace waterspout {


//==============================================================================

//------------------------------------------------------------------------------

/**
 * These macros are ripped from Intel Architecture Code Analyzer (IACA) and
 * serves the purpose to check for performance bottleneck in SIMD instructions
 *
 * http://software.intel.com/en-us/articles/intel-architecture-code-analyzer
 */

#ifdef IACA_MARKS_OFF
    #define IACA_START
    #define IACA_END
    #define IACA_MSC64_START
    #define IACA_MSC64_END

#else
    #if defined(WATERSPOUT_COMPILER_GCC) || defined(WATERSPOUT_COMPILER_MINGW) || defined(WATERSPOUT_COMPILER_CLANG)
        #define IACA_SSC_MARK(MARK_ID) \
            __asm__ __volatile__( \
                "\n\t  movl $"#MARK_ID", %%ebx"	\
                "\n\t  .byte 0x64, 0x67, 0x90" \
                : : : "memory" );

        #define IACA_UD_BYTES \
            __asm__ __volatile__( \
                "\n\t .byte 0x0F, 0x0B");

    #else
        #define IACA_UD_BYTES \
            { __asm _emit 0x0F \
              __asm _emit 0x0B }

        #define IACA_SSC_MARK(x) \
            { __asm  mov ebx, x \
              __asm  _emit 0x64 \
              __asm  _emit 0x67 \
              __asm  _emit 0x90 }

        #define IACA_VC64_START __writegsbyte(111, 111);
        #define IACA_VC64_END   __writegsbyte(222, 222);

    #endif

    #define IACA_START { IACA_UD_BYTES      IACA_SSC_MARK(111) }
    #define IACA_END   { IACA_SSC_MARK(222) IACA_UD_BYTES }

#endif


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Helper class to disallow copying classes
 */

class noncopyable
{
protected:
    noncopyable() {}
    ~noncopyable() {}
private:
    noncopyable(const noncopyable&);
    const noncopyable& operator=(const noncopyable&);
};


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Singleton class
 */

template <typename T>
class create_using_new_
{
public:
    static T* create()
    {
        return new T;
    }

    static void destroy(T* obj)
    {
        delete obj;
    }
};

template <typename T>
class create_static_
{
private:
    union max_align
    {
        char t_[sizeof(T)];
        short int short_int_;
        int int_;
        long int long_int_;
        float float_;
        double double_;
        long double long_double_;
        struct test_struct;
        int test_struct::*ptr_member_;
        int (test_struct::*ptr_nember_func_)(int);
    };

public:
    static T* create()
    {
        static max_align static_memory;

        return new(&static_memory) T;
    }

#if defined(WATERSPOUT_COMPILER_SUN)
    // Sun C++ Compiler doesn't handle `volatile` keyword same as GCC.
    static void destroy(T* obj)
#else
    static void destroy(volatile T* obj)
#endif
    {
        obj->~T();
    }
};

template <typename T,
          template <typename U> class create_policy=create_static_> class singleton
{
#if defined(WATERSPOUT_COMPILER_SUN)
    // Sun's C++ compiler will issue the following errors if create_policy<T> is used:
    // Error: A class template name was expected instead of waterspout::create_policy<waterspout::T>
    // Error: A "friend" declaration must specify a class or function.
    friend class create_policy;
#else
    friend class create_policy<T>;
#endif

    static T* ptr_instance_;
    static bool destroyed_;

    singleton(const singleton &rhs);
    singleton& operator=(const singleton&);

    static void on_dead_reference()
    {
        throw std::runtime_error("singleton: dead reference !");
    }

    static void destroy_singleton()
    {
        create_policy<T>::destroy(ptr_instance_);
        ptr_instance_ = nullptr;
        destroyed_ = true;
    }

protected:
    singleton() {}

public:
    static T& instance()
    {
        if (! ptr_instance_)
        {
            if (! ptr_instance_)
            {
                if (destroyed_)
                {
                    destroyed_ = false;
                    on_dead_reference();
                }
                else
                {
                    ptr_instance_ = create_policy<T>::create();

                    // register destruction
                    std::atexit(&destroy_singleton);
                }
            }
        }
        return *ptr_instance_;
    }
};

template <typename T,
          template <typename U> class create_policy>
T* singleton<T, create_policy>::ptr_instance_ = nullptr;

template <typename T,
          template <typename U> class create_policy>
bool singleton<T, create_policy>::destroyed_ = false;


//==============================================================================

//------------------------------------------------------------------------------

#ifndef WATERSPOUT_LOG_FORMAT
  //#define WATERSPOUT_LOG_FORMAT  Waterspout LOG> %Y-%m-%d %H:%M:%S:
  #define WATERSPOUT_LOG_FORMAT  Waterspout LOG>
#endif

#ifndef WATERSPOUT_DEFAULT_LOG_SEVERITY
  #ifdef WATERSPOUT_DEBUG
    #define WATERSPOUT_DEFAULT_LOG_SEVERITY 0
  #else
    #define WATERSPOUT_DEFAULT_LOG_SEVERITY 2
  #endif
#endif


//------------------------------------------------------------------------------

namespace logger_detail_ {

    /*
     * The main logger class
     */
    class logger :
        public singleton<logger>,
        private noncopyable
    {
    public:
        enum severity_type
        {
            debug = 0,
            warn = 1,
            error = 2,
            info = 3,
            none = 4
        };

        typedef std::map<std::string, severity_type> severity_map;

        // global security level
        static severity_type get_severity()
        {
            return severity_level_;
        }

        static void set_severity(const severity_type& severity_level)
        {
            severity_level_ = severity_level;
        }

        // per object security levels
        static severity_type get_object_severity(std::string const& object_name)
        {
            severity_map::iterator it = object_severity_level_.find(object_name);
            if (object_name.empty() || it == object_severity_level_.end())
            {
                return severity_level_;
            }
            else
            {
                return it->second;
            }
        }

        static void set_object_severity(std::string const& object_name,
                                        const severity_type& security_level)
        {
            if (! object_name.empty())
            {
                object_severity_level_[object_name] = security_level;
            }
        }

        static void clear_object_severity()
        {
            object_severity_level_.clear();
        }

        // format
        static std::string get_format()
        {
            return format_;
        }

        static void set_format(std::string const& format)
        {
            format_ = format;
        }

        // interpolate the format string for output
        static std::string str()
        {
            // update the format from getenv if this is the first time
            if (logger::format_env_check_)
            {
                logger::format_env_check_ = false;

                const char* log_format = getenv("WATERSPOUT_LOG_FORMAT");
                if (log_format != nullptr)
                {
                    logger::format_ = log_format;
                }
            }

            char buf[256];
            const time_t tm = time(0);
            strftime(buf, sizeof(buf), logger::format_.c_str(), localtime(&tm));
            return buf;
        }

        // output
        static void use_file(std::string const& filepath)
        {
            // save clog rdbuf
            if (saved_buf_ == nullptr)
            {
                saved_buf_ = std::clog.rdbuf();
            }

            // use a file to output as clog rdbuf
            if (file_name_ != filepath)
            {
                file_name_ = filepath;

                if (file_output_.is_open())
                {
                    file_output_.close();
                }

                file_output_.open(file_name_.c_str(), std::ios::out | std::ios::app);
                if (file_output_)
                {
                    std::clog.rdbuf(file_output_.rdbuf());
                }
                else
                {
                    std::stringstream s;
                    s << "logger: cannot redirect log to file " << file_output_;
                    throw std::runtime_error(s.str());
                }
            }
        }

        static void use_console()
        {
            // save clog rdbuf
            if (saved_buf_ == nullptr)
            {
                saved_buf_ = std::clog.rdbuf();
            }

            // close the file to force a flush
            if (file_output_.is_open())
            {
                file_output_.close();
            }

            std::clog.rdbuf(saved_buf_);
        }

    private:
        static severity_type severity_level_;
        static severity_map object_severity_level_;
        static bool severity_env_check_;

        static std::string format_;
        static bool format_env_check_;

        static std::ofstream file_output_;
        static std::string file_name_;
        static std::streambuf* saved_buf_;
    };

    bool logger::severity_env_check_ = true;
    bool logger::format_env_check_ = true;

    logger::severity_type logger::severity_level_ =
        #if WATERSPOUT_DEFAULT_LOG_SEVERITY == 0
            logger::debug
        #elif WATERSPOUT_DEFAULT_LOG_SEVERITY == 1
            logger::warn
        #elif WATERSPOUT_DEFAULT_LOG_SEVERITY == 2
            logger::error
        #elif WATERSPOUT_DEFAULT_LOG_SEVERITY == 3
            logger::info
        #elif WATERSPOUT_DEFAULT_LOG_SEVERITY == 4
            logger::none
        #else
            #error "Wrong default log severity level specified!"
        #endif
    ;

    logger::severity_map logger::object_severity_level_ = logger::severity_map();

    #define __xstr__(s) __str__(s)
    #define __str__(s) #s
    std::string logger::format_ = __xstr__(WATERSPOUT_LOG_FORMAT);
    #undef __xstr__
    #undef __str__

    std::ofstream logger::file_output_;
    std::string logger::file_name_;
    std::streambuf* logger::saved_buf_ = 0;


    /*
     * Default sink, it regulates access to clog
     */
    template<class Ch, class Tr, class A>
    class clog_sink
    {
    public:
        typedef std::basic_ostringstream<Ch, Tr, A> stream_buffer;

        void operator()(const logger::severity_type& /*severity*/, const stream_buffer &s)
        {
            std::clog << logger::str() << " " << s.str() << std::endl;
        }
    };


    /*
     * Base log class, should not log anything when WATERSPOUT_VOID_LOGGING is defined
     *
     * This is used for debug/warn reporting that should not output
     * anything when not compiling for speed.
     */
    template<template <class Ch, class Tr, class A> class OutputPolicy,
             logger::severity_type Severity,
             bool BypassSeverityCheck = false,
             class Ch = char,
             class Tr = std::char_traits<Ch>,
             class A = std::allocator<Ch> >
    class base_log : public noncopyable
    {
    public:
        typedef OutputPolicy<Ch, Tr, A> output_policy;

        base_log() {}

        base_log(const char* object_name)
        {
#if !defined(WATERSPOUT_VOID_LOGGING)
            if (object_name != nullptr)
            {
                object_name_ = object_name;
            }
#endif
        }

        ~base_log()
        {
#if !defined(WATERSPOUT_VOID_LOGGING)
            if (BypassSeverityCheck || check_severity())
            {
                output_policy()(Severity, streambuf_);
            }
#endif
        }

        template<class T>
        base_log &operator<<(const T &x)
        {
#if !defined(WATERSPOUT_VOID_LOGGING)
            streambuf_ << x;
#endif
            return *this;
        }

    private:
#if !defined(WATERSPOUT_VOID_LOGGING)
        inline bool check_severity()
        {
            return Severity >= logger::get_object_severity(object_name_);
        }

        typename output_policy::stream_buffer streambuf_;
        std::string object_name_;
#endif
    };


    /*
     * Base log class that always log, regardless of WATERSPOUT_VOID_LOGGING.
     *
     * This is used for error reporting that should always log something
     */
    template<template <class Ch, class Tr, class A> class OutputPolicy,
             logger::severity_type Severity,
             bool BypassSeverityCheck = false,
             class Ch = char,
             class Tr = std::char_traits<Ch>,
             class A = std::allocator<Ch> >
    class base_log_always : public noncopyable
    {
    public:
        typedef OutputPolicy<Ch, Tr, A> output_policy;

        base_log_always() {}

        base_log_always(const char* object_name)
        {
            if (object_name != nullptr)
            {
                object_name_ = object_name;
            }
        }

        ~base_log_always()
        {
            if (BypassSeverityCheck || check_severity())
            {
                output_policy()(Severity, streambuf_);
            }
        }

        template<class T>
        base_log_always &operator<<(const T &x)
        {
            streambuf_ << x;
            return *this;
        }

    private:
        inline bool check_severity()
        {
            return Severity >= logger::get_object_severity(object_name_);
        }

        typename output_policy::stream_buffer streambuf_;
        std::string object_name_;
    };


    /*
     * Real classes used in the code
     */
    typedef base_log<clog_sink, logger::debug> base_log_debug;
    typedef base_log<clog_sink, logger::warn> base_log_warn;
    typedef base_log_always<clog_sink, logger::error> base_log_error;
    typedef base_log_always<clog_sink, logger::info, true> base_log_info;

    class debug : public logger_detail_::base_log_debug {
    public:
        debug() : logger_detail_::base_log_debug() {}
        debug(const char* object_name)
            : logger_detail_::base_log_debug(object_name) {}
    };

    class warn : public logger_detail_::base_log_warn {
    public:
        warn() : logger_detail_::base_log_warn() {}
        warn(const char* object_name)
            : logger_detail_::base_log_warn(object_name) {}
    };

    class error : public logger_detail_::base_log_error {
    public:
        error() : logger_detail_::base_log_error() {}
        error(const char* object_name)
            : logger_detail_::base_log_error(object_name) {}
    };

    class info : public logger_detail_::base_log_info {
    public:
        info() : logger_detail_::base_log_info() {}
        info(const char* object_name)
            : logger_detail_::base_log_info(object_name) {}
    };

} // end namespace logger_detail_


//------------------------------------------------------------------------------

/**
 * Logging helpers
 */

typedef logger_detail_::debug logger_debug;
typedef logger_detail_::warn logger_warn;
typedef logger_detail_::error logger_error;
typedef logger_detail_::info logger_info;

#define WATERSPOUT_LOG_DEBUG(s) waterspout::logger_debug(#s)
#define WATERSPOUT_LOG_WARN(s) waterspout::logger_warn(#s)
#define WATERSPOUT_LOG_ERROR(s) waterspout::logger_error(#s)
#define WATERSPOUT_LOG_INFO(s) waterspout::logger_info(#s)


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
    FPU   = 1 <<  0, // Floating-Point Unit on-chip
    MMX   = 1 << 23, // MultiMedia eXtension
    SSE   = 1 << 25, // Streaming SIMD Extension 1
    SSE2  = 1 << 26  // Streaming SIMD Extension 2
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
    SSE3  = 1 <<  0, // Streaming SIMD Extension 3
    SSE4A = 1 <<  6, // SSE4A (only for AMD)
    SSSE3 = 1 <<  9, // SSSE3
    SSE41 = 1 << 19, // SSE41
    SSE42 = 1 << 20, // SSE42
    AVX   = 1 << 28, // AVX
    AVX2  = 1 <<  5  // AVX2
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

void cpuid(uint32 op, uint32& eax, uint32& ebx, uint32& ecx, uint32& edx)
{
#if defined(WATERSPOUT_COMPILER_GCC) || defined(WATERSPOUT_COMPILER_MINGW)
    // GCC/MINGW provides a __get_cpuid function
    __get_cpuid(op, &eax, &ebx, &ecx, &edx);

#elif defined(WATERSPOUT_COMPILER_MSVC)
    // MSVC provides a __cpuid function
    int regs[4];
    __cpuid(regs, op);
    eax = (uint32)regs[0];
    ebx = (uint32)regs[1];
    ecx = (uint32)regs[2];
    edx = (uint32)regs[3];

#endif
}


//------------------------------------------------------------------------------

/**
 * This will retrieve the CPU features available
 * \return The content of the edx register containing available features
 */

uint32 cpu_features()
{
    uint32 eax, ebx, ecx, edx;
    cpuid(1, eax, ebx, ecx, edx);
    return edx;
}


//------------------------------------------------------------------------------

/**
 * This will retrieve the extended CPU features available
 * \return The content of the ecx register containing available extended features
 */

uint32 cpu_extended_features()
{
    uint32 eax, ebx, ecx, edx;
    cpuid(1, eax, ebx, ecx, edx);
    return ecx;
}


//------------------------------------------------------------------------------

/**
 * Retrieve the processor name.
 * \param name Preallocated string containing at least room for 13 characters. Will
 *             contain the name of the processor.
 */

std::string cpu_processor_name()
{
    uint32 eax, ebx, ecx, edx;
    cpuid(0, eax, ebx, ecx, edx);

    char name[13];
    ::memcpy((void*)&name[0], (void*)&ebx, 4 * sizeof(char));
    ::memcpy((void*)&name[4], (void*)&edx, 4 * sizeof(char));
    ::memcpy((void*)&name[8], (void*)&ecx, 4 * sizeof(char));
    name[12] = 0;

    return std::string(name);
}

//------------------------------------------------------------------------------

/**
 * Retrieve the processor endianess.
 */

uint32 cpu_endianness()
{
    uint32 value;
    uint8* buffer = (uint8*)&value;

    buffer[0] = 0x00;
    buffer[1] = 0x01;
    buffer[2] = 0x02;
    buffer[3] = 0x03;

    switch (value)
    {
    case uint32(0x00010203): return ENDIAN_BIG;
    case uint32(0x03020100): return ENDIAN_LITTLE;
    case uint32(0x02030001): return ENDIAN_BIG_WORD;
    case uint32(0x01000302): return ENDIAN_LITTLE_WORD;
    default:                 return ENDIAN_UNKNOWN;
    }
}


//==============================================================================

//------------------------------------------------------------------------------

void* memory::aligned_alloc(uint32 size_bytes, uint32 alignment_bytes)
{
#if defined(WATERSPOUT_COMPILER_MSVC)
    return (void*)::_aligned_malloc(size_bytes, alignment_bytes);
#elif defined(WATERSPOUT_COMPILER_GCC) || defined(WATERSPOUT_COMPILER_MINGW) || defined(WATERSPOUT_COMPILER_CLANG)
    void* ptr = nullptr;
    if (::posix_memalign(&ptr, alignment_bytes, size_bytes) == 0) { /* do nothing */ }
    return ptr;
#else
    return (void*)::malloc(size_bytes);
#endif
}


//------------------------------------------------------------------------------

void memory::aligned_free(void* ptr)
{
#if defined(WATERSPOUT_COMPILER_MSVC)
    ::_aligned_free(ptr);
#elif defined(WATERSPOUT_COMPILER_GCC) || defined(WATERSPOUT_COMPILER_MINGW) || defined(WATERSPOUT_COMPILER_CLANG)
    ::free(ptr);
#else
    ::free(ptr);
#endif
}


//==============================================================================

//------------------------------------------------------------------------------

#define simd_unroll_head_16(s) \
    switch (align_bytes >> 4) \
    { \
    case 1:  s; \
    case 2:  s; \
    case 3:  s; \
    case 4:  s; \
    case 5:  s; \
    case 6:  s; \
    case 7:  s; \
    case 8:  s; \
    case 9:  s; \
    case 10: s; \
    case 11: s; \
    case 12: s; \
    case 13: s; \
    case 14: s; \
    case 15: s; \
    }

#define simd_unroll_tail_16(s) \
    switch (size & 15) \
    { \
    case 15: s; \
    case 14: s; \
    case 13: s; \
    case 12: s; \
    case 11: s; \
    case 10: s; \
    case 9:  s; \
    case 8:  s; \
    case 7:  s; \
    case 6:  s; \
    case 5:  s; \
    case 4:  s; \
    case 3:  s; \
    case 2:  s; \
    case 1:  s; \
    }


//------------------------------------------------------------------------------

#define simd_unroll_head_8(s) \
    switch (align_bytes >> 3) \
    { \
    case 1: s; \
    case 2: s; \
    case 3: s; \
    case 4: s; \
    case 5: s; \
    case 6: s; \
    case 7: s; \
    }

#define simd_unroll_tail_8(s) \
    switch (size & 7) \
    { \
    case 7: s; \
    case 6: s; \
    case 5: s; \
    case 4: s; \
    case 3: s; \
    case 2: s; \
    case 1: s; \
    }


//------------------------------------------------------------------------------

#define simd_unroll_head_4(s) \
    switch (align_bytes >> 2) \
    { \
    case 1: s; \
    case 2: s; \
    case 3: s; \
    }

#define simd_unroll_tail_4(s) \
    switch (size & 3) \
    { \
    case 3: s; \
    case 2: s; \
    case 1: s; \
    }


//------------------------------------------------------------------------------

#define simd_unroll_head_2(s) \
    switch (align_bytes >> 1) \
    { \
    case 1: s; \
    }

#define simd_unroll_tail_2(s) \
    switch (size & 1) \
    { \
    case 1: s; \
    }


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

#if defined(WATERSPOUT_SIMD_AVX2)
    #include "math_avx2.h"
#endif

#if defined(WATERSPOUT_SIMD_NEON)
    #include "math_neon.h"
#endif


//==============================================================================

//------------------------------------------------------------------------------

math::math(int flags, bool fallback)
  : math_implementation_(nullptr)
{
    if (! fallback)
    {
#if ! defined(WATERSPOUT_SIMD_AVX2)
        if (flags == FORCE_AVX2)
            throw std::runtime_error("math_factory: AVX2 not available!");
#endif
#if ! defined(WATERSPOUT_SIMD_AVX)
        if (flags == FORCE_AVX)
            throw std::runtime_error("math_factory: AVX not available!");
#endif
#if ! defined(WATERSPOUT_SIMD_SSE42)
        if (flags == FORCE_SSE42)
            throw std::runtime_error("math_factory: SSE42 not available!");
#endif
#if ! defined(WATERSPOUT_SIMD_SSE41)
        if (flags == FORCE_SSE41)
            throw std::runtime_error("math_factory: SSE41 not available!");
#endif
#if ! defined(WATERSPOUT_SIMD_SSSE3)
        if (flags == FORCE_SSSE3)
            throw std::runtime_error("math_factory: SSSE3 not available!");
#endif
#if ! defined(WATERSPOUT_SIMD_SSE3)
        if (flags == FORCE_SSE3)
            throw std::runtime_error("math_factory: SSE3 not available!");
#endif
#if ! defined(WATERSPOUT_SIMD_SSE2)
        if (flags == FORCE_SSE2)
            throw std::runtime_error("math_factory: SSE2 not available!");
#endif
#if ! defined(WATERSPOUT_SIMD_SSE)
        if (flags == FORCE_SSE)
            throw std::runtime_error("math_factory: SSE not available!");
#endif
#if ! defined(WATERSPOUT_SIMD_MMX)
        if (flags == FORCE_MMX)
            throw std::runtime_error("math_factory: MMX not available!");
#endif
    }
    
#if defined(WATERSPOUT_SYSTEM_ANDROID)
    if ((android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM &&
         (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0) ||
         (flags == FORCE_NEON))
    {
        math_implementation_ = new math_neon;
    }

#else
    {
        static uint32 features = cpu_features();
        static uint32 features_ext = cpu_extended_features();

        bool placeholder = false;
        if (placeholder)
        {
        	// this must be here for VS to not complain !
        	placeholder = false;
        }

    #if defined(WATERSPOUT_SIMD_AVX2)
        else if ((features_ext & AVX2)
            && flags != FORCE_AVX
            && flags != FORCE_SSE42
            && flags != FORCE_SSE41
            && flags != FORCE_SSSE3
            && flags != FORCE_SSE3
            && flags != FORCE_SSE2
            && flags != FORCE_SSE
            && flags != FORCE_MMX
            && flags != FORCE_FPU)
        {
            math_implementation_ = new math_avx2;
        }
    #endif

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
            math_implementation_ = new math_avx;
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
            math_implementation_ = new math_sse42;
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
            math_implementation_ = new math_sse41;
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
            math_implementation_ = new math_ssse3;
        }
    #endif

    #if defined(WATERSPOUT_SIMD_SSE3)
        else if ((features_ext & SSE3)
            && flags != FORCE_SSE2
            && flags != FORCE_SSE
            && flags != FORCE_MMX
            && flags != FORCE_FPU)
        {
            math_implementation_ = new math_sse3;
        }
    #endif

    #if defined(WATERSPOUT_SIMD_SSE2)
        else if ((features & SSE2)
            && flags != FORCE_SSE
            && flags != FORCE_MMX
            && flags != FORCE_FPU)
        {
            math_implementation_ = new math_sse2;
        }
    #endif

    #if defined(WATERSPOUT_SIMD_SSE)
        else if ((features & SSE)
            && flags != FORCE_MMX
            && flags != FORCE_FPU)
        {
            math_implementation_ = new math_sse;
        }
    #endif

    #if defined(WATERSPOUT_SIMD_MMX)
        else if ((features & MMX)
            && flags != FORCE_FPU)
        {
            math_implementation_ = new math_mmx;
        }
    #endif

        else // if ((features & FPU) || flags == FORCE_FPU)
        {
            math_implementation_ = new math_fpu;
        }
    }
#endif


#if defined(WATERSPOUT_DEBUG) && 0
    WATERSPOUT_LOG_DEBUG(math_factory) << "Processor name: "
        << cpu_processor_name();

    WATERSPOUT_LOG_DEBUG(math_factory) << "Processor endianess: "
        << (cpu_endianness() == ENDIAN_BIG ? "bigendian" : "littlendian");

    WATERSPOUT_LOG_DEBUG(math_factory) << "Processor features:";
    WATERSPOUT_LOG_DEBUG(math_factory)
        << "  FPU   = " << std::boolalpha << (bool)(cpu_features() & FPU);

    #if defined(WATERSPOUT_SIMD_MMX)
        WATERSPOUT_LOG_DEBUG(math_factory)
            << "  MMX   = " << std::boolalpha << (bool)(cpu_features() & MMX);
    #endif
    #if defined(WATERSPOUT_SIMD_SSE)
        WATERSPOUT_LOG_DEBUG(math_factory)
            << "  SSE   = " << std::boolalpha << (bool)(cpu_features() & SSE);
    #endif
    #if defined(WATERSPOUT_SIMD_SSE2)
        WATERSPOUT_LOG_DEBUG(math_factory)
            << "  SSE2  = " << std::boolalpha << (bool)(cpu_features() & SSE2);
    #endif
    #if defined(WATERSPOUT_SIMD_SSE3)
        WATERSPOUT_LOG_DEBUG(math_factory)
            << "  SSE3  = " << std::boolalpha << (bool)(cpu_extended_features() & SSE3);
    #endif
    #if defined(WATERSPOUT_SIMD_SSSE3)
        WATERSPOUT_LOG_DEBUG(math_factory)
            << "  SSSE3 = " << std::boolalpha << (bool)(cpu_extended_features() & SSSE3);
    #endif
    #if defined(WATERSPOUT_SIMD_SSE41)
        WATERSPOUT_LOG_DEBUG(math_factory)
            << "  SSE41 = " << std::boolalpha << (bool)(cpu_extended_features() & SSE41);
    #endif
    #if defined(WATERSPOUT_SIMD_SSE42)
        WATERSPOUT_LOG_DEBUG(math_factory)
            << "  SSE42 = " << std::boolalpha << (bool)(cpu_extended_features() & SSE42);
    #endif
    #if defined(WATERSPOUT_SIMD_SSE4A)
        WATERSPOUT_LOG_DEBUG(math_factory)
            << "  SSE4A = " << std::boolalpha << (bool)(cpu_extended_features() & SSE4A);
    #endif
    #if defined(WATERSPOUT_SIMD_AVX)
        WATERSPOUT_LOG_DEBUG(math_factory)
            << "  AVX   = " << std::boolalpha << (bool)(cpu_extended_features() & AVX);
    #endif
    #if defined(WATERSPOUT_SIMD_AVX2)
        WATERSPOUT_LOG_DEBUG(math_factory)
            << "  AVX2  = " << std::boolalpha << (bool)(cpu_extended_features() & AVX2);
    #endif

    if (math_implementation_ != nullptr)
    {
        WATERSPOUT_LOG_DEBUG(math_factory)
            << "Enabled " << math_implementation_->name() << " instructions";
    }
#endif
}


//------------------------------------------------------------------------------

math::~math()
{
}


//------------------------------------------------------------------------------

const char* math::name() const
{
    if (math_implementation_ != nullptr)
    {
        return math_implementation_->name();
    }

    return "";
}


} // end namespace


#ifdef _MSC_VER
	#pragma warning(pop)
#endif
