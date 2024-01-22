#include "DrawableMesh.h"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

void DrawableMesh::clear()
{
	vertices_attrib.clear();
	std::vector<MeshFace>().swap(mesh_faces); //Çå¿Õvector£¬ÊÍ·ÅÄÚ´æ
}

void DrawableMesh::loadMeshFromObjFile(const std::string& filename)
{
	unload();

	tinyobj::ObjReaderConfig reader_config;

	tinyobj::ObjReader reader;
	reader_config.triangulate = true;
	if (!reader.ParseFromFile(filename, reader_config))
	{
		if (!reader.Error().empty())
		{
			std::cerr << "TinyObjReader: " << reader.Error();
		}
		exit(1);
	}
	if (!reader.Warning().empty())
	{
		std::cout << "TinyObjReader: " << reader.Warning();
	}
	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();
	
	for (size_t i = 0; i < attrib.vertices.size(); i += 3)
	{
		vertices_attrib.vpositions.push_back(
			glm::vec4(attrib.vertices[i + 0], attrib.vertices[i + 1], attrib.vertices[i + 2], 1.0f));
		vertices_attrib.vcolors.push_back(
			glm::vec4(attrib.colors[i + 0], attrib.colors[i + 1], attrib.colors[i + 2], 1.0f));
	}
	for (size_t i = 0; i < attrib.normals.size(); i += 3)
	{
		vertices_attrib.vnormals.push_back(
			glm::vec3(attrib.normals[i + 0], attrib.normals[i + 1], attrib.normals[i + 2]));
	}
	for (size_t s = 0; s < shapes.size(); ++s)
	{
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
		{
			int fv = shapes[s].mesh.num_face_vertices[f];
			MeshFace face;
			if (fv > 3) std::cout << "ass";
			for (size_t v = 0; v < fv && v < 3; ++v)
			{
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				face.vposIndex[v] = idx.vertex_index;
				face.vnorIndex[v] = idx.normal_index;
			}
			mesh_faces.push_back(face);
			index_offset += fv;
		}
	}
	std::cout << mesh_faces.size() << std::endl;
}

void DrawableMesh::unload()
{
	clear();
}