#pragma once

namespace Sentinel { namespace Wrapped
{
	public enum class PrimitiveType
	{
		POINT_LIST,
		LINE_LIST,
		TRIANGLE_LIST,

		NUM_PRIMITIVES
	};

	public enum class ImageFormatType
	{
		R,
		RGB,
		RGBA,
		HDR,	// R32G32B32F

		NUM_IMAGE_FORMATS
	};

	public enum class CullType
	{
		NONE,
		CCW,
		CW,

		NUM_CULL_TYPES
	};

	public enum class FillType
	{
		SOLID,
		WIREFRAME,

		NUM_FILL_TYPES
	};

	public enum class BlendType
	{
		DEFAULT,
		ALPHA,
		PARTICLE,

		NUM_BLEND_TYPES
	};

	public enum class StencilType
	{
		DEFAULT,
		NO_ZBUFFER,
		PARTICLE,

		NUM_STENCIL_TYPES
	};

	public enum class BufferType
	{
		INVALID,
		VERTEX,
		INDEX,

		NUM_BUFFER_TYPES
	};

	public enum class TextureType
	{
		DIFFUSE,
		NORMAL,
		DEPTH,

		NUM_TEXTURES,
	};
}}
