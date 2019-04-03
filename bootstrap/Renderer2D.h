//----------------------------------------------------------------------------
// A class for rendering 2D sprites and text using imediate-mode rendering.
//
// Example usage:
// renderer->DrawBox(600, 500, 60, 20);
//----------------------------------------------------------------------------
#pragma once

namespace aie 
{

class Texture;
class Font;

class Renderer2D 
{
public:
	Renderer2D();
	virtual ~Renderer2D();

	// All draw calls must occur between a Begin()/End() pair.
	virtual void Begin();
	virtual void End();

	// Simple shape rendering.
	virtual void DrawBox(float xPos, float yPos, float width, float height, float rotation = 0.0f, float depth = 0.0f);
	virtual void DrawCircle(float xPos, float yPos, float radius, float depth = 0.0f);

	// If texture is nullptr then it renders a coloured square.
	// Depth is in the range [0,100] with lower being closer to the viewer.
	virtual void DrawSprite(Texture* texture, float xPos, float yPos, float width = 0.0f, float height = 0.0f, float rotation = 0.0f, float depth = 0.0f, float xOrigin = 0.5f, float yOrigin = 0.5f);
	virtual void DrawSpriteTransformed3x3(Texture* texture, float* transformMat3x3, float width = 0.0f, float height = 0.0f, float depth = 0.0f, float xOrigin = 0.5f, float yOrigin = 0.5f);
	virtual void DrawSpriteTransformed4x4(Texture* texture, float* transformMat4x4, float width = 0.0f, float height = 0.0f, float depth = 0.0f, float xOrigin = 0.5f, float yOrigin = 0.5f);

	// Draws a simple coloured line with a given thickness.
	// Depth is in the range [0,100] with lower being closer to the viewer.
	virtual void DrawLine(float x1, float y1, float x2, float y2, float thickness = 1.0f, float depth = 0.0f );

	// Draws simple text on the screen horizontally.
	// Depth is in the range [0,100] with lower being closer to the viewer.
	virtual void DrawText2D(Font* font, const char* text, float xPos, float yPos, float depth = 0.0f);

	// Sets the tint colour for all subsequent draw calls.
	void SetRenderColour(float r, float g, float b, float a = 1.0f);
	void SetRenderColour(unsigned int colour);

	// Can be used to set the texture coordinates for all subsequent DrawSprite() calls.
	void SetUVRect(float uvX, float uvY, float uvW, float uvH);

	// Flip sprite rendering on the horizontal or vertical axis.
	void SetFlipped(bool horizontal, bool vertical = false);

	// Specify the camera position.
	void SetCameraPos(float x, float y) { m_cameraX = x; m_cameraY = y; }
	void GetCameraPos(float& x, float& y) const { x = m_cameraX; y = m_cameraY; }

	// Specify the camera scale/zoom, increase the scale to zoom out, decrease to zoom in.
	void  SetCameraScale(float scale) { m_cameraScale = scale; }
	float GetCameraScale() { return m_cameraScale; }

protected:
	// Sprite handling.
	enum { MAX_SPRITES = 512 };
	struct SBVertex 
	{
		float pos[4];
		float color[4];
		float texcoord[2];
	};

	// Helper methods used during drawing.
	bool ShouldFlush(int additionalVertices = 0, int additionalIndices = 0);
	void FlushBatch();
	unsigned int PushTexture(Texture* texture);
	void RotateAround(float inX, float inY, float& outX, float& outY, float sin, float cos);

	// Indicates in the middle of a Begin()/End() pair.
	bool				m_renderBegun;

	// Shader used to render sprites.
	unsigned int		m_shader;

	// Data used for a virtual camera.
	float				m_projectionMatrix[16];
	float				m_cameraX, m_cameraY;
	float				m_cameraScale;

	// Texture handling.
	enum { TEXTURE_STACK_SIZE = 16 };
	Texture*			m_nullTexture;
	Texture*			m_textureStack[TEXTURE_STACK_SIZE];
	int					m_fontTexture[TEXTURE_STACK_SIZE];
	unsigned int		m_currentTexture;
	float				m_uvX, m_uvY, m_uvW, m_uvH;

	bool				m_flipH, m_flipV;

	// Data used for OpenGL to draw the sprites (with padding).
	SBVertex			m_vertices[MAX_SPRITES * 4];
	unsigned short		m_indices[MAX_SPRITES * 6];
	int					m_currentVertex, m_currentIndex;
	unsigned int		m_vao, m_vbo, m_ibo;

	// Colour in red, green, blue and alpha using a 0.0 - 1.0 range.
	float				m_r, m_g, m_b, m_a;
};

} // namespace aie