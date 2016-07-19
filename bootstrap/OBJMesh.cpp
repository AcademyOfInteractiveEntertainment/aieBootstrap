#include "OBJMesh.h"
#include "gl_core_4_4.h"
#include <glm/geometric.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace aie {

OBJMesh::~OBJMesh() {
	for (auto& c : m_meshChunks) {
		glDeleteVertexArrays(1, &c.vao);
		glDeleteBuffers(1, &c.vbo);
		glDeleteBuffers(1, &c.ibo);
	}
}

bool OBJMesh::load(const char* filename, bool loadTextures /* = true */, bool flipTextureV /* = false */) {

	if (m_meshChunks.empty() == false) {
		printf("Mesh already initialised, can't re-initialise!\n");
		return false;
	}

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string error = "";

	std::string file = filename;
	std::string folder = file.substr(0, file.find_last_of('/') + 1);

	bool success = tinyobj::LoadObj(shapes, materials, error,
									filename, folder.c_str());

	if (success == false) {
		printf("%s\n", error.c_str());
		return false;
	}

	m_filename = filename;

	// copy materials
	m_materials.resize(materials.size());
	int index = 0;
	for (auto& m : materials) {

		m_materials[index].ambient = glm::vec3(m.ambient[0], m.ambient[1], m.ambient[2]);
		m_materials[index].diffuse = glm::vec3(m.diffuse[0], m.diffuse[1], m.diffuse[2]);
		m_materials[index].specular = glm::vec3(m.specular[0], m.specular[1], m.specular[2]);
		m_materials[index].emissive = glm::vec3(m.emission[0], m.emission[1], m.emission[2]);
		m_materials[index].specularPower = m.shininess;
		m_materials[index].opacity = m.dissolve;

		// textures
		m_materials[index].alphaTexture.load((folder + m.alpha_texname).c_str());
		m_materials[index].ambientTexture.load((folder + m.ambient_texname).c_str());
		m_materials[index].diffuseTexture.load((folder + m.diffuse_texname).c_str());
		m_materials[index].specularTexture.load((folder + m.specular_texname).c_str());
		m_materials[index].specularHighlightTexture.load((folder + m.specular_highlight_texname).c_str());
		m_materials[index].normalTexture.load((folder + m.bump_texname).c_str());
		m_materials[index].displacementTexture.load((folder + m.displacement_texname).c_str());

		++index;
	}

	// copy shapes
	m_meshChunks.reserve(shapes.size());
	for (auto& s : shapes) {

		MeshChunk chunk;

		// generate buffers
		glGenBuffers(1, &chunk.vbo);
		glGenBuffers(1, &chunk.ibo);
		glGenVertexArrays(1, &chunk.vao);

		// bind vertex array aka a mesh wrapper
		glBindVertexArray(chunk.vao);

		// set the index buffer data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					 s.mesh.indices.size() * sizeof(unsigned int),
					 s.mesh.indices.data(), GL_STATIC_DRAW);

		// store index count for rendering
		chunk.indexCount = (unsigned int)s.mesh.indices.size();

		// create vertex data
		std::vector<Vertex> vertices;
		vertices.resize(s.mesh.positions.size() / 3);
		size_t vertCount = vertices.size();

		bool hasPosition = s.mesh.positions.empty() == false;
		bool hasNormal = s.mesh.normals.empty() == false;
		bool hasTexture = s.mesh.texcoords.empty() == false;

		for (size_t i = 0; i < vertCount; ++i) {
			if (hasPosition)
				vertices[i].position = glm::vec4(s.mesh.positions[i * 3 + 0], s.mesh.positions[i * 3 + 1], s.mesh.positions[i * 3 + 2], 1);
			if (hasNormal)
				vertices[i].normal = glm::vec4(s.mesh.normals[i * 3 + 0], s.mesh.normals[i * 3 + 1], s.mesh.normals[i * 3 + 2], 0);

			// flip the T / V (might not always be needed, depends on how mesh was made)
			if (hasTexture)
				vertices[i].texcoord = glm::vec2(s.mesh.texcoords[i * 2 + 0], flipTextureV ? 1.0f - s.mesh.texcoords[i * 2 + 1] : s.mesh.texcoords[i * 2 + 1]);
		}

		// calculate for normal mapping
		if (hasNormal && hasTexture)
			calculateTangents(vertices, s.mesh.indices);

		// bind vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, chunk.vbo);

		// fill vertex buffer
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		// enable first element as positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

		// enable normals
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), (void*)(sizeof(glm::vec4) * 2));

		// enable texture coords
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4) * 3));

		// enable tangents
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4) * 3 + sizeof(glm::vec2)));

		// bind 0 for safety
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// set chunk material
		chunk.materialID = s.mesh.material_ids.empty() ? -1 : s.mesh.material_ids[0];

		m_meshChunks.push_back(chunk);
	}
	
	// load obj
	return true;
}

void OBJMesh::draw(unsigned int primitiveType) {

	int program = -1;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);

	if (program == -1) {
		printf("No shader bound!\n");
		return;
	}

	// pull uniforms from the shader
	int kaUniform = glGetUniformLocation(program, "Ka");
	int kdUniform = glGetUniformLocation(program, "Kd");
	int ksUniform = glGetUniformLocation(program, "Ks");
	int keUniform = glGetUniformLocation(program, "Ke");
	int opacityUniform = glGetUniformLocation(program, "opacity");
	int specPowUniform = glGetUniformLocation(program, "specularPower");

	int alphaTexUniform = glGetUniformLocation(program, "alphaTexture");
	int ambientTexUniform = glGetUniformLocation(program, "ambientTexture");
	int diffuseTexUniform = glGetUniformLocation(program, "diffuseTexture");
	int specTexUniform = glGetUniformLocation(program, "specularTexture");
	int specHighlightTexUniform = glGetUniformLocation(program, "specularHighlightTexture");
	int normalTexUniform = glGetUniformLocation(program, "normalTexture");
	int dispTexUniform = glGetUniformLocation(program, "displacementTexture");

	// set texture slots (these don't change per material)
	if (diffuseTexUniform >= 0)
		glUniform1i(diffuseTexUniform, 0);
	if (alphaTexUniform >= 0)
		glUniform1i(alphaTexUniform, 1);
	if (ambientTexUniform >= 0)
		glUniform1i(ambientTexUniform, 2);
	if (specTexUniform >= 0)
		glUniform1i(specTexUniform, 3);
	if (specHighlightTexUniform >= 0)
		glUniform1i(specHighlightTexUniform, 4);
	if (normalTexUniform >= 0)
		glUniform1i(normalTexUniform, 5);
	if (dispTexUniform >= 0)
		glUniform1i(dispTexUniform, 6);

	int currentMaterial = -1;

	// draw the mesh chunks
	for (auto& c : m_meshChunks) {

		// bind material
		if (currentMaterial != c.materialID) {
			currentMaterial = c.materialID;
			if (kaUniform >= 0)
				glUniform3fv(kaUniform, 1, &m_materials[currentMaterial].ambient[0]);
			if (kdUniform >= 0)
				glUniform3fv(kdUniform, 1, &m_materials[currentMaterial].diffuse[0]);
			if (ksUniform >= 0)
				glUniform3fv(ksUniform, 1, &m_materials[currentMaterial].specular[0]);
			if (keUniform >= 0)
				glUniform3fv(keUniform, 1, &m_materials[currentMaterial].emissive[0]);
			if (opacityUniform >= 0)
				glUniform1f(opacityUniform, m_materials[currentMaterial].opacity);
			if (specPowUniform >= 0)
				glUniform1f(specPowUniform, m_materials[currentMaterial].specularPower);

			glActiveTexture(GL_TEXTURE0);
			if (m_materials[currentMaterial].diffuseTexture.getHandle() > 0)
				glBindTexture(GL_TEXTURE_2D, m_materials[currentMaterial].diffuseTexture.getHandle());
			else if (diffuseTexUniform >= 0)
				glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE1);
			if (m_materials[currentMaterial].alphaTexture.getHandle() > 0)
				glBindTexture(GL_TEXTURE_2D, m_materials[currentMaterial].alphaTexture.getHandle());
			else if (alphaTexUniform >= 0)
				glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE2);
			if (m_materials[currentMaterial].ambientTexture.getHandle() > 0)
				glBindTexture(GL_TEXTURE_2D, m_materials[currentMaterial].ambientTexture.getHandle());
			else if (ambientTexUniform >= 0)
				glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE3);
			if (m_materials[currentMaterial].specularTexture.getHandle() > 0)
				glBindTexture(GL_TEXTURE_2D, m_materials[currentMaterial].specularTexture.getHandle());
			else if (specTexUniform >= 0)
				glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE4);
			if (m_materials[currentMaterial].specularHighlightTexture.getHandle() > 0)
				glBindTexture(GL_TEXTURE_2D, m_materials[currentMaterial].specularHighlightTexture.getHandle());
			else if (specHighlightTexUniform >= 0)
				glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE5);
			if (m_materials[currentMaterial].normalTexture.getHandle() > 0)
				glBindTexture(GL_TEXTURE_2D, m_materials[currentMaterial].normalTexture.getHandle());
			else if (normalTexUniform >= 0)
				glBindTexture(GL_TEXTURE_2D, 0);

			glActiveTexture(GL_TEXTURE6);
			if (m_materials[currentMaterial].displacementTexture.getHandle() > 0)
				glBindTexture(GL_TEXTURE_2D, m_materials[currentMaterial].displacementTexture.getHandle());
			else if (dispTexUniform >= 0)
				glBindTexture(GL_TEXTURE_2D, 0);
		}

		// bind and draw geometry
		glBindVertexArray(c.vao);
		glDrawElements(primitiveType, c.indexCount, GL_UNSIGNED_INT, 0);
	}
}

void OBJMesh::calculateTangents(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
	unsigned int vertexCount = (unsigned int)vertices.size();
	glm::vec4* tan1 = new glm::vec4[vertexCount * 2];
	glm::vec4* tan2 = tan1 + vertexCount;
	memset(tan1, 0, vertexCount * sizeof(glm::vec4) * 2);

	unsigned int indexCount = (unsigned int)indices.size();
	for (unsigned int a = 0; a < indexCount; a += 3) {
		long i1 = indices[a];
		long i2 = indices[a + 1];
		long i3 = indices[a + 2];

		const glm::vec4& v1 = vertices[i1].position;
		const glm::vec4& v2 = vertices[i2].position;
		const glm::vec4& v3 = vertices[i3].position;

		const glm::vec2& w1 = vertices[i1].texcoord;
		const glm::vec2& w2 = vertices[i2].texcoord;
		const glm::vec2& w3 = vertices[i3].texcoord;

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0F / (s1 * t2 - s2 * t1);
		glm::vec4 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
					   (t2 * z1 - t1 * z2) * r, 0);
		glm::vec4 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
					   (s1 * z2 - s2 * z1) * r, 0);

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}

	for (unsigned int a = 0; a < vertexCount; a++) {
		const glm::vec3& n = glm::vec3(vertices[a].normal);
		const glm::vec3& t = glm::vec3(tan1[a]);

		// Gram-Schmidt orthogonalize
		vertices[a].tangent = glm::vec4(glm::normalize(t - n * glm::dot(n, t)), 0);

		// Calculate handedness (direction of bitangent)
		vertices[a].tangent.w = (glm::dot(glm::cross(glm::vec3(n), glm::vec3(t)), glm::vec3(tan2[a])) < 0.0F) ? 1.0F : -1.0F;

		// calculate bitangent (ignoring for Vertex)
		//vertices[a].bitangent = glm::vec4(glm::cross(glm::vec3(vertices[a].normal), glm::vec3(vertices[a].tangent)) * vertices[a].tangent.w, 0);
		//vertices[a].tangent.w = 0;
	}

	delete[] tan1;
}
}