#pragma once
/*
WRenderer works almost identically to the C++ version.
All functions are static.

Removed Startup as it is only useful within WGameWindow
during the BuildWindowCore function.

Adjusted Load to return WWindowInfo instead.

Left functionality as open as possible for custom
creation of windows if required.
*/
#using <System.dll>
#using <WindowsBase.dll>
#using <PresentationCore.dll>
#using <PresentationFramework.dll>

#include <Windows.h>

#include "GameWindow.h"
#include "RTexture.h"
#include "RBuffer.h"
#include "WShader.h"
#include "WColor.h"

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Interop;
using namespace System::Windows::Input;
using namespace System::Windows::Media;
using namespace System::Windows::Threading;
using namespace System::Runtime::InteropServices;

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

	LRESULT WINAPI RendererMsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
 
	public ref class WWindowInfo
	{
	private:

		WindowInfo*		mRef;

	public:

		WWindowInfo();
		WWindowInfo( const WindowInfo% info );
		WWindowInfo( WindowInfo* info );
		WWindowInfo( const WWindowInfo% info );
		~WWindowInfo();

		WindowInfo*		GetRef();

		bool			Fullscreen();

		int				Width();
		int				Height();

		float			WidthRatio();
		float			HeightRatio();
	};

	public ref class WRenderer
	{
	public:

		static RTexture^	NULL_TEXTURE;	// black default texture
		static RTexture^	BASE_TEXTURE;	// white default texture

		//////////////////////////////

		static WWindowInfo^	Load( String^ filename );

		static void			Shutdown();

		// Windows.
		//
		static void			SetWindow( WWindowInfo^ info );
		static WWindowInfo^ GetWindow();

		static bool			ShareResources( WWindowInfo^ info0, WWindowInfo^ info1 );

		// Buffers.
		//
		static RBuffer^		CreateBuffer( IntPtr data, UINT size, UINT stride, BufferType type );

		static void			SetVBO( RBuffer^ buffer );
		static void			SetIBO( RBuffer^ buffer );

		// Textures.
		//
		static RTexture^	CreateTexture( UINT width, UINT height, ImageFormatType format, bool createMips );
		static RTexture^	CreateTextureFromFile( String^ filename );
		static RTexture^	CreateTextureFromMemory( IntPtr data, UINT width, UINT height, ImageFormatType format, bool createMips );
	
		// Special Rendering.
		//
		static UINT			CreateBackbuffer();
		static UINT			CreateRenderTarget( RTexture^ texture );
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
		static WShader^		CreateShader( System::String^ filename, System::String^ attrib, System::String^ uniform );
		static void			SetShader( WShader^ shader );

		// Rendering.
		//
		static void			Clear( WColorRGBA^ color );
		static void			DrawIndexed( UINT count, UINT startIndex, UINT baseVertex );
		static void			Present();
	};
}}
