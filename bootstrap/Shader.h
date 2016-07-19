#pragma once

namespace aie {

class Shader {
public:

	Shader() : m_program(0), m_error(nullptr) {
		m_shaders[0] = m_shaders[1] = m_shaders[2] = m_shaders[3] = m_shaders[4] = 0;
	}
	~Shader();

	bool loadShader(unsigned int type, const char* filename);
	bool createShader(unsigned int type, const char* string);

	bool link();

	void bind();

	int getUniform(const char* name);

	unsigned int getHandle() const { return m_program; }

	const char* getLastError() const { return m_error; }

private:

	unsigned int	m_program;

	enum SHADER_STAGE {
		VERTEX = 0,
		TESS_CONTROL,
		TESS_EVALUATION,
		GEOMETRY,
		FRAGMENT,

		SHADER_STAGE_Count,
	};

	unsigned int	m_shaders[SHADER_STAGE_Count];
	char*			m_error;
};

} // namespace aie