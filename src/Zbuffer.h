#ifndef ZBUFFER_H
#define ZBUFFER_H

#include"RenderPipeline.h"
#include "FrameBuffer.h"
#include "TRWindowsApp.h"
//Zbuffer的基类
class Zbuffer
{
public:
	typedef std::shared_ptr<Zbuffer> ptr;

	Zbuffer(int _height, int _width) :height(_height), width(_width) {
		depthBuffer.resize(_width * _height, 1.0f);
	}
	virtual ~Zbuffer() = default;
	Zbuffer() = default;
	virtual void clear() = 0;

	int getWidth()const { return width; }
	int getHeight()const { return height; }
	Pipeline::ptr pipeline;
	frameBuffer::ptr p_frontBuffer; //当前显示的帧缓存
	frameBuffer::ptr p_backBuffer;  //当前正在写入的帧缓存
	std::vector<double> depthBuffer;          // Z-buffer
	virtual void preparePipeline(Pipeline::ptr _pipeline, frameBuffer::ptr p_back, frameBuffer::ptr p_front) = 0;
	virtual void render() = 0;
	virtual void drawWindows(TRWindowsApp::ptr winApp, glm::vec3 cameraPos, glm::vec3 lookAtTarget) = 0;
private:
	int height, width;
};


#endif // !ZBUFFER_H



