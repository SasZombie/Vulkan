#pragma once

#include "SerializeCodes.hpp"

namespace sas
{
    
    struct EngineMetadata
    {
        uint32_t majorVersion;
        uint32_t minorVersion;
        uint32_t patchVersion;
    };
    
    std::ostream &operator<<(std::ostream &os, EngineMetadata data);
    
} // namespace sas