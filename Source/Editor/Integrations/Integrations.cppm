// SPDX-License-Identifier: MIT

export module NE.Editor.Integration;

export namespace Nexus {
    class Integration {
    public:
        virtual void Start() = 0;
        virtual void Shutdown() = 0;
        virtual void Update() = 0;

        virtual ~Integration() = default;
    };
} // namespace Nexus
