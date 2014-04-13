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

		static const D3D11_PRIMITIVE_TOPOLOGY	PRIMITIVE[ PrimitiveFormat::COUNT ];
		static const D3D11_CULL_MODE			CULL_TYPE[ CullFormat::COUNT ];
		static const D3D11_FILL_MODE			FILL_TYPE[ FillFormat::COUNT ];
		static const D3D11_BLEND				BLEND_TYPE[ BlendFormat::COUNT ];
		static const D3D11_BLEND_OP				BLEND_FUNC_TYPE[ BlendFunction::COUNT ];

		class WindowInfoDX : public WindowInfo
		{
			friend class RendererDX;

		public:

			ID3D11Device*						mDevice;
			ID3D11DeviceContext*				mContext;
			IDXGISwapChain*						mSwapChain;
			ID3D11RasterizerState*				mRasterizerState;
		};

		//////////////////////////////////

		class RenderTextureDX : public RenderTexture
		{
		public:

			ID3D11RenderTargetView* mView;

			RenderTextureDX( ID3D11RenderTargetView* view ) :
				mView( view )
			{}

			~RenderTextureDX()
			{
				mView->Release();
			}
		};

		//////////////////////////////////

		class DepthStencilDX : public DepthStencil
		{
		public:

			ID3D11DepthStencilView* mView;

			DepthStencilDX( ID3D11DepthStencilView* view ) :
				mView( view )
			{}

			~DepthStencilDX()
			{
				mView->Release();
			}
		};

		//////////////////////////////////

		class BlendStateDX : public BlendState
		{
			friend class RendererDX;

		public:

			ID3D11BlendState* mBlendState;

			//////////////////////////////////////////////////

			BlendStateDX( ID3D11BlendState* blendState,
						  BlendFormat::Type srcBlendColor, BlendFormat::Type dstBlendColor,
						  BlendFormat::Type srcBlendAlpha, BlendFormat::Type dstBlendAlpha,
						  BlendFunction::Type blendFuncColor, BlendFunction::Type blendFuncAlpha) :
				mBlendState( blendState ),
				BlendState( srcBlendColor, dstBlendColor, 
							srcBlendAlpha, dstBlendAlpha,
							blendFuncColor, blendFuncAlpha )
			{}

			~BlendStateDX()
			{
				SAFE_RELEASE_PTR( mBlendState );
			}
		};

		WindowInfoDX*							mCurrWindow;

		ID3D11DepthStencilView*					mCurrStencil;
		ID3D11DepthStencilState**				mDepthStencilState;
		ID3D11RenderTargetView*					mCurrTarget;

		DXGI_SAMPLE_DESC						mSampleDesc;

	public:
		
		RendererDX()
		{
			mSampleDesc.Count   = 1;
			mSampleDesc.Quality = 0;

			mCurrWindow  = NULL;
			mCurrStencil = NULL;
			mCurrTarget  = NULL;

			mDepthStencilState = new ID3D11DepthStencilState*[ DepthFormat::COUNT ];
		}

		~RendererDX()
		{
			for( UINT x = 0; x < DepthFormat::COUNT; ++x )
			{
				SAFE_RELEASE_PTR( mDepthStencilState[ x ] );
			}
			delete[] mDepthStencilState;
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
				{	width, height,
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

				NULL_TEXTURE = std::shared_ptr< Texture >(CreateTextureFromMemory( newTex, 1, 1, ImageFormat::RGBA ));
				
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

				BASE_TEXTURE = std::shared_ptr< Texture >(CreateTextureFromMemory( newTex, 1, 1, ImageFormat::RGBA ));
				
				delete newTex;
			}

			// Create BLEND_OFF.
			//
			D3D11_BLEND_DESC blendDesc;
			ZeroMemory( &blendDesc, sizeof( D3D11_BLEND_DESC ));

			for( UINT x = 0; x < 8; ++x )
			{
				blendDesc.RenderTarget[ x ].BlendEnable				= FALSE;

				blendDesc.RenderTarget[ x ].SrcBlend				= BLEND_TYPE[ BlendFormat::SRC_ALPHA ];
				blendDesc.RenderTarget[ x ].DestBlend				= BLEND_TYPE[ BlendFormat::ONE_MINUS_SRC_ALPHA ];
				blendDesc.RenderTarget[ x ].BlendOp					= BLEND_FUNC_TYPE[ BlendFunction::ADD ];

				blendDesc.RenderTarget[ x ].SrcBlendAlpha			= BLEND_TYPE[ BlendFormat::SRC_ALPHA ];
				blendDesc.RenderTarget[ x ].DestBlendAlpha			= BLEND_TYPE[ BlendFormat::ONE_MINUS_SRC_ALPHA ];
				blendDesc.RenderTarget[ x ].BlendOpAlpha			= BLEND_FUNC_TYPE[ BlendFunction::ADD ];

				blendDesc.RenderTarget[ x ].RenderTargetWriteMask	= 0x0F;
			}

			ID3D11BlendState* blendState = NULL;
			HV_PTR( mCurrWindow->mDevice->CreateBlendState( &blendDesc, &blendState ));
			BLEND_OFF = std::shared_ptr< BlendState >(new BlendStateDX( blendState, BlendFormat::ZERO, BlendFormat::ZERO, 
																					BlendFormat::ZERO, BlendFormat::ZERO, 
																					BlendFunction::ADD, BlendFunction::ADD ));

			// Create BLEND_ALPHA.
			//
			BLEND_ALPHA = std::shared_ptr< BlendState >(CreateBlendState());

			SetBlendState( BLEND_OFF.get() );

			// Create depth stencils.
			//
			D3D11_DEPTH_STENCIL_DESC stencilDesc;
			stencilDesc.DepthEnable						= TRUE;
			stencilDesc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ALL;
				
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

			stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
			HV_PTR( mCurrWindow->mDevice->CreateDepthStencilState( &stencilDesc, &stencilState ));
			mDepthStencilState[ DepthFormat::LESS ] = stencilState;

			stencilDesc.DepthFunc = D3D11_COMPARISON_EQUAL;
			HV_PTR( mCurrWindow->mDevice->CreateDepthStencilState( &stencilDesc, &stencilState ));
			mDepthStencilState[ DepthFormat::EQUAL ] = stencilState;

			stencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			HV_PTR( mCurrWindow->mDevice->CreateDepthStencilState( &stencilDesc, &stencilState ));
			mDepthStencilState[ DepthFormat::LEQUAL ] = stencilState;

			stencilDesc.DepthFunc = D3D11_COMPARISON_GREATER;
			HV_PTR( mCurrWindow->mDevice->CreateDepthStencilState( &stencilDesc, &stencilState ));
			mDepthStencilState[ DepthFormat::GREATER ] = stencilState;

			stencilDesc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
			HV_PTR( mCurrWindow->mDevice->CreateDepthStencilState( &stencilDesc, &stencilState ));
			mDepthStencilState[ DepthFormat::NOTEQUAL ] = stencilState;

			stencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
			HV_PTR( mCurrWindow->mDevice->CreateDepthStencilState( &stencilDesc, &stencilState ));
			mDepthStencilState[ DepthFormat::GEQUAL ] = stencilState;

			stencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			HV_PTR( mCurrWindow->mDevice->CreateDepthStencilState( &stencilDesc, &stencilState ));
			mDepthStencilState[ DepthFormat::ALWAYS ] = stencilState;

			stencilDesc.DepthEnable = FALSE;
			HV_PTR( mCurrWindow->mDevice->CreateDepthStencilState( &stencilDesc, &stencilState ));
			mDepthStencilState[ DepthFormat::OFF ] = stencilState;
			
			SetDepthStencilType( DepthFormat::LEQUAL );

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
		Buffer* CreateBuffer( void* data, UINT size, UINT stride, BufferFormat::Type type, BufferAccess::Type access )
		{
			return new BufferDX( mCurrWindow->mDevice, mCurrWindow->mContext, data, size, stride, type, access );
		}

		void SetVertexBuffer( Buffer* buffer )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( mCurrWindow->mContext );

			UINT offset = 0;
			UINT stride = buffer->Stride();

			_ASSERT( stride > 0 );

			mCurrWindow->mContext->IASetVertexBuffers( 0, 1, &static_cast< BufferDX* >(buffer)->mBuffer, &stride, &offset );
		}

		void SetIndexBuffer( Buffer* buffer )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( mCurrWindow->mContext );

			mCurrWindow->mContext->IASetIndexBuffer( static_cast< BufferDX* >(buffer)->mBuffer, DXGI_FORMAT_R32_UINT, 0 );
		}

		// Textures.
		//
		Texture* CreateTextureFromFile( const char* filename, bool createMips = true )
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
					return new TextureDX( info.Width, info.Height, ImageFormat::RGBA, 0, image );
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

			Texture* texture = CreateTextureFromMemory( pixels, width, height, ImageFormat::RGBA, createMips );

			stbi_image_free( pixels );

			return texture;
		}

		Texture* CreateTextureFromMemory( void* data, UINT width, UINT height, ImageFormat::Type format, bool createMips = true )
		{
			UCHAR* newData = NULL;
			
			DXGI_FORMAT newFormat;
			switch( format )
			{
				// The texture must be converted to RGBA.
				//
				case ImageFormat::RGB:
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

				case ImageFormat::RGBA:
					newData = (UCHAR*)data;
					newFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
					break;

				case ImageFormat::R:
					newData = (UCHAR*)data;
					newFormat = DXGI_FORMAT_R32_FLOAT;
					break;

				case ImageFormat::HDR:
					newData = (UCHAR*)data;
					newFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
					break;

				case ImageFormat::DEPTH:
					newData = (UCHAR*)data;
					newFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS ;
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
			desc.BindFlags		= ((format != ImageFormat::DEPTH ) ? D3D11_BIND_RENDER_TARGET : D3D11_BIND_DEPTH_STENCIL) | D3D11_BIND_SHADER_RESOURCE;
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
			
			if( format == ImageFormat::RGB )
				free( newData );

			#ifndef NDEBUG
				SET_DEBUG_NAME( tex0 );
				SET_DEBUG_NAME( resource );
			#endif

			return new TextureDX( width, height, format, tex0, resource );
		}

		Texture* CreateTextureCube( UINT width, UINT height, ImageFormat::Type format )
		{
			_ASSERT(0);
			return 0;
		}

		void* GetTexturePixels( Texture* texture )
		{
			_ASSERT( mCurrWindow );

			TextureDX* texDX = static_cast< TextureDX* >(texture);

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
		RenderTexture* CreateBackbuffer()
		{
			_ASSERT( mCurrWindow );

			ID3D11Texture2D *tex = NULL;

			HV_PTR( mCurrWindow->mSwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (void**)&tex ));

			return CreateRenderTexture( tex );
		}

		RenderTexture* CreateRenderTexture( ID3D11Texture2D* backbuffer )
		{
			_ASSERT( mCurrWindow );

			ID3D11RenderTargetView* view = NULL;
			
			HV_PTR( mCurrWindow->mDevice->CreateRenderTargetView( backbuffer, NULL, &view ));
			
			return new RenderTextureDX( view );
		}

		RenderTexture* CreateRenderTexture( Texture* texture )
		{
			_ASSERT( texture );
			
			return CreateRenderTexture( static_cast< TextureDX* >(texture)->mTexture );
		}

		DepthStencil* CreateDepthStencil( UINT width, UINT height )
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
			ID3D11DepthStencilView* view = NULL;

			HV_PTR( mCurrWindow->mDevice->CreateTexture2D( &texDesc, NULL, &stencil ));
			HV_PTR( mCurrWindow->mDevice->CreateDepthStencilView( stencil, NULL, &view ));

			SAFE_RELEASE_PTR( stencil );

			//#ifndef NDEBUG
			//	SET_DEBUG_NAME( view );
			//#endif

			return new DepthStencilDX( view );
		}

		BlendState* CreateBlendState( BlendFormat::Type srcBlendColor = BlendFormat::SRC_ALPHA, BlendFormat::Type dstBlendColor = BlendFormat::ONE_MINUS_SRC_ALPHA, 
									  BlendFormat::Type srcBlendAlpha = BlendFormat::SRC_ALPHA, BlendFormat::Type dstBlendAlpha = BlendFormat::ONE_MINUS_SRC_ALPHA, 
									  BlendFunction::Type blendFuncColor = BlendFunction::ADD, BlendFunction::Type blendFuncAlpha = BlendFunction::ADD )
		{
			D3D11_BLEND_DESC blendDesc;
			ZeroMemory( &blendDesc, sizeof( D3D11_BLEND_DESC ));

			for( UINT x = 0; x < 8; ++x )
			{
				blendDesc.RenderTarget[ x ].BlendEnable				= TRUE;

				blendDesc.RenderTarget[ x ].SrcBlend				= BLEND_TYPE[ srcBlendColor ];
				blendDesc.RenderTarget[ x ].DestBlend				= BLEND_TYPE[ dstBlendColor ];
				blendDesc.RenderTarget[ x ].BlendOp					= BLEND_FUNC_TYPE[ blendFuncColor ];

				blendDesc.RenderTarget[ x ].SrcBlendAlpha			= BLEND_TYPE[ srcBlendAlpha ];
				blendDesc.RenderTarget[ x ].DestBlendAlpha			= BLEND_TYPE[ dstBlendAlpha ];
				blendDesc.RenderTarget[ x ].BlendOpAlpha			= BLEND_FUNC_TYPE[ blendFuncAlpha ];

				blendDesc.RenderTarget[ x ].RenderTargetWriteMask	= 0x0F;
			}

			ID3D11BlendState* blendState = NULL;
			HV_PTR( mCurrWindow->mDevice->CreateBlendState( &blendDesc, &blendState ));
			
			return new BlendStateDX( blendState, \
									 srcBlendColor, dstBlendColor, \
									 srcBlendAlpha, dstBlendAlpha, \
									 blendFuncColor, blendFuncAlpha );
		}

		UINT ResizeBuffers( UINT width, UINT height )
		{
			mCurrWindow->mWidth			= width;
			mCurrWindow->mHeight		= height;

			mCurrWindow->mWidthRatio	= (float)width  / (float)WINDOW_WIDTH_BASE;
			mCurrWindow->mHeightRatio	= (float)height / (float)WINDOW_HEIGHT_BASE;

			/*
			mCurrWindow->mContext->OMSetRenderTextures( 0, 0, 0 );

			mRenderTexture[ 0 ]->Release();

			mCurrWindow->mSwapChain->ResizeBuffers( 1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );

			CreateBackbuffer();

			//TRACE( "Resized: (" << width << ", " << height << ")" );
			*/
			return 1;
		}

		void SetRenderTexture( RenderTexture* target )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( mCurrStencil );
			_ASSERT( target );

			//mCurrWindow->mContext->OMSetRenderTextures( 0, 0, 0 );

			//ID3D11ShaderResourceView* nullViews[ 8 ] = { 0, 0, 0, 0, 0, 0, 0, 0 };
			//mCurrWindow->mContext->PSSetShaderResources( 0, 8, nullViews );

			mCurrTarget = static_cast< RenderTextureDX* >(target)->mView;

			mCurrWindow->mContext->OMSetRenderTargets( 1, &mCurrTarget, mCurrStencil );
		}

		void SetDepthStencil( DepthStencil* stencil )
		{
			_ASSERT( stencil );

			mCurrStencil = static_cast< DepthStencilDX* >(stencil)->mView;
		}

		void SetDepthStencilType( DepthFormat::Type state )
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

		UINT SetCull( CullFormat::Type type )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( type < CullFormat::COUNT );

			D3D11_RASTERIZER_DESC rasterizerstate;
			mCurrWindow->mRasterizerState->GetDesc( &rasterizerstate );

			SAFE_RELEASE_PTR( mCurrWindow->mRasterizerState );
			rasterizerstate.CullMode = CULL_TYPE[ type ];
			
			HV( mCurrWindow->mDevice->CreateRasterizerState( &rasterizerstate, &mCurrWindow->mRasterizerState ));
			mCurrWindow->mContext->RSSetState( mCurrWindow->mRasterizerState );

			return S_OK;
		}

		UINT SetFill( FillFormat::Type type )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( type < FillFormat::COUNT );

			D3D11_RASTERIZER_DESC rasterizerstate;
			mCurrWindow->mRasterizerState->GetDesc( &rasterizerstate );

			SAFE_RELEASE_PTR( mCurrWindow->mRasterizerState );
			rasterizerstate.FillMode = FILL_TYPE[ type ];
			
			HV( mCurrWindow->mDevice->CreateRasterizerState( &rasterizerstate, &mCurrWindow->mRasterizerState ));
			mCurrWindow->mContext->RSSetState( mCurrWindow->mRasterizerState );

			return S_OK;
		}

		void SetBlendState( BlendState* blend )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( mCurrWindow->mContext );
			_ASSERT( blend );

			float blendFactor[ 4 ] = { 1, 1, 1, 1 };

			mCurrWindow->mContext->OMSetBlendState( static_cast< BlendStateDX* >(blend)->mBlendState, blendFactor, 0xFFFFFFFF );
		}

		// Shaders.
		//
		Shader* CreateShaderFromFile( const char* filename )
		{
			ShaderDX* shader = new ShaderDX( mCurrWindow->mDevice, mCurrWindow->mContext );

			if( shader->CreateFromFile( filename ) != S_OK )
			{
				delete shader;
				return NULL;
			}

			return shader;
		}

		Shader* CreateShaderFromMemory( const char* source )
		{
			ShaderDX* shader = new ShaderDX( mCurrWindow->mDevice, mCurrWindow->mContext );

			if( shader->CreateFromMemory( const_cast< char* >(source) ) != S_OK )
			{
				delete shader;
				return NULL;
			}

			return shader;
		}

		// Vertex Layout.
		//
		VertexLayout* CreateVertexLayout( const std::vector< VertexAttribute::Type >& attrib )
		{
			VertexLayoutDX* layout = new VertexLayoutDX();

			UINT size = (UINT)attrib.size();
			_ASSERT( size > 0 );

			for( UINT x = 0; x < size; ++x )
			{
				layout->AddAttribute( attrib[ x ] );
			}

			if( layout->Create( mCurrWindow->mDevice ) == S_FALSE )
			{
				delete layout;
				return NULL;
			}

			return layout;
		}

		void SetVertexLayout( VertexLayout* vertexLayout )
		{
			_ASSERT( vertexLayout );

			mCurrWindow->mContext->IASetInputLayout( static_cast< const VertexLayoutDX* >(vertexLayout)->mInputLayout );
		}

		// Rendering.
		//
		void Clear( float* color, float depth )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( mCurrWindow->mContext );
			_ASSERT( mCurrTarget );
			_ASSERT( mCurrStencil );

			ClearColor( color );
			ClearDepth( depth );
		}

		void ClearColor( float* color )
		{
			mCurrWindow->mContext->ClearRenderTargetView( mCurrTarget, color );
		}

		void ClearDepth( float depth )
		{
			mCurrWindow->mContext->ClearDepthStencilView( mCurrStencil, D3D11_CLEAR_DEPTH, depth, 0 );
		}

		void Draw( PrimitiveFormat::Type primitive, UINT count, UINT baseVertex )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( mCurrWindow->mContext );
			_ASSERT( mCurrShader );

			mCurrWindow->mContext->IASetPrimitiveTopology( PRIMITIVE[ primitive ] );
			mCurrWindow->mContext->Draw( count, baseVertex );
		}

		void DrawIndexed( PrimitiveFormat::Type primitive, UINT count, UINT startIndex, UINT baseVertex )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( mCurrWindow->mContext );
			_ASSERT( mCurrShader );

			mCurrWindow->mContext->IASetPrimitiveTopology( PRIMITIVE[ primitive ] );
			mCurrWindow->mContext->DrawIndexed( count, startIndex, baseVertex );
		}

		void Present()
		{
			_ASSERT( mCurrWindow );

			mCurrWindow->mSwapChain->Present( 0, 0 );
		}
	};

	const D3D11_PRIMITIVE_TOPOLOGY RendererDX::PRIMITIVE[] = 
		{ D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		  D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		  D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	const D3D11_CULL_MODE RendererDX::CULL_TYPE[] = 
		{ D3D11_CULL_NONE,
		  D3D11_CULL_BACK,
		  D3D11_CULL_FRONT };

	const D3D11_FILL_MODE RendererDX::FILL_TYPE[] = 
		{ D3D11_FILL_SOLID,
		  D3D11_FILL_WIREFRAME };

	const D3D11_BLEND RendererDX::BLEND_TYPE[] =
		{ D3D11_BLEND_ZERO,
		  D3D11_BLEND_ONE,

		  D3D11_BLEND_SRC_COLOR,
		  D3D11_BLEND_INV_SRC_COLOR,
		  D3D11_BLEND_SRC_ALPHA,
		  D3D11_BLEND_INV_SRC_ALPHA,

		  D3D11_BLEND_DEST_COLOR,
		  D3D11_BLEND_INV_DEST_COLOR,
		  D3D11_BLEND_DEST_ALPHA,
		  D3D11_BLEND_INV_DEST_ALPHA };

	const D3D11_BLEND_OP RendererDX::BLEND_FUNC_TYPE[] =
		{ D3D11_BLEND_OP_ADD,
		  D3D11_BLEND_OP_SUBTRACT,
		  D3D11_BLEND_OP_REV_SUBTRACT,
		  D3D11_BLEND_OP_MIN,
		  D3D11_BLEND_OP_MAX };

	Renderer* BuildRendererDX()
	{
		return new RendererDX();
	}
}