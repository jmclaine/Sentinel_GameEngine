#pragma once
/*
HOW TO USE:

Create a file called 'config.xml' for easy setup.

<?xml version="1.0" ?>
<!--DIRECTX or OPENGL-->
<Renderer Type="OPENGL" Fullscreen="false" Width="1920" Height="1080" />

// This initializes the Renderer values within the XML file.
WindowInfo info;
Renderer::Load( "config.xml", info );


// Alternative to creating and loading the XML file.
Renderer::Inst( BuildRendererGL() );
* OR *
Renderer::Inst( BuildRendererDX() );


// Initialize the Renderer.
// Use appropriate HWND for Windows applications.
WindowInfo* windowInfo = Renderer::Inst()->Startup( mHWND, info.mFullscreen, info.mWidth, info.mHeight );


// Now the Renderer can perform its normal functionality.
// See functions for their applicability to desired use.

// See MeshBuilder.h for ease of object creation.
// See Mesh.h for ease of object rendering.

// Although custom object creation and rendering is possible,
// it is highly recommended to use both the MeshBuilder and Mesh
// to render objects easily.
//
// If more complex objects are desired, then it is recommended
// to create either OBJ or M3D (3ds Max plugin) models, and
// import them. (see Model.h)


EXAMPLE:

// Only necessary when rendering to multiple windows.
Renderer::Inst()->SetWindow( windowInfo );

// Clear buffers.
Renderer::Inst()->Clear( ColorRGBA( 0.0f, 0.2f, 0.8f, 1.0f ).Ptr() );

// Shaders are required.
Renderer::Inst()->SetShader( shader );

// VBOs and IBOs required.
Renderer::Inst()->SetVBO( bufferVBO );
Renderer::Inst()->SetIBO( bufferIBO );

// Set shader variables here (see Shader.h).

// Draw stuff.
Renderer::Inst()->SetRenderType( primitive );
Renderer::Inst()->DrawIndexed( bufferIBO.mCount, 0, 0 );

Renderer::Inst()->Present();


// When finished with Renderer always call:
Renderer::Destroy();

// If more than one window / handle was created:
Renderer::Inst()->SetWindow( windowInfo0 );
Renderer::Inst()->Shutdown();

Renderer::Inst()->SetWindow( windowInfo1 );
Renderer::Inst()->Shutdown();

Renderer::Destroy();
*/

#include "Common.h"
#include "Singleton.h"
#include "Util.h"
#include "Shader.h"
#include "Texture.h"
#include "Buffer.h"

#include <sstream>
#include <vector>

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

	class SENTINEL_DLL WindowInfo
	{
		friend class Renderer;

	protected:

		bool		mFullscreen;

		UINT		mWidth;
		UINT		mHeight;

		float		mWidthRatio;
		float		mHeightRatio;

	public:

		WindowInfo();

		bool	Fullscreen() const;

		UINT	Width() const;
		UINT	Height() const;

		float	WidthRatio() const;
		float	HeightRatio() const;
	};

	// Renderer is a SingletonAbstract as creating multiple instances
	// of this particular object would unnecessarily complicate the
	// shared context capability of the video card interface without
	// any gains.  Due to this class being a Singleton, this also
	// prevents a user from creating both OpenGL and DirectX at the
	// same time.
	//
	class SENTINEL_DLL Renderer : public SingletonAbstract< Renderer >
	{
	protected:

		UINT				PRIMITIVE[ NUM_PRIMITIVES ];
		UINT				CULL_TYPE[ NUM_CULL_TYPES ];
		UINT				FILL_TYPE[ NUM_FILL_TYPES ];

		Shader*				mCurrShader;

	public:

		static const UINT	WINDOW_WIDTH_BASE  = 1920;
		static const UINT	WINDOW_HEIGHT_BASE = 1080;

		Texture*			NULL_TEXTURE;	// black default texture
		Texture*			BASE_TEXTURE;	// white default texture

	public:

		virtual ~Renderer() {};

		/////////////////////////////////

		static Renderer*	Load( const char* filename, WindowInfo& info );

		virtual WindowInfo*	Startup( void* hWnd, bool fullscreen, UINT width, UINT height ) = 0;

		virtual void		Shutdown() = 0;

		// Windows.
		//
		virtual void		SetWindow( WindowInfo* info ) = 0;
		virtual WindowInfo* GetWindow() = 0;

		virtual bool		ShareResources( WindowInfo* info0, WindowInfo* info1 ) = 0;

		// Buffers.
		//
		virtual Buffer*		CreateBuffer( void* data, UINT size, UINT stride, BufferType type ) = 0;

		virtual void		SetVBO( Buffer* buffer ) = 0;
		virtual void		SetIBO( Buffer* buffer ) = 0;

		// Textures.
		//
		Texture*			CreateTexture( UINT width, UINT height, ImageFormatType format = IMAGE_FORMAT_RGBA, bool createMips = false );

		virtual Texture*	CreateTextureFromFile( const char* filename ) = 0;
		virtual Texture*	CreateTextureFromMemory( void* data, UINT width, UINT height, ImageFormatType format, bool createMips = true ) = 0;
	
		// Special Rendering.
		//
		virtual UINT		CreateBackbuffer() = 0;
		virtual UINT		CreateRenderTarget( Texture* texture ) = 0;
		virtual UINT		CreateDepthStencil( UINT width, UINT height ) = 0;
		virtual UINT		CreateViewport( UINT width, UINT height ) = 0;
		virtual UINT		ResizeBuffers( UINT width, UINT height ) = 0;

		virtual void		SetRenderType( PrimitiveType type ) = 0;
		virtual void		SetRenderTarget( UINT target ) = 0;
		virtual void		SetDepthStencil( UINT stencil ) = 0;
		virtual void		SetDepthStencilState( UINT state ) = 0;
		virtual void		SetViewport( UINT viewport ) = 0;
		virtual UINT		SetCull( CullType type ) = 0;
		virtual UINT		SetFill( FillType type ) = 0;
		virtual void		SetBlend( BlendType type ) = 0;

		// Shaders.
		//
		virtual Shader*		CreateShader( const char* filename, const char* attrib, const char* uniform ) = 0;
		virtual void		SetShader( Shader* shader ) = 0;

		// Rendering.
		//
		virtual void		Clear( float* color ) = 0;
		virtual void		DrawIndexed( UINT count, UINT startIndex, UINT baseVertex ) = 0;
		virtual void		Present() = 0;
	};

	////////////////////////////////////////////////////////////////////
	// Dual-Renderer
	//
	extern SENTINEL_DLL Renderer* BuildRendererDX();
	extern SENTINEL_DLL Renderer* BuildRendererGL();
}
