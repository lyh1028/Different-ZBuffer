
#include <iostream>
#include <vector>
#include <string>
#include "RenderPipeline.h"
#include "TRWindowsApp.h"
#include "DrawableMesh.h"
#include"VanillaZbuffer.h"
#include "ScanlineZbuffer.h"
#include "Polygen.h"
#include <chrono>
#include "HierarchZbuffer.h"
#include"OctreeHZbuffer.h"
#undef main
int main() {
	constexpr int width = 666;
	constexpr int height = 500;
	std::cout << "---------请选择你要绘制的obj文件,1: cube.obj, 2: monster_5k.obj, 3:bunny_10k, 4:armadillo_20k---------------" << std::endl;
	int obj_choice = 1;
	std::cin >> obj_choice;
	std::vector<std::string> files;
	switch (obj_choice)
	{
	case 1:
		files.push_back("model/cube.obj");
		break;
	case 2:
		files.push_back("model/diablo3_pose.obj");
		break;
	case 3:
		files.push_back("model/bunny_10k.obj");
		break;
	case 4:
		files.push_back("model/armadillo_20k.obj");
		break;
	default:
		break;
	}
	std::vector<DrawableMesh::ptr> dm_vec;
	for (auto& filename : files) {
		DrawableMesh::ptr dm = std::make_shared<DrawableMesh>();
		dm->loadMeshFromObjFile(filename);
		dm_vec.push_back(dm);
	}
	//frameBuffer
	frameBuffer::ptr p_backBuffer = std::make_shared<frameBuffer>(height, width);
	frameBuffer::ptr p_frontBuffer = std::make_shared<frameBuffer>(height, width);
	//camera
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.60811f);
	glm::vec3 lookAtTarget = glm::vec3(0.0f);

	glm::mat4 model_mat = glm::mat4(1.0f);
	Pipeline::ptr pipeline = std::make_shared<Pipeline>(height, width);
	
	pipeline->setViewMatrix(Pipeline::calcViewMatrix(cameraPos, lookAtTarget, glm::vec3(0.0, 1.0, 0.0f)));
	pipeline->setProjectMatrix(Pipeline::calcPerspProjectMatrix(45.0f, static_cast<float>(width) / height, 0.001f, 10.0f), 0.001f, 10.0f);
	pipeline->setDrawableMeshes(dm_vec);
	pipeline->setViewportMatrix(pipeline->calcViewPortMatrix(width, height));

	//light
	glm::vec3 whiteLightPos = glm::vec3(0.0f, 0.5f, 1.2f);
	glm::vec3 wLP1 = glm::vec3(1.0f, 0.5f, 0.2f);
	glm::vec3 blueLightPos = glm::vec3(-0.83f, -0.05f, -0.83f);
	glm::vec3 attenuation = glm::vec3(1.0, 0.7, 1.8);
	glm::vec3 light_color = glm::vec3(1.9f, 1.9f, 1.09f);
	pipeline->setPointLight(whiteLightPos, attenuation, light_color);
	pipeline->setPointLight(wLP1, attenuation, light_color);
	pipeline->setPointLight(blueLightPos, attenuation, glm::vec3(0.0f, 0.0f, 1.9f));
	int algorithm_choice = 0;
	
	std::cout << "--------请选择你要使用的算法,1: VanillaZbuffer, 2: scanlineZbuffer, 3:Hierarch Zbuffer, 4:Octree Hierarch Zbuffer-----------" << std::endl;
	std::cin >> algorithm_choice;
	switch (algorithm_choice)
	{
	case 1: {
		auto start = std::chrono::high_resolution_clock::now();
		VanillaZbuffer::ptr p_vz = std::make_shared<VanillaZbuffer>(height, width);
		p_vz->preparePipeline(pipeline, p_backBuffer, p_frontBuffer);
		p_vz->render();
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		// 输出单次render时间
		std::cout << "Time taken by function: " << duration.count() << " microseconds" << std::endl;
		TRWindowsApp::ptr winApp = TRWindowsApp::getInstance(width, height, "VanillaZbuffer Rasterization");
		if (winApp == nullptr)
		{
			return -1;
		}
		p_vz->drawWindows(winApp, cameraPos, lookAtTarget);
		break; }
	case 2: {
		auto start = std::chrono::high_resolution_clock::now();
		ScanlineZbuffer::ptr p_vz = std::make_shared<ScanlineZbuffer>(height, width);
		p_vz->preparePipeline(pipeline, p_backBuffer, p_frontBuffer);
		p_vz->p_backBuffer->clear(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		p_vz->clear();
		//Draw call 绘制backbuffer，并把backbuffer换到frontbuffer
		p_vz->render();
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		// 输出程序运行时间
		std::cout << "Time taken by function: " << duration.count() << " microseconds" << std::endl;
		TRWindowsApp::ptr winApp = TRWindowsApp::getInstance(width, height, "ScanlineZbuffer Rasterization");
		if (winApp == nullptr)
		{
			return -1;
		}
		std::swap(p_backBuffer, p_frontBuffer);
		while (!winApp->shouldWindowClose())
		{
			winApp->processEvent();
			double deltaTime = winApp->updateScreenSurface(
				p_frontBuffer->getFrameBuffer(),
				width,
				height,
				4);
		}
		//p_vz->drawWindows(winApp, cameraPos, lookAtTarget);
		break;
	}
	case 3: {
		auto start = std::chrono::high_resolution_clock::now();
		HierarchZbuffer::ptr p_vz = std::make_shared<HierarchZbuffer>(height, width);
		p_vz->preparePipeline(pipeline, p_backBuffer, p_frontBuffer);
		p_vz->p_backBuffer->clear(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		p_vz->clear();
		//auto start = std::chrono::high_resolution_clock::now();
		p_vz->render();
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		// 输出程序运行时间
		std::cout << "Time taken by function: " << duration.count() << " microseconds" << std::endl;
		TRWindowsApp::ptr winApp = TRWindowsApp::getInstance(width, height, "Hierarch Zbuffer Rasterization");
		if (winApp == nullptr)
		{
			return -1;
		}
		p_vz->drawWindows(winApp, cameraPos, lookAtTarget);
		break;
	}
	case 4: {
		OctreeHZBuffer::ptr p_vz = std::make_shared<OctreeHZBuffer>(height, width);
		p_vz->preparePipeline(pipeline, p_backBuffer, p_frontBuffer);
		p_vz->p_backBuffer->clear(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		p_vz->clear();
		auto start = std::chrono::high_resolution_clock::now();
		p_vz->render();
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		// 输出程序运行时间
		std::cout << "Time taken by function: " << duration.count() << " microseconds" << std::endl;
		TRWindowsApp::ptr winApp = TRWindowsApp::getInstance(width, height, "Octree Hierarch Zbuffer Rasterization");
		if (winApp == nullptr)
		{
			return -1;
		}
		p_vz->drawWindows(winApp, cameraPos, lookAtTarget);
		break;
	}
	default: {
		VanillaZbuffer::ptr p_vz = std::make_shared<VanillaZbuffer>(height, width);
		p_vz->preparePipeline(pipeline, p_backBuffer, p_frontBuffer);
		p_vz->render(); }
		break;
	}


	return 0;
}