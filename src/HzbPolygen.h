#ifndef HZBPOLYGEN_H
#define HZBPOLYGEN_H
#include "glm/glm.hpp"
#include"DrawableMesh.h"
#include<memory>

class HzbPolygen {
public:
	std::vector<VertexData> vertices;
	int xmax, xmin, ymax, ymin;
	float zmax, zmin;
	HzbPolygen() = default;
	HzbPolygen(const std::vector<VertexData>& _vertices) {
		xmax = INT_MIN; xmin = INT_MAX;
		ymax = INT_MIN; ymin = INT_MAX;
		zmax = -1.0; zmin = 1.0;
		vertices = _vertices;
		for (auto& v : vertices) {
			xmax = std::max(v.spos.x, xmax);
			xmin = std::min(v.spos.x, xmin);
			ymax = std::max(v.spos.y, ymax);
			ymin = std::min(v.spos.y, ymin);
			zmax = std::max(v.cpos.z, zmax);
			zmin = std::min(v.cpos.z, zmin);
		}
	}
};
#endif // !HZBPOLYGEN_H

