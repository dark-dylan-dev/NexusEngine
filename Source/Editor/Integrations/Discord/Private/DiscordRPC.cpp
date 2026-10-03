// SPDX-License-Identifier: MIT

module;

#define DISCORDPP_IMPLEMENTATION
#include <discordpp.h>

module NE.Editor.Integration.DiscordRPC;

import NE.Engine.Core.Types;

namespace Nexus {
    DiscordRPC::DiscordRPC() {
        Start();
    }

    void DiscordRPC::Start() {
        m_Client = std::make_unique<discordpp::Client>();
        m_Client->AddLogCallback(
            [](auto message, auto severity) { std::println("[{}] {}", EnumToString(severity), message); },
            discordpp::LoggingSeverity::Info);
        m_Client->SetApplicationId(APPLICATION_ID);
        m_Activity.SetType(discordpp::ActivityTypes::Playing);
        SetProject("");
        SetScene("");
        UpdateRichPresence();
    }

    void DiscordRPC::Shutdown() {
        m_Activity.Drop();
        m_Client.reset();
    }

    void DiscordRPC::Update() {
        discordpp::RunCallbacks();
    }

    void DiscordRPC::SetProject(const std::string& name) {
        m_Activity.SetState(name.empty() ? "Idle" : "Working on " + name);
        UpdateRichPresence();
    }

    void DiscordRPC::SetScene(const std::string& name) {
        m_Activity.SetDetails(name);
        UpdateRichPresence();
    }

    void DiscordRPC::UpdateRichPresence() const {
        m_Client->UpdateRichPresence(m_Activity, []([[maybe_unused]] discordpp::ClientResult result) {});
    }
} // namespace Nexus
