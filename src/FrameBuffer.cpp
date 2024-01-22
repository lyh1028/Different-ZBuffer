
#include "FrameBuffer.h"
#include <iostream>
void frameBuffer::clear(const glm::vec4& color)
{
	unsigned char red = static_cast<unsigned char>(255 * color.x);
	unsigned char green = static_cast<unsigned char>(255 * color.y);
	unsigned char blue = static_cast<unsigned char>(255 * color.z);
	unsigned char alpha = static_cast<unsigned char>(255 * color.w);

	for (unsigned int row = 0; row < height; ++row)
	{
		for (unsigned int col = 0; col < width; ++col)
		{
			frame_buffer[row * width * channel + col * channel + 0] = red;
			frame_buffer[row * width * channel + col * channel + 1] = green;
			frame_buffer[row * width * channel + col * channel + 2] = blue;
			frame_buffer[row * width * channel + col * channel + 3] = alpha;
		}
	}
}
float truncate(float value) {
	if (value < 0.0f) {
		return 0.0f;
	}
	else if (value > 255.0f) {
		return 255.0f;
	}
	else {
		return value;
	}
}
void frameBuffer::renderPixel(const unsigned int& x, const unsigned int& y, const glm::vec4& color)
{
	if (x < 0 || x >= width || y < 0 || y >= height)
		return;
	unsigned char red = static_cast<unsigned char>(truncate(255 * color.x));
	unsigned char green = static_cast<unsigned char>(truncate(255 * color.y));
	unsigned char blue = static_cast<unsigned char>(truncate(255 * color.z));
	unsigned char alpha = static_cast<unsigned char>(truncate(255 * color.w));
	unsigned int index = y * width * channel + x * channel;
	//std::cout << x << ' ' << y << '\n';
	frame_buffer[index + 0] = red;
	frame_buffer[index + 1] = green;
	frame_buffer[index + 2] = blue;
	frame_buffer[index + 3] = alpha;
}