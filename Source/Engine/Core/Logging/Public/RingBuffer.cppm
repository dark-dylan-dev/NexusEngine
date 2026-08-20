// SPDX-License-Identifier: MIT

export module NE.Engine.Core.Log.RingBuffer;

import NE.Engine.Core.Types;

import std;

export namespace Nexus {

    template <typename T, usize N>
    class RingBuffer {
    public:
        RingBuffer() {
            for (usize i = 0; i < N; ++i) {
                m_Sequences[i].store(i, std::memory_order_relaxed);
            }
            m_WriteIndex.store(0, std::memory_order_relaxed);
            m_ReadIndex.store(0, std::memory_order_relaxed);
        }
        ~RingBuffer() = default;

        RingBuffer(const RingBuffer&) = delete;
        RingBuffer& operator=(const RingBuffer&) = delete;

        [[nodiscard]] bool Push(T&& message) {
            usize pos = m_WriteIndex.load(std::memory_order_relaxed);

            while (true) {
                const usize idx = pos % N;
                const usize seq = m_Sequences[idx].load(std::memory_order_acquire);
                const iptr diff = static_cast<iptr>(seq) - static_cast<iptr>(pos);

                if (diff == 0) {
                    if (m_WriteIndex.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed,
                                                           std::memory_order_relaxed)) {
                        // We own this slot
                        m_Data[idx] = std::move(message);

                        // Publish
                        m_Sequences[idx].store(pos + 1, std::memory_order_release);

                        return true;
                    }
                } else if (diff < 0) {
                    // Queue is full
                    return false;
                } else {
                    pos = m_WriteIndex.load(std::memory_order_relaxed);
                }
            }
        }

        [[nodiscard]] bool Pop(T& outMessage) {
            usize pos = m_ReadIndex.load(std::memory_order_relaxed);

            while (true) {
                const usize idx = pos % N;
                const usize seq = m_Sequences[idx].load(std::memory_order_acquire);
                const iptr diff = static_cast<iptr>(seq) - static_cast<iptr>(pos + 1);

                if (diff == 0) {
                    if (m_ReadIndex.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed,
                                                          std::memory_order_relaxed)) {
                        // We own this slot
                        outMessage = std::move(m_Data[idx]);

                        // Release the slot for the next producer
                        m_Sequences[idx].store(pos + N, std::memory_order_release);

                        return true;
                    }
                } else if (diff < 0) {
                    // Queue is empty
                    return false;
                } else {
                    pos = m_ReadIndex.load(std::memory_order_relaxed);
                }
            }
        }

        usize Size() const {
            const usize w = m_WriteIndex.load(std::memory_order_acquire);
            const usize r = m_ReadIndex.load(std::memory_order_acquire);
            return w - r;
        }

        constexpr usize Capacity() const {
            return N;
        }

    private:
        static constexpr usize CACHELINE = 64;
        alignas(CACHELINE) std::atomic<usize> m_WriteIndex{0};
        alignas(CACHELINE) std::atomic<usize> m_ReadIndex{0};

        std::array<std::atomic<usize>, N> m_Sequences;
        std::array<T, N> m_Data;
    };

} // namespace Nexus
