// SPDX-License-Identifier: MIT

export module NE.Engine.Math.Simd.Config;

import NE.Engine.Core.Config;
import NE.Engine.Core.Types;

import std;

export namespace Nexus::Simd {

    enum class Feature {
        mmx,
        sse,
        sse2,
        sse3,
        ssse3,
        sse4_1,
        sse4_2,
        sse4a,
        avx,
        avx2,
        fma,
        avx512f,
        avx512bw,
        avx512cd,
        avx512dq,
        avx512ifma,
        avx512vbmi,
        avx512vbmi2,
        avx512vl,
        avx512vnni,
        avx512vpopcntdq,
        avx512bitalg,
        avx512bf16,
        avx512fp16
    };

    inline std::string FeatureToString(const Feature feature) {
        switch (feature) {
            case Feature::mmx:
                return "mmx";
            case Feature::sse:
                return "sse";
            case Feature::sse2:
                return "sse2";
            case Feature::sse3:
                return "sse3";
            case Feature::ssse3:
                return "ssse3";
            case Feature::sse4_1:
                return "sse4_1";
            case Feature::sse4_2:
                return "sse4_2";
            case Feature::sse4a:
                return "sse4a";
            case Feature::avx:
                return "avx";
            case Feature::avx2:
                return "avx2";
            case Feature::fma:
                return "fma";
            case Feature::avx512f:
                return "avx512f";
            case Feature::avx512bw:
                return "avx512bw";
            case Feature::avx512cd:
                return "avx512cd";
            case Feature::avx512dq:
                return "avx512dq";
            case Feature::avx512ifma:
                return "avx512ifma";
            case Feature::avx512vbmi:
                return "avx512vbmi";
            case Feature::avx512vbmi2:
                return "avx512vbmi2";
            case Feature::avx512vl:
                return "avx512vl";
            case Feature::avx512vnni:
                return "avx512vnni";
            case Feature::avx512vpopcntdq:
                return "avx512vpopcntdq";
            case Feature::avx512bitalg:
                return "avx512bitalg";
            case Feature::avx512bf16:
                return "avx512bf16";
            case Feature::avx512fp16:
                return "avx512fp16";
            default:
                return "";
        }
    }

#if defined(_M_X64) || defined(__x86_64__) || defined(_M_IX86) || defined(__i386__)

#    if defined(_MSC_VER)

#        include <intrin.h>

    struct cpuidResult {
        int eax;
        int ebx;
        int ecx;
        int edx;
    };

    inline cpuidResult CPUID(unsigned leaf, unsigned subleaf = 0) noexcept {
        int r[4];
        __cpuidex(r, static_cast<int>(leaf), static_cast<int>(subleaf));

        return {r[0], r[1], r[2], r[3]};
    }

    inline bool Bit(int32 value, unsigned index) noexcept {
        return (static_cast<uint32>(value) & (uint32{1} << index)) != 0;
    }

    inline unsigned maxBasicLeaf() noexcept {
        return static_cast<unsigned>(CPUID(0).eax);
    }

    inline unsigned maxExtendedLeaf() noexcept {
        return static_cast<unsigned>(CPUID(0x80000000).eax);
    }

    inline bool osAvxStateEnabled() noexcept {
        if (maxBasicLeaf() < 1)
            return false;

        const auto r = CPUID(1);

        // CPUID.1:ECX[27] = OSXSAVE
        // CPUID.1:ECX[28] = AVX
        if (!Bit(r.ecx, 27) || !Bit(r.ecx, 28))
            return false;

        // XCR0[1:2] = XMM and YMM state enabled by the OS.
        return (_xgetbv(0) & 0x6) == 0x6;
    }

    inline bool osAvx512StateEnabled() noexcept {
        if (!osAvxStateEnabled())
            return false;

        // XCR0[5:7] = opmask, ZMM_hi256, Hi16_ZMM.
        return (_xgetbv(0) & 0xE0) == 0xE0;
    }

    inline bool Has(Feature f) noexcept {
        const unsigned basicMax = maxBasicLeaf();

        switch (f) {
            case Feature::mmx:
                return basicMax >= 1 && Bit(CPUID(1).edx, 23);

            case Feature::sse:
                return basicMax >= 1 && Bit(CPUID(1).edx, 25);

            case Feature::sse2:
                return basicMax >= 1 && Bit(CPUID(1).edx, 26);

            case Feature::sse3:
                return basicMax >= 1 && Bit(CPUID(1).ecx, 0);

            case Feature::ssse3:
                return basicMax >= 1 && Bit(CPUID(1).ecx, 9);

            case Feature::sse4_1:
                return basicMax >= 1 && Bit(CPUID(1).ecx, 19);

            case Feature::sse4_2:
                return basicMax >= 1 && Bit(CPUID(1).ecx, 20);

            case Feature::sse4a:
                return maxExtendedLeaf() >= 0x80000001 && Bit(CPUID(0x80000001).ecx, 6);

            case Feature::avx:
                return basicMax >= 1 && Bit(CPUID(1).ecx, 28) && osAvxStateEnabled();

            case Feature::fma:
                return basicMax >= 1 && Bit(CPUID(1).ecx, 12) && osAvxStateEnabled();

            case Feature::avx2:
                return basicMax >= 7 && osAvxStateEnabled() && Bit(CPUID(7, 0).ebx, 5);

            case Feature::avx512f:
                return basicMax >= 7 && osAvx512StateEnabled() && Bit(CPUID(7, 0).ebx, 16);

            case Feature::avx512dq:
                return basicMax >= 7 && osAvx512StateEnabled() && Bit(CPUID(7, 0).ebx, 17);

            case Feature::avx512ifma:
                return basicMax >= 7 && osAvx512StateEnabled() && Bit(CPUID(7, 0).ebx, 21);

            case Feature::avx512cd:
                return basicMax >= 7 && osAvx512StateEnabled() && Bit(CPUID(7, 0).ebx, 28);

            case Feature::avx512bw:
                return basicMax >= 7 && osAvx512StateEnabled() && Bit(CPUID(7, 0).ebx, 30);

            case Feature::avx512vl:
                return basicMax >= 7 && osAvx512StateEnabled() && Bit(CPUID(7, 0).ebx, 31);

            case Feature::avx512vbmi:
                return basicMax >= 7 && osAvx512StateEnabled() && Bit(CPUID(7, 0).ecx, 1);

            case Feature::avx512vbmi2:
                return basicMax >= 7 && osAvx512StateEnabled() && Bit(CPUID(7, 0).ecx, 6);

            case Feature::avx512vnni:
                return basicMax >= 7 && osAvx512StateEnabled() && Bit(CPUID(7, 0).ecx, 11);

            case Feature::avx512bitalg:
                return basicMax >= 7 && osAvx512StateEnabled() && Bit(CPUID(7, 0).ecx, 12);

            case Feature::avx512vpopcntdq:
                return basicMax >= 7 && osAvx512StateEnabled() && Bit(CPUID(7, 0).ecx, 14);

            case Feature::avx512bf16:
                return basicMax >= 7 && osAvx512StateEnabled() && Bit(CPUID(7, 1).eax, 5);

            case Feature::avx512fp16:
                // See: https://cdrdv2-public.intel.com/678970/intel-avx512-fp16.pdf (chapter 2, needs avx512bw)
                return basicMax >= 7 && osAvx512StateEnabled() && Bit(CPUID(7, 0).eax, 23) && Bit(CPUID(7, 0).ebx, 30);
        }

        return false;
    }

#    elif defined(__GNUC__) || defined(__clang__)

    inline bool Has(Feature f) noexcept {
        switch (f) {
            case Feature::mmx:
                return __builtin_cpu_supports("mmx");

            case Feature::sse:
                return __builtin_cpu_supports("sse");

            case Feature::sse2:
                return __builtin_cpu_supports("sse2");

            case Feature::sse3:
                return __builtin_cpu_supports("sse3");

            case Feature::ssse3:
                return __builtin_cpu_supports("ssse3");

            case Feature::sse4_1:
                return __builtin_cpu_supports("sse4.1");

            case Feature::sse4_2:
                return __builtin_cpu_supports("sse4.2");

            case Feature::sse4a:
                return __builtin_cpu_supports("sse4a");

            case Feature::avx:
                return __builtin_cpu_supports("avx");

            case Feature::avx2:
                return __builtin_cpu_supports("avx2");

            case Feature::fma:
                return __builtin_cpu_supports("fma");

            case Feature::avx512f:
                return __builtin_cpu_supports("avx512f");

            case Feature::avx512bw:
                return __builtin_cpu_supports("avx512bw");

            case Feature::avx512cd:
                return __builtin_cpu_supports("avx512cd");

            case Feature::avx512dq:
                return __builtin_cpu_supports("avx512dq");

            case Feature::avx512ifma:
                return __builtin_cpu_supports("avx512ifma");

            case Feature::avx512vbmi:
                return __builtin_cpu_supports("avx512vbmi");

            case Feature::avx512vbmi2:
                return __builtin_cpu_supports("avx512vbmi2");

            case Feature::avx512vl:
                return __builtin_cpu_supports("avx512vl");

            case Feature::avx512vnni:
                return __builtin_cpu_supports("avx512vnni");

            case Feature::avx512vpopcntdq:
                return __builtin_cpu_supports("avx512vpopcntdq");

            case Feature::avx512bitalg:
                return __builtin_cpu_supports("avx512bitalg");

            case Feature::avx512bf16:
                return __builtin_cpu_supports("avx512bf16");

            case Feature::avx512fp16:
                // See: https://cdrdv2-public.intel.com/678970/intel-avx512-fp16.pdf (chapter 2, needs avx512bw)
                return __builtin_cpu_supports("avx512bw") && __builtin_cpu_supports("avx512fp16");
        }

        return false;
    }

#    else

    inline bool Has(Feature) noexcept {
        return false;
    }

#    endif

#else

    inline bool Has(Feature) noexcept {
        return false;
    }

#endif

} // namespace Nexus::Simd
