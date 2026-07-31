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
        }
        ~RingBuffer() = default;

        RingBuffer(const RingBuffer&) = delete;
        RingBuffer& operator=(const RingBuffer&) = delete;

        void Insert(T&& message) {
            const usize pos = m_WriteIndex.fetch_add(1, std::memory_order_relaxed);
            const usize idx = pos % N;

            // Wait for our turn on this sequence
            usize seq;
            while ((seq = m_Sequences[idx].load(std::memory_order_acquire)) != pos) {
                std::this_thread::yield();
            }

            // Write data
            m_Data[idx] = std::move(message);

            // Publish
            m_Sequences[idx].store(pos + 1, std::memory_order_release);
        }

        bool TryPop(T& outMessage) {
            const usize pos = m_ReadIndex.load(std::memory_order_relaxed);
            const usize idx = pos % N;

            // Check if ready
            if (m_Sequences[idx].load(std::memory_order_acquire) == pos + 1) {
                // Extract data
                outMessage = std::move(m_Data[idx]);

                // Reset sequence for next loop
                m_Sequences[idx].store(pos + N, std::memory_order_release);

                // Advance read index
                m_ReadIndex.store(pos + 1, std::memory_order_release);
                return true;
            }

            return false;
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
        alignas(64) std::atomic<usize> m_WriteIndex{0};
        alignas(64) std::atomic<usize> m_ReadIndex{0};

        std::array<std::atomic<usize>, N> m_Sequences;
        std::array<T, N> m_Data;
    };

} // namespace Nexus
