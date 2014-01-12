#pragma once

namespace Sentinel
{
	enum PrimitiveType
	{
		POINT_LIST,
		LINE_LIST,
		TRIANGLE_LIST,

		NUM_PRIMITIVES
	};

	enum ImageFormatType
	{
		IMAGE_FORMAT_R,
		IMAGE_FORMAT_RGB,
		IMAGE_FORMAT_RGBA,
		IMAGE_FORMAT_HDR,	// R32G32B32F

		NUM_IMAGE_FORMATS
	};

	enum CullType
	{
		CULL_NONE,
		CULL_CCW,
		CULL_CW,

		NUM_CULL_TYPES
	};

	enum FillType
	{
		FILL_SOLID,
		FILL_WIREFRAME,

		NUM_FILL_TYPES
	};

	enum BlendType
	{
		BLEND_DEFAULT,
		BLEND_ALPHA,
		BLEND_PARTICLE,

		NUM_BLEND_TYPES
	};

	enum StencilType
	{
		STENCIL_DEFAULT,
		STENCIL_NO_ZBUFFER,
		STENCIL_PARTICLE,

		NUM_STENCIL_TYPES
	};

	enum BufferType
	{
		NULL_BUFFER,
		VERTEX_BUFFER,
		INDEX_BUFFER,

		NUM_BUFFER_TYPES
	};

	enum BufferAccess
	{
		BUFFER_READ_WRITE,
		BUFFER_WRITE,

		NUM_BUFFER_ACCESS
	};

	enum TextureType
	{
		TEXTURE_DIFFUSE,
		TEXTURE_NORMAL,
		TEXTURE_DEPTH,

		NUM_TEXTURES,
	};
}
