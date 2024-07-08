#pragma once

#include "Bootstrap.h"
#include <glm/fwd.hpp>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

namespace aie
{
	// a singleton class for rendering immediate-mode 3-D primitives
	class DLL Gizmos
	{
	public:
		static void Create(unsigned int maxLines, unsigned int maxTris, unsigned int max2DLines, unsigned int max2DTris);
		static void	Destroy();

		// removes all Gizmos
		static void	Clear();

		// draws current Gizmo buffers, either using a combined (projection * view) matrix, or separate matrices
		static void	Draw(const mat4& projectionView);
		static void	Draw(const mat4& projection, const mat4& view);

		// the projection matrix here should ideally be orthographic with a near of -1 and far of 1
		static void	Draw2D(const mat4& projection);
		static void	Draw2D(float screenWidth, float screenHeight);

		// adds a single debug line
		static void	AddLine(const vec3& v0, const vec3& v1, const vec4& colour);

		// adds a single debug line
		static void	AddLine(const vec3& v0, const vec3& v1, const vec4& colour0, const vec4& colour1);

		// adds a triangle
		static void	AddTri(const vec3& v0, const vec3& v1, const vec3& v2, const vec4& colour);

		// adds 3 unit-length lines (red,green,blue) representing the 3 axis of a transform, 
		// at the transform's translation. Optional scale available
		static void	AddTransform(const mat4& transform, float scale = 1.0f);

		// adds a wireframe Axis-Aligned Bounding-Box with optional transform for rotation/translation
		static void	AddAABB(const vec3& center, const vec3& extents, const vec4& colour, const mat4* transform = nullptr);

		// adds an Axis-Aligned Bounding-Box with optional transform for rotation
		static void	AddAABBFilled(const vec3& center, const vec3& extents, const vec4& fillColour, const mat4* transform = nullptr);

		// adds a cylinder aligned to the Y-axis with optional transform for rotation
		static void	AddCylinderFilled(const vec3& center, float radius, float halfLength, unsigned int segments, const vec4& fillColour, const mat4* transform = nullptr);

		// adds a double-sided hollow ring in the XZ axis with optional transform for rotation.
		// if fillColour.w == 0 then only an outer and inner line is drawn
		static void	AddRing(const vec3& center, float innerRadius, float outerRadius, unsigned int segments, const vec4& fillColour, const mat4* transform = nullptr);

		// adds a double-sided disk in the XZ axis with optional transform for rotation.
		// if fillColour.w == 0 then only an outer line is drawn
		static void	AddDisk(const vec3& center, float radius, unsigned int segments, const vec4& fillColour, const mat4* transform = nullptr);

		// adds an arc, around the Y-axis
		// if fillColour.w == 0 then only an outer line is drawn
		static void	AddArc(const vec3& center, float rotation, float radius, float arcHalfAngle, unsigned int segments, const vec4& fillColour, const mat4* transform = nullptr);

		// adds an arc, around the Y-axis, starting at the inner radius and extending to the outer radius
		// if fillColour.w == 0 then only an outer line is drawn
		static void	AddArcRing(const vec3& center, float rotation, float innerRadius, float outerRadius, float arcHalfAngle, unsigned int segments, const vec4& fillColour, const mat4* transform = nullptr);

		// adds a Sphere at a given position, with a given number of rows, and columns, radius and a max and min long and latitude
		static void	AddSphere(const vec3& center, float radius, int rows, int columns, const vec4& fillColour, const mat4* transform = nullptr, float longMin = 0.f, float longMax = 360, float latMin = -90, float latMax = 90);

		// adds a capsule with a set height and radius
		static void	AddCapsule(const vec3& center, float height, float radius, int rows, int cols, const vec4& fillColour, const mat4* rotation = nullptr);

		// adds a single Hermite spline curve
		static void	AddHermiteSpline(const vec3& start, const vec3& end, const vec3& tangentStart, const vec3& tangentEnd, unsigned int segments, const vec4& colour);

		// 2-dimensional gizmos
		static void	Add2DLine(const vec2& start, const vec2& end, const vec4& colour);
		static void	Add2DLine(const vec2& start, const vec2& end, const vec4& colour0, const vec4& colour1);
		static void	Add2DTri(const vec2& v0, const vec2& v1, const vec2& v2, const vec4& colour);
		static void	Add2DTri(const vec2& v0, const vec2& v1, const vec2& v2, const vec4& colour0, const vec4& colour1, const vec4& colour2);
		static void	Add2DAABB(const vec2& center, const vec2& extents, const vec4& colour, const mat4* transform = nullptr);
		static void	Add2DAABBFilled(const vec2& center, const vec2& extents, const vec4& colour, const mat4* transform = nullptr);
		static void	Add2DCircle(const vec2& center, float radius, unsigned int segments, const vec4& colour, const mat4* transform = nullptr);

	private:
		struct GizmoVertex
		{
			float x, y, z, w;
			float r, g, b, a;
		};

		struct GizmoLine
		{
			GizmoVertex v0;
			GizmoVertex v1;
		};

		struct GizmoTri {
			GizmoVertex v0;
			GizmoVertex v1;
			GizmoVertex v2;
		};

	private:
		static Gizmos* m_singleton;

	private:
		unsigned int m_shader;

		// line data
		size_t m_maxLines;
		size_t m_lineCount;
		GizmoLine* m_lines;

		unsigned int m_lineVAO;
		unsigned int m_lineVBO;

		// triangle data
		unsigned int m_maxTris;
		unsigned int m_triCount;
		GizmoTri* m_tris;

		unsigned int m_triVAO;
		unsigned int m_triVBO;

		unsigned int m_transparentTriCount;
		GizmoTri* m_transparentTris;

		unsigned int m_transparentTriVAO;
		unsigned int m_transparentTriVBO;

		// 2D line data
		unsigned int m_max2DLines;
		unsigned int m_2DlineCount;
		GizmoLine* m_2Dlines;

		unsigned int m_2DlineVAO;
		unsigned int m_2DlineVBO;

		// 2D triangle data
		unsigned int m_max2DTris;
		unsigned int m_2DtriCount;
		GizmoTri* m_2Dtris;

		unsigned int m_2DtriVAO;
		unsigned int m_2DtriVBO;

	private:
		Gizmos(unsigned int maxLines, unsigned int maxTris, unsigned int max2DLines, unsigned int max2DTris);
		~Gizmos();

	};

} // namespace aie