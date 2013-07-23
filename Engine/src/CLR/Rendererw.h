#pragma once

#include "Texturew.h"
#include "Bufferw.h"
#include "Shaderw.h"

using namespace Sentinel::Assets;

namespace Sentinel { namespace Systems
{
	public enum PrimitiveType
	{
		POINT_LIST,
		LINE_LIST,
		TRIANGLE_LIST,

		NUM_PRIMITIVES
	};

	public enum ImageFormatType
	{
		IMAGE_FORMAT_R,
		IMAGE_FORMAT_RGB,
		IMAGE_FORMAT_RGBA,
		IMAGE_FORMAT_HDR,	// R32G32B32F

		NUM_IMAGE_FORMATS
	};

	public enum CullType
	{
		CULL_NONE,
		CULL_CCW,
		CULL_CW,

		NUM_CULL_TYPES
	};

	public enum BlendType
	{
		BLEND_DEFAULT,
		BLEND_ALPHA,
		BLEND_PARTICLE,

		NUM_BLEND_TYPES
	};

	public enum StencilType
	{
		STENCIL_DEFAULT,
		STENCIL_PARTICLE,

		NUM_STENCIL_TYPES
	};

	public ref class Rendererw
	{
	public:

		static const UINT	WINDOW_WIDTH_BASE  = 1920;
		static const UINT	WINDOW_HEIGHT_BASE = 1080;

		static float		WINDOW_WIDTH_RATIO;
		static float		WINDOW_HEIGHT_RATIO;

		static UINT			DESKTOP_WIDTH;
		static UINT			DESKTOP_HEIGHT;

		static UINT			WINDOW_WIDTH;
		static UINT			WINDOW_HEIGHT;
		static bool			FULLSCREEN;

		static Texturew^	NULL_TEXTURE;	// black default texture
		static Texturew^	BASE_TEXTURE;	// white default texture

		//////////////////////////////

		~Rendererw();

		static int			Load( System::String^ filename );
		static UINT			Startup( void* hWnd, bool fullscreen, UINT width, UINT height );
		static void			Shutdown();

		// Buffers.
		//
		static Bufferw^		CreateBuffer( void* data, UINT size, UINT stride, BufferType type );

		static void			SetVBO( Bufferw^ buffer );
		static void			SetIBO( Bufferw^ buffer );

		// Textures.
		//
		static Texturew^	CreateTexture( UINT width, UINT height, ImageFormatType format, bool createMips );
		static Texturew^	CreateTextureFromFile( System::String^ filename );
		static Texturew^	CreateTextureFromMemory( void* data, UINT width, UINT height, ImageFormatType format, bool createMips );
	
		// Special Rendering.
		//
		static UINT			CreateRenderTarget( Texturew^ texture );
		static UINT			CreateDepthStencil( UINT width, UINT height );
		static UINT			CreateViewport( UINT width, UINT height );
		static UINT			ResizeBuffers( UINT width, UINT height );

		static void			SetRenderType( PrimitiveType type );
		static void			SetRenderTarget( UINT target );
		static void			SetDepthStencil( UINT stencil );
		static void			SetDepthStencilState( UINT state );
		static void			SetViewport( UINT viewport );
		static UINT			SetCull( CullType type );
		static void			SetBlend( BlendType type );

		// Shaders.
		//
		static Shaderw^		CreateShader( System::String^ filename, System::String^ attrib, System::String^ uniform );
		static void			SetShader( Shader* shader );

		// Rendering.
		//
		static void			Clear( float* color );
		static void			DrawIndexed( UINT count, UINT startIndex, UINT baseVertex );
		static void			Present();
	};
}}
