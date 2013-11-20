#include <waterspout.h>
#include <iostream>

using namespace waterspout;

void check_buffers(const char* method, float* a, float*b, uint32_t size)
{
    for (int i = 0; i < size; ++i)
    {
        if (a[i] != b[i])
        {
            std::cout << method << ": Copy is invalid !" << std::endl;
        }
    }
}

int main(int argc, char* argv[])
{
    const uint32_t size = 1024 * 1024;
    
    {
        math_factory fpu(FORCE_FPU);
        float_buffer srcBuffer(size), dstBuffer(size);

        for (int i = 0; i < srcBuffer.size(); ++i)
        {
            srcBuffer[i] = i / (float)srcBuffer.size();
            dstBuffer[i] = 0.0f;
        }

        timer t;
        fpu->copy_buffer(srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        double elapsed = t.clock_elapsed();

        check_buffers(fpu.name(), srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        std::cout << fpu.name() << ": " << std::setprecision(32) <<  elapsed << " " << srcBuffer.size() << std::endl;
    }

    {
        math_factory sse(FORCE_SSE);
        float_buffer srcBuffer(size), dstBuffer(size);

        for (int i = 0; i < srcBuffer.size(); ++i)
        {
            srcBuffer[i] = i / (float)srcBuffer.size();
            dstBuffer[i] = 0.0f;
        }

        timer t;
        sse->copy_buffer(srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        double elapsed = t.clock_elapsed();
        
        check_buffers(sse.name(), srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        std::cout << sse.name() << ": " << std::setprecision(32) << elapsed << " " << srcBuffer.size() << std::endl;
    }

    {
        math_factory avx(FORCE_AVX);
        float_buffer srcBuffer(size), dstBuffer(size);

        for (int i = 0; i < srcBuffer.size(); ++i)
        {
            srcBuffer[i] = i / (float)srcBuffer.size();
            dstBuffer[i] = 0.0f;
        }

        timer t;
        avx->copy_buffer(srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        double elapsed = t.clock_elapsed();
        
        check_buffers(avx.name(), srcBuffer.data(), dstBuffer.data(), srcBuffer.size());
        std::cout << avx.name() << ": " << std::setprecision(32) << elapsed << " " << srcBuffer.size() << std::endl;
    }
    
    return 0;
}
