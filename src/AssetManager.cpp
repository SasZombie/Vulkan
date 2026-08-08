#include "AssetManager.hpp"

#include <cstring>
#include <fstream>
#include "Math.hpp"
static float _stringToFloat(const std::string &source) noexcept
{
    std::stringstream ss(source.c_str());
    float result;
    ss >> result;
    return result;
}

[[maybe_unused]] static unsigned int _stringToUint(const std::string &source) noexcept
{
    std::stringstream ss(source.c_str());
    unsigned int result;
    ss >> result;
    return result;
}

static int _stringToInt(const std::string &source) noexcept
{
    std::stringstream ss(source.c_str());
    int result;
    ss >> result;
    return result;
}

static void _stringTokenize(const std::string &source, std::vector<std::string> &tokens) noexcept
{
    tokens.clear();
    std::string aux = source;
    for (unsigned int i = 0; i < aux.size(); i++)
        if (aux[i] == '\t' || aux[i] == '\n')
            aux[i] = ' ';
    std::stringstream ss(aux, std::ios::in);
    while (ss.good())
    {
        std::string s;
        ss >> s;
        if (s.size() > 0)
            tokens.push_back(s);
    }
}

static void _faceTokenize(const std::string &source, std::vector<std::string> &tokens) noexcept
{
    std::string aux = source;
    for (unsigned int i = 0; i < aux.size(); i++)
        if (aux[i] == '\\' || aux[i] == '/')
            aux[i] = ' ';
    _stringTokenize(aux, tokens);
}

[[nodiscard]] static sas::Mesh loadObj(std::string_view filename) noexcept
{
    std::vector<sas::Vertex> vertices;
    std::vector<int> indices;

    // Reading Obj file
    std::ifstream file(std::string(filename), std::ios::in | std::ios::binary);
    if (!file)
    {
        std::cerr << "Model not found " << filename << '\n';
        return {};
    }

    std::string line;
    std::vector<std::string> tokens, facetokens;

    std::vector<sas::math::Vec3> positions;
    positions.reserve(1000);

    std::vector<sas::math::Vec3> normals;
    normals.reserve(1000);

    std::vector<sas::math::Vec2> texcoords;
    texcoords.reserve(1000);

    // Parsing obj file
    while (std::getline(file, line))
    {
        _stringTokenize(line, tokens);

        if (tokens.size() == 0)
            continue;

        // Comments
        if (tokens.size() > 0 && tokens[0].at(0) == '#')
            continue;

        // Vertices
        if (tokens.size() > 3 && tokens[0] == "v")
            positions.push_back(sas::math::Vec3(_stringToFloat(tokens[1]), _stringToFloat(tokens[2]), _stringToFloat(tokens[3])));

        // Normals
        if (tokens.size() > 3 && tokens[0] == "vn")
            normals.push_back(sas::math::Vec3(_stringToFloat(tokens[1]), _stringToFloat(tokens[2]), _stringToFloat(tokens[3])));

        // Texture Coords
        if (tokens.size() > 2 && tokens[0] == "vt")
            texcoords.push_back(sas::math::Vec2(_stringToFloat(tokens[1]), _stringToFloat(tokens[2])));

        // Faces
        if (tokens.size() >= 4 && tokens[0] == "f")
        {
            unsigned int face_format = 0;
            if (tokens[1].find("//") != std::string::npos)
                face_format = 3;
            _faceTokenize(tokens[1], facetokens);

            if (facetokens.size() == 3)
                face_format = 4;
            else
            {
                if (facetokens.size() == 2)
                {
                    if (face_format != 3)
                        face_format = 2;
                }
                else
                {
                    face_format = 1;
                }
            }

            unsigned int index_of_first_vertex_of_face = -1;

            for (unsigned int num_token = 1; num_token < tokens.size(); num_token++)
            {
                sas::Vertex vertex;

                if (tokens[num_token].at(0) == '#')
                    break;
                _faceTokenize(tokens[num_token], facetokens);

                if (face_format == 1) // Just pos
                {
                    int p_index = _stringToInt(facetokens[0]);
                    if (p_index > 0)
                        p_index -= 1;
                    else
                        p_index = positions.size() + p_index;

                    vertex.pos = {positions[p_index].x, positions[p_index].y, positions[p_index].z};

                    vertices.push_back(vertex);
                }
                else if (face_format == 2) // Pos and texcoords
                {
                    int p_index = _stringToInt(facetokens[0]);
                    if (p_index > 0)
                        p_index -= 1;
                    else
                        p_index = positions.size() + p_index;

                    int t_index = _stringToInt(facetokens[1]);
                    if (t_index > 0)
                        t_index -= 1;
                    else
                        t_index = texcoords.size() + t_index;

                    vertex.pos = {positions[p_index].x, positions[p_index].y, positions[p_index].z};
                    vertex.texCoord = {texcoords[t_index].x, texcoords[t_index].y};
                    vertices.push_back(vertex);
                }
                else if (face_format == 3)
                {
                    // Pos and normal
                    int p_index = _stringToInt(facetokens[0]);
                    if (p_index > 0)
                        p_index -= 1;
                    else
                        p_index = positions.size() + p_index;

                    int n_index = _stringToInt(facetokens[1]);
                    if (n_index > 0)
                        n_index -= 1;
                    else
                        n_index = normals.size() + n_index;

                    vertex.pos = {positions[p_index].x, positions[p_index].y, positions[p_index].z};
                    vertex.normals = {normals[n_index].x, normals[n_index].y, normals[n_index].z};

                    vertices.push_back(vertex);
                }
                else
                {
                    // Normal and texcoord
                    int p_index = _stringToInt(facetokens[0]);
                    if (p_index > 0)
                        p_index -= 1;
                    else
                        p_index = positions.size() + p_index;

                    int t_index = _stringToInt(facetokens[1]);
                    if (t_index > 0)
                        t_index -= 1;
                    else
                        t_index = normals.size() + t_index;

                    int n_index = _stringToInt(facetokens[2]);
                    if (n_index > 0)
                        n_index -= 1;
                    else
                        n_index = normals.size() + n_index;

                    vertex.pos = {positions[p_index].x, positions[p_index].y, positions[p_index].z};
                    vertex.normals = {normals[n_index].x, normals[n_index].y, normals[n_index].z};
                    vertex.texCoord = {texcoords[t_index].x, texcoords[t_index].y};

                    vertices.push_back(vertex);
                }

                if (num_token < 4)
                {
                    if (num_token == 1)
                        index_of_first_vertex_of_face = vertices.size() - 1;

                    indices.push_back(vertices.size() - 1);
                }
                else
                {
                    indices.push_back(index_of_first_vertex_of_face);
                    indices.push_back(vertices.size() - 2);
                    indices.push_back(vertices.size() - 1);
                }
            }
        }
    }

    std::cout << "Loading:  " << filename << '\n';

    return {vertices, indices};
}


//TODO: add .gltf format support
//TODO: Here some kind of dispatcher for diff formats :D and checkers for real files
sas::Mesh sas::AssetManager::getRawMesh(std::string_view path) const noexcept
{
    // const std::vector<sas::Vertex> vertices = {
    //     {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    //     {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    //     {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    //     {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

    // const std::vector<int> indices = {
    //     0, 1, 2, // First triangle
    //     2, 3, 0  // Second triangle
    // };

    return loadObj(path);
}

uint32_t sas::AssetManager::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkanCtx.vkPhysical, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        // Check 1: Is bit i set in typeFilter? (Is this memory type allowed for the buffer?)
        // Check 2: Does this memory type have all the property flags we requested?
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void sas::AssetManager::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory) const noexcept
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateBuffer(vulkanCtx.vkDevice, &bufferInfo, nullptr, &buffer);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vulkanCtx.vkDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;

    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    vkAllocateMemory(vulkanCtx.vkDevice, &allocInfo, nullptr, &bufferMemory);
    vkBindBufferMemory(vulkanCtx.vkDevice, buffer, bufferMemory, 0);
}

sas::RenderObject sas::AssetManager::createGpuMesh(const sas::Mesh &mesh) const noexcept
{
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    // 1. Create & Copy Vertex Buffer
    createBuffer(sizeof(mesh.vertices[0]) * mesh.vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 vertexBuffer, vertexBufferMemory);

    void *data;
    vkMapMemory(vulkanCtx.vkDevice, vertexBufferMemory, 0, sizeof(mesh.vertices[0]) * mesh.vertices.size(), 0, &data);
    memcpy(data, mesh.vertices.data(), (size_t)sizeof(mesh.vertices[0]) * mesh.vertices.size());
    vkUnmapMemory(vulkanCtx.vkDevice, vertexBufferMemory);

    // 2. Create & Copy Index Buffer
    createBuffer(sizeof(mesh.indices[0]) * mesh.indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 indexBuffer, indexBufferMemory);

    vkMapMemory(vulkanCtx.vkDevice, indexBufferMemory, 0, sizeof(mesh.indices[0]) * mesh.indices.size(), 0, &data);
    memcpy(data, mesh.indices.data(), (size_t)sizeof(mesh.indices[0]) * mesh.indices.size());
    vkUnmapMemory(vulkanCtx.vkDevice, indexBufferMemory);

    return {vertexBuffer, indexBuffer, vertexBufferMemory, indexBufferMemory, nullptr, mesh.indices.size()};
}

sas::RenderObject sas::AssetManager::loadMesh(const std::string &path) noexcept
{
    if (meshCache.contains(path))
    {
        return meshCache.at(path);
    }
    const auto &loadedMesh = getRawMesh(path);

    RenderObject gpuObject = createGpuMesh(loadedMesh);
    VkBuffer buffers[] = {gpuObject.vertexBuffer};

    VkDeviceSize offsets[] = {0};

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(vulkanCtx.vkCommand.getCommandBuffer(), &beginInfo);

    vkCmdBindVertexBuffers(vulkanCtx.vkCommand.getCommandBuffer(), 0, 1, buffers, offsets);

    vkCmdBindIndexBuffer(vulkanCtx.vkCommand.getCommandBuffer(), gpuObject.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkEndCommandBuffer(vulkanCtx.vkCommand.getCommandBuffer());

    meshCache[path] = gpuObject;

    return gpuObject;
}