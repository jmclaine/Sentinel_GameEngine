#pragma once

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"

#include "Singleton.h"
#include "MathLib.h"
#include "Util.h"
#include "Shader.h"
#include "Texture.h"
#include "Script.h"

#include <sstream>
#include <vector>

namespace Sentinel
{
	enum BufferType
	{
		VERTEX_BUFFER = 1,
		INDEX_BUFFER  = 2,

		NUM_BUFFER_TYPES
	};

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
		STENCIL_PARTICLE,

		NUM_STENCIL_TYPES
	};

	class Buffer
	{
	public:

		BufferType	mType;
		UINT		mSize;
		UINT		mStride;

		Buffer() {}
		virtual ~Buffer() {}

		virtual void* Lock() = 0;
		virtual void  Unlock() = 0;
		virtual void  Release() = 0;
	};

	class Renderer : public SingletonAbstract< Renderer >
	{
	protected:

		UINT			PRIMITIVE[ NUM_PRIMITIVES ];
		UINT			CULL_TYPE[ NUM_CULL_TYPES ];

		Shader*			mCurrShader;

	public:

		static const UINT	SCREEN_WIDTH_BASE  = 1920;
		static const UINT	SCREEN_HEIGHT_BASE = 1080;

		static float		SCREEN_WIDTH_RATIO;
		static float		SCREEN_HEIGHT_RATIO;

		static UINT			DESKTOP_WIDTH;
		static UINT			DESKTOP_HEIGHT;

		static UINT			SCREEN_WIDTH;
		static UINT			SCREEN_HEIGHT;
		static bool			FULLSCREEN;

		static Texture*		NULL_TEXTURE;	// black default texture
		static Texture*		BASE_TEXTURE;	// white default texture

		//////////////////////////////

		virtual ~Renderer() {}

		virtual UINT Startup( void* hWnd, bool fullscreen, UINT width = 1920, UINT height = 1080 ) = 0;
		virtual void Shutdown() = 0;

		// Buffers.
		//
		virtual Buffer* CreateBuffer( void* data, UINT size, UINT stride, BufferType type ) = 0;

		virtual void SetVBO( Buffer* buffer ) = 0;
		virtual void SetIBO( Buffer* buffer ) = 0;

		// Textures.
		//
		Texture* CreateTexture( UINT width, UINT height, ImageFormatType format = IMAGE_FORMAT_RGBA, bool createMips = false )
		{
			return CreateTextureFromMemory( 0, width, height, format, createMips );
		}

		virtual Texture* CreateTextureFromFile( const char* filename ) = 0;
		virtual Texture* CreateTextureFromMemory( void* data, UINT width, UINT height, ImageFormatType format, bool createMips = true ) = 0;
	
		// Special Rendering.
		//
		virtual UINT CreateRenderTarget( Texture* texture ) = 0;
		virtual UINT CreateDepthStencil( UINT width, UINT height ) = 0;
		virtual UINT CreateViewport( UINT width, UINT height ) = 0;
		virtual UINT ResizeBuffers( UINT width, UINT height ) = 0;

		virtual void SetRenderType( PrimitiveType type ) = 0;
		virtual void SetRenderTarget( UINT target ) = 0;
		virtual void SetDepthStencil( UINT stencil ) = 0;
		virtual void SetDepthStencilState( UINT state ) = 0;
		virtual void SetViewport( UINT viewport ) = 0;
		virtual UINT SetCull( CullType type ) = 0;
		virtual void SetBlend( BlendType type ) = 0;

		// Shaders.
		//
		virtual Shader* CreateShader( const char* filename, const char* attrib, const char* uniform ) = 0;
		virtual void    SetShader( Shader* shader ) = 0;

		// Rendering.
		//
		virtual void Clear( float* color ) = 0;
		virtual void DrawIndexed( UINT count, UINT startIndex, UINT baseVertex ) = 0;
		virtual void Present() = 0;
	};

	////////////////////////////////////////////////////////////////////
	// Dual-Renderer
	//
	extern Renderer* BuildRendererDX();
	extern Renderer* BuildRendererGL();

	// Scripted Renderer.
	//
	int CreateRenderer( const char* filename );
}
