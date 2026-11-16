// SPDX-License-Identifier: MIT

module;

#include <immintrin.h>

#if defined(_MSC_VER)
#    define FORCEINLINE __forceinline
#else
#    define FORCEINLINE inline __attribute__((always_inline))
#endif

export module NE.Engine.Math.Simd.Traits;

import NE.Engine.Core.Types;

import std;

#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-WTU-local-entity-exposure"
#endif

export namespace Nexus::Simd {

    // Bit widths: 256 (AVX/AVX2), 128 (SSE/SSE2/SSE4.1, AVX2 gather) and 0 (plain scalar, width 1).
    // Everything below is AVX2 at most (no AVX-512).
    //
    // Common interface of every specialization:
    //   type, mask_type, width
    //   load / store / from / set1 / add / sub / mul / div / sqrt
    //   neg(a)                      -a, flips the sign bit (keeps -0.0 / NaN behaviour of unary minus)
    //   min(a, b) / max(a, b)       per lane, same NaN behaviour as _mm_min/max (returns b if either is NaN)
    //   gather(base, idx)           lane k = base[idx[k]]  (idx points to `width` int32 indices)
    //   reduce_add(a)               horizontal sum of all lanes
    //   all_equal(a, b)             true when every lane of a == b
    //   compareEqual(a, b)          per-lane mask (all bits set when equal)
    //   cmp_lt / cmp_le / cmp_gt / cmp_ge(a, b)   per-lane ordered, quiet comparisons -> mask
    //   movemask(mask)              one bit per lane (bit k = lane k is set)
    //   select(a, b, mask)          lane k = b[k] when mask lane k is set, a[k] otherwise
    //   permute<I...>(a)            lane k = a[Ik]            (`width` indices)
    //   blend<Mask>(a, b)           lane k = b[k] when bit k of Mask is set, a[k] otherwise
    //
    // Width-specific constructors (the name carries the lane count):
    //   double/128 : set2(a, b)
    //   float/128  : set4(a, b, c, d)
    //   double/256 : set4(a, b, c, d)
    //   float/256  : set8(a, b, c, d, e, f, g, h)
    //   scalar     : set1(a)
    //
    // Notes:
    //   - from(v) takes the vector by reference and returns a pointer to its lanes (the by-value
    //     version returned a pointer to a dead parameter). The pointer is only valid as long as `v`.
    //   - mask_type is the vector type for SIMD widths and bool for the scalar width, so
    //     select(a, b, cmp_lt(a, b)) compiles unchanged in generic code.

    template <std::floating_point T, usize N>
    struct simd_traits;

    template <>
    struct simd_traits<float64, 128> {
        using type = __m128d;
        using mask_type = __m128d;
        static constexpr usize width = 2;

        static FORCEINLINE type load(const float64* data) {
            return _mm_loadu_pd(data);
        }

        static FORCEINLINE void store(float64* data, type vec) {
            _mm_storeu_pd(data, vec);
        }

        static FORCEINLINE float64* from(type& data) {
            return reinterpret_cast<float64*>(&data);
        }

        static FORCEINLINE type set1(float64 scalar) {
            return _mm_set1_pd(scalar);
        }

        static FORCEINLINE type set2(float64 a, float64 b) {
            return _mm_set_pd(b, a);
        }

        static FORCEINLINE type add(type a, type b) {
            return _mm_add_pd(a, b);
        }

        static FORCEINLINE type sub(type a, type b) {
            return _mm_sub_pd(a, b);
        }

        static FORCEINLINE type mul(type a, type b) {
            return _mm_mul_pd(a, b);
        }

        static FORCEINLINE type div(type a, type b) {
            return _mm_div_pd(a, b);
        }

        static FORCEINLINE type sqrt(type a) {
            return _mm_sqrt_pd(a);
        }

        static FORCEINLINE type neg(type a) {
            return _mm_xor_pd(a, _mm_set1_pd(-0.0));
        }

        static FORCEINLINE type min(type a, type b) {
            return _mm_min_pd(a, b);
        }

        static FORCEINLINE type max(type a, type b) {
            return _mm_max_pd(a, b);
        }

        template <int I0, int I1>
        static FORCEINLINE type permute(type a) {
            static_assert(I0 >= 0 && I0 < 2 && I1 >= 0 && I1 < 2, "permute index out of range");
            return _mm_shuffle_pd(a, a, I0 | (I1 << 1));
        }

        template <int Mask>
        static FORCEINLINE type blend(type a, type b) {
            static_assert(Mask >= 0 && Mask < 4, "blend mask must fit in 2 bits");
            return _mm_blend_pd(a, b, Mask);
        }

        static FORCEINLINE type gather(const float64* base, const int32* idx) {
            // only 2 indices (8 bytes) may be read here
            return _mm_i32gather_pd(base, _mm_loadl_epi64(reinterpret_cast<const __m128i*>(idx)), 8);
        }

        static FORCEINLINE float64 reduce_add(type a) {
            return _mm_cvtsd_f64(_mm_add_sd(a, _mm_unpackhi_pd(a, a)));
        }

        static FORCEINLINE bool all_equal(type a, type b) {
            return _mm_movemask_pd(_mm_cmpeq_pd(a, b)) == 0x3;
        }

        static FORCEINLINE mask_type compareEqual(type a, type b) {
            return _mm_cmpeq_pd(a, b);
        }

        static FORCEINLINE mask_type cmp_lt(type a, type b) {
            return _mm_cmp_pd(a, b, _CMP_LT_OQ);
        }

        static FORCEINLINE mask_type cmp_le(type a, type b) {
            return _mm_cmp_pd(a, b, _CMP_LE_OQ);
        }

        static FORCEINLINE mask_type cmp_gt(type a, type b) {
            return _mm_cmp_pd(a, b, _CMP_GT_OQ);
        }

        static FORCEINLINE mask_type cmp_ge(type a, type b) {
            return _mm_cmp_pd(a, b, _CMP_GE_OQ);
        }

        static FORCEINLINE int32 movemask(mask_type mask) {
            return _mm_movemask_pd(mask);
        }

        static FORCEINLINE type select(type a, type b, mask_type mask) {
            // blendv picks b where the sign bit of the mask lane is set: one instruction instead of and/andnot/or
            return _mm_blendv_pd(a, b, mask);
        }
    };

    template <>
    struct simd_traits<float32, 128> {
        using type = __m128;
        using mask_type = __m128;
        static constexpr usize width = 4;

        static FORCEINLINE type load(const float32* data) {
            return _mm_loadu_ps(data);
        }

        static FORCEINLINE void store(float32* data, type vec) {
            _mm_storeu_ps(data, vec);
        }

        static FORCEINLINE float32* from(type& data) {
            return reinterpret_cast<float32*>(&data);
        }

        static FORCEINLINE type set1(float32 scalar) {
            return _mm_set1_ps(scalar);
        }

        static FORCEINLINE type set4(float32 a, float32 b, float32 c, float32 d) {
            return _mm_set_ps(d, c, b, a);
        }

        static FORCEINLINE type add(type a, type b) {
            return _mm_add_ps(a, b);
        }

        static FORCEINLINE type sub(type a, type b) {
            return _mm_sub_ps(a, b);
        }

        static FORCEINLINE type mul(type a, type b) {
            return _mm_mul_ps(a, b);
        }

        static FORCEINLINE type div(type a, type b) {
            return _mm_div_ps(a, b);
        }

        static FORCEINLINE type sqrt(type a) {
            return _mm_sqrt_ps(a);
        }

        static FORCEINLINE type neg(type a) {
            return _mm_xor_ps(a, _mm_set1_ps(-0.0f));
        }

        static FORCEINLINE type min(type a, type b) {
            return _mm_min_ps(a, b);
        }

        static FORCEINLINE type max(type a, type b) {
            return _mm_max_ps(a, b);
        }

        template <int I0, int I1, int I2, int I3>
        static FORCEINLINE type permute(type a) {
            static_assert(I0 >= 0 && I0 < 4 && I1 >= 0 && I1 < 4 && I2 >= 0 && I2 < 4 && I3 >= 0 && I3 < 4,
                          "permute index out of range");
            return _mm_shuffle_ps(a, a, _MM_SHUFFLE(I3, I2, I1, I0));
        }

        template <int Mask>
        static FORCEINLINE type blend(type a, type b) {
            static_assert(Mask >= 0 && Mask < 16, "blend mask must fit in 4 bits");
            return _mm_blend_ps(a, b, Mask);
        }

        static FORCEINLINE type gather(const float32* base, const int32* idx) {
            return _mm_i32gather_ps(base, _mm_loadu_si128(reinterpret_cast<const __m128i*>(idx)), 4);
        }

        static FORCEINLINE float32 reduce_add(type a) {
            type t = _mm_add_ps(a, _mm_movehl_ps(a, a));
            t = _mm_add_ss(t, _mm_shuffle_ps(t, t, 0x55));
            return _mm_cvtss_f32(t);
        }

        static FORCEINLINE bool all_equal(type a, type b) {
            return _mm_movemask_ps(_mm_cmpeq_ps(a, b)) == 0xF;
        }

        static FORCEINLINE mask_type compareEqual(type a, type b) {
            return _mm_cmpeq_ps(a, b);
        }

        static FORCEINLINE mask_type cmp_lt(type a, type b) {
            return _mm_cmp_ps(a, b, _CMP_LT_OQ);
        }

        static FORCEINLINE mask_type cmp_le(type a, type b) {
            return _mm_cmp_ps(a, b, _CMP_LE_OQ);
        }

        static FORCEINLINE mask_type cmp_gt(type a, type b) {
            return _mm_cmp_ps(a, b, _CMP_GT_OQ);
        }

        static FORCEINLINE mask_type cmp_ge(type a, type b) {
            return _mm_cmp_ps(a, b, _CMP_GE_OQ);
        }

        static FORCEINLINE int32 movemask(mask_type mask) {
            return _mm_movemask_ps(mask);
        }

        static FORCEINLINE type select(type a, type b, mask_type mask) {
            return _mm_blendv_ps(a, b, mask);
        }
    };

    template <>
    struct simd_traits<float32, 256> {
        using type = __m256;
        using mask_type = __m256;
        static constexpr usize width = 8;

        static FORCEINLINE type load(const float32* data) {
            return _mm256_loadu_ps(data);
        }

        static FORCEINLINE void store(float32* data, type vec) {
            _mm256_storeu_ps(data, vec);
        }

        static FORCEINLINE float32* from(type& data) {
            return reinterpret_cast<float32*>(&data);
        }

        static FORCEINLINE type set1(float32 scalar) {
            return _mm256_set1_ps(scalar);
        }

        static FORCEINLINE type set8(float32 a, float32 b, float32 c, float32 d, float32 e, float32 f, float32 g,
                                     float32 h) {
            return _mm256_set_ps(h, g, f, e, d, c, b, a);
        }

        static FORCEINLINE type add(type a, type b) {
            return _mm256_add_ps(a, b);
        }

        static FORCEINLINE type sub(type a, type b) {
            return _mm256_sub_ps(a, b);
        }

        static FORCEINLINE type mul(type a, type b) {
            return _mm256_mul_ps(a, b);
        }

        static FORCEINLINE type div(type a, type b) {
            return _mm256_div_ps(a, b);
        }

        static FORCEINLINE type sqrt(type a) {
            return _mm256_sqrt_ps(a);
        }

        static FORCEINLINE type neg(type a) {
            return _mm256_xor_ps(a, _mm256_set1_ps(-0.0f));
        }

        static FORCEINLINE type min(type a, type b) {
            return _mm256_min_ps(a, b);
        }

        static FORCEINLINE type max(type a, type b) {
            return _mm256_max_ps(a, b);
        }

        // Full cross-lane permutation (AVX2 vpermps): lane k = a[Ik]
        template <int I0, int I1, int I2, int I3, int I4, int I5, int I6, int I7>
        static FORCEINLINE type permute(type a) {
            static_assert(I0 >= 0 && I0 < 8 && I1 >= 0 && I1 < 8 && I2 >= 0 && I2 < 8 && I3 >= 0 && I3 < 8 && I4 >= 0 &&
                              I4 < 8 && I5 >= 0 && I5 < 8 && I6 >= 0 && I6 < 8 && I7 >= 0 && I7 < 8,
                          "permute index out of range");
            return _mm256_permutevar8x32_ps(a, _mm256_setr_epi32(I0, I1, I2, I3, I4, I5, I6, I7));
        }

        template <int Mask>
        static FORCEINLINE type blend(type a, type b) {
            static_assert(Mask >= 0 && Mask < 256, "blend mask must fit in 8 bits");
            return _mm256_blend_ps(a, b, Mask);
        }

        static FORCEINLINE type gather(const float32* base, const int32* idx) {
            return _mm256_i32gather_ps(base, _mm256_loadu_si256(reinterpret_cast<const __m256i*>(idx)), 4);
        }

        static FORCEINLINE float32 reduce_add(type a) {
            const __m128 sum = _mm_add_ps(_mm256_castps256_ps128(a), _mm256_extractf128_ps(a, 1));
            return simd_traits<float32, 128>::reduce_add(sum);
        }

        static FORCEINLINE bool all_equal(type a, type b) {
            return _mm256_movemask_ps(_mm256_cmp_ps(a, b, _CMP_EQ_OQ)) == 0xFF;
        }

        static FORCEINLINE mask_type compareEqual(type a, type b) {
            return _mm256_cmp_ps(a, b, _CMP_EQ_OQ);
        }

        static FORCEINLINE mask_type cmp_lt(type a, type b) {
            return _mm256_cmp_ps(a, b, _CMP_LT_OQ);
        }

        static FORCEINLINE mask_type cmp_le(type a, type b) {
            return _mm256_cmp_ps(a, b, _CMP_LE_OQ);
        }

        static FORCEINLINE mask_type cmp_gt(type a, type b) {
            return _mm256_cmp_ps(a, b, _CMP_GT_OQ);
        }

        static FORCEINLINE mask_type cmp_ge(type a, type b) {
            return _mm256_cmp_ps(a, b, _CMP_GE_OQ);
        }

        static FORCEINLINE int32 movemask(mask_type mask) {
            return _mm256_movemask_ps(mask);
        }

        static FORCEINLINE type select(type a, type b, mask_type mask) {
            return _mm256_blendv_ps(a, b, mask);
        }
    };

    template <>
    struct simd_traits<float64, 256> {
        using type = __m256d;
        using mask_type = __m256d;
        static constexpr usize width = 4;

        static FORCEINLINE type load(const float64* data) {
            return _mm256_loadu_pd(data);
        }

        static FORCEINLINE void store(float64* data, type vec) {
            _mm256_storeu_pd(data, vec);
        }

        static FORCEINLINE float64* from(type& data) {
            return reinterpret_cast<float64*>(&data);
        }

        static FORCEINLINE type set1(float64 scalar) {
            return _mm256_set1_pd(scalar);
        }

        static FORCEINLINE type set4(float64 a, float64 b, float64 c, float64 d) {
            return _mm256_set_pd(d, c, b, a);
        }

        static FORCEINLINE type add(type a, type b) {
            return _mm256_add_pd(a, b);
        }

        static FORCEINLINE type sub(type a, type b) {
            return _mm256_sub_pd(a, b);
        }

        static FORCEINLINE type mul(type a, type b) {
            return _mm256_mul_pd(a, b);
        }

        static FORCEINLINE type div(type a, type b) {
            return _mm256_div_pd(a, b);
        }

        static FORCEINLINE type sqrt(type a) {
            return _mm256_sqrt_pd(a);
        }

        static FORCEINLINE type neg(type a) {
            return _mm256_xor_pd(a, _mm256_set1_pd(-0.0));
        }

        static FORCEINLINE type min(type a, type b) {
            return _mm256_min_pd(a, b);
        }

        static FORCEINLINE type max(type a, type b) {
            return _mm256_max_pd(a, b);
        }

        template <int I0, int I1, int I2, int I3>
        static FORCEINLINE type permute(type a) {
            static_assert(I0 >= 0 && I0 < 4 && I1 >= 0 && I1 < 4 && I2 >= 0 && I2 < 4 && I3 >= 0 && I3 < 4,
                          "permute index out of range");
            return _mm256_permute4x64_pd(a, _MM_SHUFFLE(I3, I2, I1, I0));
        }

        template <int Mask>
        static FORCEINLINE type blend(type a, type b) {
            static_assert(Mask >= 0 && Mask < 16, "blend mask must fit in 4 bits");
            return _mm256_blend_pd(a, b, Mask);
        }

        static FORCEINLINE type gather(const float64* base, const int32* idx) {
            return _mm256_i32gather_pd(base, _mm_loadu_si128(reinterpret_cast<const __m128i*>(idx)), 8);
        }

        static FORCEINLINE float64 reduce_add(type a) {
            const __m128d sum = _mm_add_pd(_mm256_castpd256_pd128(a), _mm256_extractf128_pd(a, 1));
            return simd_traits<float64, 128>::reduce_add(sum);
        }

        static FORCEINLINE bool all_equal(type a, type b) {
            return _mm256_movemask_pd(_mm256_cmp_pd(a, b, _CMP_EQ_OQ)) == 0xF;
        }

        static FORCEINLINE mask_type compareEqual(type a, type b) {
            return _mm256_cmp_pd(a, b, _CMP_EQ_OQ);
        }

        static FORCEINLINE mask_type cmp_lt(type a, type b) {
            return _mm256_cmp_pd(a, b, _CMP_LT_OQ);
        }

        static FORCEINLINE mask_type cmp_le(type a, type b) {
            return _mm256_cmp_pd(a, b, _CMP_LE_OQ);
        }

        static FORCEINLINE mask_type cmp_gt(type a, type b) {
            return _mm256_cmp_pd(a, b, _CMP_GT_OQ);
        }

        static FORCEINLINE mask_type cmp_ge(type a, type b) {
            return _mm256_cmp_pd(a, b, _CMP_GE_OQ);
        }

        static FORCEINLINE int32 movemask(mask_type mask) {
            return _mm256_movemask_pd(mask);
        }

        static FORCEINLINE type select(type a, type b, mask_type mask) {
            return _mm256_blendv_pd(a, b, mask);
        }
    };

    // Bit width 0 = scalar "batch" of one element: lets the same generic code handle the tail of a loop.
    template <std::floating_point T>
    struct simd_traits<T, 0> {
        using type = T;
        using mask_type = bool;
        static constexpr usize width = 1;

        static constexpr FORCEINLINE type load(const T* data) {
            return *data;
        }

        static constexpr FORCEINLINE void store(T* data, type vec) {
            *data = vec;
        }

        static constexpr FORCEINLINE T* from(type& data) {
            return &data;
        }

        static constexpr FORCEINLINE type set1(T scalar) {
            return scalar;
        }

        static constexpr FORCEINLINE type add(type a, type b) {
            return a + b;
        }

        static constexpr FORCEINLINE type sub(type a, type b) {
            return a - b;
        }

        static constexpr FORCEINLINE type mul(type a, type b) {
            return a * b;
        }

        static constexpr FORCEINLINE type div(type a, type b) {
            return a / b;
        }

        static constexpr FORCEINLINE type sqrt(type a) {
            return std::sqrt(a);
        }

        static constexpr FORCEINLINE type neg(type a) {
            return -a;
        }

        // Same semantics as _mm_min/max: returns b when a and b are unordered (NaN)
        static constexpr FORCEINLINE type min(type a, type b) {
            return a < b ? a : b;
        }

        static constexpr FORCEINLINE type max(type a, type b) {
            return a > b ? a : b;
        }

        template <int I0>
        static constexpr FORCEINLINE type permute(type a) {
            static_assert(I0 == 0, "permute index out of range");
            return a;
        }

        template <int Mask>
        static constexpr FORCEINLINE type blend(type a, type b) {
            static_assert(Mask >= 0 && Mask < 2, "blend mask must fit in 1 bit");
            return (Mask & 1) ? b : a;
        }

        static constexpr FORCEINLINE type gather(const T* base, const int32* idx) {
            return base[idx[0]];
        }

        static constexpr FORCEINLINE T reduce_add(type a) {
            return a;
        }

        static constexpr FORCEINLINE bool all_equal(type a, type b) {
            return a == b;
        }

        static constexpr FORCEINLINE mask_type compareEqual(type a, type b) {
            return a == b;
        }

        static constexpr FORCEINLINE mask_type cmp_lt(type a, type b) {
            return a < b;
        }

        static constexpr FORCEINLINE mask_type cmp_le(type a, type b) {
            return a <= b;
        }

        static constexpr FORCEINLINE mask_type cmp_gt(type a, type b) {
            return a > b;
        }

        static constexpr FORCEINLINE mask_type cmp_ge(type a, type b) {
            return a >= b;
        }

        static constexpr FORCEINLINE int32 movemask(mask_type mask) {
            return mask ? 1 : 0;
        }

        static constexpr FORCEINLINE type select(type a, type b, mask_type mask) {
            return mask ? b : a;
        }
    };
} // namespace Nexus::Simd

#if defined(__clang__)
#    pragma clang diagnostic pop
#endif
