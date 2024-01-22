#ifndef SCANLINEZBUFFER_H
#define SCANLINEZBUFFER_H
#include"DrawableMesh.h"
#include"Polygen.h"
#include "RenderPipeline.h"
#include "FrameBuffer.h"
#include"Zbuffer.h"
#include<map>
#include<List>

class ScanlineZbuffer :public Zbuffer
{
public:
	typedef std::shared_ptr<ScanlineZbuffer> ptr;

	ScanlineZbuffer(int _height, int _width) :height(_height), width(_width) {
		depthBuffer.resize(_width, 1.0f);
	}
	~ScanlineZbuffer() = default;

	void clear() override;

	int getWidth()const { return width; }
	int getHeight()const { return height; }
	double readDepth(const unsigned int& x) const;
	void writeDepth(const unsigned int& x, const double& value);
	Pipeline::ptr pipeline;
	frameBuffer::ptr p_frontBuffer; //当前显示的帧缓存
	frameBuffer::ptr p_backBuffer;  //当前正在写入的帧缓存
	std::vector<double> depthBuffer;          // Z-buffer
	void preparePipeline(Pipeline::ptr _pipeline, frameBuffer::ptr p_back, frameBuffer::ptr p_front) override;
	void render() override;
	void drawWindows(TRWindowsApp::ptr winApp, glm::vec3 cameraPos, glm::vec3 lookAtTarget) override;
private:
	int height, width;
};
void AddActive(int width, int height, int scanlineY, std::unordered_map<int, std::list<Edge> >& class_edge_table, std::unordered_map<int, std::list<Polygen> >& class_poly_table, std::vector<ActiveEdgePair>& active_edge_pairs);
#endif // !SCANLINEZBUFFER_H

