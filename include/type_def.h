#ifndef _TYPE_DEF_H_
#define _TYPE_DEF_H_

#include <cstdint>
	struct u32_point{
		std::uint32_t x;
		std::uint32_t y;
	};
	struct l32_point{
		std::int_least32_t x;
		std::int_least32_t y;
	};

	struct u32_size{
		std::uint32_t w;
		std::uint32_t h;
	};
	struct u32_rect{
		u32_point start;
		u32_size size;
	};
	enum ImageFmt{
		YUV420,
		YUV422,
		RGB888,
		GRAY,
	};
	struct ImgInfor{
		std::uint8_t *data_buf;
		std::uint32_t width;
		std::uint32_t height;
		std::uint32_t width_step;
		ImageFmt image_fmt;
	};
#endif