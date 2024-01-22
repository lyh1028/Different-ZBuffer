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
	//�����ͶӰ����Ļ�ϵı�
public:
	typedef std::shared_ptr<Edge> ptr;
	glm::vec4 cpos; //�ߵ��϶˵�NDC�ռ�����
	float x;//�ߵ��϶˵���Ļ�ռ�x���꣬���ڴ���+-dx����������float����
	float dx; //б��Ϊk����dx = -1/k
	int dy; //�ñ��ܹ�������ٸ�ɨ����
	int id; //�������Ķ���α��
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
	float xl; //��դ����ֻ����������������������Ҫ�ȱ������������ż���
	float dxl;
	int dyl;

	float xr;
	float dxr;
	int dyr;

	double zl;
	double dzx;
	double dzy;
	int id; //��߶����ڵĶ���εı��
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

