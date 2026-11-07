// SPDX-License-Identifier: MIT

module;

#if defined(_WIN32) && defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
#    include <intrin.h>
#endif

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
        avx512fp16,

        count
    };

    inline constexpr std::string_view FeatureToString(const Feature feature) noexcept {
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
                return {};
        }
    }

#if defined(_M_X64) || defined(__x86_64__) || defined(_M_IX86) || defined(__i386__)

#    if defined(_MSC_VER)

    namespace detail {

        struct CpuIdResult {
            uint32 eax;
            uint32 ebx;
            uint32 ecx;
            uint32 edx;
        };

        inline CpuIdResult Cpuid(const int leaf, const int subleaf = 0) noexcept {
            int registers[4];
            __cpuidex(registers, leaf, subleaf);

            return {static_cast<uint32>(registers[0]), static_cast<uint32>(registers[1]),
                    static_cast<uint32>(registers[2]), static_cast<uint32>(registers[3])};
        }

        constexpr bool Bit(const uint32 value, const uint32 bit) noexcept {
            return (value & (uint32{1} << bit)) != 0;
        }

        struct CpuFeatures {
            std::array<bool, static_cast<usize>(Feature::count)> features{};

            constexpr bool Has(const Feature feature) const noexcept {
                return features[static_cast<usize>(feature)];
            }
        };

        constexpr CpuFeatures DetectCpuFeatures() noexcept {
            CpuFeatures result{};

            const CpuIdResult leaf0 = Cpuid(0);
            const uint32 maxBasicLeaf = leaf0.eax;

            if (maxBasicLeaf < 1)
                return result;

            const CpuIdResult leaf1 = Cpuid(1);

            // Basic SIMD features.
            result.features[static_cast<usize>(Feature::mmx)] = Bit(leaf1.edx, 23);
            result.features[static_cast<usize>(Feature::sse)] = Bit(leaf1.edx, 25);
            result.features[static_cast<usize>(Feature::sse2)] = Bit(leaf1.edx, 26);
            result.features[static_cast<usize>(Feature::sse3)] = Bit(leaf1.ecx, 0);
            result.features[static_cast<usize>(Feature::ssse3)] = Bit(leaf1.ecx, 9);
            result.features[static_cast<usize>(Feature::sse4_1)] = Bit(leaf1.ecx, 19);
            result.features[static_cast<usize>(Feature::sse4_2)] = Bit(leaf1.ecx, 20);
            result.features[static_cast<usize>(Feature::fma)] = Bit(leaf1.ecx, 12);

            const bool cpuAvx = Bit(leaf1.ecx, 28);
            const bool osxSave = Bit(leaf1.ecx, 27);

            // AVX requires both CPU support and OS support for XMM/YMM state.
            bool osAvx = false;
            bool osAvx512 = false;

            if (cpuAvx && osxSave) {
                const uint64 xcr0 = _xgetbv(0);
                // XMM + YMM state.
                osAvx = (xcr0 & 0x6) == 0x6;
                // Opmask + ZMM_Hi256 + Hi16_ZMM.
                osAvx512 = osAvx && (xcr0 & 0xE0) == 0xE0;
            }

            result.features[static_cast<usize>(Feature::avx)] = cpuAvx && osAvx;
            result.features[static_cast<usize>(Feature::fma)] = Bit(leaf1.ecx, 12) && osAvx;

            // Extended AMD features.
            const CpuIdResult extendedLeaf0 = Cpuid(0x80000000);
            const uint32 maxExtendedLeaf = extendedLeaf0.eax;

            if (maxExtendedLeaf >= 0x80000001) {
                const CpuIdResult leaf80000001 = Cpuid(0x80000001);
                result.features[static_cast<usize>(Feature::sse4a)] = Bit(leaf80000001.ecx, 6);
            }

            if (maxBasicLeaf < 7)
                return result;

            // Structured feature enumeration.
            const CpuIdResult leaf7 = Cpuid(7, 0);

            result.features[static_cast<usize>(Feature::avx2)] = osAvx && Bit(leaf7.ebx, 5);

            if (!osAvx512)
                return result;

            result.features[static_cast<usize>(Feature::avx512f)]         = Bit(leaf7.ebx, 16);
            result.features[static_cast<usize>(Feature::avx512dq)]        = Bit(leaf7.ebx, 17);
            result.features[static_cast<usize>(Feature::avx512ifma)]      = Bit(leaf7.ebx, 21);
            result.features[static_cast<usize>(Feature::avx512cd)]        = Bit(leaf7.ebx, 28);
            result.features[static_cast<usize>(Feature::avx512bw)]        = Bit(leaf7.ebx, 30);
            result.features[static_cast<usize>(Feature::avx512vl)]        = Bit(leaf7.ebx, 31);
            result.features[static_cast<usize>(Feature::avx512vbmi)]      = Bit(leaf7.ecx, 1);
            result.features[static_cast<usize>(Feature::avx512vbmi2)]     = Bit(leaf7.ecx, 6);
            result.features[static_cast<usize>(Feature::avx512vnni)]      = Bit(leaf7.ecx, 11);
            result.features[static_cast<usize>(Feature::avx512bitalg)]    = Bit(leaf7.ecx, 12);
            result.features[static_cast<usize>(Feature::avx512vpopcntdq)] = Bit(leaf7.ecx, 14);

            // Leaf 7 subleaf 1.
            if (leaf7.eax >= 1) {
                const CpuIdResult leaf7_1 = Cpuid(7, 1);
                result.features[static_cast<usize>(Feature::avx512bf16)] = Bit(leaf7_1.eax, 5);
            }

            // AVX512_FP16 is enumerated by CPUID.07H:EDX[23].
            result.features[static_cast<usize>(Feature::avx512fp16)] =
                Bit(leaf7.edx, 23) && Bit(leaf7.ebx, 30); // AVX512BW

            return result;
        }

        // Detect once, then reuse forever.
        const CpuFeatures cpuFeatures = DetectCpuFeatures();

    } // namespace detail

    inline bool Has(const Feature feature) noexcept {
        return detail::cpuFeatures.Has(feature);
    }

#    elif defined(__GNUC__) || defined(__clang__)

    inline bool Has(const Feature feature) noexcept {
        switch (feature) {
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

            default:
                return false;
        }
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
