#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include "imathvec.h"

/* 
 * Not a good way to deal with a namespace, but quite convenient
*/
using namespace Imath;
using namespace std;

/*
 * Feel free to specify the template with other types
 */
typedef Vec3<float> Vec3f;
typedef Vec2<float> Vec2f;

struct Mesh
{
	vector<Vec3f> m_v;		// array of vertices
	vector<Vec3f> m_n;		// array of normals
	vector<Vec2f> m_texc;	// array of texture coordinates; assume using 2d textures

	vector<int> m_vi;	// vertex indices
	vector<int> m_ni;	// normal indices
	vector<int> m_ti;	// texcoord indices
};

#endif