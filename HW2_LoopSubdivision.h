#ifndef __LoopSubdivision_h__
#define __LoopSubdivision_h__
#include <unordered_map>
#include <vector>
#include "Mesh.h"

////
std::vector<int> GetIncidentVertices(int vertexIndex, const std::vector<Vector3i>& old_tri);
////

inline void LoopSubdivision(TriangleMesh<3>& mesh)
{
	std::vector<Vector3>& old_vtx=mesh.Vertices();
	std::vector<Vector3i>& old_tri=mesh.Elements();
	std::vector<Vector3> new_vtx;		////vertex array for the new mesh
	std::vector<Vector3i> new_tri;		////element array for the new mesh
	
	new_vtx=old_vtx;	////copy all the old vertices to the new_vtx array

	////step 1: add mid-point vertices and triangles
	////for each old triangle, 
	////add three new vertices (in the middle of each edge) to new_vtx 
	////add four new triangles to new_tri

	for (const auto& tri : old_tri) {
		// calculate midpoints
		Vector3 mid01 = 0.5 * (old_vtx[tri[0]] + old_vtx[tri[1]]);
		Vector3 mid12 = 0.5 * (old_vtx[tri[1]] + old_vtx[tri[2]]);
		Vector3 mid20 = 0.5 * (old_vtx[tri[2]] + old_vtx[tri[0]]);

		// add midpoints
		new_vtx.push_back(mid01);
		new_vtx.push_back(mid12);
		new_vtx.push_back(mid20);

		// add triangles
		new_tri.push_back(Vector3i(tri[0], new_vtx.size() - 3, new_vtx.size() - 1));
		new_tri.push_back(Vector3i(new_vtx.size() - 3, tri[1], new_vtx.size() - 2));
		new_tri.push_back(Vector3i(new_vtx.size() - 2, tri[2], new_vtx.size() - 1));
		new_tri.push_back(Vector3i(new_vtx.size() - 3, new_vtx.size() - 2, new_vtx.size() - 1));
	}

	////step 2: update the position for each new mid-point vertex: 
	////for each mid-point vertex, find its two end-point vertices A and B, 
	////and find the two opposite-side vertices on the two incident triangles C and D,
	////then update the new position as .375*(A+B)+.125*(C+D)

	for (size_t i = old_vtx.size(); i < new_vtx.size(); i++) {
		// find endpoints a and b
		Vector3 a = old_vtx[old_tri[i - old_vtx.size()][0]];
		Vector3 b = old_vtx[old_tri[i - old_vtx.size()][1]];

		// find c and d on opposite side
		Vector3 c = old_vtx[old_tri[i - old_vtx.size()][2]];
		Vector3 d = old_vtx[old_tri[i - old_vtx.size()][0]];

		// update position
		//new_vtx[i] = 0.375 * (a + b) + 0.125 * (c + d);
	}

	////step 3: update vertices of each old vertex
	////for each old vertex, find its incident old vertices, and update its position according its incident vertices

	for (size_t i = 0; i < old_vtx.size(); ++i) {
		// find old vertices
		const auto& incident_vertices = GetIncidentVertices(i, old_tri);

		// find weighted avg for even vertices
		Vector3 updated_position(0.0, 0.0, 0.0);
		double aa = 1.0 / incident_vertices.size();
		for (const auto& next : incident_vertices) {
			updated_position += old_vtx[next];
		}
		updated_position = (1.0 - aa) * old_vtx[i] + (aa / incident_vertices.size()) * updated_position;

		// update vertex position
		new_vtx[i] = updated_position;
	}

	////update subdivided vertices and triangles onto the input mesh
	old_vtx=new_vtx;
	old_tri=new_tri;
}

// find vertices next to the vertex
std::vector<int> GetIncidentVertices(int vertexIndex, const std::vector<Vector3i>& old_tri)
{
	std::vector<int> incidentVertices;
	for (const auto& tri : old_tri) {
		for (int j = 0; j < 3; ++j) {
			if (tri[j] == vertexIndex) {
				incidentVertices.push_back(tri[(j + 1) % 3]);
				incidentVertices.push_back(tri[(j + 2) % 3]);
				break;
			}
		}
	}
	return incidentVertices;
}

#endif
