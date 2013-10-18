#pragma once
/*
Removed Startup as it is only useful within WGameWindow
during the BuildWindowCore function.
*/
#include "Property.h"
#include "WRendererTypes.h"
#include "Renderer.h"

namespace Sentinel
{
	class Renderer;
	class GameWindow;
	class WindowInfo;

namespace Wrapped
{
	ref class WTexture;
	ref class WBuffer;
	ref class WShader;
	ref class WColorRGBA;

	public ref class WWindowInfo
	{
		DECLARE_REF( WindowInfo );

	public:

		WWindowInfo();
		WWindowInfo( WindowInfo* info );
		
		//////////////////////////////

		DECLARE_OP_PTR( WindowInfo );

		bool			Fullscreen();

		UINT			Width();
		UINT			Height();

		float			WidthRatio();
		float			HeightRatio();
	};

	DECLARE_CLASS_REF( WindowInfo );

	////////////////////////////////////////////////////////////

	public ref class WRenderer
	{
		DECLARE_REF_PTR( Renderer );

	public:

		WRenderer( Renderer* renderer );

		void			Release();

		WTexture^		NullTexture();
		WTexture^		BaseTexture();

		//////////////////////////////

		DECLARE_OP_PTR( Renderer );

		static WRenderer^ Create( System::String^ filename, WWindowInfo^% info );

		void			Shutdown();

		// Windows.
		//
		void			SetWindow( WWindowInfo^ info );
		WWindowInfo^	GetWindow();

		bool			ShareResources( WWindowInfo^ info0, WWindowInfo^ info1 );

		// Buffers.
		//
		WBuffer^		CreateBuffer( System::IntPtr data, UINT size, UINT stride, BufferType type );

		void			SetVBO( WBuffer^ buffer );
		void			SetIBO( WBuffer^ buffer );

		// Textures.
		//
		WTexture^		CreateTexture( UINT width, UINT height, ImageFormatType format, bool createMips );
		WTexture^		CreateTextureFromFile( System::String^ filename );
		WTexture^		CreateTextureFromMemory( System::IntPtr data, UINT width, UINT height, ImageFormatType format, bool createMips );
	
		// Special Rendering.
		//
		UINT			CreateBackbuffer();
		UINT			CreateRenderTarget( WTexture^ texture );
		UINT			CreateDepthStencil( UINT width, UINT height );
		UINT			ResizeBuffers( UINT width, UINT height );

		void			SetRenderType( PrimitiveType type );
		void			SetRenderTarget( UINT target );
		void			SetDepthStencil( UINT stencil );
		void			SetDepthStencilState( StencilType state );
		void			SetViewport( int x, int y, UINT width, UINT height );
		UINT			SetCull( CullType type );
		UINT			SetFill( FillType type );
		void			SetBlend( BlendType type );

		// Shaders.
		//
		WShader^		CreateShaderFromFile( System::String^ filename, System::String^ attrib, System::String^ uniform );
		void			SetShader( WShader^ shader );

		// Rendering.
		//
		void			Clear( WColorRGBA^ color );
		void			DrawIndexed( UINT count, UINT startIndex, UINT baseVertex );
		void			Present();

		static WRenderer^ BuildGL();
		static WRenderer^ BuildDX();
	};
}}
