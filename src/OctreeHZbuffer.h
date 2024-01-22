#ifndef OCTREEHZBUFFER_H
#define OCTREEHZBUFFER_H
#include"Octree.h"
#include"QuadTree.h"
#include"Zbuffer.h"
#include"HzbPolygen.h"
class OctreeHZBuffer :public Zbuffer
{
public:
	typedef std::shared_ptr<OctreeHZBuffer> ptr;
	QuadTreeNode* z_pyramid;
	OctreeNode* scene_octree;
	OctreeHZBuffer(int _height, int _width) :height(_height), width(_width) {
		depthBuffer.resize(_width * _height, 1.0f);
		z_pyramid = z_pyramid->buildTree(width - 1, 0, height, 0);
	}
	~OctreeHZBuffer() = default;
	void renderPolygen(HzbPolygen& poly);
	void renderOctreeNode(OctreeNode* o);
	void clear() override;
	double readDepth(const unsigned int& x, const unsigned int& y) const;
	void writeDepth(const unsigned int& x, const unsigned int& y, const double& value);
	Pipeline::ptr pipeline;
	frameBuffer::ptr p_frontBuffer;
	frameBuffer::ptr p_backBuffer;
	std::vector<double> depthBuffer;          // Z-buffer
	void preparePipeline(Pipeline::ptr _pipeline, frameBuffer::ptr p_back, frameBuffer::ptr p_front) override;
	void render() override;
	void drawWindows(TRWindowsApp::ptr winApp, glm::vec3 cameraPos, glm::vec3 lookAtTarget) override;
private:
	int height, width;
};

#endif
