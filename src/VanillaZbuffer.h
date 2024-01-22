#ifndef VANILLAZBUFFER_H
#define VANILLAZBUFFER_H

#include "RenderPipeline.h"
#include "FrameBuffer.h"
#include"Zbuffer.h"
class VanillaZbuffer :public Zbuffer
{
public:
	typedef std::shared_ptr<VanillaZbuffer> ptr;

	VanillaZbuffer(int _height, int _width):height(_height), width(_width) {
		depthBuffer.resize(_width * _height, 1.0f);
	}
	~VanillaZbuffer() = default;

	void clear() override;

	double readDepth(const unsigned int& x, const unsigned int& y) const;
	void writeDepth(const unsigned int& x, const unsigned int& y, const double& value);
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

#endif // !ZBUFFER_H

