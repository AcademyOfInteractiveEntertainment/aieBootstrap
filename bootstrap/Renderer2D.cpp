//----------------------------------------------------------------------------
// A class for rendering 2D sprites and text using imediate-mode rendering.
//----------------------------------------------------------------------------
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "Renderer2D.h"
#include "Texture.h"
#include "Font.h"
#include <glm/ext.hpp>
#include <stb_truetype.h>
#include "Shader.h"

namespace aie 
{

Renderer2D::Renderer2D() 
{
	SetRenderColour(1,1,1,1);
	SetUVRect(0.0f, 0.0f, 1.0f, 1.0f);
	SetFlipped(false, false);

	m_cameraX = 0;
	m_cameraY = 0;
	m_cameraScale = 1.0f;

	unsigned int pixels = 0xFFFFFFFF;
	m_nullTexture = new Texture(1, 1, Texture::RGBA, (unsigned char*)&pixels);

	m_currentVertex = 0;
	m_currentIndex = 0;
	m_renderBegun = false;

	m_vao = -1;
	m_vbo = -1;
	m_ibo = -1;

	m_currentTexture = 0;

	for (int i = 0; i < TEXTURE_STACK_SIZE; i++) 
	{
		m_textureStack[i] = nullptr;
		m_fontTexture[i] = 0;
	}
	
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, &sk_spriteVertexShader, 0);
	glCompileShader(vs);

	glShaderSource(fs, 1, &sk_spriteFragmentShader, 0);
	glCompileShader(fs);

	m_shader = glCreateProgram();
	glAttachShader(m_shader, vs);
	glAttachShader(m_shader, fs);
	glBindAttribLocation(m_shader, 0, "position");
	glBindAttribLocation(m_shader, 1, "colour");
	glBindAttribLocation(m_shader, 2, "texcoord");
	glLinkProgram(m_shader);

	int success = GL_FALSE;
	glGetProgramiv(m_shader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) 
	{
		int infoLogLength = 0;
		glGetProgramiv(m_shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_shader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link SpriteBatch shader program!\n%s\n", infoLog);
		delete[] infoLog;
	}

	glUseProgram(m_shader);

	// Set texture locations.
	char buf[32];
	for (int i = 0; i < TEXTURE_STACK_SIZE; ++i) 
	{
		sprintf_s(buf, "textureStack[%i]", i);
		glUniform1i(glGetUniformLocation(m_shader, buf), i);
	}

	glUseProgram(0);

	glDeleteShader(vs);
	glDeleteShader(fs);
	
	// Pre calculate the indices... they will always be the same.
	int index = 0;
	for (int i = 0; i<(MAX_SPRITES*6);) 
	{
		m_indices[i++] = (index + 0);
		m_indices[i++] = (index + 1);
		m_indices[i++] = (index + 2);

		m_indices[i++] = (index + 0);
		m_indices[i++] = (index + 2);
		m_indices[i++] = (index + 3);
		index += 4;
	}
	
	// Create the vao, vio and vbo.
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (MAX_SPRITES * 6) * sizeof(unsigned short), (void *)(&m_indices[0]), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, (MAX_SPRITES * 4) * sizeof(SBVertex), m_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(SBVertex), (char *)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SBVertex), (char *)16);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SBVertex), (char *)32);
	glBindVertexArray(0);
}

Renderer2D::~Renderer2D() 
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
	glDeleteBuffers(1, &m_vao);
	glDeleteProgram(m_shader);
	delete m_nullTexture;
}

void Renderer2D::Begin() 
{
	m_renderBegun = true;
	m_currentIndex = 0;
	m_currentVertex = 0;
	m_currentTexture = 0;

	int width = 0, height = 0;
	auto window = glfwGetCurrentContext();
	glfwGetWindowSize(window, &width, &height);
	
	glUseProgram(m_shader);

	// scale the width/height based on cameraScale
	float scaledWidth = (float)width * m_cameraScale;
	float scaledHeight = (float)height * m_cameraScale;

	// get the middle of the window in order to get the scaled bounds
	float midX = m_cameraX + (width  * 0.5f);
	float midY = m_cameraY + (height * 0.5f);

	// get the bounds to use in the projection matrix
	float top = midY + (scaledHeight * 0.5f);
	float right = midX + (scaledWidth * 0.5f);
	float bottom = midY - (scaledHeight * 0.5f);
	float left = midX - (scaledWidth * 0.5f);

	auto projection = glm::ortho(left, right, bottom, top, 1.0f, -101.0f);
	glUniformMatrix4fv(glGetUniformLocation(m_shader, "projectionMatrix"), 1, false, &projection[0][0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SetRenderColour(1,1,1,1);
}

void Renderer2D::End() 
{
	if (m_renderBegun == false)
		return;

	FlushBatch();

	glUseProgram(0);

	m_renderBegun = false;
}

void Renderer2D::DrawBox(float xPos, float yPos, float width, float height, float rotation, float depth) 
{
	DrawSprite(nullptr, xPos, yPos, width, height, rotation, depth);
}

void Renderer2D::DrawCircle(float xPos, float yPos, float radius, float depth) 
{
	if (ShouldFlush(33, 96))
		FlushBatch();
	unsigned int textureID = PushTexture(m_nullTexture);

	int startIndex = m_currentVertex;

	// Centre vertex.
	m_vertices[m_currentVertex].pos[0] = xPos;
	m_vertices[m_currentVertex].pos[1] = yPos;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = 0;
	m_vertices[m_currentVertex].texcoord[1] = 0;
	m_currentVertex++;

	// Splitting circle into 32 segments.
	// Calculate delta as 2PI (circle) divided by the number of segments.
	float rotDelta = glm::pi<float>() * 2.0f / 32.0f;

	// 32 segment circle.
	for (int i = 0; i < 32; ++i) 
	{
		if (ShouldFlush())
			FlushBatch();

		m_vertices[m_currentVertex].pos[0] = glm::sin(rotDelta * i) * radius + xPos;
		m_vertices[m_currentVertex].pos[1] = glm::cos(rotDelta * i) * radius + yPos;
		m_vertices[m_currentVertex].pos[2] = depth;
		m_vertices[m_currentVertex].pos[3] = (float)textureID;
		m_vertices[m_currentVertex].color[0] = m_r;
		m_vertices[m_currentVertex].color[1] = m_g;
		m_vertices[m_currentVertex].color[2] = m_b;
		m_vertices[m_currentVertex].color[3] = m_a;
		m_vertices[m_currentVertex].texcoord[0] = 0.5f;
		m_vertices[m_currentVertex].texcoord[1] = 0.5f;
		m_currentVertex++;

		if (i == (32-1)) 
		{
			m_indices[m_currentIndex++] = startIndex;
			m_indices[m_currentIndex++] = startIndex + 1;
			m_indices[m_currentIndex++] = m_currentVertex - 1;
		}
		else 
		{
			m_indices[m_currentIndex++] = startIndex;
			m_indices[m_currentIndex++] = m_currentVertex;
			m_indices[m_currentIndex++] = m_currentVertex - 1;
		}
	}
}

void Renderer2D::DrawSprite(Texture * texture,
							 float xPos, float yPos, 
							 float width, float height, 
							 float rotation, float depth, float xOrigin, float yOrigin) 
{
	if (texture == nullptr)
		texture = m_nullTexture;

	if (ShouldFlush())
		FlushBatch();
	unsigned int textureID = PushTexture(texture);

	if (width == 0.0f)
		width = (float)texture->GetWidth();
	if (height == 0.0f)
		height = (float)texture->GetHeight();

	if (m_flipH)
		width *= -1.0f;

	if (m_flipV)
		height *= -1.0f;

	float tlX = (0.0f - xOrigin) * width;		float tlY = (0.0f - yOrigin) * height;
	float trX = (1.0f - xOrigin) * width;		float trY = (0.0f - yOrigin) * height;
	float brX = (1.0f - xOrigin) * width;		float brY = (1.0f - yOrigin) * height;
	float blX = (0.0f - xOrigin) * width;		float blY = (1.0f - yOrigin) * height;

	if (rotation != 0.0f) 
	{
		float si = glm::sin(rotation); float co = glm::cos(rotation);
		RotateAround(tlX, tlY, tlX, tlY, si, co);
		RotateAround(trX, trY, trX, trY, si, co);
		RotateAround(brX, brY, brX, brY, si, co);
		RotateAround(blX, blY, blX, blY, si, co);
	}

	int index = m_currentVertex;

	m_vertices[m_currentVertex].pos[0] = xPos + tlX;
	m_vertices[m_currentVertex].pos[1] = yPos + tlY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY + m_uvH;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = xPos + trX;
	m_vertices[m_currentVertex].pos[1] = yPos + trY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX + m_uvW;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY + m_uvH;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = xPos + brX;
	m_vertices[m_currentVertex].pos[1] = yPos + brY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX + m_uvW;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = xPos + blX;
	m_vertices[m_currentVertex].pos[1] = yPos + blY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY;
	m_currentVertex++;

	m_indices[m_currentIndex++] = (index + 0);
	m_indices[m_currentIndex++] = (index + 2);
	m_indices[m_currentIndex++] = (index + 3);

	m_indices[m_currentIndex++] = (index + 0);
	m_indices[m_currentIndex++] = (index + 1);
	m_indices[m_currentIndex++] = (index + 2);
}

void Renderer2D::DrawSpriteTransformed3x3(Texture * texture,
										   float * transformMat3x3, 
										   float width, float height, float depth,
										   float xOrigin, float yOrigin) 
{
	if (texture == nullptr)
		texture = m_nullTexture;

	if (ShouldFlush())
		FlushBatch();

	unsigned int textureID = PushTexture(texture);

	if (width == 0.0f)
		width = (float)texture->GetWidth();
	if (height == 0.0f)
		height = (float)texture->GetHeight();

	if (m_flipH)
		width *= -1.0f;

	if (m_flipV)
		height *= -1.0f;

	float tlX = (0.0f - xOrigin) * width;		float tlY = (0.0f - yOrigin) * height;
	float trX = (1.0f - xOrigin) * width;		float trY = (0.0f - yOrigin) * height;
	float brX = (1.0f - xOrigin) * width;		float brY = (1.0f - yOrigin) * height;
	float blX = (0.0f - xOrigin) * width;		float blY = (1.0f - yOrigin) * height;

	// transform the points by the matrix
	// 0 3 6
	// 1 4 7
	// 2 5 8
	float x, y;
	x = tlX; y = tlY;
	tlX = x * transformMat3x3[0] + y * transformMat3x3[3] + transformMat3x3[6];
	tlY = x * transformMat3x3[1] + y * transformMat3x3[4] + transformMat3x3[7];
	x = trX; y = trY;
	trX = x * transformMat3x3[0] + y * transformMat3x3[3] + transformMat3x3[6];
	trY = x * transformMat3x3[1] + y * transformMat3x3[4] + transformMat3x3[7];
	x = brX; y = brY;
	brX = x * transformMat3x3[0] + y * transformMat3x3[3] + transformMat3x3[6];
	brY = x * transformMat3x3[1] + y * transformMat3x3[4] + transformMat3x3[7];
	x = blX; y = blY;
	blX = x * transformMat3x3[0] + y * transformMat3x3[3] + transformMat3x3[6];
	blY = x * transformMat3x3[1] + y * transformMat3x3[4] + transformMat3x3[7];	

	int index = m_currentVertex;
	
	m_vertices[m_currentVertex].pos[0] = tlX;
	m_vertices[m_currentVertex].pos[1] = tlY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY + m_uvH;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = trX;
	m_vertices[m_currentVertex].pos[1] = trY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX + m_uvW;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY + m_uvH;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = brX;
	m_vertices[m_currentVertex].pos[1] = brY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX + m_uvW;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = blX;
	m_vertices[m_currentVertex].pos[1] = blY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY;
	m_currentVertex++;

	m_indices[m_currentIndex++] = (index + 0);
	m_indices[m_currentIndex++] = (index + 2);
	m_indices[m_currentIndex++] = (index + 3);

	m_indices[m_currentIndex++] = (index + 0);
	m_indices[m_currentIndex++] = (index + 1);
	m_indices[m_currentIndex++] = (index + 2);
}

void Renderer2D::DrawSpriteTransformed4x4(Texture * texture,
										   float * transformMat4x4, 
										   float width, float height, float depth,
										   float xOrigin, float yOrigin) 
{
	if (texture == nullptr)
		texture = m_nullTexture;

	if (ShouldFlush())
		FlushBatch();
	unsigned int textureID = PushTexture(texture);

	if (width == 0.0f)
		width = (float)texture->GetWidth();
	if (height == 0.0f)
		height = (float)texture->GetHeight();

	if (m_flipH)
		width *= -1.0f;

	if (m_flipV)
		height *= -1.0f;

	float tlX = (0.0f - xOrigin) * width;		float tlY = (0.0f - yOrigin) * height;
	float trX = (1.0f - xOrigin) * width;		float trY = (0.0f - yOrigin) * height;
	float brX = (1.0f - xOrigin) * width;		float brY = (1.0f - yOrigin) * height;
	float blX = (0.0f - xOrigin) * width;		float blY = (1.0f - yOrigin) * height;

	// transform the points by the matrix
	// 0 4 8  12
	// 1 5 9  13
	// 2 6 10 14
	// 3 7 11 15
	float x, y;
	x = tlX; y = tlY;
	tlX = x * transformMat4x4[0] + y * transformMat4x4[4] + transformMat4x4[12];
	tlY = x * transformMat4x4[1] + y * transformMat4x4[5] + transformMat4x4[13];
	x = trX; y = trY;
	trX = x * transformMat4x4[0] + y * transformMat4x4[4] + transformMat4x4[12];
	trY = x * transformMat4x4[1] + y * transformMat4x4[5] + transformMat4x4[13];
	x = brX; y = brY;
	brX = x * transformMat4x4[0] + y * transformMat4x4[4] + transformMat4x4[12];
	brY = x * transformMat4x4[1] + y * transformMat4x4[5] + transformMat4x4[13];
	x = blX; y = blY;
	blX = x * transformMat4x4[0] + y * transformMat4x4[4] + transformMat4x4[12];
	blY = x * transformMat4x4[1] + y * transformMat4x4[5] + transformMat4x4[13];

	int index = m_currentVertex;

	m_vertices[m_currentVertex].pos[0] = tlX;
	m_vertices[m_currentVertex].pos[1] = tlY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY + m_uvH;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = trX;
	m_vertices[m_currentVertex].pos[1] = trY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX + m_uvW;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY + m_uvH;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = brX;
	m_vertices[m_currentVertex].pos[1] = brY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX + m_uvW;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY;
	m_currentVertex++;

	m_vertices[m_currentVertex].pos[0] = blX;
	m_vertices[m_currentVertex].pos[1] = blY;
	m_vertices[m_currentVertex].pos[2] = depth;
	m_vertices[m_currentVertex].pos[3] = (float)textureID;
	m_vertices[m_currentVertex].color[0] = m_r;
	m_vertices[m_currentVertex].color[1] = m_g;
	m_vertices[m_currentVertex].color[2] = m_b;
	m_vertices[m_currentVertex].color[3] = m_a;
	m_vertices[m_currentVertex].texcoord[0] = m_uvX;
	m_vertices[m_currentVertex].texcoord[1] = m_uvY;
	m_currentVertex++;
	
	m_indices[m_currentIndex++] = (index + 0);
	m_indices[m_currentIndex++] = (index + 2);
	m_indices[m_currentIndex++] = (index + 3);

	m_indices[m_currentIndex++] = (index + 0);
	m_indices[m_currentIndex++] = (index + 1);
	m_indices[m_currentIndex++] = (index + 2);
}

void Renderer2D::DrawLine(float x1, float y1, float x2, float y2, float thickness, float depth) 
{
	float xDiff = x2 - x1;
	float yDiff = y2 - y1;
	float len = glm::sqrt(xDiff * xDiff + yDiff * yDiff);
	float xDir = xDiff / len;
	float yDir = yDiff / len;

	float rot = glm::atan(yDir, xDir);

	float uvX = m_uvX;
	float uvY = m_uvY;
	float uvW = m_uvW;
	float uvH = m_uvH;

	SetUVRect(0.0f, 0.0f, 1.0f, 1.0f);

	DrawSprite(m_nullTexture, x1, y1, len, thickness, rot, depth, 0.0f, 0.5f);

	SetUVRect(uvX, uvY, uvW, uvH);
}

void Renderer2D::DrawText2D(Font * font, const char* text, float xPos, float yPos, float depth) {

	if (font == nullptr ||
		font->m_glHandle == 0)
		return;

	stbtt_aligned_quad Q = {};

	if (ShouldFlush() || m_currentTexture >= TEXTURE_STACK_SIZE - 1)
		FlushBatch();

	glActiveTexture(GL_TEXTURE0 + m_currentTexture++);
	glBindTexture(GL_TEXTURE_2D, font->GetTextureHandle());
	glActiveTexture(GL_TEXTURE0);
	m_fontTexture[m_currentTexture - 1] = 1;

	// font renders top to bottom, so we need to invert it
	int w = 0, h = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &w, &h);

	yPos = h - yPos;

	while (*text != 0) 
	{
		if (ShouldFlush() || m_currentTexture >= TEXTURE_STACK_SIZE - 1) 
		{
			FlushBatch();

			glActiveTexture(GL_TEXTURE0 + m_currentTexture++);
			glBindTexture(GL_TEXTURE_2D, font->GetTextureHandle());
			glActiveTexture(GL_TEXTURE0);
			m_fontTexture[m_currentTexture - 1] = 1;
		}

		stbtt_GetBakedQuad(font->m_glyphData, font->m_textureWidth, font->m_textureHeight, *text, &xPos, &yPos, &Q, 1);

		int index = m_currentVertex;

		m_vertices[m_currentVertex].pos[0] = Q.x0;
		m_vertices[m_currentVertex].pos[1] = h - Q.y1;
		m_vertices[m_currentVertex].pos[2] = depth;
		m_vertices[m_currentVertex].pos[3] = (float)m_currentTexture - 1;
		m_vertices[m_currentVertex].color[0] = m_r;
		m_vertices[m_currentVertex].color[1] = m_g;
		m_vertices[m_currentVertex].color[2] = m_b;
		m_vertices[m_currentVertex].color[3] = m_a;
		m_vertices[m_currentVertex].texcoord[0] = Q.s0;
		m_vertices[m_currentVertex].texcoord[1] = Q.t1;
		m_currentVertex++;
		m_vertices[m_currentVertex].pos[0] = Q.x1;
		m_vertices[m_currentVertex].pos[1] = h - Q.y1;
		m_vertices[m_currentVertex].pos[2] = depth;
		m_vertices[m_currentVertex].pos[3] = (float)m_currentTexture - 1;
		m_vertices[m_currentVertex].color[0] = m_r;
		m_vertices[m_currentVertex].color[1] = m_g;
		m_vertices[m_currentVertex].color[2] = m_b;
		m_vertices[m_currentVertex].color[3] = m_a;
		m_vertices[m_currentVertex].texcoord[0] = Q.s1;
		m_vertices[m_currentVertex].texcoord[1] = Q.t1;
		m_currentVertex++;
		m_vertices[m_currentVertex].pos[0] = Q.x1;
		m_vertices[m_currentVertex].pos[1] = h - Q.y0;
		m_vertices[m_currentVertex].pos[2] = depth;
		m_vertices[m_currentVertex].pos[3] = (float)m_currentTexture - 1;
		m_vertices[m_currentVertex].color[0] = m_r;
		m_vertices[m_currentVertex].color[1] = m_g;
		m_vertices[m_currentVertex].color[2] = m_b;
		m_vertices[m_currentVertex].color[3] = m_a;
		m_vertices[m_currentVertex].texcoord[0] = Q.s1;
		m_vertices[m_currentVertex].texcoord[1] = Q.t0;
		m_currentVertex++;
		m_vertices[m_currentVertex].pos[0] = Q.x0;
		m_vertices[m_currentVertex].pos[1] = h - Q.y0;
		m_vertices[m_currentVertex].pos[2] = depth;
		m_vertices[m_currentVertex].pos[3] = (float)m_currentTexture - 1;
		m_vertices[m_currentVertex].color[0] = m_r;
		m_vertices[m_currentVertex].color[1] = m_g;
		m_vertices[m_currentVertex].color[2] = m_b;
		m_vertices[m_currentVertex].color[3] = m_a;
		m_vertices[m_currentVertex].texcoord[0] = Q.s0;
		m_vertices[m_currentVertex].texcoord[1] = Q.t0;
		m_currentVertex++;
		
		m_indices[m_currentIndex++] = (index + 0);
		m_indices[m_currentIndex++] = (index + 2);
		m_indices[m_currentIndex++] = (index + 3);

		m_indices[m_currentIndex++] = (index + 0);
		m_indices[m_currentIndex++] = (index + 1);
		m_indices[m_currentIndex++] = (index + 2);

		text++;
	}
}

bool Renderer2D::ShouldFlush(int additionalVertices, int additionalIndices) 
{
	return (m_currentVertex + additionalVertices) >= (MAX_SPRITES * 4) || 
		(m_currentIndex + additionalIndices) >= (MAX_SPRITES * 6);
}

void Renderer2D::FlushBatch() 
{
	// Dont render anything.
	if (m_currentVertex == 0 || m_currentIndex == 0 || m_renderBegun == false)
		return; 
	
	char buf[32];
	for (int i = 0; i < TEXTURE_STACK_SIZE; ++i) 
	{
		sprintf_s(buf, "isFontTexture[%i]", i);
		glUniform1i(glGetUniformLocation(m_shader, buf), m_fontTexture[i]);
	}

	int depthFunc = GL_LESS;
	glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
	glDepthFunc(GL_LEQUAL);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

	glBufferSubData(GL_ARRAY_BUFFER, 0, m_currentVertex * sizeof(SBVertex), m_vertices);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_currentIndex * sizeof(unsigned short), m_indices);

	glDrawElements(GL_TRIANGLES, m_currentIndex, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);

	glDepthFunc(depthFunc);

	// Clear the active textures.
	memset(m_textureStack, 0, sizeof(Texture*) * m_currentTexture);
	memset(m_fontTexture, 0, sizeof(int) * m_currentTexture);

	// Reset vertex, index and texture count.
	m_currentIndex = 0;
	m_currentVertex = 0;
	m_currentTexture = 0;
}

unsigned int Renderer2D::PushTexture(Texture* texture) 
{
	// Check if the texture is already in use.
	// If so, return as we dont need to add it to our list of active txtures again.
	for (unsigned int i = 0; i <= m_currentTexture; i++) 
	{
		if (m_textureStack[i] == texture)
			return i;
	}

	// If we've used all the textures we can, than we need to flush to make room for another texture change.
	if (m_currentTexture >= TEXTURE_STACK_SIZE - 1)
		FlushBatch();

	// add the texture to our active texture list
	m_textureStack[m_currentTexture] = texture;

	glActiveTexture(GL_TEXTURE0 + m_currentTexture);
	glBindTexture(GL_TEXTURE_2D, texture->GetHandle());
	glActiveTexture(GL_TEXTURE0);

	// return what the current texture was and increment
	return m_currentTexture++;
}

void Renderer2D::SetRenderColour(float r, float g, float b, float a) 
{
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}

void Renderer2D::SetRenderColour(unsigned int colour) 
{
	m_r = ((colour & 0xFF000000) >> 24) / 255.0f;
	m_g = ((colour & 0x00FF0000) >> 16) / 255.0f;
	m_b = ((colour & 0x0000FF00) >> 8) / 255.0f;
	m_a = ((colour & 0x000000FF) >> 0) / 255.0f;
}

void Renderer2D::SetUVRect(float uvX, float uvY, float uvW, float uvH) 
{
	m_uvX = uvX;
	m_uvY = uvY;
	m_uvW = uvW;
	m_uvH = uvH;
}

void Renderer2D::SetFlipped(bool horizontal, bool vertical)
{
	m_flipH = horizontal;
	m_flipV = vertical;
}

void Renderer2D::RotateAround(float inX, float inY, float& outX, float& outY, float sin, float cos) 
{
	outX = inX * cos - inY * sin;
	outY = inX * sin + inY * cos;
}

} // namespace aie