#include "aie/bootstrap/Gizmos.h"

#include <glew/glew.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>

#define COMPARE(x, y) (fabsf((x) - (y)) <= (FLT_EPSILON + 0.000001f) * fmaxf(1.f, fmaxf(fabsf(x), fabsf(y))))

namespace aie
{
	Gizmos* Gizmos::m_singleton = nullptr;

	Gizmos::Gizmos(unsigned int maxLines, unsigned int maxTris, unsigned int max2DLines, unsigned int max2DTris)
		: m_maxLines{ maxLines }, m_lineCount{ 0 }, m_lines{ new GizmoLine[maxLines] }, m_maxTris{ maxTris },
		m_triCount{ 0 }, m_tris{ new GizmoTri[maxTris] }, m_transparentTriCount{ 0 },
		m_transparentTris{ new GizmoTri[maxTris] }, m_max2DLines{ max2DLines }, m_2DlineCount{ 0 },
		m_2Dlines{ new GizmoLine[max2DLines] }, m_max2DTris{ max2DTris }, m_2DtriCount{ 0 },
		m_2Dtris{ new GizmoTri[max2DTris] }
	{
		// create shaders
		const char* vsSource = "#version 150\n \
					 in vec4 Position; \
					 in vec4 Colour; \
					 out vec4 vColour; \
					 uniform mat4 ProjectionView; \
					 void main() { vColour = Colour; gl_Position = ProjectionView * Position; }";

		const char* fsSource = "#version 150\n \
					 in vec4 vColour; \
                     out vec4 FragColor; \
					 void main()	{ FragColor = vColour; }";

		const unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
		const unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vs, 1, &vsSource, nullptr);
		glCompileShader(vs);

		glShaderSource(fs, 1, &fsSource, nullptr);
		glCompileShader(fs);

		m_shader = glCreateProgram();
		glAttachShader(m_shader, vs);
		glAttachShader(m_shader, fs);
		glBindAttribLocation(m_shader, 0, "Position");
		glBindAttribLocation(m_shader, 1, "Colour");
		glLinkProgram(m_shader);

		int success = GL_FALSE;
		glGetProgramiv(m_shader, GL_LINK_STATUS, &success);
		if (success == GL_FALSE) 
		{
			int infoLogLength = 0;
			glGetProgramiv(m_shader, GL_INFO_LOG_LENGTH, &infoLogLength);
			char* infoLog = new char[infoLogLength + 1];

			glGetProgramInfoLog(m_shader, infoLogLength, nullptr, infoLog);
			printf("Error: Failed to link Gizmo shader program!\n%s\n", infoLog);
			delete[] infoLog;
		}

		glDeleteShader(vs);
		glDeleteShader(fs);

		// create VBOs
		glGenBuffers(1, &m_lineVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
		glBufferData(GL_ARRAY_BUFFER, m_maxLines * sizeof(GizmoLine), m_lines, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &m_triVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_triVBO);
		glBufferData(GL_ARRAY_BUFFER, m_maxTris * sizeof(GizmoTri), m_tris, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &m_transparentTriVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_transparentTriVBO);
		glBufferData(GL_ARRAY_BUFFER, m_maxTris * sizeof(GizmoTri), m_transparentTris, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &m_2DlineVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_2DlineVBO);
		glBufferData(GL_ARRAY_BUFFER, m_max2DLines * sizeof(GizmoLine), m_2Dlines, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &m_2DtriVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_2DtriVBO);
		glBufferData(GL_ARRAY_BUFFER, m_max2DTris * sizeof(GizmoTri), m_2Dtris, GL_DYNAMIC_DRAW);

		glGenVertexArrays(1, &m_lineVAO);
		glBindVertexArray(m_lineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), nullptr);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), reinterpret_cast<void*>(16));

		glGenVertexArrays(1, &m_triVAO);
		glBindVertexArray(m_triVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_triVBO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), nullptr);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), reinterpret_cast<void*>(16));

		glGenVertexArrays(1, &m_transparentTriVAO);
		glBindVertexArray(m_transparentTriVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_transparentTriVBO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), nullptr);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), reinterpret_cast<void*>(16));

		glGenVertexArrays(1, &m_2DlineVAO);
		glBindVertexArray(m_2DlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_2DlineVBO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), nullptr);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), reinterpret_cast<void*>(16));

		glGenVertexArrays(1, &m_2DtriVAO);
		glBindVertexArray(m_2DtriVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_2DtriVBO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), nullptr);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), reinterpret_cast<void*>(16));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	Gizmos::~Gizmos() {
		delete[] m_lines;
		delete[] m_tris;
		delete[] m_transparentTris;
		glDeleteBuffers(1, &m_lineVBO);
		glDeleteBuffers(1, &m_triVBO);
		glDeleteBuffers(1, &m_transparentTriVBO);
		glDeleteVertexArrays(1, &m_lineVAO);
		glDeleteVertexArrays(1, &m_triVAO);
		glDeleteVertexArrays(1, &m_transparentTriVAO);
		delete[] m_2Dlines;
		delete[] m_2Dtris;
		glDeleteBuffers(1, &m_2DlineVBO);
		glDeleteBuffers(1, &m_2DtriVBO);
		glDeleteVertexArrays(1, &m_2DlineVAO);
		glDeleteVertexArrays(1, &m_2DtriVAO);
		glDeleteProgram(m_shader);
	}

	void Gizmos::Create(unsigned int maxLines, unsigned int maxTris, unsigned int max2DLines, unsigned int max2DTris) {
		if (m_singleton == nullptr)
			m_singleton = new Gizmos(maxLines, maxTris, max2DLines, max2DTris);
	}

	void Gizmos::Destroy()
	{
		delete m_singleton;
		m_singleton = nullptr;
	}

	void Gizmos::Clear()
	{
		m_singleton->m_lineCount = 0;
		m_singleton->m_triCount = 0;
		m_singleton->m_transparentTriCount = 0;
		m_singleton->m_2DlineCount = 0;
		m_singleton->m_2DtriCount = 0;
	}

	// Adds 3 unit-length lines (red,green,blue) representing the 3 axis of a transform, 
	// at the transform's translation. Optional scale available.
	void Gizmos::AddTransform(const mat4& transform, float scale)
	{
		vec4 vXAxis = transform[3] + transform[0] * scale;
		vec4 vYAxis = transform[3] + transform[1] * scale;
		vec4 vZAxis = transform[3] + transform[2] * scale;

		vec4 vRed(1, 0, 0, 1);
		vec4 vGreen(0, 1, 0, 1);
		vec4 vBlue(0, 0, 1, 1);

		AddLine(vec3(transform[3]), vec3(vXAxis), vRed, vRed);
		AddLine(vec3(transform[3]), vec3(vYAxis), vGreen, vGreen);
		AddLine(vec3(transform[3]), vec3(vZAxis), vBlue, vBlue);
	}

	void Gizmos::AddAABB(const vec3& center, const vec3& rvExtents, const vec4& colour, const mat4* transform)
	{
		vec3 vVerts[8];
		vec3 c = center;
		vec3 vX(rvExtents.x, 0, 0);
		vec3 vY(0, rvExtents.y, 0);
		vec3 vZ(0, 0, rvExtents.z);

		if (transform != nullptr) 
		{
			vX = vec3(*transform * vec4(vX, 0));
			vY = vec3(*transform * vec4(vY, 0));
			vZ = vec3(*transform * vec4(vZ, 0));
			c = vec3((*transform)[3]) + c;
		}

		// top verts
		vVerts[0] = c - vX - vZ - vY;
		vVerts[1] = c - vX + vZ - vY;
		vVerts[2] = c + vX + vZ - vY;
		vVerts[3] = c + vX - vZ - vY;

		// bottom verts
		vVerts[4] = c - vX - vZ + vY;
		vVerts[5] = c - vX + vZ + vY;
		vVerts[6] = c + vX + vZ + vY;
		vVerts[7] = c + vX - vZ + vY;

		AddLine(vVerts[0], vVerts[1], colour, colour);
		AddLine(vVerts[1], vVerts[2], colour, colour);
		AddLine(vVerts[2], vVerts[3], colour, colour);
		AddLine(vVerts[3], vVerts[0], colour, colour);

		AddLine(vVerts[4], vVerts[5], colour, colour);
		AddLine(vVerts[5], vVerts[6], colour, colour);
		AddLine(vVerts[6], vVerts[7], colour, colour);
		AddLine(vVerts[7], vVerts[4], colour, colour);

		AddLine(vVerts[0], vVerts[4], colour, colour);
		AddLine(vVerts[1], vVerts[5], colour, colour);
		AddLine(vVerts[2], vVerts[6], colour, colour);
		AddLine(vVerts[3], vVerts[7], colour, colour);
	}

	void Gizmos::AddAABBFilled(const vec3& center, const vec3& rvExtents, 
		const vec4& fillColour, const mat4* transform)
	{
		vec3 vVerts[8];
		vec3 tempCenter = center;
		vec3 vX(rvExtents.x, 0, 0);
		vec3 vY(0, rvExtents.y, 0);
		vec3 vZ(0, 0, rvExtents.z);

		if (transform != nullptr) 
		{
			vX = vec3(*transform * vec4(vX, 0));
			vY = vec3(*transform * vec4(vY, 0));
			vZ = vec3(*transform * vec4(vZ, 0));
			tempCenter = vec3((*transform)[3]) + tempCenter;
		}

		// top verts
		vVerts[0] = tempCenter - vX - vZ - vY;
		vVerts[1] = tempCenter - vX + vZ - vY;
		vVerts[2] = tempCenter + vX + vZ - vY;
		vVerts[3] = tempCenter + vX - vZ - vY;

		// bottom verts
		vVerts[4] = tempCenter - vX - vZ + vY;
		vVerts[5] = tempCenter - vX + vZ + vY;
		vVerts[6] = tempCenter + vX + vZ + vY;
		vVerts[7] = tempCenter + vX - vZ + vY;

		constexpr vec4 vWhite(1, 1, 1, 1);

		AddLine(vVerts[0], vVerts[1], vWhite, vWhite);
		AddLine(vVerts[1], vVerts[2], vWhite, vWhite);
		AddLine(vVerts[2], vVerts[3], vWhite, vWhite);
		AddLine(vVerts[3], vVerts[0], vWhite, vWhite);

		AddLine(vVerts[4], vVerts[5], vWhite, vWhite);
		AddLine(vVerts[5], vVerts[6], vWhite, vWhite);
		AddLine(vVerts[6], vVerts[7], vWhite, vWhite);
		AddLine(vVerts[7], vVerts[4], vWhite, vWhite);

		AddLine(vVerts[0], vVerts[4], vWhite, vWhite);
		AddLine(vVerts[1], vVerts[5], vWhite, vWhite);
		AddLine(vVerts[2], vVerts[6], vWhite, vWhite);
		AddLine(vVerts[3], vVerts[7], vWhite, vWhite);

		// top
		AddTri(vVerts[2], vVerts[1], vVerts[0], fillColour);
		AddTri(vVerts[3], vVerts[2], vVerts[0], fillColour);

		// bottom
		AddTri(vVerts[5], vVerts[6], vVerts[4], fillColour);
		AddTri(vVerts[6], vVerts[7], vVerts[4], fillColour);

		// front
		AddTri(vVerts[4], vVerts[3], vVerts[0], fillColour);
		AddTri(vVerts[7], vVerts[3], vVerts[4], fillColour);

		// back
		AddTri(vVerts[1], vVerts[2], vVerts[5], fillColour);
		AddTri(vVerts[2], vVerts[6], vVerts[5], fillColour);

		// left
		AddTri(vVerts[0], vVerts[1], vVerts[4], fillColour);
		AddTri(vVerts[1], vVerts[5], vVerts[4], fillColour);

		// right
		AddTri(vVerts[2], vVerts[3], vVerts[7], fillColour);
		AddTri(vVerts[6], vVerts[2], vVerts[7], fillColour);
	}

	void Gizmos::AddCylinderFilled(const vec3& center, float radius, float fHalfLength,
		unsigned int segments, const vec4& fillColour, const mat4* transform)
	{
		vec4 white(1, 1, 1, 1);

		vec3 tempCenter = transform != nullptr ? vec3((*transform)[3]) + center : center;

		float segmentSize = 2 * glm::pi<float>() / static_cast<float>(segments);

		for (unsigned int i = 0; i < segments; ++i) 
		{
			const float iF = static_cast<float>(i);

			vec3 v0Top(0, fHalfLength, 0);
			vec3 v1Top(sinf(iF * segmentSize) * radius, fHalfLength, cosf(iF * segmentSize) * radius);
			vec3 v2Top(sinf((iF + 1.f) * segmentSize) * radius, fHalfLength, cosf((iF + 1.f) * segmentSize) * radius);
			vec3 v0Bottom(0, -fHalfLength, 0);
			vec3 v1Bottom(sinf(iF * segmentSize) * radius, -fHalfLength, cosf(iF * segmentSize) * radius);
			vec3 v2Bottom(sinf((iF + 1.f) * segmentSize) * radius, -fHalfLength, cosf((iF + 1.f) * segmentSize) * radius);

			if (transform != nullptr) 
			{
				v0Top = vec3(*transform * vec4(v0Top, 0));
				v1Top = vec3(*transform * vec4(v1Top, 0));
				v2Top = vec3(*transform * vec4(v2Top, 0));
				v0Bottom = vec3(*transform * vec4(v0Bottom, 0));
				v1Bottom = vec3(*transform * vec4(v1Bottom, 0));
				v2Bottom = vec3(*transform * vec4(v2Bottom, 0));
			}

			// triangles
			AddTri(tempCenter + v0Top, tempCenter + v1Top, tempCenter + v2Top, fillColour);
			AddTri(tempCenter + v0Bottom, tempCenter + v2Bottom, tempCenter + v1Bottom, fillColour);
			AddTri(tempCenter + v2Top, tempCenter + v1Top, tempCenter + v1Bottom, fillColour);
			AddTri(tempCenter + v1Bottom, tempCenter + v2Bottom, tempCenter + v2Top, fillColour);

			// lines
			AddLine(tempCenter + v1Top, tempCenter + v2Top, white, white);
			AddLine(tempCenter + v1Top, tempCenter + v1Bottom, white, white);
			AddLine(tempCenter + v1Bottom, tempCenter + v2Bottom, white, white);
		}
	}

	void Gizmos::AddRing(const vec3& center, float innerRadius, float outerRadius,
		unsigned int segments, const vec4& fillColour, const mat4* transform)
	{
		vec4 vSolid = fillColour;
		vSolid.w = 1;

		vec3 tempCenter = transform != nullptr ? vec3((*transform)[3]) + center : center;

		float fSegmentSize = 2 * glm::pi<float>() / static_cast<float>(segments);

		for (unsigned int i = 0; i < segments; ++i) 
		{
			const float iF = static_cast<float>(i);

			vec3 v1Outer(sinf(iF * fSegmentSize) * outerRadius, 0, cosf(iF * fSegmentSize) * outerRadius);
			vec3 v2Outer(sinf((iF + 1.f) * fSegmentSize) * outerRadius, 0, cosf((iF + 1.f) * fSegmentSize) * outerRadius);
			vec3 v1Inner(sinf(iF * fSegmentSize) * innerRadius, 0, cosf(iF * fSegmentSize) * innerRadius);
			vec3 v2Inner(sinf((iF + 1.f) * fSegmentSize) * innerRadius, 0, cosf((iF + 1.f) * fSegmentSize) * innerRadius);

			if (transform != nullptr) 
			{
				v1Outer = vec3(*transform * vec4(v1Outer, 0));
				v2Outer = vec3(*transform * vec4(v2Outer, 0));
				v1Inner = vec3(*transform * vec4(v1Inner, 0));
				v2Inner = vec3(*transform * vec4(v2Inner, 0));
			}

			if (!COMPARE(fillColour.w, 0.f)) 
			{
				AddTri(tempCenter + v2Outer, tempCenter + v1Outer, tempCenter + v1Inner, fillColour);
				AddTri(tempCenter + v1Inner, tempCenter + v2Inner, tempCenter + v2Outer, fillColour);

				AddTri(tempCenter + v1Inner, tempCenter + v1Outer, tempCenter + v2Outer, fillColour);
				AddTri(tempCenter + v2Outer, tempCenter + v2Inner, tempCenter + v1Inner, fillColour);
			}
			else 
			{
				// line
				AddLine(tempCenter + v1Inner, tempCenter + v2Inner, vSolid, vSolid);
				AddLine(tempCenter + v1Outer, tempCenter + v2Outer, vSolid, vSolid);
			}
		}
	}

	void Gizmos::AddDisk(const vec3& center, const float radius, const unsigned int segments, 
		const vec4& fillColour, const mat4* transform)
	{
		vec4 vSolid = fillColour;
		vSolid.w = 1;

		const vec3 tempCenter = transform != nullptr ? vec3((*transform)[3]) + center : center;

		const float fSegmentSize = 2 * glm::pi<float>() / static_cast<float>(segments);

		for (unsigned int i = 0; i < segments; ++i) 
		{
			const float iF = static_cast<float>(i);

			vec3 v1Outer(sinf(iF * fSegmentSize) * radius, 0, cosf(iF * fSegmentSize) * radius);
			vec3 v2Outer(sinf((iF + 1.f) * fSegmentSize) * radius, 0, cosf((iF + 1.f) * fSegmentSize) * radius);

			if (transform != nullptr) 
			{
				v1Outer = vec3((*transform * vec4(v1Outer, 0)));
				v2Outer = vec3((*transform * vec4(v2Outer, 0)));
			}

			if (!COMPARE(fillColour.w, 0)) 
			{
				AddTri(tempCenter, tempCenter + v1Outer, tempCenter + v2Outer, fillColour);
				AddTri(tempCenter + v2Outer, tempCenter + v1Outer, tempCenter, fillColour);
			}
			else 
			{
				// line
				AddLine(tempCenter + v1Outer, tempCenter + v2Outer, vSolid, vSolid);
			}
		}
	}

	void Gizmos::AddArc(const vec3& center, float rotation,	float radius, float arcHalfAngle,
		unsigned int segments, const vec4& fillColour, const mat4* transform)
	{
		vec4 vSolid = fillColour;
		vSolid.w = 1;

		vec3 tempCenter = transform != nullptr ? vec3((*transform)[3]) + center : center;

		float fSegmentSize = 2.f * arcHalfAngle / static_cast<float>(segments);

		for (unsigned int i = 0; i < segments; ++i) 
		{
			const float iF = static_cast<float>(i);

			vec3 v1Outer(sinf(iF * fSegmentSize - arcHalfAngle + rotation) * radius, 0, cosf(iF * fSegmentSize - arcHalfAngle + rotation) * radius);
			vec3 v2Outer(sinf((iF + 1.f) * fSegmentSize - arcHalfAngle + rotation) * radius, 0, cosf((iF + 1.f) * fSegmentSize - arcHalfAngle + rotation) * radius);

			if (transform != nullptr) 
			{
				v1Outer = vec3(*transform * vec4(v1Outer, 0));
				v2Outer = vec3(*transform * vec4(v2Outer, 0));
			}

			if (!COMPARE(fillColour.w, 0)) 
			{
				AddTri(tempCenter, tempCenter + v1Outer, tempCenter + v2Outer, fillColour);
				AddTri(tempCenter + v2Outer, tempCenter + v1Outer, tempCenter, fillColour);
			}
			else 
			{
				// line
				AddLine(tempCenter + v1Outer, tempCenter + v2Outer, vSolid, vSolid);
			}
		}

		// edge lines
		if (COMPARE(fillColour.w, 0)) 
		{
			vec3 v1Outer(sinf(-arcHalfAngle + rotation) * radius, 0, cosf(-arcHalfAngle + rotation) * radius);
			vec3 v2Outer(sinf(arcHalfAngle + rotation) * radius, 0, cosf(arcHalfAngle + rotation) * radius);

			if (transform != nullptr) 
			{
				v1Outer = vec3(*transform * vec4(v1Outer, 0));
				v2Outer = vec3(*transform * vec4(v2Outer, 0));
			}

			AddLine(tempCenter, tempCenter + v1Outer, vSolid, vSolid);
			AddLine(tempCenter, tempCenter + v2Outer, vSolid, vSolid);
		}
	}

	void Gizmos::AddArcRing(const vec3& center, float rotation, float innerRadius, float outerRadius, 
		float arcHalfAngle, unsigned int segments, const vec4& fillColour, const mat4* transform)
	{
		vec4 vSolid = fillColour;
		vSolid.w = 1;

		vec3 tempCenter = transform != nullptr ? vec3((*transform)[3]) + center : center;

		float fSegmentSize = 2.f * arcHalfAngle / static_cast<float>(segments);

		for (unsigned int i = 0; i < segments; ++i) 
		{
			const float iF = static_cast<float>(i);

			vec3 v1Outer(sinf(iF * fSegmentSize - arcHalfAngle + rotation) * outerRadius, 0, cosf(iF * fSegmentSize - arcHalfAngle + rotation) * outerRadius);
			vec3 v2Outer(sinf((iF + 1.f) * fSegmentSize - arcHalfAngle + rotation) * outerRadius, 0, cosf((iF + 1.f) * fSegmentSize - arcHalfAngle + rotation) * outerRadius);
			vec3 v1Inner(sinf(iF * fSegmentSize - arcHalfAngle + rotation) * innerRadius, 0, cosf(iF * fSegmentSize - arcHalfAngle + rotation) * innerRadius);
			vec3 v2Inner(sinf((iF + 1.f) * fSegmentSize - arcHalfAngle + rotation) * innerRadius, 0, cosf((iF + 1.f) * fSegmentSize - arcHalfAngle + rotation) * innerRadius);

			if (transform != nullptr) 
			{
				v1Outer = vec3(*transform * vec4(v1Outer, 0));
				v2Outer = vec3(*transform * vec4(v2Outer, 0));
				v1Inner = vec3(*transform * vec4(v1Inner, 0));
				v2Inner = vec3(*transform * vec4(v2Inner, 0));
			}

			if (!COMPARE(fillColour.w, 0)) 
			{
				AddTri(tempCenter + v2Outer, tempCenter + v1Outer, tempCenter + v1Inner, fillColour);
				AddTri(tempCenter + v1Inner, tempCenter + v2Inner, tempCenter + v2Outer, fillColour);

				AddTri(tempCenter + v1Inner, tempCenter + v1Outer, tempCenter + v2Outer, fillColour);
				AddTri(tempCenter + v2Outer, tempCenter + v2Inner, tempCenter + v1Inner, fillColour);
			}
			else
			{
				// line
				AddLine(tempCenter + v1Inner, tempCenter + v2Inner, vSolid, vSolid);
				AddLine(tempCenter + v1Outer, tempCenter + v2Outer, vSolid, vSolid);
			}
		}

		// edge lines
		if (COMPARE(fillColour.w, 0))
		{
			vec3 v1Outer(sinf(-arcHalfAngle + rotation) * outerRadius, 0, cosf(-arcHalfAngle + rotation) * outerRadius);
			vec3 v2Outer(sinf(arcHalfAngle + rotation) * outerRadius, 0, cosf(arcHalfAngle + rotation) * outerRadius);
			vec3 v1Inner(sinf(-arcHalfAngle + rotation) * innerRadius, 0, cosf(-arcHalfAngle + rotation) * innerRadius);
			vec3 v2Inner(sinf(arcHalfAngle + rotation) * innerRadius, 0, cosf(arcHalfAngle + rotation) * innerRadius);

			if (transform != nullptr) 
			{
				v1Outer = vec3(*transform * vec4(v1Outer, 0));
				v2Outer = vec3(*transform * vec4(v2Outer, 0));
				v1Inner = vec3(*transform * vec4(v1Inner, 0));
				v2Inner = vec3(*transform * vec4(v2Inner, 0));
			}

			AddLine(tempCenter + v1Inner, tempCenter + v1Outer, vSolid, vSolid);
			AddLine(tempCenter + v2Inner, tempCenter + v2Outer, vSolid, vSolid);
		}
	}

	void Gizmos::AddSphere(const vec3& center, const float radius, const int rows, const int columns, 
		const vec4& fillColour, const mat4* transform, const float longMin, const float longMax, 
		const float latMin, const float latMax)
	{
		const float inverseRadius = 1 / radius;

		// invert these first as the multiply is slightly quicker
		const float invColumns = 1.0f / static_cast<float>(columns);
		const float invRows = 1.0f / static_cast<float>(rows);

		constexpr float deg2Rad = glm::pi<float>() / 180;

		const vec3 tempCenter = transform != nullptr ? vec3((*transform)[3]) + center : center;

		//Let's put everything in radians first
		const float latitudinalRange = (latMax - latMin) * deg2Rad;
		const float longitudinalRange = (longMax - longMin) * deg2Rad;

		// for each row of the mesh
		const auto v4Array = new vec3[rows * columns + columns];

		for (int row = 0; row <= rows; ++row)
		{
			// y ordinates this may be a little confusing but here we are navigating around the xAxis in GL
			const float ratioAroundXAxis = static_cast<float>(row) * invRows;
			const float radiansAboutXAxis = ratioAroundXAxis * latitudinalRange + latMin * deg2Rad;
			const float y = radius * sin(radiansAboutXAxis);
			const float z = radius * cos(radiansAboutXAxis);

			for (int col = 0; col <= columns; ++col) 
			{
				const float ratioAroundYAxis = static_cast<float>(col) * invColumns;
				const float theta = ratioAroundYAxis * longitudinalRange + longMin * deg2Rad;
				vec3 v4Point(-z * sinf(theta), y, -z * cosf(theta));
				vec3 v4Normal(inverseRadius * v4Point.x, inverseRadius * v4Point.y, inverseRadius * v4Point.z);

				if (transform != nullptr) 
				{
					v4Point = vec3(*transform * vec4(v4Point, 0));
					v4Normal = vec3(*transform * vec4(v4Normal, 0));
				}

				const int index = row * columns + (col % columns);
				v4Array[index] = v4Point;
			}
		}

		for (int face = 0; face < rows * columns; ++face) 
		{
			int iNextFace = face + 1;

			if (iNextFace % columns == 0)
				iNextFace = iNextFace - (columns);

			AddLine(tempCenter + v4Array[face], tempCenter + v4Array[face + columns], vec4(1.f, 1.f, 1.f, 1.f), vec4(1.f, 1.f, 1.f, 1.f));

			if (face % columns == 0 && longitudinalRange < (glm::pi<float>() * 2))
				continue;
			AddLine(tempCenter + v4Array[iNextFace + columns], tempCenter + v4Array[face + columns], vec4(1.f, 1.f, 1.f, 1.f), vec4(1.f, 1.f, 1.f, 1.f));

			AddTri(tempCenter + v4Array[iNextFace + columns], tempCenter + v4Array[face], tempCenter + v4Array[iNextFace], fillColour);
			AddTri(tempCenter + v4Array[iNextFace + columns], tempCenter + v4Array[face + columns], tempCenter + v4Array[face], fillColour);
		}

		delete[] v4Array;
	}

	void Gizmos::AddCapsule(const vec3& center, float height, float radius, int rows, int cols, 
		const vec4& fillColour, const mat4* rotation)
	{
		float sphereCenters = (height * 0.5f) - radius;
		vec4 top = vec4(0, sphereCenters, 0, 0);
		vec4 bottom = vec4(0, -sphereCenters, 0, 0);
		vec4 white(1);

		if (rotation) {
			top = *rotation * top + (*rotation)[3];
			bottom = *rotation * bottom + (*rotation)[3];
		}

		vec3 topCenter = center + vec3(top);
		vec3 bottomCenter = center + vec3(bottom);

		float inverseRadius = 1 / radius;

		// invert these first as the multiply is slightly quicker
		float invColumns = 1.0f / static_cast<float>(cols);
		float invRows = 1.0f / static_cast<float>(rows);

		float deg2Rad = glm::pi<float>() / 180;
		float latMin = -90, latMax = 90, longMin = 0.f, longMax = 360;

		// Let's put everything in radians first
		float latitudinalRange = (latMax - latMin) * deg2Rad;
		float longitudinalRange = (longMax - longMin) * deg2Rad;

		// for each row of the mesh
		vec3* v4Array = new vec3[rows * cols + cols];

		for (int row = 0; row <= (rows); ++row) 
		{
			// y ordinates this may be a little confusing but here we are navigating around the xAxis in GL
			float ratioAroundXAxis = static_cast<float>(row) * invRows;
			float radiansAboutXAxis = ratioAroundXAxis * latitudinalRange + (latMin * deg2Rad);
			float y = radius * sin(radiansAboutXAxis);
			float z = radius * cos(radiansAboutXAxis);

			for (int col = 0; col <= cols; ++col) 
			{
				float ratioAroundYAxis = static_cast<float>(col) * invColumns;
				float theta = ratioAroundYAxis * longitudinalRange + longMin * deg2Rad;
				vec3 v4Point(-z * sinf(theta), y, -z * cosf(theta));
				vec3 v4Normal(inverseRadius * v4Point.x, inverseRadius * v4Point.y, inverseRadius * v4Point.z);

				if (rotation != nullptr) 
				{
					v4Point = vec3(*rotation * vec4(v4Point, 0));
					v4Normal = vec3(*rotation * vec4(v4Normal, 0));
				}

				int index = row * cols + (col % cols);
				v4Array[index] = v4Point;
			}
		}

		// create a sphere that is split in two
		for (int face = 0; face < rows * cols; ++face) 
		{
			int iNextFace = face + 1;

			if (iNextFace % cols == 0)
				iNextFace = iNextFace - (cols);

			vec3 tempCenter = topCenter;
			if (face < (rows / 2) * cols)
				tempCenter = bottomCenter;

			AddLine(tempCenter + v4Array[face], tempCenter + v4Array[face + cols], white, white);

			if (face % cols == 0 && longitudinalRange < (glm::pi<float>() * 2))
				continue;

			AddLine(tempCenter + v4Array[iNextFace + cols], tempCenter + v4Array[face + cols], white, white);
			AddTri(tempCenter + v4Array[iNextFace + cols], tempCenter + v4Array[face], tempCenter + v4Array[iNextFace], fillColour);
			AddTri(tempCenter + v4Array[iNextFace + cols], tempCenter + v4Array[face + cols], tempCenter + v4Array[face], fillColour);
		}

		delete[] v4Array;

		for (int i = 0; i < cols; ++i) 
		{
			float x = static_cast<float>(i) / static_cast<float>(cols);
			float x1 = static_cast<float>(i + 1) / static_cast<float>(cols);
			x *= 2.0f * glm::pi<float>();
			x1 *= 2.0f * glm::pi<float>();

			vec3 pos = vec3(cosf(x), 0, sinf(x)) * radius;
			vec3 pos1 = vec3(cosf(x1), 0, sinf(x1)) * radius;

			if (rotation) {
				pos = vec3(*rotation * vec4(pos, 0));
				pos1 = vec3(*rotation * vec4(pos1, 0));
			}

			AddTri(topCenter + pos1, bottomCenter + pos1, bottomCenter + pos, fillColour);
			AddTri(topCenter + pos1, bottomCenter + pos, topCenter + pos, fillColour);

			AddLine(topCenter + pos, topCenter + pos1, white, white);
			AddLine(bottomCenter + pos, bottomCenter + pos1, white, white);
			AddLine(topCenter + pos, bottomCenter + pos, white, white);
		}
	}

	void Gizmos::AddHermiteSpline(const vec3& start, const vec3& end, const vec3& tangentStart, 
		const vec3& tangentEnd, unsigned int segments, const vec4& colour)
	{
		segments = segments > 1 ? segments : 1;

		vec3 prev = start;

		for (unsigned int i = 1; i <= segments; ++i) 
		{
			const float s = static_cast<float>(i) / static_cast<float>(segments);

			const float s2 = s * s;
			const float s3 = s2 * s;
			const float h1 = (2.0f * s3) - (3.0f * s2) + 1.0f;
			const float h2 = (-2.0f * s3) + (3.0f * s2);
			const float h3 = s3 - (2.0f * s2) + s;
			const float h4 = s3 - s2;
			vec3 p = (start * h1) + (end * h2) + (tangentStart * h3) + (tangentEnd * h4);

			AddLine(prev, p, colour, colour);
			prev = p;
		}
	}

	void Gizmos::AddLine(const vec3& v0, const vec3& v1, const vec4& colour)
	{
		AddLine(v0, v1, colour, colour);
	}

	void Gizmos::AddLine(const vec3& v0, const vec3& v1, const vec4& colour0, const vec4& colour1)
	{
		if (m_singleton != nullptr && m_singleton->m_lineCount < m_singleton->m_maxLines) 
		{
			m_singleton->m_lines[m_singleton->m_lineCount].v0.x = v0.x;
			m_singleton->m_lines[m_singleton->m_lineCount].v0.y = v0.y;
			m_singleton->m_lines[m_singleton->m_lineCount].v0.z = v0.z;
			m_singleton->m_lines[m_singleton->m_lineCount].v0.w = 1;
			m_singleton->m_lines[m_singleton->m_lineCount].v0.r = colour0.r;
			m_singleton->m_lines[m_singleton->m_lineCount].v0.g = colour0.g;
			m_singleton->m_lines[m_singleton->m_lineCount].v0.b = colour0.b;
			m_singleton->m_lines[m_singleton->m_lineCount].v0.a = colour0.a;

			m_singleton->m_lines[m_singleton->m_lineCount].v1.x = v1.x;
			m_singleton->m_lines[m_singleton->m_lineCount].v1.y = v1.y;
			m_singleton->m_lines[m_singleton->m_lineCount].v1.z = v1.z;
			m_singleton->m_lines[m_singleton->m_lineCount].v1.w = 1;
			m_singleton->m_lines[m_singleton->m_lineCount].v1.r = colour1.r;
			m_singleton->m_lines[m_singleton->m_lineCount].v1.g = colour1.g;
			m_singleton->m_lines[m_singleton->m_lineCount].v1.b = colour1.b;
			m_singleton->m_lines[m_singleton->m_lineCount].v1.a = colour1.a;

			m_singleton->m_lineCount++;
		}
	}

	void Gizmos::AddTri(const vec3& v0, const vec3& v1, const vec3& v2, const vec4& colour) {
		if (m_singleton != nullptr) 
		{
			if (COMPARE(colour.w, 1.f)) 
			{
				if (m_singleton->m_triCount < m_singleton->m_maxTris) 
				{
					m_singleton->m_tris[m_singleton->m_triCount].v0.x = v0.x;
					m_singleton->m_tris[m_singleton->m_triCount].v0.y = v0.y;
					m_singleton->m_tris[m_singleton->m_triCount].v0.z = v0.z;
					m_singleton->m_tris[m_singleton->m_triCount].v0.w = 1;
					m_singleton->m_tris[m_singleton->m_triCount].v1.x = v1.x;
					m_singleton->m_tris[m_singleton->m_triCount].v1.y = v1.y;
					m_singleton->m_tris[m_singleton->m_triCount].v1.z = v1.z;
					m_singleton->m_tris[m_singleton->m_triCount].v1.w = 1;
					m_singleton->m_tris[m_singleton->m_triCount].v2.x = v2.x;
					m_singleton->m_tris[m_singleton->m_triCount].v2.y = v2.y;
					m_singleton->m_tris[m_singleton->m_triCount].v2.z = v2.z;
					m_singleton->m_tris[m_singleton->m_triCount].v2.w = 1;

					m_singleton->m_tris[m_singleton->m_triCount].v0.r = colour.r;
					m_singleton->m_tris[m_singleton->m_triCount].v0.g = colour.g;
					m_singleton->m_tris[m_singleton->m_triCount].v0.b = colour.b;
					m_singleton->m_tris[m_singleton->m_triCount].v0.a = colour.a;
					m_singleton->m_tris[m_singleton->m_triCount].v1.r = colour.r;
					m_singleton->m_tris[m_singleton->m_triCount].v1.g = colour.g;
					m_singleton->m_tris[m_singleton->m_triCount].v1.b = colour.b;
					m_singleton->m_tris[m_singleton->m_triCount].v1.a = colour.a;
					m_singleton->m_tris[m_singleton->m_triCount].v2.r = colour.r;
					m_singleton->m_tris[m_singleton->m_triCount].v2.g = colour.g;
					m_singleton->m_tris[m_singleton->m_triCount].v2.b = colour.b;
					m_singleton->m_tris[m_singleton->m_triCount].v2.a = colour.a;

					m_singleton->m_triCount++;
				}
			}
			else 
			{
				if (m_singleton->m_transparentTriCount < m_singleton->m_maxTris) 
				{
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v0.x = v0.x;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v0.y = v0.y;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v0.z = v0.z;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v0.w = 1;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v1.x = v1.x;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v1.y = v1.y;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v1.z = v1.z;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v1.w = 1;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v2.x = v2.x;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v2.y = v2.y;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v2.z = v2.z;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v2.w = 1;

					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v0.r = colour.r;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v0.g = colour.g;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v0.b = colour.b;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v0.a = colour.a;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v1.r = colour.r;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v1.g = colour.g;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v1.b = colour.b;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v1.a = colour.a;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v2.r = colour.r;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v2.g = colour.g;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v2.b = colour.b;
					m_singleton->m_transparentTris[m_singleton->m_transparentTriCount].v2.a = colour.a;

					m_singleton->m_transparentTriCount++;
				}
			}
		}
	}

	void Gizmos::Add2DAABB(const vec2& center, const vec2& extents, const vec4& colour, const mat4* transform)
	{
		vec2 verts[4];
		vec2 vX(extents.x, 0);
		vec2 vY(0, extents.y);

		if (transform != nullptr) 
		{
			vX = vec2(*transform * vec4(vX, 0, 0));
			vY = vec2(*transform * vec4(vY, 0, 0));
		}

		verts[0] = center - vX - vY;
		verts[1] = center + vX - vY;
		verts[2] = center - vX + vY;
		verts[3] = center + vX + vY;

		Add2DLine(verts[0], verts[1], colour, colour);
		Add2DLine(verts[1], verts[3], colour, colour);
		Add2DLine(verts[2], verts[3], colour, colour);
		Add2DLine(verts[2], verts[0], colour, colour);
	}

	void Gizmos::Add2DAABBFilled(const vec2& center, const vec2& extents, const vec4& colour, const mat4* transform)
	{
		vec2 verts[4];
		vec2 vX(extents.x, 0);
		vec2 vY(0, extents.y);

		if (transform != nullptr) 
		{
			vX = vec2(*transform * vec4(vX, 0, 0));
			vY = vec2(*transform * vec4(vY, 0, 0));
		}

		verts[0] = center - vX - vY;
		verts[1] = center + vX - vY;
		verts[2] = center + vX + vY;
		verts[3] = center - vX + vY;

		Add2DTri(verts[0], verts[1], verts[2], colour);
		Add2DTri(verts[0], verts[2], verts[3], colour);
	}

	void Gizmos::Add2DCircle(const vec2& center, const float radius, const unsigned int segments, 
		const vec4& colour, const mat4* transform)
	{
		vec4 solidColour = colour;
		solidColour.w = 1;

		const float segmentSize = 2 * glm::pi<float>() / static_cast<float>(segments);

		for (unsigned int i = 0; i < segments; ++i) 
		{
			const float iF = static_cast<float>(i);

			vec2 v1outer(sinf(iF * segmentSize) * radius, cosf(iF * segmentSize) * radius);
			vec2 v2outer(sinf((iF + 1.f) * segmentSize) * radius, cosf((iF + 1.f) * segmentSize) * radius);

			if (transform != nullptr)
			{
				v1outer = vec2(*transform * vec4(v1outer, 0, 0));
				v2outer = vec2(*transform * vec4(v2outer, 0, 0));
			}

			if (!COMPARE(colour.w, 0)) 
			{
				Add2DTri(center, center + v1outer, center + v2outer, colour);
				Add2DTri(center + v2outer, center + v1outer, center, colour);
			}
			else 
			{
				// line
				Add2DLine(center + v1outer, center + v2outer, solidColour, solidColour);
			}
		}
	}

	void Gizmos::Add2DLine(const vec2& rv0, const vec2& rv1, const vec4& colour)
	{
		Add2DLine(rv0, rv1, colour, colour);
	}

	void Gizmos::Add2DLine(const vec2& start, const vec2& end, const vec4& colour0, const vec4& colour1)
	{
		if (m_singleton != nullptr && m_singleton->m_2DlineCount < m_singleton->m_max2DLines) 
		{
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v0.x = start.x;
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v0.y = start.y;
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v0.z = 1;
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v0.w = 1;
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v0.r = colour0.r;
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v0.g = colour0.g;
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v0.b = colour0.b;
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v0.a = colour0.a;
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v1.x = end.x;
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v1.y = end.y;
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v1.z = 1;
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v1.w = 1;
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v1.r = colour1.r;
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v1.g = colour1.g;
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v1.b = colour1.b;
			m_singleton->m_2Dlines[m_singleton->m_2DlineCount].v1.a = colour1.a;

			m_singleton->m_2DlineCount++;
		}
	}

	void Gizmos::Add2DTri(const vec2& v0, const vec2& v1, const vec2& v2, const vec4& colour)
	{
		Add2DTri(v0, v1, v2, colour, colour, colour);
	}

	void Gizmos::Add2DTri(const vec2& v0, const vec2& v1, const vec2& v2, const vec4& colour0, 
		const vec4& colour1, const vec4& colour2)
	{
		if (m_singleton != nullptr) 
		{
			if (m_singleton->m_2DtriCount < m_singleton->m_max2DTris) 
			{
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v0.x = v0.x;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v0.y = v0.y;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v0.z = 1;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v0.w = 1;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v1.x = v1.x;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v1.y = v1.y;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v1.z = 1;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v1.w = 1;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v2.x = v2.x;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v2.y = v2.y;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v2.z = 1;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v2.w = 1;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v0.r = colour0.r;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v0.g = colour0.g;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v0.b = colour0.b;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v0.a = colour0.a;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v1.r = colour1.r;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v1.g = colour1.g;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v1.b = colour1.b;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v1.a = colour1.a;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v2.r = colour2.r;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v2.g = colour2.g;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v2.b = colour2.b;
				m_singleton->m_2Dtris[m_singleton->m_2DtriCount].v2.a = colour2.a;

				m_singleton->m_2DtriCount++;
			}
		}
	}

	void Gizmos::Draw(const mat4& projection, const mat4& view)
	{
		Draw(projection * view);
	}

	void Gizmos::Draw(const mat4& projectionView)
	{
		if (m_singleton != nullptr && (m_singleton->m_lineCount > 0 || 
			m_singleton->m_triCount > 0 || m_singleton->m_transparentTriCount > 0)) 
		{
			int shader = 0;
			glGetIntegerv(GL_CURRENT_PROGRAM, &shader);

			glUseProgram(m_singleton->m_shader);

			const int projectionViewUniform = glGetUniformLocation(m_singleton->m_shader, "ProjectionView");
			glUniformMatrix4fv(projectionViewUniform, 1, false, value_ptr(projectionView));

			if (m_singleton->m_lineCount > 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_singleton->m_lineVBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, m_singleton->m_lineCount * sizeof(GizmoLine), m_singleton->m_lines);

				glBindVertexArray(m_singleton->m_lineVAO);
				glDrawArrays(GL_LINES, 0, m_singleton->m_lineCount * 2);
			}

			if (m_singleton->m_triCount > 0) 
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_singleton->m_triVBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, m_singleton->m_triCount * sizeof(GizmoTri), m_singleton->m_tris);

				glBindVertexArray(m_singleton->m_triVAO);
				glDrawArrays(GL_TRIANGLES, 0, m_singleton->m_triCount * 3);
			}

			if (m_singleton->m_transparentTriCount > 0)
			{
				// not ideal to store these, but Gizmos must work stand-alone
				GLboolean blendEnabled = glIsEnabled(GL_BLEND);
				GLboolean depthMask = GL_TRUE;
				glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
				int src, dst;
				glGetIntegerv(GL_BLEND_SRC, &src);
				glGetIntegerv(GL_BLEND_DST, &dst);

				// setup blend states
				if (blendEnabled == GL_FALSE)
					glEnable(GL_BLEND);

				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDepthMask(GL_FALSE);

				glBindBuffer(GL_ARRAY_BUFFER, m_singleton->m_transparentTriVBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, m_singleton->m_transparentTriCount * sizeof(GizmoTri), m_singleton->m_transparentTris);

				glBindVertexArray(m_singleton->m_transparentTriVAO);
				glDrawArrays(GL_TRIANGLES, 0, m_singleton->m_transparentTriCount * 3);

				// reset state
				glDepthMask(depthMask);
				glBlendFunc(src, dst);
				if (blendEnabled == GL_FALSE)
					glDisable(GL_BLEND);
			}

			glUseProgram(shader);
		}
	}

	void Gizmos::Draw2D(const float screenWidth, const float screenHeight)
	{
		Draw2D(glm::ortho(0.f, screenWidth, 0.f, screenHeight));
	}

	void Gizmos::Draw2D(const mat4& projection)
	{
		if (m_singleton != nullptr && (m_singleton->m_2DlineCount > 0 || m_singleton->m_2DtriCount > 0)) 
		{
			int shader = 0;
			glGetIntegerv(GL_CURRENT_PROGRAM, &shader);

			glUseProgram(m_singleton->m_shader);

			const int projectionViewUniform = glGetUniformLocation(m_singleton->m_shader, "ProjectionView");
			glUniformMatrix4fv(projectionViewUniform, 1, false, value_ptr(projection));

			if (m_singleton->m_2DlineCount > 0) 
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_singleton->m_2DlineVBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, m_singleton->m_2DlineCount * sizeof(GizmoLine), m_singleton->m_2Dlines);

				glBindVertexArray(m_singleton->m_2DlineVAO);
				glDrawArrays(GL_LINES, 0, m_singleton->m_2DlineCount * 2);
			}

			if (m_singleton->m_2DtriCount > 0) 
			{
				GLboolean blendEnabled = glIsEnabled(GL_BLEND);

				GLboolean depthMask = GL_TRUE;
				glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);

				int src, dst;
				glGetIntegerv(GL_BLEND_SRC, &src);
				glGetIntegerv(GL_BLEND_DST, &dst);

				if (blendEnabled == GL_FALSE)
					glEnable(GL_BLEND);

				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				glDepthMask(GL_FALSE);

				glBindBuffer(GL_ARRAY_BUFFER, m_singleton->m_2DtriVBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, m_singleton->m_2DtriCount * sizeof(GizmoTri), m_singleton->m_2Dtris);

				glBindVertexArray(m_singleton->m_2DtriVAO);
				glDrawArrays(GL_TRIANGLES, 0, m_singleton->m_2DtriCount * 3);

				glDepthMask(depthMask);

				glBlendFunc(src, dst);

				if (blendEnabled == GL_FALSE)
					glDisable(GL_BLEND);
			}

			glUseProgram(shader);
		}
	}

} // namespace aie