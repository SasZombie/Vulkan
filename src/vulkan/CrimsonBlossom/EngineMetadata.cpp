#include "EngineMetadata.hpp"

std::ostream &sas::operator<<(std::ostream &os, sas::EngineMetadata data)
{
    return os << SerializeCodesText::MAJOR_VERSION << ' ' << data.majorVersion
              << ' ' << SerializeCodesText::MINOR_VERSION << ' ' << data.minorVersion
              << ' ' << SerializeCodesText::PATCH_VERSION << ' ' << data.patchVersion;
}
