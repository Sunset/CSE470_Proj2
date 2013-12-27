#ifndef _OBJ_LOADER_H_
#define _OBJ_LOADER_H_

#include "mesh.h"
#include <string>

class ObjLoader
{	
public:
	static Mesh* load(const char* file);

private:

	//static bool loadMtl(const char* file, STL_VEC<RefPtr<Material> >& mat_set);
};

#endif