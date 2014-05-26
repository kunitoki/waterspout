![ScreenShot](https://raw.github.com/kunitoki/waterspout/master/waterspout.png)

waterspout
==========

[![Build Status](https://travis-ci.org/kunitoki/waterspout.svg?branch=master)](https://travis-ci.org/kunitoki/waterspout)

Waterspout is a SIMD abstraction library wrapper especially crafted for
audio/image manipulation.

This library is born with the purpose of helping in optimization when developing
audio and imaging applications where you process a lot of data and want to make
usage of the streaming instructions of the newer cpu: the purpose is to abstract
as much as possible from the underlying implementation and leave the developer
with a single, smooth and slick lightweight interface to process buffers (copy,
clear, scale, apply panning, dry/wet processing, filtering and so on) as faster
as possible.

Waterspout will check the CPU and choose the best streaming instruction
set suited for that particular run of the application. It's also possible to
opt-in or out at compile time which kind of instructions to enable, and with
the software FPU fallback it's possible to run the same code on different and
even older CPUs.


Strong points
-------------
  * simple and clear
  * accurate
  * fast
  * automatically detect the best configuration based on the running CPU features
  * drop in include in your project (2 files only needed to be included)
  * no dependencies on third party libraries
  * extensive unit testing for all operations


Usage
-----

```C++
#include <waterspout.h>

int main(int argc, char* argv[])
{
  waterspout::math m;
  waterspout::float_buffer bufferA(100), bufferB(100);

  for (int i = 0; i < bufferA.size(); ++i)
  {
    bufferA[i] = i / 100.0f;
  }

  m->clear_buffer_float(bufferB.data(), 100);
  
  m->copy_buffer_float(bufferA.data(), bufferB.data(), 100);
}
```

References
----------

General:
  * http://software.intel.com/sites/products/documentation/studio/composer/en-us/2011Update/compiler_c/intref_cls/common/intref_bk_intrinsics.htm
  * http://softpixel.com/~cwright/programming/simd/index.php

MMX:
  * http://msdn.microsoft.com/it-it/library/ccky3awe(v=vs.90).aspx
  * http://en.wikipedia.org/wiki/MMX_(instruction_set)
  
SSE:
  * http://msdn.microsoft.com/it-it/library/t467de55(v=vs.90).aspx
  * http://en.wikipedia.org/wiki/Streaming_SIMD_Extensions

SSE2:
  * http://msdn.microsoft.com/it-it/library/kcwz153a(v=vs.90).aspx
  * http://en.wikipedia.org/wiki/SSE2

SSE3:
  * http://software.intel.com/sites/products/documentation/studio/composer/en-us/2011Update/compiler_c/intref_cls/common/intref_sse3_overview.htm
  * http://en.wikipedia.org/wiki/SSE3

SSSE3:
  * http://en.wikipedia.org/wiki/SSSE3

SSE4 (SSE41 - SSE42 - SSE4A):
  * http://msdn.microsoft.com/it-it/library/bb892950(v=vs.90).aspx
  * http://msdn.microsoft.com/it-it/library/bb892945(v=vs.90).aspx
  * http://en.wikipedia.org/wiki/SSE4

AVX:
  * http://en.wikipedia.org/wiki/Advanced_Vector_Extensions
