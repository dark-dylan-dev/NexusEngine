// SPDX-License-Identifier: MIT

module;

module NE.Engine.RHI.Device;
#if defined(NEXUS_RHI_VULKAN)
import NE.Engine.RHI.Vulkan.Device;
#elif defined(NEXUS_RHI_DX12)
#endif

namespace Nexus::RHI {
    std::unique_ptr<Device> Device::Create() {
#if defined(NEXUS_RHI_VULKAN)
        return std::make_unique<VulkanDevice>();
#elif defined(NEXUS_RHI_DX12)
        std::unreachable();
#endif
        return nullptr;
    }
} // namespace Nexus::RHI
