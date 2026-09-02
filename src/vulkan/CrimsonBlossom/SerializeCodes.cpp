#include "SerializeCodes.hpp"

std::ostream &sas::operator<<(std::ostream &os, sas::SerializeCodesBinary code)
{
    return os << std::to_underlying(code);
}

std::ostream &sas::operator<<(std::ostream &os, sas::SerializeCodesText code)
{
    return os << enumToString(code);
}