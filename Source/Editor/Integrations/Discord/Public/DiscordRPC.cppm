// SPDX-License-Identifier: MIT

module;

#include <discordpp.h>

export module NE.Editor.Integration.DiscordRPC;

import NE.Editor.Integration;
import NE.Engine.Core.Types;

import std;

export namespace Nexus {
    class DiscordRPC final : public Integration {
    public:
        DiscordRPC();

        void Start() override;
        void Shutdown() override;
        void Update() override;

        void SetProject(const std::string& name);
        void SetScene(const std::string& name);

    private:
        void UpdateRichPresence() const;

    public:
        static constexpr uint64 APPLICATION_ID = 1555623817894699061;

    private:
        discordpp::Activity m_Activity{};
        std::unique_ptr<discordpp::Client> m_Client = nullptr;
    };
} // namespace Nexus
