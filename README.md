waterspout
==========

[![Build Status](https://api.travis-ci.org/kunitoki/waterspout.png)](https://travis-ci.org/kunitoki/waterspout)

simd abstraction library especially creafted for audio/image manipulation

This library is born with the purpose of helping in optimization when developing
audio and imaging applications where you process a lot of data and want to make
usage of the streaming instructions of the newer cpu: the purpose is to abstract
as much as possible from the underlying implementation and leave the developer
with a single, smooth and slick lightweight interface to process buffers (copy,
clear, scale, apply panning, dry/wet processing) as faster as possible.

Waterspout will check the CPU and choose the best streaming instruction
set suited for that particular run of the application. It's also possible to
opt-in or out at compile time which kind of instructions to enable, and with
the software FPU fallback it's possible to run the same code on different and
even old CPUs.
