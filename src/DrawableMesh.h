#ifndef DRAWABLEMESH_H
#define DRAWABLEMESH_H

#include "glm/glm.hpp"
#include <vector>
#include <memory>
#include <string>
#include<iostream>

/*
��obj�ļ��ж�ȡ���еĶ������Ƭ
*/
class VertexData {
public:
	glm::vec4 pos;  //World space position
	glm::vec4 col;  //World space color
	glm::vec3 nor;  //World space normal
	glm::vec4 cpos; //Clip space position
	glm::ivec2 spos;//Screen space position
	VertexData(glm::vec4 _pos) :pos(_pos) {}
	VertexData(glm::vec4 _pos, glm::vec4 _col, glm::vec3 _nor) :pos(_pos), col(_col), nor(_nor) {}
	VertexData(glm::vec4 _pos, glm::vec4 _col) :pos(_pos), col(_col) {}
	VertexData(glm::vec4 _col, glm::vec3 _nor, glm::vec4 _cpos, glm::ivec2 _spos):col(_col),nor(_nor),cpos(_cpos),spos(_spos){}
	VertexData(const VertexData& v) {
		pos = v.pos; col = v.col; nor = v.nor; cpos = v.cpos; spos = v.spos;
	}
	VertexData() = default;
};
class VertexAttrib final //��������
{
	public:
		std::vector<glm::vec4> vpositions;  //�������
		std::vector<glm::vec4> vcolors;		//��ɫ
		std::vector<glm::vec3> vnormals;	//����

		void clear()
		{
			std::vector<glm::vec4>().swap(vpositions);
			std::vector<glm::vec4>().swap(vcolors);
			std::vector<glm::vec3>().swap(vnormals);

		}
};

class MeshFace final //������
{
public://�������꣬�������� ��Ӧ���±�
	unsigned int vposIndex[3];
	unsigned int vnorIndex[3];
};

class DrawableMesh final  //��¼һ��mesh
{
public:

	typedef std::shared_ptr<DrawableMesh> ptr;

	DrawableMesh() = default;
	~DrawableMesh() = default;

	DrawableMesh(const DrawableMesh& mesh)
		: vertices_attrib(mesh.vertices_attrib), mesh_faces(mesh.mesh_faces) {}

	DrawableMesh& operator=(const DrawableMesh& mesh)
	{
		if (&mesh == this)
			return *this;
		vertices_attrib = mesh.vertices_attrib;
		mesh_faces = mesh.mesh_faces;
		return *this;
	}

	void loadMeshFromObjFile(const std::string& filename);

	const VertexAttrib& getVerticesAttrib() const { return vertices_attrib;}
	const std::vector<MeshFace>& getMeshFaces() const { return mesh_faces; }
	void clear();
	void unload();
private:
	VertexAttrib vertices_attrib;
	std::vector<MeshFace> mesh_faces;
};









#endif
