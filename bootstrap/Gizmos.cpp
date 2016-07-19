#include "Gizmos.h"
#include "gl_core_4_4.h"
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace aie {

Gizmos* Gizmos::sm_singleton = nullptr;

Gizmos::Gizmos(unsigned int a_maxLines, unsigned int a_maxTris,
			   unsigned int a_max2DLines, unsigned int a_max2DTris)
	: m_maxLines(a_maxLines),
	m_lineCount(0),
	m_lines(new GizmoLine[a_maxLines]),
	m_maxTris(a_maxTris),
	m_triCount(0),
	m_tris(new GizmoTri[a_maxTris]),
	m_transparentTriCount(0),
	m_transparentTris(new GizmoTri[a_maxTris]),
	m_max2DLines(a_max2DLines),
	m_2DlineCount(0),
	m_2Dlines(new GizmoLine[a_max2DLines]),
	m_max2DTris(a_max2DTris),
	m_2DtriCount(0),
	m_2Dtris(new GizmoTri[a_max2DTris]) {
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
    
    
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, (const char**)&vsSource, 0);
	glCompileShader(vs);

	glShaderSource(fs, 1, (const char**)&fsSource, 0);
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
        
		glGetProgramInfoLog(m_shader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link Gizmo shader program!\n%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
    
    // create VBOs
	glGenBuffers( 1, &m_lineVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxLines * sizeof(GizmoLine), m_lines, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_triVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_triVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxTris * sizeof(GizmoTri), m_tris, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_transparentTriVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_transparentTriVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxTris * sizeof(GizmoTri), m_transparentTris, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_2DlineVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_2DlineVBO);
	glBufferData(GL_ARRAY_BUFFER, m_max2DLines * sizeof(GizmoLine), m_2Dlines, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_2DtriVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_2DtriVBO);
	glBufferData(GL_ARRAY_BUFFER, m_max2DTris * sizeof(GizmoTri), m_2Dtris, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &m_lineVAO);
	glBindVertexArray(m_lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), (void*)16);

	glGenVertexArrays(1, &m_triVAO);
	glBindVertexArray(m_triVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_triVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), (void*)16);

	glGenVertexArrays(1, &m_transparentTriVAO);
	glBindVertexArray(m_transparentTriVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_transparentTriVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), (void*)16);

	glGenVertexArrays(1, &m_2DlineVAO);
	glBindVertexArray(m_2DlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_2DlineVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), (void*)16);

	glGenVertexArrays(1, &m_2DtriVAO);
	glBindVertexArray(m_2DtriVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_2DtriVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GizmoVertex), (void*)16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Gizmos::~Gizmos() {
	delete[] m_lines;
	delete[] m_tris;
	delete[] m_transparentTris;
	glDeleteBuffers( 1, &m_lineVBO );
	glDeleteBuffers( 1, &m_triVBO );
	glDeleteBuffers( 1, &m_transparentTriVBO );
	glDeleteVertexArrays( 1, &m_lineVAO );
	glDeleteVertexArrays( 1, &m_triVAO );
	glDeleteVertexArrays( 1, &m_transparentTriVAO );
	delete[] m_2Dlines;
	delete[] m_2Dtris;
	glDeleteBuffers( 1, &m_2DlineVBO );
	glDeleteBuffers( 1, &m_2DtriVBO );
	glDeleteVertexArrays( 1, &m_2DlineVAO );
	glDeleteVertexArrays( 1, &m_2DtriVAO );
	glDeleteProgram(m_shader);
}

void Gizmos::create(unsigned int a_maxLines /* = 0xffff */, unsigned int a_maxTris /* = 0xffff */,
					unsigned int a_max2DLines /* = 0xff */, unsigned int a_max2DTris /* = 0xff */) {
	if (sm_singleton == nullptr)
		sm_singleton = new Gizmos(a_maxLines,a_maxTris,a_max2DLines,a_max2DTris);
}

void Gizmos::destroy() {
	delete sm_singleton;
	sm_singleton = nullptr;
}

void Gizmos::clear() {
	sm_singleton->m_lineCount = 0;
	sm_singleton->m_triCount = 0;
	sm_singleton->m_transparentTriCount = 0;
	sm_singleton->m_2DlineCount = 0;
	sm_singleton->m_2DtriCount = 0;
}

// Adds 3 unit-length lines (red,green,blue) representing the 3 axis of a transform, 
// at the transform's translation. Optional scale available.
void Gizmos::addTransform(const glm::mat4& a_transform, float a_fScale /* = 1.0f */) {
	glm::vec4 vXAxis = a_transform[3] + a_transform[0] * a_fScale;
	glm::vec4 vYAxis = a_transform[3] + a_transform[1] * a_fScale;
	glm::vec4 vZAxis = a_transform[3] + a_transform[2] * a_fScale;

	glm::vec4 vRed(1,0,0,1);
	glm::vec4 vGreen(0,1,0,1);
	glm::vec4 vBlue(0,0,1,1);

	addLine(a_transform[3].xyz(), vXAxis.xyz(), vRed, vRed);
	addLine(a_transform[3].xyz(), vYAxis.xyz(), vGreen, vGreen);
	addLine(a_transform[3].xyz(), vZAxis.xyz(), vBlue, vBlue);
}

void Gizmos::addAABB(const glm::vec3& a_center, 
	const glm::vec3& a_rvExtents, 
	const glm::vec4& a_colour, 
	const glm::mat4* a_transform /* = nullptr */) {
	glm::vec3 vVerts[8];
	glm::vec3 c = a_center;
	glm::vec3 vX(a_rvExtents.x, 0, 0);
	glm::vec3 vY(0, a_rvExtents.y, 0);
	glm::vec3 vZ(0, 0, a_rvExtents.z);

	if (a_transform != nullptr)
	{
		vX = (*a_transform * glm::vec4(vX,0)).xyz();
		vY = (*a_transform * glm::vec4(vY,0)).xyz();
		vZ = (*a_transform * glm::vec4(vZ,0)).xyz();
		c = (*a_transform * glm::vec4(c, 1)).xyz();
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

	addLine(vVerts[0], vVerts[1], a_colour, a_colour);
	addLine(vVerts[1], vVerts[2], a_colour, a_colour);
	addLine(vVerts[2], vVerts[3], a_colour, a_colour);
	addLine(vVerts[3], vVerts[0], a_colour, a_colour);

	addLine(vVerts[4], vVerts[5], a_colour, a_colour);
	addLine(vVerts[5], vVerts[6], a_colour, a_colour);
	addLine(vVerts[6], vVerts[7], a_colour, a_colour);
	addLine(vVerts[7], vVerts[4], a_colour, a_colour);

	addLine(vVerts[0], vVerts[4], a_colour, a_colour);
	addLine(vVerts[1], vVerts[5], a_colour, a_colour);
	addLine(vVerts[2], vVerts[6], a_colour, a_colour);
	addLine(vVerts[3], vVerts[7], a_colour, a_colour);
}

void Gizmos::addAABBFilled(const glm::vec3& a_center, 
	const glm::vec3& a_rvExtents, 
	const glm::vec4& a_fillColour, 
	const glm::mat4* a_transform /* = nullptr */) {
	glm::vec3 vVerts[8];
	glm::vec3 center = a_center;
	glm::vec3 vX(a_rvExtents.x, 0, 0);
	glm::vec3 vY(0, a_rvExtents.y, 0);
	glm::vec3 vZ(0, 0, a_rvExtents.z);

	if (a_transform != nullptr)
	{
		vX = (*a_transform * glm::vec4(vX, 0)).xyz();
		vY = (*a_transform * glm::vec4(vY, 0)).xyz();
		vZ = (*a_transform * glm::vec4(vZ, 0)).xyz();
		center = (*a_transform * glm::vec4(center, 1)).xyz();
	}

	// top verts
	vVerts[0] = center - vX - vZ - vY;
	vVerts[1] = center - vX + vZ - vY;
	vVerts[2] = center + vX + vZ - vY;
	vVerts[3] = center + vX - vZ - vY;

	// bottom verts
	vVerts[4] = center - vX - vZ + vY;
	vVerts[5] = center - vX + vZ + vY;
	vVerts[6] = center + vX + vZ + vY;
	vVerts[7] = center + vX - vZ + vY;

	glm::vec4 vWhite(1,1,1,1);

	addLine(vVerts[0], vVerts[1], vWhite, vWhite);
	addLine(vVerts[1], vVerts[2], vWhite, vWhite);
	addLine(vVerts[2], vVerts[3], vWhite, vWhite);
	addLine(vVerts[3], vVerts[0], vWhite, vWhite);

	addLine(vVerts[4], vVerts[5], vWhite, vWhite);
	addLine(vVerts[5], vVerts[6], vWhite, vWhite);
	addLine(vVerts[6], vVerts[7], vWhite, vWhite);
	addLine(vVerts[7], vVerts[4], vWhite, vWhite);

	addLine(vVerts[0], vVerts[4], vWhite, vWhite);
	addLine(vVerts[1], vVerts[5], vWhite, vWhite);
	addLine(vVerts[2], vVerts[6], vWhite, vWhite);
	addLine(vVerts[3], vVerts[7], vWhite, vWhite);

	// top
	addTri(vVerts[2], vVerts[1], vVerts[0], a_fillColour);
	addTri(vVerts[3], vVerts[2], vVerts[0], a_fillColour);

	// bottom
	addTri(vVerts[5], vVerts[6], vVerts[4], a_fillColour);
	addTri(vVerts[6], vVerts[7], vVerts[4], a_fillColour);

	// front
	addTri(vVerts[4], vVerts[3], vVerts[0], a_fillColour);
	addTri(vVerts[7], vVerts[3], vVerts[4], a_fillColour);

	// back
	addTri(vVerts[1], vVerts[2], vVerts[5], a_fillColour);
	addTri(vVerts[2], vVerts[6], vVerts[5], a_fillColour);

	// left
	addTri(vVerts[0], vVerts[1], vVerts[4], a_fillColour);
	addTri(vVerts[1], vVerts[5], vVerts[4], a_fillColour);

	// right
	addTri(vVerts[2], vVerts[3], vVerts[7], a_fillColour);
	addTri(vVerts[6], vVerts[2], vVerts[7], a_fillColour);
}

void Gizmos::addCylinderFilled(const glm::vec3& a_center, float a_radius, float a_fHalfLength,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */) {
	glm::vec4 white(1,1,1,1);

	glm::vec3 center = a_transform != nullptr ? (*a_transform * glm::vec4(a_center, 1)).xyz() : a_center;

	float segmentSize = (2 * glm::pi<float>()) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v0top(0,a_fHalfLength,0);
		glm::vec3 v1top( sinf( i * segmentSize ) * a_radius, a_fHalfLength, cosf( i * segmentSize ) * a_radius);
		glm::vec3 v2top( sinf( (i+1) * segmentSize ) * a_radius, a_fHalfLength, cosf( (i+1) * segmentSize ) * a_radius);
		glm::vec3 v0bottom(0,-a_fHalfLength,0);
		glm::vec3 v1bottom( sinf( i * segmentSize ) * a_radius, -a_fHalfLength, cosf( i * segmentSize ) * a_radius);
		glm::vec3 v2bottom( sinf( (i+1) * segmentSize ) * a_radius, -a_fHalfLength, cosf( (i+1) * segmentSize ) * a_radius);

		if (a_transform != nullptr)
		{
			v0top = (*a_transform * glm::vec4(v0top, 0)).xyz();
			v1top = (*a_transform * glm::vec4(v1top, 0)).xyz();
			v2top = (*a_transform * glm::vec4(v2top, 0)).xyz();
			v0bottom = (*a_transform * glm::vec4(v0bottom, 0)).xyz();
			v1bottom = (*a_transform * glm::vec4(v1bottom, 0)).xyz();
			v2bottom = (*a_transform * glm::vec4(v2bottom, 0)).xyz();
		}

		// triangles
		addTri(center + v0top, center + v1top, center + v2top, a_fillColour);
		addTri(center + v0bottom, center + v2bottom, center + v1bottom, a_fillColour);
		addTri(center + v2top, center + v1top, center + v1bottom, a_fillColour);
		addTri(center + v1bottom, center + v2bottom, center + v2top, a_fillColour);

		// lines
		addLine(center + v1top, center + v2top, white, white);
		addLine(center + v1top, center + v1bottom, white, white);
		addLine(center + v1bottom, center + v2bottom, white, white);
	}
}

void Gizmos::addRing(const glm::vec3& a_center, float a_innerRadius, float a_outerRadius,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */) {
	glm::vec4 vSolid = a_fillColour;
	vSolid.w = 1;

	glm::vec3 center = a_transform != nullptr ? (*a_transform * glm::vec4(a_center, 1)).xyz() : a_center;

	float fSegmentSize = (2 * glm::pi<float>()) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v1outer( sinf( i * fSegmentSize ) * a_outerRadius, 0, cosf( i * fSegmentSize ) * a_outerRadius );
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize ) * a_outerRadius, 0, cosf( (i+1) * fSegmentSize ) * a_outerRadius );
		glm::vec3 v1inner( sinf( i * fSegmentSize ) * a_innerRadius, 0, cosf( i * fSegmentSize ) * a_innerRadius );
		glm::vec3 v2inner( sinf( (i+1) * fSegmentSize ) * a_innerRadius, 0, cosf( (i+1) * fSegmentSize ) * a_innerRadius );

		if (a_transform != nullptr)
		{
			v1outer = (*a_transform * glm::vec4(v1outer, 0)).xyz();
			v2outer = (*a_transform * glm::vec4(v2outer, 0)).xyz();
			v1inner = (*a_transform * glm::vec4(v1inner, 0)).xyz();
			v2inner = (*a_transform * glm::vec4(v2inner, 0)).xyz();
		}

		if (a_fillColour.w != 0)
		{
			addTri(center + v2outer, center + v1outer, center + v1inner, a_fillColour);
			addTri(center + v1inner, center + v2inner, center + v2outer, a_fillColour);

			addTri(center + v1inner, center + v1outer, center + v2outer, a_fillColour);
			addTri(center + v2outer, center + v2inner, center + v1inner, a_fillColour);
		}
		else
		{
			// line
			addLine(center + v1inner + a_center, a_center + v2inner + a_center, vSolid, vSolid);
			addLine(center + v1outer + a_center, a_center + v2outer + a_center, vSolid, vSolid);
		}
	}
}

void Gizmos::addDisk(const glm::vec3& a_center, float a_radius,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */) {
	glm::vec4 vSolid = a_fillColour;
	vSolid.w = 1;

	glm::vec3 center = a_transform != nullptr ? (*a_transform * glm::vec4(a_center, 1)).xyz() : a_center;

	float fSegmentSize = (2 * glm::pi<float>()) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v1outer( sinf( i * fSegmentSize ) * a_radius, 0, cosf( i * fSegmentSize ) * a_radius );
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize ) * a_radius, 0, cosf( (i+1) * fSegmentSize ) * a_radius );

		if (a_transform != nullptr)
		{
			v1outer = (*a_transform * glm::vec4(v1outer, 0)).xyz();
			v2outer = (*a_transform * glm::vec4(v2outer, 0)).xyz();
		}

		if (a_fillColour.w != 0)
		{
			addTri(center, center + v1outer, center + v2outer, a_fillColour);
			addTri(center + v2outer, center + v1outer, center, a_fillColour);
		}
		else
		{
			// line
			addLine(center + v1outer, center + v2outer, vSolid, vSolid);
		}
	}
}

void Gizmos::addArc(const glm::vec3& a_center, float a_rotation,
	float a_radius, float a_arcHalfAngle,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */) {
	glm::vec4 vSolid = a_fillColour;
	vSolid.w = 1;

	glm::vec3 center = a_transform != nullptr ? (*a_transform * glm::vec4(a_center, 1)).xyz() : a_center;

	float fSegmentSize = (2 * a_arcHalfAngle) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v1outer( sinf( i * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_radius, 0, cosf( i * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_radius);
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_radius, 0, cosf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_radius);

		if (a_transform != nullptr)
		{
			v1outer = (*a_transform * glm::vec4(v1outer, 0)).xyz();
			v2outer = (*a_transform * glm::vec4(v2outer, 0)).xyz();
		}

		if (a_fillColour.w != 0)
		{
			addTri(center, center + v1outer, center + v2outer, a_fillColour);
			addTri(center + v2outer, center + v1outer, center, a_fillColour);
		}
		else
		{
			// line
			addLine(center + v1outer, center + v2outer, vSolid, vSolid);
		}
	}

	// edge lines
	if (a_fillColour.w == 0)
	{
		glm::vec3 v1outer( sinf( -a_arcHalfAngle + a_rotation ) * a_radius, 0, cosf( -a_arcHalfAngle + a_rotation ) * a_radius );
		glm::vec3 v2outer( sinf( a_arcHalfAngle + a_rotation ) * a_radius, 0, cosf( a_arcHalfAngle + a_rotation ) * a_radius );

		if (a_transform != nullptr)
		{
			v1outer = (*a_transform * glm::vec4(v1outer, 0)).xyz();
			v2outer = (*a_transform * glm::vec4(v2outer, 0)).xyz();
		}

		addLine(center, center + v1outer, vSolid, vSolid);
		addLine(center, center + v2outer, vSolid, vSolid);
	}
}

void Gizmos::addArcRing(const glm::vec3& a_center, float a_rotation, 
	float a_innerRadius, float a_outerRadius, float a_arcHalfAngle,
	unsigned int a_segments, const glm::vec4& a_fillColour, const glm::mat4* a_transform /* = nullptr */) {
	glm::vec4 vSolid = a_fillColour;
	vSolid.w = 1;

	glm::vec3 center = a_transform != nullptr ? (*a_transform * glm::vec4(a_center, 1)).xyz() : a_center;

	float fSegmentSize = (2 * a_arcHalfAngle) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec3 v1outer( sinf( i * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( i * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v1inner( sinf( i * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( i * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius );
		glm::vec3 v2inner( sinf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( (i+1) * fSegmentSize - a_arcHalfAngle + a_rotation  ) * a_innerRadius );

		if (a_transform != nullptr)
		{
			v1outer = (*a_transform * glm::vec4(v1outer, 0)).xyz();
			v2outer = (*a_transform * glm::vec4(v2outer, 0)).xyz();
			v1inner = (*a_transform * glm::vec4(v1inner, 0)).xyz();
			v2inner = (*a_transform * glm::vec4(v2inner, 0)).xyz();
		}

		if (a_fillColour.w != 0)
		{
			addTri(center + v2outer, center + v1outer, center + v1inner, a_fillColour);
			addTri(center + v1inner, center + v2inner, center + v2outer, a_fillColour);

			addTri(center + v1inner, center + v1outer, center + v2outer, a_fillColour);
			addTri(center + v2outer, center + v2inner, center + v1inner, a_fillColour);
		}
		else
		{
			// line
			addLine(center + v1inner, center + v2inner, vSolid, vSolid);
			addLine(center + v1outer, center + v2outer, vSolid, vSolid);
		}
	}

	// edge lines
	if (a_fillColour.w == 0)
	{
		glm::vec3 v1outer( sinf( -a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( -a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v2outer( sinf( a_arcHalfAngle + a_rotation ) * a_outerRadius, 0, cosf( a_arcHalfAngle + a_rotation ) * a_outerRadius );
		glm::vec3 v1inner( sinf( -a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( -a_arcHalfAngle + a_rotation  ) * a_innerRadius );
		glm::vec3 v2inner( sinf( a_arcHalfAngle + a_rotation  ) * a_innerRadius, 0, cosf( a_arcHalfAngle + a_rotation  ) * a_innerRadius );

		if (a_transform != nullptr)
		{
			v1outer = (*a_transform * glm::vec4(v1outer, 0)).xyz();
			v2outer = (*a_transform * glm::vec4(v2outer, 0)).xyz();
			v1inner = (*a_transform * glm::vec4(v1inner, 0)).xyz();
			v2inner = (*a_transform * glm::vec4(v2inner, 0)).xyz();
		}

		addLine(center + v1inner, center + v1outer, vSolid, vSolid);
		addLine(center + v2inner, center + v2outer, vSolid, vSolid);
	}
}

void Gizmos::addSphere(const glm::vec3& a_center, float a_radius, int a_rows, int a_columns, const glm::vec4& a_fillColour, 
								const glm::mat4* a_transform /*= nullptr*/, float a_longMin /*= 0.f*/, float a_longMax /*= 360*/, 
								float a_latMin /*= -90*/, float a_latMax /*= 90*/) {
	float inverseRadius = 1/a_radius;
	//Invert these first as the multiply is slightly quicker
	float invColumns = 1.0f/float(a_columns);
	float invRows = 1.0f/float(a_rows);

	float DEG2RAD = glm::pi<float>() / 180;

	glm::vec3 center = a_transform != nullptr ? (*a_transform * glm::vec4(a_center, 1)).xyz() : a_center;
	
	//Lets put everything in radians first
	float latitiudinalRange = (a_latMax - a_latMin) * DEG2RAD;
	float longitudinalRange = (a_longMax - a_longMin) * DEG2RAD;
	// for each row of the mesh
	glm::vec3* v4Array = new glm::vec3[a_rows*a_columns + a_columns];

	for (int row = 0; row <= a_rows; ++row)
	{
		// y ordinates this may be a little confusing but here we are navigating around the xAxis in GL
		float ratioAroundXAxis = float(row) * invRows;
		float radiansAboutXAxis  = ratioAroundXAxis * latitiudinalRange + (a_latMin * DEG2RAD);
		float y  =  a_radius * sin(radiansAboutXAxis);
		float z  =  a_radius * cos(radiansAboutXAxis);
		
		for ( int col = 0; col <= a_columns; ++col )
		{
			float ratioAroundYAxis   = float(col) * invColumns;
			float theta = ratioAroundYAxis * longitudinalRange + (a_longMin * DEG2RAD);
			glm::vec3 v4Point( -z * sinf(theta), y, -z * cosf(theta) );
			glm::vec3 v4Normal( inverseRadius * v4Point.x, inverseRadius * v4Point.y, inverseRadius * v4Point.z);

			if (a_transform != nullptr)
			{
				v4Point = (*a_transform * glm::vec4(v4Point, 0)).xyz();
				v4Normal = (*a_transform * glm::vec4(v4Normal, 0)).xyz();
			}

			int index = row * a_columns + (col % a_columns);
			v4Array[index] = v4Point;
		}
	}
	
	for (int face = 0; face < (a_rows)*(a_columns); ++face ) {
		int iNextFace = face + 1;		
		
		if( iNextFace % a_columns == 0 )
			iNextFace = iNextFace - (a_columns);

		addLine(center + v4Array[face], center + v4Array[face+a_columns], glm::vec4(1.f,1.f,1.f,1.f), glm::vec4(1.f,1.f,1.f,1.f));
		
		if( face % a_columns == 0 && longitudinalRange < (glm::pi<float>() * 2))
			continue;
		addLine(center + v4Array[iNextFace+a_columns], center + v4Array[face+a_columns], glm::vec4(1.f,1.f,1.f,1.f), glm::vec4(1.f,1.f,1.f,1.f));

		addTri(center + v4Array[iNextFace+a_columns], center + v4Array[face], center + v4Array[iNextFace], a_fillColour);
		addTri(center + v4Array[iNextFace+a_columns], center + v4Array[face+a_columns], center + v4Array[face], a_fillColour);
	}

	delete[] v4Array;	
}

void Gizmos::addCapsule(const glm::vec3& a_center, float a_height, float a_radius,
						int a_rows, int a_cols, const glm::vec4& a_fillColour, const glm::mat4* a_rotation /* = nullptr */) {

	float sphereCenters = (a_height * 0.5f) - a_radius;
	glm::vec4 top = glm::vec4(0, sphereCenters, 0, 0);
	glm::vec4 bottom = glm::vec4(0, -sphereCenters, 0, 0);
	glm::vec4 white(1);

	if (a_rotation) {
		top = (*a_rotation) * top;
		bottom = (*a_rotation) * bottom;
	}

	glm::vec3 topCenter = a_center + top.xyz();
	glm::vec3 bottomCenter = a_center + bottom.xyz();

	float inverseRadius = 1 / a_radius;

	// invert these first as the multiply is slightly quicker
	float invColumns = 1.0f / float(a_cols);
	float invRows = 1.0f / float(a_rows);

	float DEG2RAD = glm::pi<float>() / 180;
	float a_latMin = -90, a_latMax = 90, a_longMin = 0.f, a_longMax = 360;

	// lets put everything in radians first
	float latitiudinalRange = (a_latMax - a_latMin) * DEG2RAD;
	float longitudinalRange = (a_longMax - a_longMin) * DEG2RAD;

	// for each row of the mesh
	glm::vec3* v4Array = new glm::vec3[a_rows*a_cols + a_cols];

	for (int row = 0; row <= (a_rows); ++row) {
		// y ordinates this may be a little confusing but here we are navigating around the xAxis in GL
		float ratioAroundXAxis = float(row) * invRows;
		float radiansAboutXAxis = ratioAroundXAxis * latitiudinalRange + (a_latMin * DEG2RAD);
		float y = a_radius * sin(radiansAboutXAxis);
		float z = a_radius * cos(radiansAboutXAxis);

		for (int col = 0; col <= a_cols; ++col) {
			float ratioAroundYAxis = float(col) * invColumns;
			float theta = ratioAroundYAxis * longitudinalRange + (a_longMin * DEG2RAD);
			glm::vec3 v4Point(-z * sinf(theta), y, -z * cosf(theta));
			glm::vec3 v4Normal(inverseRadius * v4Point.x, inverseRadius * v4Point.y, inverseRadius * v4Point.z);

			if (a_rotation != nullptr) {
				v4Point = (*a_rotation * glm::vec4(v4Point, 0)).xyz();
				v4Normal = (*a_rotation * glm::vec4(v4Normal, 0)).xyz();
			}

			int index = row * a_cols + (col % a_cols);
			v4Array[index] = v4Point;
		}
	}

	// create a sphere that is split in two
	for (int face = 0; face < (a_rows)*(a_cols); ++face) {
		int iNextFace = face + 1;

		if (iNextFace % a_cols == 0)
			iNextFace = iNextFace - (a_cols);

		glm::vec3 center = topCenter;
		if (face < (a_rows / 2)*a_cols)
			center = bottomCenter;

		addLine(center + v4Array[face], center + v4Array[face + a_cols], white, white);

		if (face % a_cols == 0 && longitudinalRange < (glm::pi<float>() * 2))
			continue;

		addLine(center + v4Array[iNextFace + a_cols], center + v4Array[face + a_cols], white, white);
		addTri(center + v4Array[iNextFace + a_cols], center + v4Array[face], center + v4Array[iNextFace], a_fillColour);
		addTri(center + v4Array[iNextFace + a_cols], center + v4Array[face + a_cols], center + v4Array[face], a_fillColour);
	}

	delete[] v4Array;

	for (int i = 0; i < a_cols; ++i) {
		float x = (float)i / (float)a_cols;
		float x1 = (float)(i+1) / (float)a_cols;
		x *= 2.0f * glm::pi<float>();
		x1 *= 2.0f * glm::pi<float>();

		glm::vec4 pos = glm::vec4(cosf(x), 0, sinf(x), 0) * a_radius;
		glm::vec4 pos1 = glm::vec4(cosf(x1), 0, sinf(x1), 0) * a_radius;

		if (a_rotation) {
			pos = (*a_rotation) * pos;
			pos1 = (*a_rotation) * pos1;
		}

		addTri(topCenter + pos1.xyz(), bottomCenter + pos1.xyz(), bottomCenter + pos.xyz(), a_fillColour);
		addTri(topCenter + pos1.xyz(), bottomCenter + pos.xyz(), topCenter + pos.xyz(), a_fillColour);

		addLine(topCenter + pos.xyz(), topCenter + pos1.xyz(), white, white);
		addLine(bottomCenter + pos.xyz(), bottomCenter + pos1.xyz(), white, white);
		addLine(topCenter + pos.xyz(), bottomCenter + pos.xyz(), white, white);
	}
}

void Gizmos::addHermiteSpline(const glm::vec3& a_start, const glm::vec3& a_end,
	const glm::vec3& a_tangentStart, const glm::vec3& a_tangentEnd, unsigned int a_segments, const glm::vec4& a_colour) {
	a_segments = a_segments > 1 ? a_segments : 1;

	glm::vec3 prev = a_start;

	for ( unsigned int i = 1 ; i <= a_segments ; ++i )
	{
		float s = i / (float)a_segments;

		float s2 = s * s;
		float s3 = s2 * s;
		float h1 = (2.0f * s3) - (3.0f * s2) + 1.0f;
		float h2 = (-2.0f * s3) + (3.0f * s2);
		float h3 =  s3- (2.0f * s2) + s;
		float h4 = s3 - s2;
		glm::vec3 p = (a_start * h1) + (a_end * h2) + (a_tangentStart * h3) + (a_tangentEnd * h4);

		addLine(prev,p,a_colour,a_colour);
		prev = p;
	}
}

void Gizmos::addLine(const glm::vec3& a_rv0,  const glm::vec3& a_rv1, const glm::vec4& a_colour) {
	addLine(a_rv0,a_rv1,a_colour,a_colour);
}

void Gizmos::addLine(const glm::vec3& a_rv0, const glm::vec3& a_rv1, const glm::vec4& a_colour0, const glm::vec4& a_colour1) {
	if (sm_singleton != nullptr &&
		sm_singleton->m_lineCount < sm_singleton->m_maxLines)
	{
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.x = a_rv0.x;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.y = a_rv0.y;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.z = a_rv0.z;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.w = 1;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.r = a_colour0.r;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.g = a_colour0.g;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.b = a_colour0.b;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v0.a = a_colour0.a;

		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.x = a_rv1.x;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.y = a_rv1.y;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.z = a_rv1.z;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.w = 1;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.r = a_colour1.r;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.g = a_colour1.g;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.b = a_colour1.b;
		sm_singleton->m_lines[sm_singleton->m_lineCount].v1.a = a_colour1.a;

		sm_singleton->m_lineCount++;
	}
}

void Gizmos::addTri(const glm::vec3& a_rv0, const glm::vec3& a_rv1, const glm::vec3& a_rv2, const glm::vec4& a_colour) {
	if (sm_singleton != nullptr)
	{
		if (a_colour.w == 1)
		{
			if (sm_singleton->m_triCount < sm_singleton->m_maxTris)
			{
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.x = a_rv0.x;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.y = a_rv0.y;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.z = a_rv0.z;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.w = 1;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.x = a_rv1.x;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.y = a_rv1.y;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.z = a_rv1.z;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.w = 1;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.x = a_rv2.x;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.y = a_rv2.y;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.z = a_rv2.z;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.w = 1;

				sm_singleton->m_tris[sm_singleton->m_triCount].v0.r = a_colour.r;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.g = a_colour.g;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.b = a_colour.b;
				sm_singleton->m_tris[sm_singleton->m_triCount].v0.a = a_colour.a;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.r = a_colour.r;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.g = a_colour.g;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.b = a_colour.b;
				sm_singleton->m_tris[sm_singleton->m_triCount].v1.a = a_colour.a;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.r = a_colour.r;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.g = a_colour.g;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.b = a_colour.b;
				sm_singleton->m_tris[sm_singleton->m_triCount].v2.a = a_colour.a;

				sm_singleton->m_triCount++;
			}
		}
		else
		{
			if (sm_singleton->m_transparentTriCount < sm_singleton->m_maxTris)
			{
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.x = a_rv0.x;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.y = a_rv0.y;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.z = a_rv0.z;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.w = 1;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.x = a_rv1.x;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.y = a_rv1.y;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.z = a_rv1.z;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.w = 1;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.x = a_rv2.x;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.y = a_rv2.y;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.z = a_rv2.z;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.w = 1;

				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.r = a_colour.r;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.g = a_colour.g;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.b = a_colour.b;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v0.a = a_colour.a;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.r = a_colour.r;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.g = a_colour.g;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.b = a_colour.b;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v1.a = a_colour.a;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.r = a_colour.r;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.g = a_colour.g;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.b = a_colour.b;
				sm_singleton->m_transparentTris[sm_singleton->m_transparentTriCount].v2.a = a_colour.a;

				sm_singleton->m_transparentTriCount++;
			}
		}
	}
}

void Gizmos::add2DAABB(const glm::vec2& a_center, const glm::vec2& a_extents, const glm::vec4& a_colour, const glm::mat4* a_transform /*= nullptr*/) {	
	glm::vec2 verts[4];
	glm::vec2 vX(a_extents.x, 0);
	glm::vec2 vY(0, a_extents.y);

	if (a_transform != nullptr)
	{
		vX = (*a_transform * glm::vec4(vX,0,0)).xy();
		vY = (*a_transform * glm::vec4(vY,0,0)).xy();
	}

	verts[0] = a_center - vX - vY;
	verts[1] = a_center + vX - vY;
	verts[2] = a_center - vX + vY;
	verts[3] = a_center + vX + vY;

	add2DLine(verts[0], verts[1], a_colour, a_colour);
	add2DLine(verts[1], verts[3], a_colour, a_colour);
	add2DLine(verts[2], verts[3], a_colour, a_colour);
	add2DLine(verts[2], verts[0], a_colour, a_colour);
}

void Gizmos::add2DAABBFilled(const glm::vec2& a_center, const glm::vec2& a_extents, const glm::vec4& a_colour, const glm::mat4* a_transform /*= nullptr*/) {	
	glm::vec2 verts[4];
	glm::vec2 vX(a_extents.x, 0);
	glm::vec2 vY(0, a_extents.y);

	if (a_transform != nullptr)
	{
		vX = (*a_transform * glm::vec4(vX,0,0)).xy();
		vY = (*a_transform * glm::vec4(vY,0,0)).xy();
	}

	verts[0] = a_center - vX - vY;
	verts[1] = a_center + vX - vY;
	verts[2] = a_center + vX + vY;
	verts[3] = a_center - vX + vY;
	
	add2DTri(verts[0], verts[1], verts[2], a_colour);
	add2DTri(verts[0], verts[2], verts[3], a_colour);
}

void Gizmos::add2DCircle(const glm::vec2& a_center, float a_radius, unsigned int a_segments, const glm::vec4& a_colour, const glm::mat4* a_transform /*= nullptr*/) {
	glm::vec4 solidColour = a_colour;
	solidColour.w = 1;

	float segmentSize = (2 * glm::pi<float>()) / a_segments;

	for ( unsigned int i = 0 ; i < a_segments ; ++i )
	{
		glm::vec2 v1outer( sinf( i * segmentSize ) * a_radius, cosf( i * segmentSize ) * a_radius );
		glm::vec2 v2outer( sinf( (i+1) * segmentSize ) * a_radius, cosf( (i+1) * segmentSize ) * a_radius );

		if (a_transform != nullptr)
		{
			v1outer = (*a_transform * glm::vec4(v1outer,0,0)).xy();
			v2outer = (*a_transform * glm::vec4(v2outer,0,0)).xy();
		}

		if (a_colour.w != 0)
		{
			add2DTri(a_center, a_center + v1outer, a_center + v2outer, a_colour);
			add2DTri(a_center + v2outer, a_center + v1outer, a_center, a_colour);
		}
		else
		{
			// line
			add2DLine(a_center + v1outer, a_center + v2outer, solidColour, solidColour);
		}
	}
}

void Gizmos::add2DLine(const glm::vec2& a_rv0,  const glm::vec2& a_rv1, const glm::vec4& a_colour) {
	add2DLine(a_rv0,a_rv1,a_colour,a_colour);
}

void Gizmos::add2DLine(const glm::vec2& a_rv0, const glm::vec2& a_rv1, const glm::vec4& a_colour0, const glm::vec4& a_colour1) {
	if (sm_singleton != nullptr &&
		sm_singleton->m_2DlineCount < sm_singleton->m_max2DLines)
	{
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.x = a_rv0.x;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.y = a_rv0.y;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.z = 1;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.w = 1;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.r = a_colour0.r;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.g = a_colour0.g;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.b = a_colour0.b;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v0.a = a_colour0.a;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.x = a_rv1.x;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.y = a_rv1.y;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.z = 1;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.w = 1;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.r = a_colour1.r;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.g = a_colour1.g;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.b = a_colour1.b;
		sm_singleton->m_2Dlines[sm_singleton->m_2DlineCount].v1.a = a_colour1.a;

		sm_singleton->m_2DlineCount++;
	}
}

void Gizmos::add2DTri(const glm::vec2& a_rv0, const glm::vec2& a_rv1, const glm::vec2& a_rv2, const glm::vec4& a_colour) {
	if (sm_singleton != nullptr)
	{
		if (sm_singleton->m_2DtriCount < sm_singleton->m_max2DTris)
		{
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.x = a_rv0.x;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.y = a_rv0.y;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.z = 1;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.w = 1;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.x = a_rv1.x;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.y = a_rv1.y;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.z = 1;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.w = 1;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.x = a_rv2.x;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.y = a_rv2.y;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.z = 1;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.w = 1;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.r = a_colour.r;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.g = a_colour.g;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.b = a_colour.b;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v0.a = a_colour.a;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.r = a_colour.r;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.g = a_colour.g;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.b = a_colour.b;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v1.a = a_colour.a;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.r = a_colour.r;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.g = a_colour.g;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.b = a_colour.b;
			sm_singleton->m_2Dtris[sm_singleton->m_2DtriCount].v2.a = a_colour.a;

			sm_singleton->m_2DtriCount++;
		}
	}
}

void Gizmos::draw(const glm::mat4& a_projection, const glm::mat4& a_view) {
	draw(a_projection * a_view);
}

void Gizmos::draw(const glm::mat4& a_projectionView) {
	if ( sm_singleton != nullptr && (sm_singleton->m_lineCount > 0 || sm_singleton->m_triCount > 0 || sm_singleton->m_transparentTriCount > 0))
	{
		int shader = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &shader);

		glUseProgram(sm_singleton->m_shader);
		
		unsigned int projectionViewUniform = glGetUniformLocation(sm_singleton->m_shader,"ProjectionView");
		glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(a_projectionView));

		if (sm_singleton->m_lineCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_lineVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_lineCount * sizeof(GizmoLine), sm_singleton->m_lines);

			glBindVertexArray(sm_singleton->m_lineVAO);
			glDrawArrays(GL_LINES, 0, sm_singleton->m_lineCount * 2);
		}

		if (sm_singleton->m_triCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_triVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_triCount * sizeof(GizmoTri), sm_singleton->m_tris);

			glBindVertexArray(sm_singleton->m_triVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_singleton->m_triCount * 3);
		}
		
		if (sm_singleton->m_transparentTriCount > 0)
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

			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_transparentTriVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_transparentTriCount * sizeof(GizmoTri), sm_singleton->m_transparentTris);

			glBindVertexArray(sm_singleton->m_transparentTriVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_singleton->m_transparentTriCount * 3);

			// reset state
			glDepthMask(depthMask);
			glBlendFunc(src, dst);
			if (blendEnabled == GL_FALSE)
				glDisable(GL_BLEND);
		}

		glUseProgram(shader);
	}
}

void Gizmos::draw2D(const glm::mat4& a_projection) {
	if ( sm_singleton != nullptr && (sm_singleton->m_2DlineCount > 0 || sm_singleton->m_2DtriCount > 0))
	{
		int shader = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &shader);

		glUseProgram(sm_singleton->m_shader);
		
		unsigned int projectionViewUniform = glGetUniformLocation(sm_singleton->m_shader,"ProjectionView");
		glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(a_projection));

		if (sm_singleton->m_2DlineCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_2DlineVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_2DlineCount * sizeof(GizmoLine), sm_singleton->m_2Dlines);

			glBindVertexArray(sm_singleton->m_2DlineVAO);
			glDrawArrays(GL_LINES, 0, sm_singleton->m_2DlineCount * 2);
		}

		if (sm_singleton->m_2DtriCount > 0)
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

			glBindBuffer(GL_ARRAY_BUFFER, sm_singleton->m_2DtriVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_singleton->m_2DtriCount * sizeof(GizmoTri), sm_singleton->m_2Dtris);

			glBindVertexArray(sm_singleton->m_2DtriVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_singleton->m_2DtriCount * 3);

			glDepthMask(depthMask);

			glBlendFunc(src, dst);

			if (blendEnabled == GL_FALSE)
				glDisable(GL_BLEND);
		}

		glUseProgram(shader);
	}
}

} // namespace aie