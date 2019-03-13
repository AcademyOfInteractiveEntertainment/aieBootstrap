//----------------------------------------------------------------------------
// Shaders written in GLSL
// https://www.khronos.org/opengl/wiki/Core_Language_%28GLSL%29
//----------------------------------------------------------------------------
#pragma once

static const char* sk_spriteVertexShader =
	"#version 150\n																				\
																								\
	in vec4 position;																			\
	in vec4 colour;																				\
	in vec2 texcoord;																			\
																								\
	out vec4 vColour;																			\
	out vec2 vTexCoord;																			\
	out float vTextureID;																		\
																								\
	uniform mat4 projectionMatrix;																\
																								\
	void main()																					\
	{																							\
		vColour = colour;																		\
		vTexCoord = texcoord;																	\
		vTextureID = position.w;																\
																								\
		gl_Position = projectionMatrix * vec4(position.x, position.y, position.z, 1.0f);		\
	}";

static const char* sk_spriteFragmentShader = 
	"#version 150\n																				\
																								\
	in vec4 vColour;																			\
	in vec2 vTexCoord;																			\
	in float vTextureID;																		\
																								\
	out vec4 fragColour;																		\
																								\
	const int TEXTURE_STACK_SIZE = 16;															\
	uniform sampler2D textureStack[TEXTURE_STACK_SIZE];											\
	uniform int isFontTexture[TEXTURE_STACK_SIZE];												\
																								\
	void main()																					\
	{																							\
		int id = int(vTextureID);																\
		if (id < TEXTURE_STACK_SIZE)															\
		{																						\
			vec4 rgba = texture2D(textureStack[id], vTexCoord);									\
			if (isFontTexture[id] == 1)															\
				rgba = rgba.rrrr;																\
																								\
			fragColour = rgba * vColour;														\
		}																						\
		else																					\
			fragColour = vColour;																\
																								\
		if (fragColour.a < 0.001f)																\
			discard;																			\
	}";

static const char* sk_gizmoVertexShader = 
	"#version 150\n																				\
																								\
	in vec4 Position;																			\
	in vec4 Colour;																				\
																								\
	out vec4 vColour;																			\
	uniform mat4 ProjectionView;																\
																								\
	void main()																					\
	{																							\
		vColour = Colour;																		\
		gl_Position = ProjectionView * Position;												\
	}";

static const char* sk_gizmoFragmentShader = 
	"#version 150\n																				\
	in vec4 vColour;																			\
    out vec4 FragColor;																			\
																								\
	void main()																					\
	{																							\
		FragColor = vColour;																	\
	}";