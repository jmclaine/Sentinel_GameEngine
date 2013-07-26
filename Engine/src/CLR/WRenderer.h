#pragma once

// WRenderer works almost identically to the C++ version.
//
// The biggest change is the removal of the Singleton.
// The removal allows the Renderer to create its own
// window / handle, and interface directly with the
// WPF Window, which makes drawing to multiple windows
// and controls easy and intuitive.
//
#using <System.dll>
#using <WindowsBase.dll>
#using <PresentationCore.dll>
#using <PresentationFramework.dll>

#include <Windows.h>

#include "WTexture.h"
#include "WBuffer.h"
#include "WShader.h"
#include "WColor.h"

using namespace Sentinel::Assets;
using namespace System;
using namespace System::Windows;
using namespace System::Windows::Interop;
using namespace System::Windows::Input;
using namespace System::Windows::Media;
using namespace System::Windows::Threading;
using namespace System::Runtime::InteropServices;

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

	#define MAX_LOADSTRING 100

	LRESULT WINAPI RendererMsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
 
	public ref class WRenderer : public HwndHost
	{
	private:

		HWND				mHWND;
		HINSTANCE			mINST;
		wchar_t* 			mTitle;
		wchar_t* 			mWindowClass;

	public:

		WTexture^			NULL_TEXTURE;	// black default texture
		WTexture^			BASE_TEXTURE;	// white default texture

		//////////////////////////////

		WRenderer();
		~WRenderer();

		static bool			Load( String^ filename );
		UINT				Startup( IntPtr hWnd );
		void				Update( Object^ sender, EventArgs^ e );
		void				Destroy();

		// Buffers.
		//
		WBuffer^			CreateBuffer( IntPtr data, UINT size, UINT stride, BufferType type );

		void				SetVBO( WBuffer^ buffer );
		void				SetIBO( WBuffer^ buffer );

		// Textures.
		//
		WTexture^			CreateTexture( UINT width, UINT height, ImageFormatType format, bool createMips );
		WTexture^			CreateTextureFromFile( String^ filename );
		WTexture^			CreateTextureFromMemory( IntPtr data, UINT width, UINT height, ImageFormatType format, bool createMips );
	
		// Special Rendering.
		//
		UINT				CreateRenderTarget( WTexture^ texture );
		UINT				CreateDepthStencil( UINT width, UINT height );
		UINT				CreateViewport( UINT width, UINT height );
		UINT				ResizeBuffers( UINT width, UINT height );

		void				SetRenderType( PrimitiveType type );
		void				SetRenderTarget( UINT target );
		void				SetDepthStencil( UINT stencil );
		void				SetDepthStencilState( UINT state );
		void				SetViewport( UINT viewport );
		UINT				SetCull( CullType type );
		void				SetBlend( BlendType type );

		// Shaders.
		//
		WShader^			CreateShader( System::String^ filename, System::String^ attrib, System::String^ uniform );
		void				SetShader( WShader^ shader );

		// Rendering.
		//
		void				Clear( WColorRGBA^ color );
		void				DrawIndexed( UINT count, UINT startIndex, UINT baseVertex );
		void				Present();

	protected:
		
		virtual IntPtr		WndProc( IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, bool% handled ) override;

		virtual void		OnRenderSizeChanged( SizeChangedInfo^ sizeInfo ) override;

		virtual void		OnRender( DrawingContext^ drawingContext ) override;

		virtual void		OnMouseDown( MouseButtonEventArgs^ e ) override;

		virtual void		DestroyWindowCore( HandleRef hwnd ) override;

		bool				RegisterWindowClass();

		virtual HandleRef	BuildWindowCore( HandleRef hwndParent ) override;
	};
}}
