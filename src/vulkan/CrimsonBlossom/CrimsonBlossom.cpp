#include "CrimsonBlossom.hpp"
#include "Transform.hpp"
#include "SerializeCodes.hpp"

#include <filesystem>


std::vector<sas::DrawingComponents> sas::CrimsonBlossom::getDrawingData() noexcept
{
    auto it = scenes.find(activeSceneId);
    if (it == scenes.end())
    {
        return {};
    }
    auto renderData = it->second.sceneRegistry.getCombined<RenderObject, ObjectTransform3D>();

    std::sort(renderData.begin(), renderData.end(), [](const Combined<RenderObject, ObjectTransform3D> &rendObj, const Combined<RenderObject, ObjectTransform3D> &other)
              { return rendObj.get<RenderObject>()->material->shader->getId() < other.get<RenderObject>()->material->shader->getId(); });

    return renderData;
}

void sas::CrimsonBlossom::update() noexcept
{
    commandBus.processQueue();
    engineUi.updateFrame();

    vkRenderer.drawFrame(getDrawingData());
}

void sas::CrimsonBlossom::createUi() const noexcept
{
}

void sas::CrimsonBlossom::saveScenes() const noexcept
{
    namespace fs = std::filesystem;

    // const std::string path = "sources/Crbl.scn";
    const fs::path mainFolderPath = "sources/";

    std::error_code ec;
    if(!fs::create_directories(mainFolderPath, ec) && ec)
    {
        logger->error("Cannot create directory" + mainFolderPath.string());
    }

    const fs::path sceneFolderPath = "sources/scenes/";

    if(!fs::create_directories(sceneFolderPath, ec) && ec)
    {
        logger->error("Cannot create directory" + sceneFolderPath.string());
    }

    const fs::path metaDataPath = mainFolderPath.string() + "crbl.mdat";
    std::ofstream outStream{metaDataPath};

    savePreScene(outStream);

    for (const auto &scene : getScenes())
    {
        scene.serialize(sceneFolderPath);
    }
}

void sas::CrimsonBlossom::savePreScene(std::ofstream &outStream) const noexcept
{
    constexpr char magic[] = "CRIMBLOS";

    outStream << magic << '\n' << SerializeCodesText::COMMENT << " This is auto generated human readable format "
    << SerializeCodesText::END << '\n' << SerializeCodesText::META_INFO << ' ' << metaData << '\n'
    << SerializeCodesText::NUMBER_OF_SUB_OBJECTS << ' ' << scenes.size() << '\n';
}