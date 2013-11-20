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

#ifndef __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_H__
#define __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_H__

#include <cstdlib>
#include <cmath>
#include <ctime>

#include <cassert>

#include <iostream>
#include <sstream>
#include <ostream>
#include <fstream>
#include <iomanip>

#include <string>
#include <memory>

#include <exception>
#include <stdexcept>


//------------------------------------------------------------------------------

/**
 * SIMD intrinsics definitions
 */

#if defined(__MMX__)
  #define WATERSPOUT_SIMD_MMX
  #include <mmintrin.h>  // MMX
#endif

#if defined(__SSE__)
  #define WATERSPOUT_SIMD_SSE
  #include <xmmintrin.h> // SSE
#endif

#if defined(__SSE2__)
  #define WATERSPOUT_SIMD_SSE2
  #include <emmintrin.h> // SSE2
#endif

#if defined(__SSE3__)
  #define WATERSPOUT_SIMD_SSE3
  #include <pmmintrin.h> // SSE3
#endif

#if defined(__SSSE3__)
  #define WATERSPOUT_SIMD_SSSE3
  #include <tmmintrin.h> // SSSE3
#endif

#if defined(__SSE4_1__)
  #define WATERSPOUT_SIMD_SSE41
  #include <smmintrin.h> // SSE4.1
#endif

#if defined(__SSE4_2__)
  #define WATERSPOUT_SIMD_SSE42
  #include <nmmintrin.h> // SSE4.2
#endif

#if defined(__SSE4A__)
  #define WATERSPOUT_SIMD_SSE4A
  #include <ammintrin.h> // SSE4A
#endif

#if defined(__AES__)
  #define WATERSPOUT_SIMD_AES
  #include <wmmintrin.h> // AES
#endif

#if defined(__AVX__)
  #define WATERSPOUT_SIMD_AVX
  #include <immintrin.h> // AVX
#endif

// #include <x86intrin.h> // pull al the others in !


/**
 * System definitions
 */

#ifdef _WIN32
    // Windows (x64 and x86)
    #define WATERSPOUT_SYSTEM_WINDOWS 1
    #define NOMINMAX
    #include <windows.h>

#elif __linux__
    #if defined(__ANDROID__)
        // Android
        #define WATERSPOUT_SYSTEM_ANDROID 1
        #include <cpu-features.h>
    #else
        // Linux
        #define WATERSPOUT_SYSTEM_LINUX 1
        #include <sys/time.h> // for gettimeofday() on unix
        #include <sys/resource.h>
   #endif

#elif defined(__APPLE__) && defined(__MACH__)
    // MacOSX
    #define WATERSPOUT_SYSTEM_MACOSX 1

#elif defined(sun) || defined(__sun)
    #if defined(__SVR4) || defined(__svr4__)
        // Solaris
        #define WATERSPOUT_SYSTEM_SOLARIS 1
    #else
        // SunOS
        #define WATERSPOUT_SYSTEM_SUNOS 1
    #endif

#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
    // BSD
    #define WATERSPOUT_SYSTEM_BSD 1

#endif


/**
 * Compiler definition
 */

#if defined(_MSC_VER)
    #define WATERSPOUT_COMPILER_MSVC 1

#elif defined(__MINGW32__)
    #define WATERSPOUT_COMPILER_MINGW 1

#elif defined(__GNUC__)
    #if defined(__clang__)
        #define WATERSPOUT_COMPILER_CLANG 1
    #else
        #define WATERSPOUT_COMPILER_GCC 1
    #endif

#elif defined(__INTEL_COMPILER)
    #define WATERSPOUT_COMPILER_INTEL 1

#elif defined(__SUNPRO_CC)
    #define WATERSPOUT_COMPILER_SUN 1

#else
    #error "Unknown compiler: this compiler is not supported !"

#endif


/**
 * C++0x or C++11 support (TODO for MSVC/INTEL)
 */
#if defined(__GXX_EXPERIMENTAL_CXX0X__)
    #define WATERSPOUT_SUPPORT_CXX0X
#endif


/**
 * Debug and Release definitions
 */

#if defined(DEBUG) || defined(_DEBUG)
    #define WATERSPOUT_DEBUG 1
#endif


//------------------------------------------------------------------------------

/**
 * Waterspout global macros helpers by compiler
 */

#if defined(WATERSPOUT_COMPILER_GCC) || defined(WATERSPOUT_COMPILER_MINGW) || defined(WATERSPOUT_COMPILER_CLANG)
    #include <stdint.h>
    #include <malloc.h>
    #include <cpuid.h>
    #include <fenv.h>

    #define aligned(type_name, alignment) \
        __attribute__ ((aligned(alignment))) type_name

    #define forcedinline inline __attribute__ ((always_inline))

    #define enable_floating_point_assertions \
        ::feenableexcept(FE_DIVBYZERO | FE_INVALID | \
          FE_OVERFLOW | FE_UNDERFLOW); \
        ::feclearexcept(FE_ALL_EXCEPT);
                                                                 
    #define disable_floating_point_assertions \
        ::fedisableexcept(FE_ALL_EXCEPT); \
        ::feclearexcept(FE_ALL_EXCEPT);

    //#define isnan(value) isnan(value) // already defined by gcc !
    //#define isinf(value) isinf(value) // already defined by gcc ! 

    namespace waterspout {
        forcedinline double time_now()
        {
            struct timeval t;
            struct timezone tzp;
            gettimeofday(&t, &tzp);
            return t.tv_sec + t.tv_usec * 1e-6;
        }
    } // end namespace

#elif defined(WATERSPOUT_COMPILER_MSVC)
    #include <intrin.h>
    #include <stdint.h> // TODO - check _MSC_VER
    #include <float.h>

    #define aligned(type_name, alignment) \
        __declspec(align(alignment)) type_name 

    #define forcedinline __forceinline

    #define enable_floating_point_assertions \
        ::_clearfp(); \
        ::_controlfp((unsigned)~(_EM_INVALID | _EM_ZERODIVIDE | \
          _EM_OVERFLOW | _EM_UNDERFLOW |_EM_DENORMAL),  \
          (unsigned)_MCW_EM);
                                                                 
    #define disable_floating_point_assertions \
        ::_clearfp(); \
        ::_controlfp((unsigned)(_EM_INVALID | _EM_ZERODIVIDE |  \
          _EM_OVERFLOW | _EM_UNDERFLOW |_EM_DENORMAL | _EM_INEXACT),  \
          (unsigned)_MCW_EM);

    #define isnan(value) ::_isnan(value)
    #define isinf(value) (!::_finite(value))

    namespace waterspout {
        forcedinline double time_now()
        {
            LARGE_INTEGER t, f;
            QueryPerformanceCounter(&t);
            QueryPerformanceFrequency(&f);
            return double(t.QuadPart) / double(f.QuadPart);
        }
    } // end namespace

#endif


//------------------------------------------------------------------------------

/**
 * Common define helpers
 */

// always false assertion
#define assertfalse \
    do { assert(false); } while(0);

// add this to your float to avoid denormalized numbers
#define antidenormal 1.0E-25f

// fast way to undenormalize a float
#define undernormalize(floatvalue) \
    floatvalue += 1.0E-18f; floatvalue -= 1.0E-18f;


//------------------------------------------------------------------------------

namespace waterspout {


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Helper class to disallow copying classes
 */

namespace noncopyable_detail_
{
    class noncopyable
    {
    protected:
        noncopyable() {}
        ~noncopyable() {}
    private:
        noncopyable(const noncopyable&);
        const noncopyable& operator=(const noncopyable&);
    };
}

typedef noncopyable_detail_::noncopyable noncopyable;


//==============================================================================

//------------------------------------------------------------------------------

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
        short int shortInt_;
        int int_;
        long int longInt_;
        float float_;
        double double_;
        long double longDouble_;
        struct test_struct;
        int test_struct::*pMember_;
        int (test_struct::*pMemberFn_)(int);
    };

public:
    static T* create()
    {
        static max_align staticMemory;

        return new(&staticMemory) T;
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

    static T* pInstance_;
    static bool destroyed_;

    singleton(const singleton &rhs);
    singleton& operator=(const singleton&);

    static void on_dead_reference()
    {
        throw std::runtime_error("dead reference!");
    }

    static void destroy_singleton()
    {
        create_policy<T>::destroy(pInstance_);
        pInstance_ = 0;
        destroyed_ = true;
    }

protected:
    singleton() {}

public:
    static T& instance()
    {
        if (! pInstance_)
        {
            if (! pInstance_)
            {
                if (destroyed_)
                {
                    destroyed_ = false;
                    on_dead_reference();
                }
                else
                {
                    pInstance_ = create_policy<T>::create();

                    // register destruction
                    std::atexit(&destroy_singleton);
                }
            }
        }
        return *pInstance_;
    }
};

template <typename T,
          template <typename U> class create_policy> T* singleton<T, create_policy>::pInstance_ = NULL;
template <typename T,
          template <typename U> class create_policy> bool singleton<T, create_policy>::destroyed_ = false;


//==============================================================================

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
            none = 3
        };

        //typedef boost::unordered_map<std::string, severity_type> severity_map;

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
            /*
            severity_map::iterator it = object_severity_level_.find(object_name);
            if (object_name.empty() || it == object_severity_level_.end())
            {
                return severity_level_;
            }
            else
            {
                return it->second;
            }
            */

            return severity_level_;
        }

        static void set_object_severity(std::string const& object_name,
                                        const severity_type& security_level)
        {
            /*
            if (! object_name.empty())
            {
                object_severity_level_[object_name] = security_level;
            }
            */
        }

        static void clear_object_severity()
        {
            /*
            object_severity_level_.clear();
            */
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
        static std::string str();

        // output
        static void use_file(std::string const& filepath);
        static void use_console();

    private:
        static severity_type severity_level_;
        //static severity_map object_severity_level_;
        static bool severity_env_check_;

        static std::string format_;
        static bool format_env_check_;

        static std::ofstream file_output_;
        static std::string file_name_;
        static std::streambuf* saved_buf_;
    };


    /*
     * Default sink, it regulates access to clog
     */
    template<class Ch, class Tr, class A>
    class clog_sink
    {
    public:
        typedef std::basic_ostringstream<Ch, Tr, A> stream_buffer;

        void operator()(const logger::severity_type& severity, const stream_buffer &s)
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
            if (object_name != NULL)
            {
                object_name_ = object_name;
            }
#endif
        }

        ~base_log()
        {
#if !defined(WATERSPOUT_VOID_LOGGING)
            if (check_severity())
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
            if (object_name != NULL)
            {
                object_name_ = object_name;
            }
        }

        ~base_log_always()
        {
            if (check_severity())
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
    typedef base_log_always<clog_sink, logger::error> base_log_info;

    class debug : public logger_detail_::base_log_debug {
    public:
        debug() : logger_detail_::base_log_debug() {}
        debug(const char* object_name) : logger_detail_::base_log_debug(object_name) {}
    };

    class warn : public logger_detail_::base_log_warn {
    public:
        warn() : logger_detail_::base_log_warn() {}
        warn(const char* object_name) : logger_detail_::base_log_warn(object_name) {}
    };

    class error : public logger_detail_::base_log_error {
    public:
        error() : logger_detail_::base_log_error() {}
        error(const char* object_name) : logger_detail_::base_log_error(object_name) {}
    };

    class info : public logger_detail_::base_log_info {
    public:
        info() : logger_detail_::base_log_info() {}
        info(const char* object_name) : logger_detail_::base_log_info(object_name) {}
    };

} // namespace detail


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
 * The timer class to measure elapsed time and benchmarking
 */

class timer : private noncopyable
{
public:
    timer()
    {
        restart();
    }

    virtual ~timer()
    {
    }

    void restart()
    {
        stopped_ = false;
        clock_start_ = time_now();
        cpu_start_ = clock();
    }

    virtual void stop()
    {
        stopped_ = true;
        cpu_end_ = clock();
        clock_end_ = time_now();
    }

    double cpu_elapsed()
    {
        // return elapsed CPU time in ms
        if (! stopped_)
        {
            stop();
        }

        return ((double) (cpu_end_ - cpu_start_)) / CLOCKS_PER_SEC * 1000.0;
    }

    double clock_elapsed()
    {
        // return elapsed wall clock time in ms
        if (! stopped_)
        {
            stop();
        }

        return (clock_end_ - clock_start_) * 1000.0;
    }

protected:
    double clock_start_, clock_end_;
    clock_t cpu_start_, cpu_end_;
    bool stopped_;
};


//==============================================================================

//------------------------------------------------------------------------------

/**
 * @brief The memory class
 */

class memory : private noncopyable
{
public:

    static void* aligned_alloc(uint32_t size_bytes, uint32_t alignment_bytes)
    {
        return (void*)
#if defined(WATERSPOUT_COMPILER_MSVC)
            ::_aligned_malloc(size_bytes, alignment_bytes);
#elif defined(WATERSPOUT_COMPILER_GCC) || defined(WATERSPOUT_COMPILER_MINGW) || defined(WATERSPOUT_COMPILER_CLANG)
            ::memalign(alignment_bytes, size_bytes);
#endif
    }

    static void aligned_free(void* ptr)
    {
#if defined(WATERSPOUT_COMPILER_MSVC)
        ::_aligned_free(ptr);
#elif defined(WATERSPOUT_COMPILER_GCC) || defined(WATERSPOUT_COMPILER_MINGW) || defined(WATERSPOUT_COMPILER_CLANG)
        ::free(ptr);
#endif
    }
};


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Base aligned buffer memory support
 */

template<class T, uint32_t alignment_bytes=32>
class aligned_buffer : private noncopyable
{
public:
    aligned_buffer()
      : data_(NULL),
        size_(0)
    {
    }

    aligned_buffer(uint32_t size)
      : data_(NULL),
        size_(0)
    {
        allocate(size);
    }

    ~aligned_buffer()
    {
        deallocate();
    }

    void resize(uint32_t size)
    {
        allocate(size);
    }

    forcedinline T& operator[](uint32_t index)
    {
        assert(data_ != NULL);
        assert(index < size_);

        return data_[index];
    }
    
    forcedinline const T& operator[](uint32_t index) const
    {
        assert(data_ != NULL);
        assert(index < size_);

        return data_[index];
    }

    forcedinline T* data()
    {
        return data_;
    }

    forcedinline uint32_t size()
    {
        return size_;
    }

private:
    void allocate(uint32_t size)
    {
        deallocate();

        const uint32_t size_bytes = size * sizeof(T);
        
        data_ = (T*)memory::aligned_alloc(size_bytes, alignment_bytes);
        size_ = size;
    }

    void deallocate()
    {
        if (data_ != NULL)
        {
            memory::aligned_free(data_);
            data_ = NULL;
        }
    }

    T* data_;
    uint32_t size_;
};


typedef aligned_buffer<float, 32> float_buffer;
typedef aligned_buffer<double, 32> double_buffer;


//==============================================================================

//------------------------------------------------------------------------------

/**
 * Base math class interface
 */

class math : private noncopyable
{
public:
    // Define a name for the math implementation
    virtual const char* name() const = 0;

    // Mono buffer manipulation
    virtual void clear_buffer(
        float* srcBuffer,
        uint32_t size) const = 0;

    virtual void scale_buffer(
        float* srcBuffer,
        uint32_t size,
        float gain) const = 0;

    virtual void copy_buffer(
        float* srcBuffer,
        float* dstBuffer,
        uint32_t size) const = 0;

    // Mono buffer arithmetic
    virtual void add_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size) const = 0;

    virtual void subtract_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size) const = 0;

    virtual void multiply_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size) const = 0;

    virtual void divide_buffers(
        float* srcBufferA,
        float* srcBufferB,
        float* dstBuffer,
        uint32_t size) const = 0;


    virtual ~math() { }

protected:
    math() { }
};


//==============================================================================

//------------------------------------------------------------------------------

enum MathFlags
{
    AUTODETECT  =  0,
    FORCE_FPU   =  1,
    FORCE_MMX   =  2,
    FORCE_SSE   =  3,
    FORCE_SSE2  =  4,
    FORCE_SSE3  =  5,
    FORCE_SSSE3 =  6,
    FORCE_SSE41 =  7,
    FORCE_SSE42 =  8,
    FORCE_AVX   =  9,
    FORCE_NEON  = 10
};


//------------------------------------------------------------------------------

/**
 * The math factory class let's you instantiate a math helper class suitable for
 * your own processor, it will scale from faster to slower, falling back to FPU
 * when no other SIMD extensions are found.
 */

class math_factory : private noncopyable
{
public:
    math_factory(int flag=AUTODETECT, bool fallback=true);
    virtual ~math_factory();

    // returns the current arch name
    const char* name() const;

    // operate on the underlying math object
    forcedinline math* operator->() const
    {
        return math_;
    }

protected:

    math* math_;
};


} // end namespace

#endif // __WATERSPOUT_SIMD_ABSTRACTION_FRAMEWORK_H__
