#pragma once
/*
WRenderer works almost identically to the C++ version.
All functions are static.

Removed Startup as it is only useful within WGameWindow
during the BuildWindowCore function.

Adjusted Load to return WWindowInfo instead.
*/
#using <System.dll>
#using <WindowsBase.dll>
#using <PresentationCore.dll>
#using <PresentationFramework.dll>

#include <Windows.h>

#include "Property.h"
#include "GameWindow.h"
#include "WTexture.h"
#include "WBuffer.h"
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

	////////////////////////////////////////////////////////////

	public ref class WWindowInfo
	{
		DECLARE_REF( WindowInfo );

	public:

		WWindowInfo();
		WWindowInfo( const WindowInfo% info );
		WWindowInfo( WindowInfo* info );
		WWindowInfo( const WWindowInfo% info );
		
		//////////////////////////////

		bool			Fullscreen();

		int				Width();
		int				Height();

		float			WidthRatio();
		float			HeightRatio();
	};

	DECLARE_CLASS_REF( WindowInfo );

	////////////////////////////////////////////////////////////

	public ref class WRenderer
	{
	public:

		static WTexture^	NullTexture();
		static WTexture^	BaseTexture();

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
		static WBuffer^		CreateBuffer( IntPtr data, UINT size, UINT stride, BufferType type );

		static void			SetVBO( WBuffer^ buffer );
		static void			SetIBO( WBuffer^ buffer );

		// Textures.
		//
		static WTexture^	CreateTexture( UINT width, UINT height, ImageFormatType format, bool createMips );
		static WTexture^	CreateTextureFromFile( String^ filename );
		static WTexture^	CreateTextureFromMemory( IntPtr data, UINT width, UINT height, ImageFormatType format, bool createMips );
	
		// Special Rendering.
		//
		static UINT			CreateBackbuffer();
		static UINT			CreateRenderTarget( WTexture^ texture );
		static UINT			CreateDepthStencil( UINT width, UINT height );
		static UINT			CreateViewport( UINT width, UINT height );
		static UINT			ResizeBuffers( UINT width, UINT height );

		static void			SetRenderType( PrimitiveType type );
		static void			SetRenderTarget( UINT target );
		static void			SetDepthStencil( UINT stencil );
		static void			SetDepthStencilState( StencilType state );
		static void			SetViewport( UINT viewport );
		static UINT			SetCull( CullType type );
		static UINT			SetFill( FillType type );
		static void			SetBlend( BlendType type );

		// Shaders.
		//
		static WShader^		CreateShader( String^ filename, String^ attrib, String^ uniform );
		static void			SetShader( WShader^ shader );

		// Rendering.
		//
		static void			Clear( WColorRGBA^ color );
		static void			DrawIndexed( UINT count, UINT startIndex, UINT baseVertex );
		static void			Present();
	};
}}
