#pragma once

#include <glm/fwd.hpp>

namespace aie {

class Gizmos {
public:

	static void		create(unsigned int a_maxLines, unsigned int a_maxTris,
						   unsigned int a_max2DLines, unsigned int a_max2DTris);
	static void		destroy();

	// removes all Gizmos
	static void		clear();

	// draws current Gizmo buffers, either using a combined (projection * view) matrix, or separate matrices
	static void		draw(const glm::mat4& a_projectionView);
	static void		draw(const glm::mat4& a_projection, const glm::mat4& a_view);
	
	// the projection matrix here should ideally be orthographic with a near of -1 and far of 1
	static void		draw2D(const glm::mat4& a_projection);

	// Adds a single debug line
	static void		addLine(const glm::vec3& a_rv0,  const glm::vec3& a_rv1, 
							const glm::vec4& a_colour);

	// Adds a single debug line
	static void		addLine(const glm::vec3& a_rv0, const glm::vec3& a_rv1, 
							const glm::vec4& a_colour0, const glm::vec4& a_colour1);

	// Adds a triangle.
	static void		addTri(const glm::vec3& a_rv0, const glm::vec3& a_rv1, const glm::vec3& a_rv2, const glm::vec4& a_colour);

	// Adds 3 unit-length lines (red,green,blue) representing the 3 axis of a transform, 
	// at the transform's translation. Optional scale available.
	static void		addTransform(const glm::mat4& a_transform, float a_fScale = 1.0f);
	
	// Adds a wireframe Axis-Aligned Bounding-Box with optional transform for rotation/translation.
	static void		addAABB(const glm::vec3& a_center, const glm::vec3& a_extents, 
							const glm::vec4& a_colour, const glm::mat4* a_transform = nullptr);

	// Adds an Axis-Aligned Bounding-Box with optional transform for rotation.
	static void		addAABBFilled(const glm::vec3& a_center, const glm::vec3& a_extents, 
								  const glm::vec4& a_fillColour, const glm::mat4* a_transform = nullptr);

	// Adds a cylinder aligned to the Y-axis with optional transform for rotation.
	static void		addCylinderFilled(const glm::vec3& a_center, float a_radius, float a_fHalfLength,
									  unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform = nullptr);

	// Adds a double-sided hollow ring in the XZ axis with optional transform for rotation.
	// If a_rvFilLColour.w == 0 then only an outer and inner line is drawn.
	static void		addRing(const glm::vec3& a_center, float a_innerRadius, float a_outerRadius,
							unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform = nullptr);

	// Adds a double-sided disk in the XZ axis with optional transform for rotation.
	// If a_rvFilLColour.w == 0 then only an outer line is drawn.
	static void		addDisk(const glm::vec3& a_center, float a_radius,
							unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform = nullptr);

	// Adds an arc, around the Y-axis
	// If a_rvFilLColour.w == 0 then only an outer line is drawn.
	static void		addArc(const glm::vec3& a_center, float a_rotation,
						   float a_radius, float a_halfAngle,
						   unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform = nullptr);

	// Adds an arc, around the Y-axis, starting at the inner radius and extending to the outer radius
	// If a_rvFilLColour.w == 0 then only an outer line is drawn.
	static void		addArcRing(const glm::vec3& a_center, float a_rotation, 
							   float a_innerRadius, float a_outerRadius, float a_arcHalfAngle,
							   unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform = nullptr);

	// Adds a Sphere at a given position, with a given number of rows, and columns, radius and a max and min long and latitude
	static void		addSphere(const glm::vec3& a_center, float a_radius, int a_rows, int a_columns, const glm::vec4& a_fillColour, 
							  const glm::mat4* a_transform = nullptr, float a_longMin = 0.f, float a_longMax = 360, 
							  float a_latMin = -90, float a_latMax = 90 );

	// Adds a capsule with a set height and radius
	static void		addCapsule(const glm::vec3& a_center, float a_height, float a_radius,
							   int a_rows, int a_cols, const glm::vec4& a_fillColour, const glm::mat4* a_rotation = nullptr);

	// Adds a single Hermite spline curve
	static void		addHermiteSpline(const glm::vec3& a_start, const glm::vec3& a_end,
									 const glm::vec3& a_tangentStart, const glm::vec3& a_tangentEnd, unsigned int a_segments, const glm::vec4& a_colour);

	// 2-dimensional gizmos
	static void		add2DLine(const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec4& a_colour);
	static void		add2DLine(const glm::vec2& a_start, const glm::vec2& a_end, const glm::vec4& a_colour0, const glm::vec4& a_colour1);	
	static void		add2DTri(const glm::vec2& a_0, const glm::vec2& a_1, const glm::vec2& a_2, const glm::vec4& a_colour);	
	static void		add2DAABB(const glm::vec2& a_center, const glm::vec2& a_extents, const glm::vec4& a_colour, const glm::mat4* a_transform = nullptr);	
	static void		add2DAABBFilled(const glm::vec2& a_center, const glm::vec2& a_extents, const glm::vec4& a_colour, const glm::mat4* a_transform = nullptr);	
	static void		add2DCircle(const glm::vec2& a_center, float a_radius, unsigned int a_segments, const glm::vec4& a_colour, const glm::mat4* a_transform = nullptr);
	
private:

	Gizmos(unsigned int a_maxLines, unsigned int a_maxTris,
		   unsigned int a_max2DLines, unsigned int a_max2DTris);
	~Gizmos();

	struct GizmoVertex {
		float x, y, z, w;
		float r, g, b, a;
	};

	struct GizmoLine {
		GizmoVertex v0;
		GizmoVertex v1;
	};

	struct GizmoTri {
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