// SPDX-License-Identifier: MIT

#pragma once

namespace Nexus {
    template <typename F>
    ScopeGuard<F>::ScopeGuard(F&& f) {
        m_Func = std::forward<F>(f);
        m_Active = true;
    }

    template <typename F>
    ScopeGuard<F>::~ScopeGuard() {
        if (m_Active) {
            m_Func();
        }
    }

    template <typename F>
    void ScopeGuard<F>::Dismiss() {
        m_Active = false;
    }

    template <typename F>
    [[nodiscard]] ScopeGuard<F> Defer(F&& f) {
        return ScopeGuard<F>(std::forward<F>(f));
    }
} // namespace Nexus
