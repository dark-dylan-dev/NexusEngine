// SPDX-License-Identifier: MIT

module;

#include <Platform/DLL/Export.hpp>

export module NE.Engine.Core.Utils;

import std;

export namespace Nexus {
    /// Gets the time in seconds since the start of the program
    NEXUS_API double GetEngineTime() {
        const static auto start = std::chrono::steady_clock::now();
        const auto now = std::chrono::steady_clock::now();
        return std::chrono::duration<double>(now - start).count();
    }

    template <typename F>
    class NEXUS_API ScopeGuard {
    public:
        explicit ScopeGuard(F&& f);
        ~ScopeGuard();
        void Dismiss();

    private:
        F m_Func;
        bool m_Active;
    };

    template <typename F>
    NEXUS_API ScopeGuard<F> Defer(F&& f);
} // namespace Nexus

#include "Utils.inl"
