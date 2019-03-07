//----------------------------------------------------------------------------
// A singleton class for rendering immediate-mode 3-D primitives.
//----------------------------------------------------------------------------
#pragma once

#include <glm/fwd.hpp>

namespace aie 
{

class Gizmos 
{
public:
	static void Create(unsigned int maxLines, unsigned int maxTris, unsigned int max2DLines, unsigned int max2DTris);
	static void Destroy();

	// Removes all Gizmos
	static void	Clear();

	// Draws current Gizmo buffers, either using a combined (projection * view) matrix, or separate matrices
	static void	Draw(const glm::mat4& projectionView);
	static void	Draw(const glm::mat4& projection, const glm::mat4& view);
	
	// The projection matrix here should ideally be orthographic with a near of -1 and far of 1
	static void	Draw2D(const glm::mat4& projection);
	static void	Draw2D(float screenWidth, float screenHeight);

	// Adds a single debug line
	static void	AddLine(const glm::vec3& v0, const glm::vec3& v1, const glm::vec4& colour);
	static void	AddLine(const glm::vec3& v0, const glm::vec3& v1, const glm::vec4& colour0, const glm::vec4& colour1);

	// Adds a triangle
	static void	AddTri(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& colour);

	// Adds 3 unit-length lines (red,green,blue) representing the 3 axis of a transform, 
	// at the transform's translation. Optional scale available.
	static void	AddTransform(const glm::mat4& transform, float scale = 1.0f);
	
	// Adds a wireframe Axis-Aligned Bounding-Box with optional transform for rotation/translation.
	static void	AddAABB(const glm::vec3& center, const glm::vec3& extents, 
							const glm::vec4& colour, const glm::mat4* transform = nullptr);

	// Adds an Axis-Aligned Bounding-Box with optional transform for rotation.
	static void	AddAABBFilled(const glm::vec3& center, const glm::vec3& extents, 
								  const glm::vec4& fillColour, const glm::mat4* transform = nullptr);

	// Adds a cylinder aligned to the Y-axis with optional transform for rotation.
	static void	AddCylinderFilled(const glm::vec3& center, float radius, float halfLength,
									  unsigned int segments, const glm::vec4& fillColour, const glm::mat4* transform = nullptr);

	// Adds a double-sided hollow ring in the XZ axis with optional transform for rotation.
	// If fillColour.w == 0 then only an outer and inner line is drawn.
	static void	AddRing(const glm::vec3& center, float innerRadius, float outerRadius,
							unsigned int segments, const glm::vec4& fillColour, const glm::mat4* transform = nullptr);

	// Adds a double-sided disk in the XZ axis with optional transform for rotation.
	// If fillColour.w == 0 then only an outer line is drawn.
	static void	AddDisk(const glm::vec3& center, float radius,
							unsigned int segments, const glm::vec4& fillColour, const glm::mat4* transform = nullptr);

	// Adds an arc, around the Y-axis.
	// If fillColour.w == 0 then only an outer line is drawn.
	static void	AddArc(const glm::vec3& center, float rotation, float radius, float halfAngle,
						   unsigned int segments, const glm::vec4& fillColour, const glm::mat4* transform = nullptr);

	// Adds an arc, around the Y-axis, starting at the inner radius and extending to the outer radius.
	// If fillColour.w == 0 then only an outer line is drawn.
	static void	AddArcRing(const glm::vec3& center, float rotation, 
							   float innerRadius, float outerRadius, float arcHalfAngle,
							   unsigned int segments, const glm::vec4& fillColour, const glm::mat4* transform = nullptr);

	// Adds a Sphere at a given position, with a given number of rows, and columns, radius and a max and min long and latitude.
	static void	AddSphere(const glm::vec3& center, float radius, int rows, int columns, const glm::vec4& fillColour, 
							  const glm::mat4* transform = nullptr, float longMin = 0.f, float longMax = 360, 
							  float latMin = -90, float latMax = 90 );

	// Adds a capsule with a set height and radius.
	static void	AddCapsule(const glm::vec3& center, float height, float radius,
							   int rows, int cols, const glm::vec4& fillColour, const glm::mat4* rotation = nullptr);

	// Adds a single Hermite spline curve.
	static void	AddHermiteSpline(const glm::vec3& start, const glm::vec3& end,
									 const glm::vec3& tangentStart, const glm::vec3& tangentEnd, unsigned int segments, const glm::vec4& colour);

	// 2-dimensional gizmos.
	static void	Add2DLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& colour);
	static void	Add2DLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& colour0, const glm::vec4& colour1);	
	static void	Add2DTri(const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2, const glm::vec4& colour);	
	static void	Add2DAABB(const glm::vec2& center, const glm::vec2& extents, const glm::vec4& colour, const glm::mat4* transform = nullptr);	
	static void	Add2DAABBFilled(const glm::vec2& center, const glm::vec2& extents, const glm::vec4& colour, const glm::mat4* transform = nullptr);	
	static void	Add2DCircle(const glm::vec2& center, float radius, unsigned int segments, const glm::vec4& colour, const glm::mat4* transform = nullptr);
	
private:
	Gizmos(unsigned int maxLines, unsigned int maxTris, unsigned int max2DLines, unsigned int max2DTris);
	~Gizmos();

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

	struct GizmoTri 
	{
		GizmoVertex v0;
		GizmoVertex v1;
		GizmoVertex v2;
	};

	unsigned int	m_shader;

	// line data
	unsigned int	m_maxLines;
	unsigned int	m_lineCount;
	GizmoLine*		m_lines;

	unsigned int	m_lineVAO;
	unsigned int 	m_lineVBO;

	// triangle data
	unsigned int	m_maxTris;
	unsigned int	m_triCount;
	GizmoTri*		m_tris;

	unsigned int	m_triVAO;
	unsigned int 	m_triVBO;
	
	unsigned int	m_transparentTriCount;
	GizmoTri*		m_transparentTris;

	unsigned int	m_transparentTriVAO;
	unsigned int 	m_transparentTriVBO;
	
	// 2D line data
	unsigned int	m_max2DLines;
	unsigned int	m_2DlineCount;
	GizmoLine*		m_2Dlines;

	unsigned int	m_2DlineVAO;
	unsigned int 	m_2DlineVBO;

	// 2D triangle data
	unsigned int	m_max2DTris;
	unsigned int	m_2DtriCount;
	GizmoTri*		m_2Dtris;

	unsigned int	m_2DtriVAO;
	unsigned int 	m_2DtriVBO;

	static Gizmos*	sm_singleton;
};

} // namespace aie