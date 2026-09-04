#include "Scene.hpp"

#include "SerializeCodes.hpp"

void sas::Scene::serialize(const std::filesystem::path& path) const noexcept
{
    const std::filesystem::path filePath = path.string() + std::to_string(id) + ".scn";

    std::ofstream outFile{filePath};

    GetFormatInfoCommand cmd;
    bus.dispatch<GetFormatInfoCommand>(cmd);

    //Atm 1 format
    outFile << SerializeCodesText::START_SCENE << '\n' << SerializeCodesText::SCENE_FORMAT_VERSION << ' ' << cmd.formatType << '\n' 
    << SerializeCodesText::COMMENT << " This is a .scn scene " << SerializeCodesText::END << '\n'
    << SerializeCodesText::ID << ' ' << id << ' ' << SerializeCodesText::NUMBER_OF_SUB_OBJECTS << ' ' << sceneRegistry.getEntityCount() << '\n'
    << SerializeCodesText::END;

    
}