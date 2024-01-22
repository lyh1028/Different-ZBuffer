#ifndef POLYGEN_H
#define POLYGEN_H
#include "glm/glm.hpp"
#include<unordered_map>
#include <vector>
#include <memory>
#include <limits>
#include"DrawableMesh.h"
#include<list>
#include<utility>

class Edge {
	//多边形投影到屏幕上的边
public:
	typedef std::shared_ptr<Edge> ptr;
	glm::vec4 cpos; //边的上端点NDC空间坐标
	float x;//边的上端点屏幕空间x坐标，由于存在+-dx，所以先用float代替
	float dx; //斜率为k，则dx = -1/k
	int dy; //该边总共跨过多少个扫描线
	int id; //边所属的多边形编号
	Edge() = default;
	bool operator<(const Edge& other) const {
		if (x == other.x) return dx < other.dx;
		return x < other.x;
	}
	//Edge(int _x)
};


class ActiveEdgePair {
public:
	typedef std::shared_ptr<ActiveEdgePair> ptr;
	float xl; //光栅化后只能是整数，但这里我们需要先保留浮点数等着计算
	float dxl;
	int dyl;

	float xr;
	float dxr;
	int dyr;

	double zl;
	double dzx;
	double dzy;
	int id; //活化边对所在的多边形的编号
	glm::vec4 color;
};
class Polygen {
public:
	typedef std::shared_ptr<Polygen> ptr;
	~Polygen() = default;
	float a;
	float b;
	float c;
	float d;
	glm::vec4 color;
	std::vector<VertexData> v;
	int ymax;
	int id;
	int dy;
	Polygen() {}
	Polygen(int _id, glm::vec4 _color) :id(_id), color(_color) {}
	bool operator<(const Polygen& other) const {
		return ymax > other.ymax;
	}
};
std::pair<int,int> makeEdges(const std::vector<VertexData>& vertices, std::unordered_map<int, std::list<Edge> >& edge_table, int id);


#endif // !POLYGEN_H

