#include <vector>
#include <crtdbg.h>

#include "Renderer.h"
#include "Archive.h"

#include "CommonDX.h"
#include "BufferDX.h"
#include "TextureDX.h"
#include "ShaderDX.h"
#include "VertexLayoutDX.h"

namespace Sentinel
{
	#define HV( func )\
		if( (func) != S_OK )\
			return S_FALSE;

	#define HV_PTR( func )\
		if( (func) != S_OK )\
			return NULL;

	////////////////////////////////////////////////////////////////////////////////////
	// DirectX11 Renderer.
	//
	class RendererDX : public Renderer
	{
		friend class TextureDX;
		friend class ShaderDX;

	private:

		static const UINT PRIMITIVE[ NUM_PRIMITIVES ];
		static const UINT CULL_TYPE[ NUM_CULL_TYPES ];
		static const UINT FILL_TYPE[ NUM_FILL_TYPES ];

		class WindowInfoDX : public WindowInfo
		{
			friend class RendererDX;

		public:

			ID3D11Device*						mDevice;
			ID3D11DeviceContext*				mContext;
			IDXGISwapChain*						mSwapChain;
			ID3D11RasterizerState*				mRasterizerState;
		};

		WindowInfoDX*							mCurrWindow;

		std::vector< ID3D11DepthStencilView* >	mDepthStencil;
		ID3D11DepthStencilView*					mCurrStencil;

		std::vector< ID3D11DepthStencilState* >	mDepthStencilState;

		std::vector< ID3D11RenderTargetView* >	mRenderTarget;
		ID3D11RenderTargetView*					mCurrTarget;

		std::vector< ID3D11BlendState* >		mBlendState;

		DXGI_SAMPLE_DESC						mSampleDesc;

	public:
		
		RendererDX()
		{
			mSampleDesc.Count   = 1;
			mSampleDesc.Quality = 0;

			mCurrWindow  = NULL;
			mCurrStencil = NULL;
			mCurrTarget  = NULL;

			mRenderTarget.push_back( NULL );
		}

		~RendererDX()
		{
			SAFE_RELEASE_PTR_LIST( mRenderTarget );
			SAFE_RELEASE_PTR_LIST( mDepthStencil );
			SAFE_RELEASE_PTR_LIST( mBlendState );
			SAFE_RELEASE_PTR_LIST( mDepthStencilState );
		}

	private:

		WindowInfo* Startup( void* hWnd, bool fullscreen, UINT width, UINT height )
		{
			D3D_FEATURE_LEVEL featurelevels[] = 
			{
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
			};

			DXGI_SWAP_CHAIN_DESC sd =
			{
				{
					width, height,
					{ 1, 60 },
					DXGI_FORMAT_R8G8B8A8_UNORM,
					DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
					DXGI_MODE_SCALING_UNSPECIFIED
				},
				mSampleDesc,
				DXGI_USAGE_RENDER_TARGET_OUTPUT,
				1,
				(HWND)hWnd,
				!fullscreen,
				DXGI_SWAP_EFFECT_DISCARD,
				DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
			};
			
			UINT createdeviceflags = 0;

			// Windows update now makes this fail.
			//
			//#ifdef _DEBUG
			//	createdeviceflags |= D3D11_CREATE_DEVICE_DEBUG;
			//#endif
	
			mCurrWindow = new WindowInfoDX();

			mCurrWindow->mHandle		= hWnd;
			mCurrWindow->mFullscreen	= fullscreen;
			mCurrWindow->mWidth			= width;
			mCurrWindow->mHeight		= height;
			mCurrWindow->mWidthRatio	= (float)width  / (float)WINDOW_WIDTH_BASE;
			mCurrWindow->mHeightRatio	= (float)height / (float)WINDOW_HEIGHT_BASE;

			HV_PTR( D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createdeviceflags, 
												   featurelevels, sizeof(featurelevels)/sizeof(D3D_FEATURE_LEVEL),
												   D3D11_SDK_VERSION, &sd, &mCurrWindow->mSwapChain, 
												   &mCurrWindow->mDevice, 0, &mCurrWindow->mContext ));

			TRACE( "Created DX11 Renderer..." );

			if( fullscreen )
			{
				HV_PTR( mCurrWindow->mSwapChain->ResizeBuffers( 1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ));
				HV_PTR( mCurrWindow->mSwapChain->SetFullscreenState( TRUE, NULL ));
			}

			D3D11_RASTERIZER_DESC rasterizerstate = { D3D11_FILL_SOLID, D3D11_CULL_BACK, true, 0, 0, 0, true, false, false, false };
			HV_PTR( mCurrWindow->mDevice->CreateRasterizerState( &rasterizerstate, &mCurrWindow->mRasterizerState ));
			mCurrWindow->mContext->RSSetState( mCurrWindow->mRasterizerState );

			#ifndef NDEBUG
				SET_DEBUG_NAME( mCurrWindow->mSwapChain );
				SET_DEBUG_NAME( mCurrWindow->mDevice );
				SET_DEBUG_NAME( mCurrWindow->mContext );
				SET_DEBUG_NAME( mCurrWindow->mRasterizerState );
			#endif

			// Create NULL_TEXTURE (Black).
			//
			if( !NULL_TEXTURE )
			{
				UCHAR* newTex = new UCHAR[ 4 ];

				newTex[ 0 ] = 0;
				newTex[ 1 ] = 0;
				newTex[ 2 ] = 0;
				newTex[ 3 ] = 255;

				NULL_TEXTURE = CreateTextureFromMemory( newTex, 1, 1, IMAGE_FORMAT_RGBA );
				
				delete newTex;
			}

			// Create BASE_TEXTURE (White).
			//
			if( !BASE_TEXTURE )
			{
				UCHAR* newTex = new UCHAR[ 4 ];

				newTex[ 0 ] = 255;
				newTex[ 1 ] = 255;
				newTex[ 2 ] = 255;
				newTex[ 3 ] = 255;

				BASE_TEXTURE = CreateTextureFromMemory( newTex, 1, 1, IMAGE_FORMAT_RGBA );
				
				delete newTex;
			}

			if( mBlendState.empty() )
			{
				D3D11_BLEND_DESC blend_desc;
				ZeroMemory( &blend_desc, sizeof( D3D11_BLEND_DESC ));

				// Create BLEND_DEFAULT.
				// No alpha.
				//
				for( UINT x = 0; x < 8; ++x )
				{
					blend_desc.RenderTarget[ x ].BlendEnable			= FALSE;
					blend_desc.RenderTarget[ x ].SrcBlend				= D3D11_BLEND_SRC_ALPHA;
					blend_desc.RenderTarget[ x ].DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;
					blend_desc.RenderTarget[ x ].BlendOp				= D3D11_BLEND_OP_ADD;
					blend_desc.RenderTarget[ x ].SrcBlendAlpha			= D3D11_BLEND_ONE;
					blend_desc.RenderTarget[ x ].DestBlendAlpha			= D3D11_BLEND_ZERO;
					blend_desc.RenderTarget[ x ].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
					blend_desc.RenderTarget[ x ].RenderTargetWriteMask	= 0x0F;
				}

				ID3D11BlendState* blend_state = NULL;
				HV_PTR( mCurrWindow->mDevice->CreateBlendState( &blend_desc, &blend_state ));
				mBlendState.push_back( blend_state );

				// Create BLEND_ALPHA.
				//
				for( UINT x = 0; x < 8; ++x )
				{
					blend_desc.RenderTarget[ x ].BlendEnable			= TRUE;
				}

				HV_PTR( mCurrWindow->mDevice->CreateBlendState( &blend_desc, &blend_state ));
				mBlendState.push_back( blend_state );
			
				// Create BLEND_PARTICLE.
				// Specially created to add transparency without regard to render order.
				//
				for( UINT x = 0; x < 8; ++x )
				{
					blend_desc.RenderTarget[ x ].SrcBlend				= D3D11_BLEND_SRC_ALPHA;
					blend_desc.RenderTarget[ x ].DestBlend				= D3D11_BLEND_ONE;
					blend_desc.RenderTarget[ x ].SrcBlendAlpha			= D3D11_BLEND_SRC_ALPHA;
					blend_desc.RenderTarget[ x ].DestBlendAlpha			= D3D11_BLEND_ONE;
					blend_desc.RenderTarget[ x ].BlendOpAlpha			= D3D11_BLEND_OP_ADD;
					blend_desc.RenderTarget[ x ].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;
				}

				HV_PTR( mCurrWindow->mDevice->CreateBlendState( &blend_desc, &blend_state ));
				mBlendState.push_back( blend_state );
			}

			SetBlend( BLEND_ALPHA );

			// Create depth STENCIL_DEFAULT.
			//
			if( mDepthStencilState.empty() )
			{
				D3D11_DEPTH_STENCIL_DESC stencilDesc;
				stencilDesc.DepthEnable						= TRUE;
				stencilDesc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ALL;
				stencilDesc.DepthFunc						= D3D11_COMPARISON_LESS_EQUAL;

				stencilDesc.StencilEnable					= FALSE;
				stencilDesc.StencilReadMask					= 0xFF;
				stencilDesc.StencilWriteMask				= 0xFF;

				stencilDesc.FrontFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
				stencilDesc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_INCR;
				stencilDesc.FrontFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
				stencilDesc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

				stencilDesc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
				stencilDesc.BackFace.StencilDepthFailOp		= D3D11_STENCIL_OP_INCR;
				stencilDesc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
				stencilDesc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

				ID3D11DepthStencilState* stencilState = NULL;
				HV_PTR( mCurrWindow->mDevice->CreateDepthStencilState( &stencilDesc, &stencilState ));
				mDepthStencilState.push_back( stencilState );

				// Create depth STENCIL_PARTICLE.
				//
				stencilDesc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ZERO;
				stencilDesc.DepthFunc						= D3D11_COMPARISON_ALWAYS;
				HV_PTR( mCurrWindow->mDevice->CreateDepthStencilState( &stencilDesc, &stencilState ));
				mDepthStencilState.push_back( stencilState );

				// Create depth STENCIL_NO_ZBUFFER.
				//
				stencilDesc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ZERO;
				stencilDesc.DepthFunc						= D3D11_COMPARISON_LESS_EQUAL;
				HV_PTR( mCurrWindow->mDevice->CreateDepthStencilState( &stencilDesc, &stencilState ));
				mDepthStencilState.push_back( stencilState );
			}

			SetDepthStencilState( STENCIL_DEFAULT );

			return mCurrWindow;
		}

		void Shutdown()
		{
			if( mCurrWindow )
			{
				if( mCurrWindow->mSwapChain != NULL )
				{
					mCurrWindow->mSwapChain->SetFullscreenState( FALSE, NULL );

					SAFE_RELEASE_PTR( mCurrWindow->mSwapChain );
					SAFE_RELEASE_PTR( mCurrWindow->mRasterizerState );
					SAFE_RELEASE_PTR( mCurrWindow->mContext );
				}
			}
		}

		// Windows.
		//
		void SetWindow( WindowInfo* info )
		{
			_ASSERT( info );

			mCurrWindow = (WindowInfoDX*)info;
		}

		WindowInfo* GetWindow()
		{
			return mCurrWindow;
		}

		bool ShareResources( WindowInfo* info0, WindowInfo* info1 )
		{
			//((WindowInfoDX*)info0)->
			/*IDXGIResource* res = NULL;

			if( ((WindowInfoDX*)info0)->mContext->QueryInterface( __uuidof(IDXGIResource), (void**)&res ) != S_OK )
				return false;

			HWND hWnd;
			if( res->GetSharedHandle( &hWnd );*/
			return false;
		}

		// Buffers.
		//
		Buffer* CreateBuffer( void* data, UINT size, UINT stride, BufferType type, BufferAccess access )
		{
			return new BufferDX( mCurrWindow->mDevice, mCurrWindow->mContext, data, size, stride, type, access );
		}

		void SetVBO( Buffer* buffer )
		{
			_ASSERT( mCurrWindow );

			UINT offset = 0;
			UINT stride = buffer->Stride();

			mCurrWindow->mContext->IASetVertexBuffers( 0, 1, &static_cast< BufferDX* >(buffer)->mBuffer, &stride, &offset );
		}

		void SetIBO( Buffer* buffer )
		{
			_ASSERT( mCurrWindow );

			mCurrWindow->mContext->IASetIndexBuffer( static_cast< BufferDX* >(buffer)->mBuffer, DXGI_FORMAT_R32_UINT, 0 );
		}

		// Textures.
		//
		std::shared_ptr< Texture > CreateTextureFromFile( const char* filename, bool createMips = true )
		{
			// TODO: Check for compatible texture size.

			_ASSERT( strlen( filename ) > 0 );
				
			// DDS files are loaded from DirectX.
			// stbi does not support them.
			//
			int count = strlen( filename )-1;
			if( toupper(filename[ count-2 ]) == 'D' && toupper(filename[ count-1 ]) == 'D' && toupper(filename[ count ]) == 'S' )
			{
				D3DX11_IMAGE_LOAD_INFO info;
				ID3D11ShaderResourceView* image;

				if( D3DX11CreateShaderResourceViewFromFileA( mCurrWindow->mDevice, filename, &info, NULL, &image, NULL ) == S_OK )
				{
					return std::shared_ptr< Texture >( new TextureDX( info.Width, info.Height, 0, image ));
				}
				else
				{
					REPORT_ERROR( "Failed to load " << filename, "Texture Load Failure" );
					return NULL;
				}
			}

			// Load texture for 32-bits.
			//
			int width, height;
			int nChannels;
			unsigned char* pixels = stbi_load( filename, &width, &height, &nChannels, 4 );

			if( pixels == NULL )
			{
				REPORT_ERROR( filename << " failed to load.", "Texture load failure." );
				return NULL;
			}

			std::shared_ptr< Texture > texture = CreateTextureFromMemory( pixels, width, height, IMAGE_FORMAT_RGBA, createMips );

			stbi_image_free( pixels );

			return texture;
		}

		std::shared_ptr< Texture > CreateTextureFromMemory( void* data, UINT width, UINT height, ImageFormatType format, bool createMips = true )
		{
			UCHAR* newData = NULL;
			
			DXGI_FORMAT newFormat;
			switch( format )
			{
				// The texture must be converted to RGBA.
				//
				case IMAGE_FORMAT_RGB:
				{
					UINT prevSize = width * height;
					newData = (UCHAR*)malloc( (width * height)<<2 );

					for( UINT i = 0; i < prevSize; ++i )
					{
						for( UINT j = 0; j < 3; ++j )
							newData[ (i<<2)+j ] = reinterpret_cast<UCHAR*>(data)[ i*3+j ];

						newData[ (i<<2)+3 ] = (UCHAR)(255);
					}

					newFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
				}
					break;

				case IMAGE_FORMAT_RGBA:
					newData = (UCHAR*)data;
					newFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
					break;

				case IMAGE_FORMAT_R:
					newData = (UCHAR*)data;
					newFormat = DXGI_FORMAT_R32_FLOAT;
					break;

				case IMAGE_FORMAT_HDR:
					newData = (UCHAR*)data;
					newFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
					break;

				default:
					_ASSERT( false );	// Invalid image format.
					break;
			}

			D3D11_TEXTURE2D_DESC desc;
			desc.Width			= width;
			desc.Height			= height;
			desc.MipLevels		= createMips ? 0 : 1;
			desc.ArraySize		= 1;
			desc.Format			= newFormat;
			desc.SampleDesc		= mSampleDesc;
			desc.Usage			= D3D11_USAGE_DEFAULT;
			desc.BindFlags		= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = D3D11_USAGE_DEFAULT;
			desc.MiscFlags		= createMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

			ID3D11Texture2D* tex0;
			HV_PTR( mCurrWindow->mDevice->CreateTexture2D( &desc, NULL, &tex0 ));
			
			D3D11_SHADER_RESOURCE_VIEW_DESC rsv =
			{
				newFormat,
				D3D_SRV_DIMENSION_TEXTURE2D
			};
			rsv.Texture2D.MipLevels			= 1;
			rsv.Texture2D.MostDetailedMip	= 0;

			// Create mipmaps.
			//
			if( createMips )
			{
				D3D11_SUBRESOURCE_DATA res =
				{
					newData,
					width<<2, 0
				};

				rsv.Texture2D.MipLevels = UINT_MAX;

				desc.MipLevels		= 1;
				desc.Usage			= D3D11_USAGE_STAGING;
				desc.BindFlags		= 0;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				desc.MiscFlags		= 0;

				ID3D11Texture2D* tex1;
				HV_PTR( mCurrWindow->mDevice->CreateTexture2D( &desc, &res, &tex1 ));
			
				D3D11_BOX box =
				{
					0, 0, 0,
					width, height, 1
				};

				mCurrWindow->mContext->CopySubresourceRegion( tex0, 0, 0, 0, 0, tex1, 0, &box );
				SAFE_RELEASE_PTR( tex1 );
			}
			
			ID3D11ShaderResourceView* resource;

			HV_PTR( mCurrWindow->mDevice->CreateShaderResourceView( tex0, &rsv, &resource ));
			mCurrWindow->mContext->GenerateMips( resource );
			
			if( format == IMAGE_FORMAT_RGB )
				free( newData );

			#ifndef NDEBUG
				SET_DEBUG_NAME( tex0 );
				SET_DEBUG_NAME( resource );
			#endif

			return std::shared_ptr< Texture >( new TextureDX( width, height, tex0, resource ));
		}

		void* GetTexturePixels( std::shared_ptr< Texture > texture )
		{
			_ASSERT( mCurrWindow );

			TextureDX* texDX = static_cast< TextureDX* >(texture.get());

			ID3D11Texture2D* texCopy = NULL;

			D3D11_TEXTURE2D_DESC desc;
			texDX->mTexture->GetDesc( &desc );
			desc.MipLevels		= 0;
			desc.BindFlags		= 0;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
			desc.Usage			= D3D11_USAGE_STAGING;
			desc.MiscFlags		= D3D11_USAGE_DEFAULT;

			HRESULT hr = mCurrWindow->mDevice->CreateTexture2D( &desc, NULL, &texCopy );
			if( texCopy )
			{
				mCurrWindow->mContext->CopyResource( texCopy, texDX->mTexture );

				D3D11_MAPPED_SUBRESOURCE res;
				UINT subres = D3D11CalcSubresource( 0, 0, 0 );

				HV_PTR( mCurrWindow->mContext->Map( texCopy, subres, D3D11_MAP_READ_WRITE, 0, &res ));
				
				UINT  size = (texDX->Width() << 2) * texDX->Height();
				BYTE* data = new BYTE[ size ];

				memcpy( data, res.pData, size );

				mCurrWindow->mContext->Unmap( texCopy, 0 );

				return data;
			}

			return NULL;
		}

		// Special Rendering.
		//
		UINT CreateBackbuffer()
		{
			_ASSERT( mCurrWindow );

			ID3D11Texture2D *tex = NULL;

			HV_PTR( mCurrWindow->mSwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (void**)&tex ));

			ID3D11RenderTargetView* rendertarget = NULL;
			
			if( mCurrWindow->mDevice->CreateRenderTargetView( tex, NULL, &rendertarget ) != S_OK )
				return -1;

			mRenderTarget[ 0 ] = rendertarget;

			return 0;
		}

		UINT CreateRenderTarget( ID3D11Texture2D *backbuffer )
		{
			_ASSERT( mCurrWindow );

			ID3D11RenderTargetView* rendertarget = NULL;
			
			HV( mCurrWindow->mDevice->CreateRenderTargetView( backbuffer, NULL, &rendertarget ));
			mRenderTarget.push_back( rendertarget );
			
			return mRenderTarget.size()-1;
		}

		UINT CreateRenderTarget( std::shared_ptr< Texture > texture )
		{
			return CreateRenderTarget( static_cast< TextureDX* >(texture.get())->mTexture );
		}

		UINT CreateDepthStencil( UINT width, UINT height )
		{
			_ASSERT( mCurrWindow );

			D3D11_TEXTURE2D_DESC texDesc =
			{
				width, height,
				1, 1,
				DXGI_FORMAT_D32_FLOAT,
				mSampleDesc,
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_DEPTH_STENCIL,
				0, 0
			};

			ID3D11Texture2D *stencil = NULL;
			ID3D11DepthStencilView* stenciltarget = NULL;

			HV( mCurrWindow->mDevice->CreateTexture2D( &texDesc, NULL, &stencil ));
			HV( mCurrWindow->mDevice->CreateDepthStencilView( stencil, NULL, &stenciltarget ));

			mDepthStencil.push_back( stenciltarget );
			SAFE_RELEASE_PTR( stencil );

			#ifndef NDEBUG
				SET_DEBUG_NAME( stenciltarget );
			#endif

			return mDepthStencil.size()-1;
		}

		UINT ResizeBuffers( UINT width, UINT height )
		{
			mCurrWindow->mWidth			= width;
			mCurrWindow->mHeight		= height;

			mCurrWindow->mWidthRatio	= (float)width  / (float)WINDOW_WIDTH_BASE;
			mCurrWindow->mHeightRatio	= (float)height / (float)WINDOW_HEIGHT_BASE;

			mCurrWindow->mContext->OMSetRenderTargets( 0, 0, 0 );

			mRenderTarget[ 0 ]->Release();

			mCurrWindow->mSwapChain->ResizeBuffers( 1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );

			CreateBackbuffer();

			//TRACE( "Resized: (" << width << ", " << height << ")" );

			return 1;
		}

		void SetRenderType( PrimitiveType type )
		{
			_ASSERT( type < NUM_PRIMITIVES );

			mCurrWindow->mContext->IASetPrimitiveTopology( static_cast< D3D11_PRIMITIVE_TOPOLOGY >( PRIMITIVE[ type ] ));
		}

		void SetRenderTarget( UINT target )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( mCurrStencil );
			_ASSERT( target < mRenderTarget.size() );

			// Unbind render targets.
			//
			mCurrWindow->mContext->OMSetRenderTargets( 0, 0, 0 );

			ID3D11ShaderResourceView* nullViews[ 8 ] = { 0, 0, 0, 0, 0, 0, 0, 0 };
			mCurrWindow->mContext->PSSetShaderResources( 0, 8, nullViews );

			mCurrTarget = mRenderTarget[ target ];
			mCurrWindow->mContext->OMSetRenderTargets( 1, &mCurrTarget, mCurrStencil );
		}

		void SetDepthStencil( UINT stencil )
		{
			_ASSERT( stencil < mDepthStencil.size() );

			mCurrStencil = mDepthStencil[ stencil ];
		}

		void SetDepthStencilState( StencilType state )
		{
			_ASSERT( mCurrWindow );
			
			mCurrWindow->mContext->OMSetDepthStencilState( mDepthStencilState[ state ], 0 );
		}

		void SetViewport( int x, int y, UINT width, UINT height )
		{
			_ASSERT( mCurrWindow );

			RECT rect;
			GetClientRect( (HWND)mCurrWindow->mHandle, &rect );
			
			D3D11_VIEWPORT vp = 
			{
				static_cast< FLOAT >(x), static_cast< FLOAT >(y),
				(width != Renderer::WINDOW_WIDTH_BASE) ? static_cast< FLOAT >(Renderer::WINDOW_WIDTH_BASE) * (static_cast< FLOAT >(rect.right) / static_cast< FLOAT >(width)) : static_cast< FLOAT >(Renderer::WINDOW_WIDTH_BASE),
				(height != Renderer::WINDOW_HEIGHT_BASE) ? static_cast< FLOAT >(Renderer::WINDOW_HEIGHT_BASE) * (static_cast< FLOAT >(rect.bottom) / static_cast< FLOAT >(height)) : static_cast< FLOAT >(Renderer::WINDOW_HEIGHT_BASE),
				0.0f, 1.0f
			};

			mCurrWindow->mContext->RSSetViewports( 1, &vp );
		}

		UINT SetCull( CullType type )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( type < NUM_CULL_TYPES );

			D3D11_RASTERIZER_DESC rasterizerstate;
			mCurrWindow->mRasterizerState->GetDesc( &rasterizerstate );

			SAFE_RELEASE_PTR( mCurrWindow->mRasterizerState );
			rasterizerstate.CullMode = (D3D11_CULL_MODE)CULL_TYPE[ type ];
			
			HV( mCurrWindow->mDevice->CreateRasterizerState( &rasterizerstate, &mCurrWindow->mRasterizerState ));
			mCurrWindow->mContext->RSSetState( mCurrWindow->mRasterizerState );

			return S_OK;
		}

		UINT SetFill( FillType type )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( type < NUM_FILL_TYPES );

			D3D11_RASTERIZER_DESC rasterizerstate;
			mCurrWindow->mRasterizerState->GetDesc( &rasterizerstate );

			SAFE_RELEASE_PTR( mCurrWindow->mRasterizerState );
			rasterizerstate.FillMode = (D3D11_FILL_MODE)FILL_TYPE[ type ];
			
			HV( mCurrWindow->mDevice->CreateRasterizerState( &rasterizerstate, &mCurrWindow->mRasterizerState ));
			mCurrWindow->mContext->RSSetState( mCurrWindow->mRasterizerState );

			return S_OK;
		}

		void SetBlend( BlendType type )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( type < NUM_BLEND_TYPES );

			float blend_factor[ 4 ] = { 1, 1, 1, 1 };
			mCurrWindow->mContext->OMSetBlendState( mBlendState[ type ], blend_factor, 0xFFFFFFFF );
		}

		// Shaders.
		//
		std::shared_ptr< Shader > CreateShaderFromFile( const char* filename )
		{
			ShaderDX* shader = new ShaderDX( mCurrWindow->mDevice, mCurrWindow->mContext );

			if( shader->CreateFromFile( filename ) != S_OK )
			{
				delete shader;
				return NULL;
			}

			return std::shared_ptr< Shader >( shader );
		}

		std::shared_ptr< Shader > CreateShaderFromMemory( const char* source )
		{
			ShaderDX* shader = new ShaderDX( mCurrWindow->mDevice, mCurrWindow->mContext );

			if( shader->CreateFromMemory( const_cast< char* >(source) ) != S_OK )
			{
				delete shader;
				return NULL;
			}

			return std::shared_ptr< Shader >( shader );
		}

		void SetShader( const std::shared_ptr< Shader >& shader )
		{
			mCurrShader = shader;

			mCurrShader->Enable();
		}

		// Vertex Layout.
		//
		std::shared_ptr< VertexLayout > CreateVertexLayout( const std::vector< AttributeType >& attrib )
		{
			VertexLayoutDX* layout = new VertexLayoutDX();

			UINT size = (UINT)attrib.size();
			_ASSERT( size > 0 );

			for( UINT x = 0; x < size; ++x )
			{
				layout->AddAttribute( attrib[ x ] );
			}

			layout->Create( mCurrWindow->mDevice );

			return std::shared_ptr< VertexLayout >(layout);
		}

		void SetVertexLayout( std::shared_ptr< VertexLayout > vertexLayout )
		{
			_ASSERT( vertexLayout );

			mCurrWindow->mContext->IASetInputLayout( static_cast< const VertexLayoutDX* >(vertexLayout.get())->mInputLayout );
		}

		// Rendering.
		//
		void Clear( float* color )
		{
			_ASSERT( mCurrWindow );

			mCurrWindow->mContext->ClearRenderTargetView( mCurrTarget, color );
			mCurrWindow->mContext->ClearDepthStencilView( mCurrStencil, D3D11_CLEAR_DEPTH, 1.0f, 0 );
		}

		void DrawIndexed( UINT count, UINT startIndex, UINT baseVertex )
		{
			_ASSERT( mCurrWindow );

			mCurrWindow->mContext->DrawIndexed( count, startIndex, baseVertex );

			mCurrShader->Disable();
		}

		void Present()
		{
			_ASSERT( mCurrWindow );

			mCurrWindow->mSwapChain->Present( 0, 0 );
		}
	};

	const UINT RendererDX::PRIMITIVE[] = 
		{ D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		  D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		  D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	const UINT RendererDX::CULL_TYPE[] = 
		{ D3D11_CULL_NONE,
		  D3D11_CULL_BACK,
		  D3D11_CULL_FRONT };

	const UINT RendererDX::FILL_TYPE[] = 
		{ D3D11_FILL_SOLID,
		  D3D11_FILL_WIREFRAME };

	Renderer* BuildRendererDX()
	{
		return new RendererDX();
	}
}