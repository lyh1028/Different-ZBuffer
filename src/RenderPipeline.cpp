#include"RenderPipeline.h"
#include <glm/ext/matrix_transform.hpp>


glm::mat4 Pipeline::calcViewPortMatrix(int width, int height)
{
	//Setup viewport matrix (ndc space -> screen space)
	glm::mat4 vpMat;
	
	float hwidth = width * 0.5f;
	float hheight = height * 0.5f;
	vpMat[0][0] = hwidth; vpMat[0][1] = 0.0f;    vpMat[0][2] = 0.0f; vpMat[0][3] = 0.0f;
	vpMat[1][0] = 0.0f;	  vpMat[1][1] = -hheight; vpMat[1][2] = 0.0f; vpMat[1][3] = 0.0f;
	vpMat[2][0] = 0.0f;   vpMat[2][1] = 0.0f;    vpMat[2][2] = 1.0f; vpMat[2][3] = 0.0f;
	vpMat[3][0] = hwidth; vpMat[3][1] = hheight; vpMat[3][2] = 0.0f; vpMat[3][3] = 1.0f;
	
	return vpMat;
}

glm::mat4 Pipeline::calcViewMatrix(glm::vec3 camera, glm::vec3 target, glm::vec3 worldUp)
{
	//Setup view matrix (world space -> camera space)
	return glm::lookAt(camera, target, worldUp);
	glm::mat4 vMat;
	glm::vec3 zAxis = glm::normalize(camera - target);
	glm::vec3 xAxis = glm::normalize(glm::cross(worldUp, zAxis));
	glm::vec3 yAxis = glm::normalize(glm::cross(zAxis, xAxis));

	vMat[0][0] = xAxis.x; vMat[0][1] = yAxis.x; vMat[0][2] = zAxis.x; vMat[0][3] = 0.0f;
	vMat[1][0] = xAxis.y; vMat[1][1] = yAxis.y; vMat[1][2] = zAxis.y; vMat[1][3] = 0.0f;
	vMat[2][0] = xAxis.z; vMat[2][1] = yAxis.z; vMat[2][2] = zAxis.z; vMat[2][3] = 0.0f;
	vMat[3][0] = -glm::dot(xAxis, camera);
	vMat[3][1] = -glm::dot(yAxis, camera);
	vMat[3][2] = -glm::dot(zAxis, camera);
	vMat[3][3] = 1.0f;

	return vMat;
}

glm::mat4 Pipeline::calcPerspProjectMatrix(float fovy, float aspect, float near, float far)
{
	//Setup perspective matrix (camera space -> homogeneous space)
	//return glm::perspective(fovy, aspect, near, far);
	glm::mat4 pMat = glm::mat4(1.0f);

	float rFovy = fovy * 3.14159265358979323846 / 180;
	const float tanHalfFovy = std::tan(rFovy * 0.5f);
	float f_n = far - near;
	pMat[0][0] = 1.0f / (aspect * tanHalfFovy); pMat[0][1] = 0.0f;				pMat[0][2] = 0.0f;					pMat[0][3] = 0.0f;
	pMat[1][0] = 0.0f;						pMat[1][1] = 1.0f / tanHalfFovy;  pMat[1][2] = 0.0f;					pMat[1][3] = 0.0f;
	pMat[2][0] = 0.0f;						pMat[2][1] = 0.0f;			    pMat[2][2] = -(far + near) / f_n;		pMat[2][3] = -1.0f;
	pMat[3][0] = 0.0f;						pMat[3][1] = 0.0f;				pMat[3][2] = -2.0f * near * far / f_n;	pMat[3][3] = 0.0f;

	return pMat;
}

VertexData Pipeline::lerp(
	const VertexData& v0,
	const VertexData& v1,
	float frac)
{
	//Linear interpolation
	VertexData result;
	result.pos = (1.0f - frac) * v0.pos + frac * v1.pos;
	result.col = (1.0f - frac) * v0.col + frac * v1.col;
	result.nor = (1.0f - frac) * v0.nor + frac * v1.nor;
	result.cpos = (1.0f - frac) * v0.cpos + frac * v1.cpos;
	result.spos.x = (1.0f - frac) * v0.spos.x + frac * v1.spos.x;
	result.spos.y = (1.0f - frac) * v0.spos.y + frac * v1.spos.y;

	return result;
}

VertexData Pipeline::barycentricLerp(
	const VertexData& v0,
	const VertexData& v1,
	const VertexData& v2,
	glm::vec3 w)
{
	VertexData result;
	result.pos = w.x * v0.pos + w.y * v1.pos + w.z * v2.pos;
	result.col = w.x * v0.col + w.y * v1.col + w.z * v2.col;
	result.nor = w.x * v0.nor + w.y * v1.nor + w.z * v2.nor;
	result.cpos = w.x * v0.cpos + w.y * v1.cpos + w.z * v2.cpos;
	result.spos.x = w.x * v0.spos.x + w.y * v1.spos.x + w.z * v2.spos.x + 0.5f;
	result.spos.y = w.x * v0.spos.y + w.y * v1.spos.y + w.z * v2.spos.y + 0.5f;

	return result;
}

void Pipeline::prePerspCorrection(VertexData& v)
{
	//Perspective correction: the world space properties should be multipy by 1/w before rasterization
	//https://zhuanlan.zhihu.com/p/144331875
	//We use pos.w to store 1/w
	v.pos.w = 1.0f / v.cpos.w;
	v.pos = glm::vec4(v.pos.x * v.pos.w, v.pos.y * v.pos.w, v.pos.z * v.pos.w, v.pos.w);
	v.nor = v.nor * v.pos.w;
	v.col = v.col * v.pos.w;
}


void Pipeline::aftPrespCorrection(VertexData& v)
{
	//Perspective correction: the world space properties should be multipy by w after rasterization
	//https://zhuanlan.zhihu.com/p/144331875
	//We use pos.w to store 1/w
	float w = 1.0f / v.pos.w;
	v.pos = v.pos * w;
	v.nor = v.nor * w;
	v.col = v.col * w;
}

void Pipeline::vertexShader(VertexData& v)
{
	v.pos = modelMatrix * glm::vec4(v.pos.x, v.pos.y, v.pos.z, 1.0f);
	v.cpos = projectMatrix * viewMatrix * v.pos;
	//v.spos = viewportMatrix * v.cpos;
}

void Pipeline::fragmentShader(const VertexData& data, glm::vec4& fragColor)
{
	//只有漫反射+环境光
	fragColor = glm::vec4(1.0, 1.0, 1.0, 1.0f);
	glm::vec3 fragPos = glm::vec3(data.pos);
	glm::vec3 normal = glm::normalize(data.nor);
	glm::vec3 tmp = glm::vec3();
	for (auto& point_light : pointlights) {
		glm::vec3 lightDir = glm::normalize(point_light.lightPos - fragPos); //入射方向

		glm::vec3 diffuse; //diffuse: Ld = kd(I/r^2)max(0,n dot l)
		float kd = 1.0f;
		float attenuation = 1.0f;
		float dist = glm::length(fragPos - point_light.lightPos);
		diffuse.r = kd * (point_light.lightColor.r / (dist * dist)) * std::max(0.0f, glm::dot(lightDir, normal));
		diffuse.g = kd * (point_light.lightColor.g / (dist * dist)) * std::max(0.0f, glm::dot(lightDir, normal));
		diffuse.b = kd * (point_light.lightColor.b / (dist * dist)) * std::max(0.0f, glm::dot(lightDir, normal));
		attenuation = (1.0f / (point_light.attenuation.x + point_light.attenuation.y * dist + point_light.attenuation.z * dist * dist));
		tmp += (diffuse + 0.5f) * attenuation; //diffuse + 环境光 * 衰减
	}
	fragColor = fragColor * glm::vec4(tmp, 1.0f);
	
}

//绘制三角形
void Pipeline::rasterize_fill_edge_function(
	const VertexData& v0,
	const VertexData& v1,
	const VertexData& v2,
	const unsigned int& screen_width,
	const unsigned int& screen_height,
	std::vector<VertexData>& rasterized_points)
{
	//Edge-function rasterization algorithm
	//  use VertexData::barycentricLerp(v0, v1, v2, w) for interpolation, 
	//       Interpolated points are discarded if they are outside the window. 
	//       v0.spos, v1.spos and v2.spos are the screen space vertices.
	//对于v0,v1,v2,其bounding box左上角的点是(min(v.x),max(v.y)),右下角的点是max(v.x),min(v.y)
	int x0 = v0.spos.x, x1 = v1.spos.x, x2 = v2.spos.x;
	int y0 = v0.spos.y, y1 = v1.spos.y, y2 = v2.spos.y;

	int rdx = (x0 > x1) ? x0 : x1;
	rdx = x2 > rdx ? x2 : rdx;
	int lpy = (y0 > y1) ? y0 : y1;
	lpy = y2 > lpy ? y2 : lpy;
	int lpx = (x0 < x1) ? x0 : x1;
	lpx = x2 < lpx ? x2 : lpx;
	int rdy = (y0 < y1) ? y0 : y1;
	rdy = y2 < rdy ? y2 : rdy;
	for (int i = lpx; i <= rdx; i++) {
		for (int j = rdy; j <= lpy; j++) {
			glm::vec3 t(i, j, 0);
			glm::vec3 r1 = glm::cross(glm::vec3(t.x - x0, t.y - y0, 0), glm::vec3(v1.spos - v0.spos, 0));
			bool f1 = r1.z >= 0;
			glm::vec3 r2 = glm::cross(glm::vec3(t.x - x1, t.y - y1, 0), glm::vec3(v2.spos - v1.spos, 0));
			bool f2 = r2.z >= 0;
			glm::vec3 r3 = glm::cross(glm::vec3(t.x - x2, t.y - y2, 0), glm::vec3(v0.spos - v2.spos, 0));
			bool f3 = r3.z >= 0;
			if (f1 == f2 && f2 == f3)
			{
				glm::fvec3 sx(0, 0, 0);
				glm::fvec3 sy(0, 0, 0);
				sx.y = x2 - x0;
				sx.x = x1 - x0;
				sx.z = x0 - t.x;
				sy.y = y2 - y0;
				sy.x = y1 - y0;
				sy.z = y0 - t.y;
				glm::vec3 u = glm::cross(sx, sy);
				if (std::abs(u.z) > 1e-2)
				{
					glm::vec3 w(1.0f - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z);
					auto mid = Pipeline::barycentricLerp(v0, v1, v2, w);
					//mid.spos = glm::vec4(i, j, 0.0f, 1.0f);
					if (mid.spos.x >= 0 && mid.spos.x < screen_width && mid.spos.y >= 0 && mid.spos.y < screen_height)
					{
						rasterized_points.push_back(mid);
					}
				}
				/*else {
					glm::vec3 w(-1.0f, 1.0f, 1.0f);
					auto mid = VertexData::barycentricLerp(v0, v1, v2, w);barycentricLerp
					//mid.spos = glm::vec4(i, j, 0.0f, 1.0f);
					if (mid.spos.x >= 0 && mid.spos.x < screen_width && mid.spos.y >= 0 && mid.spos.y < screen_height)
					{
						rasterized_points.push_back(mid);
					}
				}*/
			}

		}
	}
}
//裁剪不在屏幕中的点:
std::vector<VertexData> Pipeline::cliping(
	const VertexData& v0,
	const VertexData& v1,
	const VertexData& v2) const {
	//Clipping in the homogeneous clipping space

		//三个点都不在屏幕内才 return {v0, v1, v2}. Otherwise, return {}
	if (v0.cpos.x > v0.cpos.w && v1.cpos.x > v1.cpos.w && v2.cpos.x > v2.cpos.w) return {};
	else if (v0.cpos.w < frustum_near_far.x && v1.cpos.w < frustum_near_far.x && v2.cpos.w < frustum_near_far.x) return {};
	else if (v0.cpos.w > frustum_near_far.y && v1.cpos.w > frustum_near_far.y && v2.cpos.w > frustum_near_far.y) return {};
	else if (v0.cpos.x < -v0.cpos.w && v1.cpos.x < -v1.cpos.w && v2.cpos.x < -v2.cpos.w) return{};
	else if (v0.cpos.y > v0.cpos.w && v1.cpos.y > v1.cpos.w && v2.cpos.y > v2.cpos.w) return {};
	else if (v0.cpos.y < -v0.cpos.w && v1.cpos.y < -v1.cpos.w && v2.cpos.y < -v2.cpos.w) return {};
	else if (v0.cpos.z > v0.cpos.w && v1.cpos.z > v1.cpos.w && v2.cpos.z > v2.cpos.w) return {};
	else if (v0.cpos.z < -v0.cpos.w && v1.cpos.z < -v1.cpos.w && v2.cpos.z < -v2.cpos.w) return {};
	//       Please Use v0.cpos, v1.cpos, v2.cpos
	//       m_frustum_near_far.x -> near plane
	//       m_frustum_near_far.y -> far plane

	return { v0, v1, v2 };
}
//背面剔除
bool Pipeline::isTowardBackFace(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2) const
{
	//Back face culling in the ndc space

	//逆时针标点，tmp1叉乘tmp2方向向外
	glm::vec3 tmp1 = glm::vec3(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
	glm::vec3 tmp2 = glm::vec3(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
	glm::vec3 normal = glm::normalize(glm::cross(tmp1, tmp2));
	glm::vec3 view = glm::vec3(0, 0, -1);

	return glm::dot(normal, view) > 0;
}

void Pipeline::unloadDrawableMesh()
{
	for (size_t i = 0; i < drawableMeshes.size(); ++i)
	{
		drawableMeshes[i]->unload();
	}
	std::vector<DrawableMesh::ptr>().swap(drawableMeshes);
}