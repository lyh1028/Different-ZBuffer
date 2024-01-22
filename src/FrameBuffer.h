#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include"glm/vec4.hpp"
#include<vector>
#include<memory>
class frameBuffer {
public:
	typedef std::shared_ptr<frameBuffer> ptr;
	int getWidth()const { return width; }
	int getHeight()const { return height; }
	frameBuffer(int _h, int _w) :height(_h), width(_w), channel(4){
		frame_buffer.resize(_h * _w * channel, 255);
	}
	unsigned char* getFrameBuffer() {
		return frame_buffer.data();
	}
	void clear(const glm::vec4& color);
	void renderPixel(const unsigned int& x, const unsigned int& y, const glm::vec4& color);
private:
	std::vector<unsigned char> frame_buffer;
	int height, width, channel;
};


#endif // ! FRAMEBUFFER_H