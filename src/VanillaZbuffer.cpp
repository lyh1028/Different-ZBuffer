#include"VanillaZbuffer.h"
#include <glm/ext/matrix_transform.hpp>

void VanillaZbuffer::clear() {
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			depthBuffer[row * width + col] = 1.0f;
		}
	}
}

void VanillaZbuffer::preparePipeline(Pipeline::ptr _pipeline, frameBuffer::ptr p_back, frameBuffer::ptr p_front) {
	pipeline = _pipeline;
	p_backBuffer = p_back;
	p_frontBuffer = p_front;
}

double VanillaZbuffer::readDepth(const unsigned int& x, const unsigned int& y) const
{
	if (x < 0 || x >= width || y < 0 || y >= height)
		return 0.0f;
	return depthBuffer[y * width + x];
}
void VanillaZbuffer::writeDepth(const unsigned int& x, const unsigned int& y, const double& value)
{
	if (x < 0 || x >= width || y < 0 || y >= height)
		return;
	unsigned int index = y * width + x;
	depthBuffer[index] = value;
}


void VanillaZbuffer::render() 
{
	std::vector<VertexData> rasterized_points;
	rasterized_points.reserve(p_backBuffer->getWidth() * p_backBuffer->getHeight());
	auto m_drawableMeshes = pipeline->getDrawableMeshes();
	auto viewportMatrix = pipeline->getViewportMatrix();

	for (size_t m = 0; m < m_drawableMeshes.size(); ++m)
	{
		const auto& vertices = m_drawableMeshes[m]->getVerticesAttrib();
		const auto& faces = m_drawableMeshes[m]->getMeshFaces();
		for (size_t f = 0; f < faces.size(); ++f)
		{
			//A triangle as primitive
			VertexData v[3];
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
				if (pipeline->isTowardBackFace(v[0].cpos, v[1].cpos, v[2].cpos)) {
					continue;
				}
				//Perspective correction before rasterization
				Pipeline::prePerspCorrection(v[0]);
				Pipeline::prePerspCorrection(v[1]);
				Pipeline::prePerspCorrection(v[2]);
				//From clip space -> ndc space
				v[0].cpos /= v[0].cpos.w;
				v[1].cpos /= v[1].cpos.w;
				v[2].cpos /= v[2].cpos.w;
			}


			//Rasterization stage
			{
				//Transform to screen space & Rasterization
				{
					v[0].spos = glm::ivec2(viewportMatrix * v[0].cpos + glm::vec4(0.5f));
					v[1].spos = glm::ivec2(viewportMatrix * v[1].cpos + glm::vec4(0.5f));
					v[2].spos = glm::ivec2(viewportMatrix * v[2].cpos + glm::vec4(0.5f));
					pipeline->rasterize_fill_edge_function(v[0], v[1], v[2],
						p_backBuffer->getWidth(), p_backBuffer->getHeight(), rasterized_points);
				}
			}

			//Fragment shader & Depth testing
			{
				for (auto& points : rasterized_points)
				{
					if (readDepth(points.spos.x, points.spos.y) > points.cpos.z)
					{
						//Perspective correction after rasterization
						Pipeline::aftPrespCorrection(points);
						glm::vec4 fragColor = glm::vec4();
						pipeline->fragmentShader(points, fragColor);
						p_backBuffer->renderPixel(points.spos.x, points.spos.y, fragColor);
						writeDepth(points.spos.x, points.spos.y, points.cpos.z);
					}
				}
				rasterized_points.clear();
			}
			
		}
	}
	std::swap(p_backBuffer, p_frontBuffer);
}

void VanillaZbuffer::drawWindows(TRWindowsApp::ptr winApp, glm::vec3 cameraPos, glm::vec3 lookAtTarget) {

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