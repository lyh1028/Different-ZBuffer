#include"ScanlineZbuffer.h"
#include <algorithm>
#include <map>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
void ScanlineZbuffer::clear() {
	for (int col = 0; col < width; col++) {
		depthBuffer[col] = 1.0f;
	}
}

void ScanlineZbuffer::preparePipeline(Pipeline::ptr _pipeline, frameBuffer::ptr p_back, frameBuffer::ptr p_front) {
	pipeline = _pipeline;
	p_backBuffer = p_back;
	p_frontBuffer = p_front;
}

double ScanlineZbuffer::readDepth(const unsigned int& x) const
{
	if (x < 0 || x >= width )
		return 0.0f;
	return depthBuffer[x];
}
void ScanlineZbuffer::writeDepth(const unsigned int& x, const double& value)
{
	if (x < 0 || x >= width )
		return;
	unsigned int index =  x;
	depthBuffer[index] = value;
}
glm::vec4 calcPolygenCoeff(const std::vector<VertexData>& vert) {//计算平面方程
	glm::vec3 A = glm::vec3(vert[0].cpos.x, vert[0].cpos.y, vert[0].cpos.z);
	glm::vec3 B = glm::vec3(vert[1].cpos.x, vert[1].cpos.y, vert[1].cpos.z);
	glm::vec3 C = glm::vec3(vert[2].cpos.x, vert[2].cpos.y, vert[2].cpos.z);
	glm::vec3 normal = glm::cross(B - A, C - A);

	// 归一化法向量
	normal = glm::normalize(normal);

	// 计算常数项d
	float d = -glm::dot(normal, A);
	return glm::vec4(normal, d);
}

void AddActive(int width, int height, int scanlineY, std::unordered_map<int, std::list<Edge> >& class_edge_table, std::unordered_map<int, std::list<Polygen> >& class_poly_table, std::vector<ActiveEdgePair>& active_edge_pairs, std::vector<Polygen>& active_poly_table) {
	//check if scanlineY has relative polygen
	if (class_poly_table.find(scanlineY) != class_poly_table.end()) {
		for (auto iter = class_poly_table[scanlineY].begin(); iter != class_poly_table[scanlineY].end();) {
			auto poly = *iter;
			active_poly_table.push_back(poly);
			//实际上不需要活化多边形，并根据多边形构造ActiveEdgePair
			std::vector<Edge> relative_edges;
			for (auto it = class_edge_table[scanlineY].begin(); it != class_edge_table[scanlineY].end();) {
				if (it->id != poly.id) {
					it++;
					continue;
				}
				else {
					relative_edges.push_back(*it);
					it = class_edge_table[scanlineY].erase(it);
				}
			}
			if (relative_edges.size() % 2 != 0) { std::cout << "直线与多边形交点个数不为偶数！\n"; }
			std::sort(relative_edges.begin(), relative_edges.end());
			ActiveEdgePair active_edge_pair;
			active_edge_pair.id = poly.id;
			if (abs(poly.c)>1e-7) { 
				active_edge_pair.dzx = -2.0/width * (poly.a / poly.c);
				active_edge_pair.dzy = -2.0/height * (poly.b / poly.c);//见实验报告中的推导
			}
			else { //如果c=0，深度不变，应当取NDC空间内平面上任意一点的cpos.z作为zl
				active_edge_pair.dzx = 0;
				active_edge_pair.dzy = 0;
			}
			active_edge_pair.color = poly.color;
			for (auto it = relative_edges.begin(); it != relative_edges.end(); it++) {
				
				auto zl = -(poly.d + poly.a * it->cpos.x + poly.b * it->cpos.y) / poly.c;
				active_edge_pair.xl = it->x;
				active_edge_pair.dxl = it->dx;
				active_edge_pair.dyl = it->dy;
				if (poly.c == 0) {
					active_edge_pair.zl = it->cpos.z;
				}
				else active_edge_pair.zl = -(poly.d + poly.a * it->cpos.x + poly.b * it->cpos.y) / poly.c;
				//printf_s("活化边顶点%d,%d的深度：%f\n", int(active_edge_pair.xl), scanlineY, active_edge_pair.zl);
				it++;
				active_edge_pair.xr = it->x;
				active_edge_pair.dxr = it->dx;
				active_edge_pair.dyr = it->dy;
				active_edge_pairs.push_back(active_edge_pair);
			}
			//iter = class_poly_table[scanlineY].erase(iter);// 使用erase反而慢了
			iter++;
		}
	}
}

void ScanlineZbuffer::render()
{
	std::vector<VertexData> rasterized_points;
	rasterized_points.reserve(p_backBuffer->getWidth() * p_backBuffer->getHeight());
	auto m_drawableMeshes = pipeline->getDrawableMeshes();
	auto viewportMatrix = pipeline->getViewportMatrix();
	std::unordered_map<int, std::list<Polygen> > class_poly_table;//分类多边形表
	std::vector<Polygen> active_poly_table;
	std::unordered_map<int, std::list<Edge> > class_edge_table;//分类边表
	std::vector<ActiveEdgePair> active_edge_pairs; //活化边对
	int scanline_start_Y = 0, scanline_end_Y = height-1;
	for (size_t m = 0; m < m_drawableMeshes.size(); ++m)
	{
		const auto& vertices = m_drawableMeshes[m]->getVerticesAttrib();
		const auto& faces = m_drawableMeshes[m]->getMeshFaces();
		for (size_t f = 0; f < faces.size(); ++f)
		{
			//A triangle as primitive
			std::vector<VertexData> v(3);
			{
				v[0].pos = vertices.vpositions[faces[f].vposIndex[0]];
				v[0].col = glm::vec4(vertices.vcolors[faces[f].vposIndex[0]]);
				v[0].nor = vertices.vnormals[faces[f].vnorIndex[0]];

				v[1].pos = vertices.vpositions[faces[f].vposIndex[1]];
				v[1].col = glm::vec4(vertices.vcolors[faces[f].vposIndex[1]]);
				v[1].nor = vertices.vnormals[faces[f].vnorIndex[1]];

				v[2].pos = vertices.vpositions[faces[f].vposIndex[2]];
				v[2].col = glm::vec4(vertices.vcolors[faces[f].vposIndex[2]]);
				v[2].nor = vertices.vnormals[faces[f].vnorIndex[2]];
			}

			//Vertex shader stage
				//Vertex shader
			{
				pipeline->vertexShader(v[0]);
				pipeline->vertexShader(v[1]);
				pipeline->vertexShader(v[2]);
				auto clip_vert = pipeline->cliping(v[0], v[1], v[2]);
				if (clip_vert.size() < 3) {
					continue;
				}
				//Perspective correction before rasterization
				Pipeline::prePerspCorrection(v[0]);
				Pipeline::prePerspCorrection(v[1]);
				Pipeline::prePerspCorrection(v[2]);
				//From clip space -> ndc space (透视除法)
				v[0].cpos /= v[0].cpos.w;
				v[1].cpos /= v[1].cpos.w;
				v[2].cpos /= v[2].cpos.w;
			}
			//Transform to screen space
			{
				//auto test = viewportMatrix * v[0].cpos;
				v[0].spos = glm::ivec2(viewportMatrix * v[0].cpos + glm::vec4(0.5f));
				v[1].spos = glm::ivec2(viewportMatrix * v[1].cpos + glm::vec4(0.5f));
				v[2].spos = glm::ivec2(viewportMatrix * v[2].cpos + glm::vec4(0.5f));
				if (pipeline->isTowardBackFace(v[0].cpos, v[1].cpos, v[2].cpos)) {
					continue;
				}
				Pipeline::aftPrespCorrection(v[0]);
				Pipeline::aftPrespCorrection(v[1]);
				Pipeline::aftPrespCorrection(v[2]);
			}
			//1. 构建对应的多边形,并按照ymax加入对应的多边形表
			glm::vec4 fragColor = glm::vec4();
			VertexData center;
			glm::vec4 real_coeff = calcPolygenCoeff(v);
			center.nor = glm::vec3(real_coeff.x, real_coeff.y, real_coeff.z);
			center.pos = v[0].pos;
			pipeline->fragmentShader(center, fragColor); //计算多边形的颜色
			int id = f; 
			//构造分类边表
			std::pair<int,int> y_range = makeEdges(v, class_edge_table, id);
			int ymax = y_range.first, ymin = std::max(y_range.second,0); //ymax不取min的原因是我们需要从头开始更新多边形表

			//构造分类多边形表
			auto polygen = Polygen(id, fragColor);
		
			polygen.a = real_coeff.x; polygen.b = real_coeff.y; polygen.c = real_coeff.z;
			polygen.d = real_coeff.w; polygen.ymax = ymax;
			polygen.v = v;
			class_poly_table[polygen.ymax].push_back(polygen);

			scanline_start_Y = std::max(scanline_start_Y, polygen.ymax);
			scanline_end_Y = std::min(scanline_end_Y, ymin);
		}
		//2. 开扫! 由于SDL2库的窗口左上角是(0,0),从左下角开始,从下到上扫描
		for (int scanlineY = scanline_start_Y; scanlineY >= scanline_end_Y; scanlineY--) {
			depthBuffer.assign(width, 1.0f);//初始化深度缓冲
			//add
			AddActive(width, height, scanlineY, class_edge_table, class_poly_table, active_edge_pairs, active_poly_table);
			if (active_edge_pairs.empty()) continue;
			//draw
			for (auto& it = active_edge_pairs.begin(); it != active_edge_pairs.end();it++) {
				auto& edge_pair = *it;
				double zx = edge_pair.zl;
				int cur_x = round(edge_pair.xl);
				do {
					double buffer_z = this->readDepth(cur_x);
					if (buffer_z > zx) {
						p_backBuffer->renderPixel(cur_x, scanlineY, edge_pair.color);
						this->writeDepth(cur_x, zx);
					}
					zx = zx + edge_pair.dzx;
					cur_x++;
				} while (cur_x <= round(edge_pair.xr) && cur_x < width);
				//update
				edge_pair.dyl--; edge_pair.dyr--;
				edge_pair.xl = (edge_pair.xl + edge_pair.dxl);
				edge_pair.xr = (edge_pair.xr + edge_pair.dxr);
				edge_pair.zl += edge_pair.dzx * edge_pair.dxl + edge_pair.dzy;//zl = (-d/c - ax/c - by/c)
				if (edge_pair.dyl < 0 || edge_pair.dyr < 0) {
					if (scanlineY == 0) continue;
					if (edge_pair.dyl < 0 && edge_pair.dyr < 0) {
						continue;
					}
					else {
						for (auto iter = class_edge_table[scanlineY - 1].begin(); iter != class_edge_table[scanlineY - 1].end(); ) {
							if (iter->id == edge_pair.id) {
								if (edge_pair.dyl < 0) { //L换成下一条边
									edge_pair.xl = (*iter).x;
									edge_pair.dxl = (*iter).dx;
									edge_pair.dyl = (*iter).dy;
								}
								else if (edge_pair.dyr < 0) { //R换成下一条边
									edge_pair.xr = (*iter).x;
									edge_pair.dxr = (*iter).dx;
									edge_pair.dyr = (*iter).dy;
								}
								iter = class_edge_table[scanlineY - 1].erase(iter);
							}
							else ++iter;
						}
					}
				}

			}
			//delete
			// 移除 dy < 0 的活化边
			int last = 0;
			int actEdgeSize = active_edge_pairs.size();
			for (int i = 0; i < actEdgeSize; ++i, ++last) {
				while (active_edge_pairs[i].dyl < 0 || active_edge_pairs[i].dyr < 0) {
					++i;
					if (i >= actEdgeSize)break;
				}
				if (i >= actEdgeSize)break;
				active_edge_pairs[last] = active_edge_pairs[i];
			}
			active_edge_pairs.resize(last);
		}
	}
	//std::swap(p_backBuffer, p_frontBuffer);
	
	int a = 1;
}

void ScanlineZbuffer::drawWindows(TRWindowsApp::ptr winApp, glm::vec3 cameraPos, glm::vec3 lookAtTarget) {
	
	winApp->readyToStart();
	//Rendering loop
	while (!winApp->shouldWindowClose())
	{
		//Process event
		winApp->processEvent();

		//Clear frame buffer (both color buffer and depth buffer)
		p_backBuffer->clear(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		clear();
		//Draw call 绘制backbuffer，并把backbuffer换到frontbuffer
		render();

		//Display to screen
		double deltaTime = winApp->updateScreenSurface(
			p_frontBuffer->getFrameBuffer(),
			width,
			height,
			4);
		//Camera operation
		{
			//Camera rotation
			if (winApp->getIsMouseLeftButtonPressed())
			{
				int deltaX = winApp->getMouseMotionDeltaX();
				glm::mat4 cameraRotMat = glm::rotate(glm::mat4(1.0f), -deltaX * 0.001f, glm::vec3(0, 1, 0));
				cameraPos = glm::vec3(cameraRotMat * glm::vec4(cameraPos, 1.0f));
				pipeline->setViewerPos(cameraPos);
				pipeline->setViewMatrix(Pipeline::calcViewMatrix(cameraPos, lookAtTarget, glm::vec3(0.0, 1.0, 0.0f)));
			}

			//Camera zoom in and zoom out
			if (winApp->getMouseWheelDelta() != 0)
			{
				glm::vec3 dir = glm::normalize(cameraPos - lookAtTarget);
				float dist = glm::length(cameraPos - lookAtTarget);
				glm::vec3 newPos = cameraPos + (winApp->getMouseWheelDelta() * 0.1f) * dir;
				if (glm::length(newPos - lookAtTarget) > 1.0f)
				{
					cameraPos = newPos;
					pipeline->setViewMatrix(Pipeline::calcViewMatrix(cameraPos, lookAtTarget, glm::vec3(0.0, 1.0, 0.0f)));
				}
			}
		}
	}
	pipeline->unloadDrawableMesh();
}