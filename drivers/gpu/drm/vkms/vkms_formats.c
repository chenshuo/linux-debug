// SPDX-License-Identifier: GPL-2.0+

#include <linux/kernel.h>
#include <linux/minmax.h>
#include <drm/drm_rect.h>
#include <drm/drm_fixed.h>

#include "vkms_formats.h"

static size_t pixel_offset(const struct vkms_frame_info *frame_info, int x, int y)
{
	return frame_info->offset + (y * frame_info->pitch)
				  + (x * frame_info->cpp);
}

/*
 * packed_pixels_addr - Get the pointer to pixel of a given pair of coordinates
 *
 * @frame_info: Buffer metadata
 * @x: The x(width) coordinate of the 2D buffer
 * @y: The y(Heigth) coordinate of the 2D buffer
 *
 * Takes the information stored in the frame_info, a pair of coordinates, and
 * returns the address of the first color channel.
 * This function assumes the channels are packed together, i.e. a color channel
 * comes immediately after another in the memory. And therefore, this function
 * doesn't work for YUV with chroma subsampling (e.g. YUV420 and NV21).
 */
static void *packed_pixels_addr(const struct vkms_frame_info *frame_info,
				int x, int y)
{
	size_t offset = pixel_offset(frame_info, x, y);

	return (u8 *)frame_info->map[0].vaddr + offset;
}

static void *get_packed_src_addr(const struct vkms_frame_info *frame_info, int y)
{
	int x_src = frame_info->src.x1 >> 16;
	int y_src = y - frame_info->dst.y1 + (frame_info->src.y1 >> 16);

	return packed_pixels_addr(frame_info, x_src, y_src);
}

static void ARGB8888_to_argb_u16(struct line_buffer *stage_buffer,
				 const struct vkms_frame_info *frame_info, int y)
{
	struct pixel_argb_u16 *out_pixels = stage_buffer->pixels;
	u8 *src_pixels = get_packed_src_addr(frame_info, y);
	int x_limit = min_t(size_t, drm_rect_width(&frame_info->dst),
			    stage_buffer->n_pixels);

	for (size_t x = 0; x < x_limit; x++, src_pixels += 4) {
		/*
		 * The 257 is the "conversion ratio". This number is obtained by the
		 * (2^16 - 1) / (2^8 - 1) division. Which, in this case, tries to get
		 * the best color value in a pixel format with more possibilities.
		 * A similar idea applies to others RGB color conversions.
		 */
		out_pixels[x].a = (u16)src_pixels[3] * 257;
		out_pixels[x].r = (u16)src_pixels[2] * 257;
		out_pixels[x].g = (u16)src_pixels[1] * 257;
		out_pixels[x].b = (u16)src_pixels[0] * 257;
	}
}

static void XRGB8888_to_argb_u16(struct line_buffer *stage_buffer,
				 const struct vkms_frame_info *frame_info, int y)
{
	struct pixel_argb_u16 *out_pixels = stage_buffer->pixels;
	u8 *src_pixels = get_packed_src_addr(frame_info, y);
	int x_limit = min_t(size_t, drm_rect_width(&frame_info->dst),
			    stage_buffer->n_pixels);

	for (size_t x = 0; x < x_limit; x++, src_pixels += 4) {
		out_pixels[x].a = (u16)0xffff;
		out_pixels[x].r = (u16)src_pixels[2] * 257;
		out_pixels[x].g = (u16)src_pixels[1] * 257;
		out_pixels[x].b = (u16)src_pixels[0] * 257;
	}
}

static void ARGB16161616_to_argb_u16(struct line_buffer *stage_buffer,
				     const struct vkms_frame_info *frame_info,
				     int y)
{
	struct pixel_argb_u16 *out_pixels = stage_buffer->pixels;
	u16 *src_pixels = get_packed_src_addr(frame_info, y);
	int x_limit = min_t(size_t, drm_rect_width(&frame_info->dst),
			    stage_buffer->n_pixels);

	for (size_t x = 0; x < x_limit; x++, src_pixels += 4) {
		out_pixels[x].a = le16_to_cpu(src_pixels[3]);
		out_pixels[x].r = le16_to_cpu(src_pixels[2]);
		out_pixels[x].g = le16_to_cpu(src_pixels[1]);
		out_pixels[x].b = le16_to_cpu(src_pixels[0]);
	}
}

static void XRGB16161616_to_argb_u16(struct line_buffer *stage_buffer,
				     const struct vkms_frame_info *frame_info,
				     int y)
{
	struct pixel_argb_u16 *out_pixels = stage_buffer->pixels;
	u16 *src_pixels = get_packed_src_addr(frame_info, y);
	int x_limit = min_t(size_t, drm_rect_width(&frame_info->dst),
			    stage_buffer->n_pixels);

	for (size_t x = 0; x < x_limit; x++, src_pixels += 4) {
		out_pixels[x].a = (u16)0xffff;
		out_pixels[x].r = le16_to_cpu(src_pixels[2]);
		out_pixels[x].g = le16_to_cpu(src_pixels[1]);
		out_pixels[x].b = le16_to_cpu(src_pixels[0]);
	}
}

static void RGB565_to_argb_u16(struct line_buffer *stage_buffer,
			       const struct vkms_frame_info *frame_info, int y)
{
	struct pixel_argb_u16 *out_pixels = stage_buffer->pixels;
	u16 *src_pixels = get_packed_src_addr(frame_info, y);
	int x_limit = min_t(size_t, drm_rect_width(&frame_info->dst),
			       stage_buffer->n_pixels);

	s64 fp_rb_ratio = drm_fixp_div(drm_int2fixp(65535), drm_int2fixp(31));
	s64 fp_g_ratio = drm_fixp_div(drm_int2fixp(65535), drm_int2fixp(63));

	for (size_t x = 0; x < x_limit; x++, src_pixels++) {
		u16 rgb_565 = le16_to_cpu(*src_pixels);
		s64 fp_r = drm_int2fixp((rgb_565 >> 11) & 0x1f);
		s64 fp_g = drm_int2fixp((rgb_565 >> 5) & 0x3f);
		s64 fp_b = drm_int2fixp(rgb_565 & 0x1f);

		out_pixels[x].a = (u16)0xffff;
		out_pixels[x].r = drm_fixp2int(drm_fixp_mul(fp_r, fp_rb_ratio));
		out_pixels[x].g = drm_fixp2int(drm_fixp_mul(fp_g, fp_g_ratio));
		out_pixels[x].b = drm_fixp2int(drm_fixp_mul(fp_b, fp_rb_ratio));
	}
}

/*
 * The following  functions take an line of argb_u16 pixels from the
 * src_buffer, convert them to a specific format, and store them in the
 * destination.
 *
 * They are used in the `compose_active_planes` to convert and store a line
 * from the src_buffer to the writeback buffer.
 */
static void argb_u16_to_ARGB8888(struct vkms_frame_info *frame_info,
				 const struct line_buffer *src_buffer, int y)
{
	int x_dst = frame_info->dst.x1;
	u8 *dst_pixels = packed_pixels_addr(frame_info, x_dst, y);
	struct pixel_argb_u16 *in_pixels = src_buffer->pixels;
	int x_limit = min_t(size_t, drm_rect_width(&frame_info->dst),
			    src_buffer->n_pixels);

	for (size_t x = 0; x < x_limit; x++, dst_pixels += 4) {
		/*
		 * This sequence below is important because the format's byte order is
		 * in little-endian. In the case of the ARGB8888 the memory is
		 * organized this way:
		 *
		 * | Addr     | = blue channel
		 * | Addr + 1 | = green channel
		 * | Addr + 2 | = Red channel
		 * | Addr + 3 | = Alpha channel
		 */
		dst_pixels[3] = DIV_ROUND_CLOSEST(in_pixels[x].a, 257);
		dst_pixels[2] = DIV_ROUND_CLOSEST(in_pixels[x].r, 257);
		dst_pixels[1] = DIV_ROUND_CLOSEST(in_pixels[x].g, 257);
		dst_pixels[0] = DIV_ROUND_CLOSEST(in_pixels[x].b, 257);
	}
}

static void argb_u16_to_XRGB8888(struct vkms_frame_info *frame_info,
				 const struct line_buffer *src_buffer, int y)
{
	int x_dst = frame_info->dst.x1;
	u8 *dst_pixels = packed_pixels_addr(frame_info, x_dst, y);
	struct pixel_argb_u16 *in_pixels = src_buffer->pixels;
	int x_limit = min_t(size_t, drm_rect_width(&frame_info->dst),
			    src_buffer->n_pixels);

	for (size_t x = 0; x < x_limit; x++, dst_pixels += 4) {
		dst_pixels[3] = 0xff;
		dst_pixels[2] = DIV_ROUND_CLOSEST(in_pixels[x].r, 257);
		dst_pixels[1] = DIV_ROUND_CLOSEST(in_pixels[x].g, 257);
		dst_pixels[0] = DIV_ROUND_CLOSEST(in_pixels[x].b, 257);
	}
}

static void argb_u16_to_ARGB16161616(struct vkms_frame_info *frame_info,
				     const struct line_buffer *src_buffer, int y)
{
	int x_dst = frame_info->dst.x1;
	u16 *dst_pixels = packed_pixels_addr(frame_info, x_dst, y);
	struct pixel_argb_u16 *in_pixels = src_buffer->pixels;
	int x_limit = min_t(size_t, drm_rect_width(&frame_info->dst),
			    src_buffer->n_pixels);

	for (size_t x = 0; x < x_limit; x++, dst_pixels += 4) {
		dst_pixels[3] = cpu_to_le16(in_pixels[x].a);
		dst_pixels[2] = cpu_to_le16(in_pixels[x].r);
		dst_pixels[1] = cpu_to_le16(in_pixels[x].g);
		dst_pixels[0] = cpu_to_le16(in_pixels[x].b);
	}
}

static void argb_u16_to_XRGB16161616(struct vkms_frame_info *frame_info,
				     const struct line_buffer *src_buffer, int y)
{
	int x_dst = frame_info->dst.x1;
	u16 *dst_pixels = packed_pixels_addr(frame_info, x_dst, y);
	struct pixel_argb_u16 *in_pixels = src_buffer->pixels;
	int x_limit = min_t(size_t, drm_rect_width(&frame_info->dst),
			    src_buffer->n_pixels);

	for (size_t x = 0; x < x_limit; x++, dst_pixels += 4) {
		dst_pixels[3] = 0xffff;
		dst_pixels[2] = cpu_to_le16(in_pixels[x].r);
		dst_pixels[1] = cpu_to_le16(in_pixels[x].g);
		dst_pixels[0] = cpu_to_le16(in_pixels[x].b);
	}
}

static void argb_u16_to_RGB565(struct vkms_frame_info *frame_info,
			       const struct line_buffer *src_buffer, int y)
{
	int x_dst = frame_info->dst.x1;
	u16 *dst_pixels = packed_pixels_addr(frame_info, x_dst, y);
	struct pixel_argb_u16 *in_pixels = src_buffer->pixels;
	int x_limit = min_t(size_t, drm_rect_width(&frame_info->dst),
			    src_buffer->n_pixels);

	s64 fp_rb_ratio = drm_fixp_div(drm_int2fixp(65535), drm_int2fixp(31));
	s64 fp_g_ratio = drm_fixp_div(drm_int2fixp(65535), drm_int2fixp(63));

	for (size_t x = 0; x < x_limit; x++, dst_pixels++) {
		s64 fp_r = drm_int2fixp(in_pixels[x].r);
		s64 fp_g = drm_int2fixp(in_pixels[x].g);
		s64 fp_b = drm_int2fixp(in_pixels[x].b);

		u16 r = drm_fixp2int(drm_fixp_div(fp_r, fp_rb_ratio));
		u16 g = drm_fixp2int(drm_fixp_div(fp_g, fp_g_ratio));
		u16 b = drm_fixp2int(drm_fixp_div(fp_b, fp_rb_ratio));

		*dst_pixels = cpu_to_le16(r << 11 | g << 5 | b);
	}
}

void *get_frame_to_line_function(u32 format)
{
	switch (format) {
	case DRM_FORMAT_ARGB8888:
		return &ARGB8888_to_argb_u16;
	case DRM_FORMAT_XRGB8888:
		return &XRGB8888_to_argb_u16;
	case DRM_FORMAT_ARGB16161616:
		return &ARGB16161616_to_argb_u16;
	case DRM_FORMAT_XRGB16161616:
		return &XRGB16161616_to_argb_u16;
	case DRM_FORMAT_RGB565:
		return &RGB565_to_argb_u16;
	default:
		return NULL;
	}
}

void *get_line_to_frame_function(u32 format)
{
	switch (format) {
	case DRM_FORMAT_ARGB8888:
		return &argb_u16_to_ARGB8888;
	case DRM_FORMAT_XRGB8888:
		return &argb_u16_to_XRGB8888;
	case DRM_FORMAT_ARGB16161616:
		return &argb_u16_to_ARGB16161616;
	case DRM_FORMAT_XRGB16161616:
		return &argb_u16_to_XRGB16161616;
	case DRM_FORMAT_RGB565:
		return &argb_u16_to_RGB565;
	default:
		return NULL;
	}
}
