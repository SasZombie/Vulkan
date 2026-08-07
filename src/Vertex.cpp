#include "Vertex.hpp"

// Mesh::Mesh(std::vector<Vertex> nVertices, std::vector<int> nIndices, const std::filesystem::path& pth) noexcept
// 	: vertices(nVertices), indices(nIndices), path(pth)
// {
// 	setup2();
// }

// Mesh::Mesh(std::vector<Vertex> nVertices, std::vector<int> nIndices, std::shared_ptr<std::vector<Texture>> nTextures) noexcept
// 	: vertices(nVertices), indices(nIndices), textures(nTextures)
// {
// 	setup();
// }

// void Mesh::draw(const Shader& shader) noexcept
// {
// 	unsigned int diffuseNr = 1;
// 	unsigned int specularNr = 1;
// 	unsigned int normalNr = 1;
// 	unsigned int heightNr = 1;

// 	for (unsigned int i = 0; i < textures->size(); i++)
// 	{
// 		glActiveTexture(GL_TEXTURE0 + i); 

// 		const auto& textureDeref = (*textures); 
											
// 		std::string number;
// 		std::string name = textureDeref[i].type;
// 		// std::string name = textures->operator[](i).type;
// 		//Ugly ahh sytax, aint doing that^

// 		if (name == "texture_diffuse")
// 			number = std::to_string(diffuseNr++);
// 		else if (name == "texture_specular")
// 			number = std::to_string(specularNr++); 
// 		else if (name == "texture_normal")
// 			number = std::to_string(normalNr++);
// 		else if (name == "texture_height")
// 			number = std::to_string(heightNr++); 

// 		glUniform1i(glGetUniformLocation(shader.getId(), (name + number).c_str()), i);
// 		glBindTexture(GL_TEXTURE_2D, textureDeref[i].id);
// 	}

// 	glBindVertexArray(vao);
// 	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
// 	glBindVertexArray(0);

// 	glActiveTexture(GL_TEXTURE0);
// }

// void Mesh::setup() noexcept
// {
// 	glGenVertexArrays(1, &vao);
// 	glGenBuffers(1, &vbo);
// 	glGenBuffers(1, &ibo);

// 	glBindVertexArray(vao);
// 	glBindBuffer(GL_ARRAY_BUFFER, vbo);
// 	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
// 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

// 	glEnableVertexAttribArray(0);
// 	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

// 	glEnableVertexAttribArray(1);
// 	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normals));

// 	glEnableVertexAttribArray(2);
// 	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));

// 	glBindVertexArray(0);
// }

// // no textures yet
// void Mesh::setup2() noexcept
// {
// 	glGenVertexArrays(1, &vao);
// 	glGenBuffers(1, &vbo);
// 	glGenBuffers(1, &ibo);

// 	glBindVertexArray(vao);
// 	glBindBuffer(GL_ARRAY_BUFFER, vbo);
// 	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
// 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

// 	glEnableVertexAttribArray(0);
// 	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

// 	glBindVertexArray(0);
// }

// void Mesh::setTextures(std::shared_ptr<std::vector<Texture>> nTextures) noexcept
// {
// 	this->textures = nTextures;
// 	setup();
// }

// std::ostream &operator<<(std::ostream &os, const Mesh &mesh)
// {
// 	os << mesh.path.string() << '\n';

// 	if(mesh.textures->empty())
// 	{
// 		os << 0 << '\n';
// 		return os;
// 	}

// 	os << mesh.textures->size() << '\n';
// 	for(const auto& texture : *mesh.textures)
// 	{
// 		os << texture.path.string() << '\n';
// 	}
	
// 	return os;
// }
