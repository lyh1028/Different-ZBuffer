#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H

#include <vector>
#include <memory>
#include"DrawableMesh.h"
#include "glm/glm.hpp"
class PointLight
{
public:
	glm::vec3 lightPos;//world space position of light source
	glm::vec3 attenuation; // 1/kc + kl*d +kq*d^2, attenuation三个维度分别是kc, kl, kq, d是光源到物体的距离
	glm::vec3 lightColor;

	PointLight(glm::vec3 pos, glm::vec3 atten, glm::vec3 color)
		: lightPos(pos), attenuation(atten), lightColor(color) {}
	PointLight() = default;
};


class Pipeline {
public:
	typedef std::shared_ptr<Pipeline> ptr;
	static glm::mat4 calcViewPortMatrix(int width, int height);
	static glm::mat4 calcViewMatrix(glm::vec3 camera, glm::vec3 target, glm::vec3 worldUp);
	static glm::mat4 calcPerspProjectMatrix(float fovy, float aspect, float near, float far);
	//Linear interpolation
	static VertexData lerp(const VertexData& v0, const VertexData& v1, float frac);
	//三角形重心坐标插值
	static VertexData barycentricLerp(const VertexData& v0, const VertexData& v1, const VertexData& v2, glm::vec3 w);

	//Perspective correction for interpolation
	static void prePerspCorrection(VertexData& v);
	static void aftPrespCorrection(VertexData& v);
	static void Pipeline::rasterize_fill_edge_function(
		const VertexData& v0,
		const VertexData& v1,
		const VertexData& v2,
		const unsigned int& screen_width,
		const unsigned int& screen_height,
		std::vector<VertexData>& rasterized_points);

	void vertexShader(VertexData& v);
	Pipeline(int h, int w) :height(h), width(w) {}
	void setViewMatrix(const glm::mat4& view_M) {
		viewMatrix = view_M;
	}
	void setModelMatrix(const glm::mat4& model_M) {
		modelMatrix = model_M;
	}
	void setProjectMatrix(const glm::mat4& project_M, float near, float far) {
		projectMatrix = project_M;
		frustum_near_far = glm::vec2(near, far);
	}
	void setDrawableMeshes(std::vector<DrawableMesh::ptr>& _drawableMeshes) {
		drawableMeshes = _drawableMeshes;
	}
	void setViewportMatrix(const glm::mat4& viewport_M) {
		viewportMatrix = viewport_M;
	}
	//设置接收光线的位置(摄像机位置)
	void setViewerPos(const glm::vec3& camera_pos) {
		viewer_pos = camera_pos;
	}

	//设置点光源
	void setPointLight(const glm::vec3& lightPos, const glm::vec3& attenuation, const glm::vec3& lightColor) {
		PointLight point_light;
		point_light.lightPos = lightPos;
		point_light.attenuation = attenuation;
		point_light.lightColor = lightColor;
		pointlights.push_back(point_light);
	}


	glm::mat4 getViewportMatrix() {
		return viewportMatrix;
	}
	std::vector<DrawableMesh::ptr> getDrawableMeshes() {
		return drawableMeshes;
	}
	std::vector<VertexData> cliping(
		const VertexData& v0,
		const VertexData& v1,
		const VertexData& v2) const;
	void fragmentShader(const VertexData& data, glm::vec4& fragColor);
	void unloadDrawableMesh();
	bool isTowardBackFace(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2) const;

private:
	int height;
	int width;
	//Drawable mesh array
	std::vector<DrawableMesh::ptr> drawableMeshes;
	std::vector<PointLight> pointlights;
	//MVP transformation matrices
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 projectMatrix = glm::mat4(1.0f);
	
	glm::vec3 viewer_pos = glm::vec4(0, 0, 0, 0);
	//Near plane & far plane
	glm::vec2 frustum_near_far;

	//Viewport transformation (ndc space -> screen space)
	glm::mat4 viewportMatrix = glm::mat4(1.0f);
};


#endif

