#include "Renderer.h"

#include <d3d11.h>
#include <d3dx11.h>
#include <D3D11Shader.h>
#include <D3DX11Effect.h>

#ifndef NDEBUG
	#include <D3DCommon.h>
	#pragma comment (lib, "dxguid.lib") 

	#define SET_DEBUG_NAME( device )\
	{\
		const char name[] = #device;\
		device->SetPrivateData( WKPDID_D3DDebugObjectName, sizeof( name ) - 1, name );\
	}
#endif

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "effects11.lib")

#include <vector>
#include <crtdbg.h>

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"

namespace Sentinel
{
	#define HV( func )\
		if( (func) != S_OK )\
			return S_FALSE;

	#define HV_PTR( func )\
		if( (func) != S_OK )\
			return NULL;

	////////////////////////////////////////////////////////////////////////////////////

	class TextureDX : public Texture
	{
	public:

		ID3D11ShaderResourceView*	mResource;
		ID3D11Texture2D*			mTexture;

		TextureDX( std::string filename, UINT width, UINT height, UINT id )
		{
			mResource	= NULL;
			mTexture	= NULL;

			mFilename	= filename;
			mWidth		= width;
			mHeight		= height;
			mID			= id;
		}

		void Release()
		{
			SAFE_RELEASE_PTR( mTexture );
			SAFE_RELEASE_PTR( mResource );
		}
	};

	////////////////////////////////////////////////////////////////////////////////////
	
	class ShaderDX : public Shader
	{
	private:
		
		ID3D11Device*			mDevice;
		ID3D11DeviceContext*	mContext;

		ID3DX11Effect*			mEffect;
		ID3DX11EffectPass*		mPass;
		ID3D11InputLayout*		mLayout;

		std::vector< ID3DX11EffectVariable*	> mUniformDX;

	public:

		ShaderDX()
		{
			mDevice		= NULL;
			mContext	= NULL;

			mEffect		= NULL;
			mPass		= NULL;
			mLayout		= NULL;

			mVertexSize = 0;
		}

		~ShaderDX()
		{
			Release();
		}
			
		// Use the filename without the extension.
		// DirectX uses .fx files.
		//
		UINT Startup( std::string filename, const std::string& attrib, const std::string& uniform, 
					  ID3D11Device* device, ID3D11DeviceContext* context )
		{
			mAttribute  = attrib;
			mUniform	= uniform;

			mDevice		= device;
			mContext	= context;

			// Compile the shader and report errors.
			//
			D3DX11_PASS_DESC desc;
			ID3D10Blob* fxblob  = NULL;
			ID3D10Blob* errblob = NULL;

			filename.append( ".fx" );

			if( FAILED( D3DX11CompileFromFileA( filename.c_str(), NULL, NULL, "", "fx_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &fxblob, &errblob, NULL )))
			{
				if( errblob )
				{
					TRACE( (char*)errblob->GetBufferPointer() );
				}
				else
				{
					TRACE( filename.c_str() << " loaded" );
				}

				SAFE_RELEASE_PTR( errblob );
				SAFE_RELEASE_PTR( fxblob );

				return S_FALSE;
			}

			SAFE_RELEASE_PTR( errblob );
			HV( D3DX11CreateEffectFromMemory( fxblob->GetBufferPointer(), fxblob->GetBufferSize(), 0, mDevice, &mEffect ));
			SAFE_RELEASE_PTR( fxblob );

			mPass = mEffect->GetTechniqueByIndex( 0 )->GetPassByIndex( 0 );
			HV( mPass->GetDesc( &desc ));
			
			int len = attrib.size();

			// Extend the element length depending on the attribute type.
			//
			for( int i = 0; i < len; ++i )
			{
				if( attrib[ i ] == 'B' )
				{
					len += 2;
					break;
				}
				else
				if( attrib[ i ] == 'x' )
				{
					len += 3;
					break;
				}
			}

			D3D11_INPUT_ELEMENT_DESC *decl = new D3D11_INPUT_ELEMENT_DESC[ len ];
			memset( decl, 0, sizeof( D3D11_INPUT_ELEMENT_DESC ) * len );
			int semidx = 0;

			UINT attribIndex = 0;
			for( UINT i = 0; i < (UINT)attrib.size(); ++i )
			{
				D3D11_INPUT_ELEMENT_DESC &d = decl[ i ];
				d.AlignedByteOffset = mVertexSize;

				switch( attrib[ i ] )
				{
				// Position.
				//
				case 'P':
					d.SemanticName = "POSITION";
					d.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					mVertexSize += 12;
					break;

				// Normal.
				//
				case 'N':
					d.SemanticName = "NORMAL";
					d.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					mVertexSize += 12;
					break;

				// Color as UINT.
				//
				case 'C':
					d.SemanticName = "COLOR";
					d.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					mVertexSize += 4;
					break;

				// Texture Coordinates.
				//
				case 'X':
					d.SemanticName = "TEXCOORD";
					d.Format = DXGI_FORMAT_R32G32_FLOAT;
					d.SemanticIndex = semidx++;
					mVertexSize += 8;
					break;

				// Vertex Matrix.
				//
				case 'x':
					for( UINT x = 0; x < 4; ++x )
					{
						D3D11_INPUT_ELEMENT_DESC &d = decl[ attribIndex++ ];
						d.AlignedByteOffset = mVertexSize;
						d.SemanticName = "MATRIX";
						d.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
						d.SemanticIndex = x;
						mVertexSize += 16;
					}
					--attribIndex;
					break;

				// Tangent Vector with sign.
				//
				case 'T':
					d.SemanticName = "TANGENT";
					d.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					mVertexSize += 16;
					break;

				// Bones.
				//
				case 'B':
					{
					// Number of bones.
					//
					d.SemanticName = "NORMAL";
					d.Format = DXGI_FORMAT_R32_SINT;
					d.SemanticIndex = 1;
					mVertexSize += 4;

					{
					// Bone index.
					//
					D3D11_INPUT_ELEMENT_DESC &d = decl[ ++attribIndex ];
					d.AlignedByteOffset = mVertexSize;
					d.SemanticName = "BONE_INDEX";
					d.Format = DXGI_FORMAT_R32G32B32A32_SINT;
					mVertexSize += 16;
					}
						
					{
					// Weight.
					//
					D3D11_INPUT_ELEMENT_DESC &d = decl[ ++attribIndex ];
					d.AlignedByteOffset = mVertexSize;
					d.SemanticName = "BONE_WEIGHT";
					d.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					mVertexSize += 16;
					}
					}
					break;

				default:
					TRACE( attrib[ i ] << " = unknown Attribute" );
					_ASSERT(0);
				}

				++attribIndex;
			}

			HRESULT hr = ( mDevice->CreateInputLayout( decl, len, desc.pIAInputSignature, desc.IAInputSignatureSize, &mLayout ));

			delete[] decl;

			if( hr != S_OK  )
			{
				Release();
				return S_FALSE;
			}

			ProcessUniforms();

			TRACE( filename.c_str() << " Shader Created Successfully!" );

			return S_OK;
		}

		void ApplyLayout()
		{
			mContext->IASetInputLayout( mLayout );
		}

		void ApplyPass()
		{
			mPass->Apply( 0, mContext );
		}

		void Release()
		{
			mDevice		= NULL;
			mContext	= NULL;
			mPass		= NULL;

			SAFE_RELEASE_PTR( mEffect );
			SAFE_RELEASE_PTR( mLayout );

			mUniform.clear();
		}

		void SetFloat( UINT uniform, float data )
		{
			mUniformDX[ uniform ]->AsScalar()->SetFloat( data );
		}

		void SetFloat2( UINT uniform, float* data, UINT offset, UINT count )
		{
			SetVector( uniform, data, offset, count );
		}

		void SetFloat3( UINT uniform, float* data, UINT offset, UINT count )
		{
			SetVector( uniform, data, offset, count );
		}

		void SetFloat4( UINT uniform, float* data, UINT offset, UINT count )
		{
			SetVector( uniform, data, offset, count );
		}

		void SetVector( UINT uniform, float* data, UINT offset, UINT count )
		{
			if( count == 1 )
			{
				mUniformDX[ uniform ]->AsVector()->SetFloatVector( data );
			}
			else
			{
				mUniformDX[ uniform ]->AsVector()->SetFloatVectorArray( data, offset, count );
			}
		}

		void SetMatrix( UINT uniform, float* matrix, UINT offset, UINT count )
		{
			mUniformDX[ uniform ]->AsMatrix()->SetMatrixArray( matrix, offset, count );
		}

		void SetTexture( UINT uniform, Texture* texture )
		{
			mUniformDX[ uniform ]->AsShaderResource()->SetResource( static_cast< TextureDX* >(texture)->mResource );
		}

		void CreateUniform( const char* name )
		{
			mUniformDX.push_back( mEffect->GetVariableByName( name ));
		}
	};

	////////////////////////////////////////////////////////////////////////////////////

	class BufferDX : public Buffer
	{
	public:

		ID3D11DeviceContext*	mContext;
		ID3D11Buffer*			mBuffer;

		BufferDX()
		{
			mBuffer = NULL;
			mStride = 0;
		}

		void* Lock()
		{
			D3D11_MAPPED_SUBRESOURCE mapRes;
				
			mContext->Map( mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapRes );

			return mapRes.pData;
		}

		void Unlock()
		{
			mContext->Unmap( mBuffer, 0 );
		}

		void Release()
		{
			SAFE_RELEASE_PTR( mBuffer );
			mStride = 0;
		}
	};

	////////////////////////////////////////////////////////////////////////////////////
	// DirectX11 Renderer.
	//
	class RendererDX : public Renderer
	{
	private:

		class WindowInfoDX : public WindowInfo
		{
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

		std::vector< D3D11_VIEWPORT >			mViewport;
		std::vector< ID3D11BlendState* >		mBlendState;

		DXGI_SAMPLE_DESC						mSampleDesc;

		std::vector< TextureDX* >				mTexture;
		
	public:
		
		RendererDX()
		{
			PRIMITIVE[ POINT_LIST ]     = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
			PRIMITIVE[ LINE_LIST ]		= D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
			PRIMITIVE[ TRIANGLE_LIST ]	= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			CULL_TYPE[ CULL_NONE ]		= D3D11_CULL_NONE;
			CULL_TYPE[ CULL_CCW  ]		= D3D11_CULL_BACK;
			CULL_TYPE[ CULL_CW   ]		= D3D11_CULL_FRONT;

			mSampleDesc.Count   = 1;
			mSampleDesc.Quality = 0;
		}

		~RendererDX()
		{}

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
					{ 0, 0 },
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

			mCurrWindow->mFullscreen	= fullscreen;
			mCurrWindow->mWidth			= width;
			mCurrWindow->mHeight		= height;
			mCurrWindow->mWidthRatio	= (float)width  / (float)WINDOW_WIDTH_BASE;
			mCurrWindow->mHeightRatio	= (float)height / (float)WINDOW_HEIGHT_BASE;

			HV_PTR( D3D11CreateDeviceAndSwapChain( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createdeviceflags, 
												   featurelevels, sizeof(featurelevels)/sizeof(D3D_FEATURE_LEVEL),
												   D3D11_SDK_VERSION, &sd, &mCurrWindow->mSwapChain, 
												   &mCurrWindow->mDevice, 0, &mCurrWindow->mContext ));

			if( fullscreen )
			{
				HV_PTR( mCurrWindow->mSwapChain->ResizeBuffers( 1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ));
				HV_PTR( mCurrWindow->mSwapChain->SetFullscreenState( TRUE, NULL ));
			}

			D3D11_RASTERIZER_DESC rasterizerstate = { D3D11_FILL_SOLID, D3D11_CULL_BACK, true, 0, 0, 0, true, false, true, false };
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
					blend_desc.RenderTarget[ x ].DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;
					blend_desc.RenderTarget[ x ].SrcBlendAlpha			= D3D11_BLEND_SRC_ALPHA;
					blend_desc.RenderTarget[ x ].DestBlendAlpha			= D3D11_BLEND_ZERO;
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
				stencilDesc.DepthFunc						= D3D11_COMPARISON_LESS;

				stencilDesc.StencilEnable					= FALSE;
				stencilDesc.StencilReadMask					= 0xFF;
				stencilDesc.StencilWriteMask				= 0xFF;

				stencilDesc.FrontFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
				stencilDesc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_INCR;
				stencilDesc.FrontFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
				stencilDesc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

				stencilDesc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
				stencilDesc.BackFace.StencilDepthFailOp		= D3D11_STENCIL_OP_DECR;
				stencilDesc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
				stencilDesc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

				ID3D11DepthStencilState* stencilState = NULL;
				HV_PTR( mCurrWindow->mDevice->CreateDepthStencilState( &stencilDesc, &stencilState ));
				mDepthStencilState.push_back( stencilState );

				// Create depth STENCIL_PARTICLE.
				//
				stencilDesc.DepthEnable						= TRUE;
				stencilDesc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ZERO;
				stencilDesc.DepthFunc						= D3D11_COMPARISON_LESS;
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

					SAFE_RELEASE_PTR_LIST( mRenderTarget );
					SAFE_RELEASE_PTR_LIST( mDepthStencil );
					SAFE_RELEASE_PTR_LIST( mBlendState );
					SAFE_RELEASE_PTR_LIST( mDepthStencilState );

					for( UINT i = 0; i < (UINT)mTexture.size(); ++i )
						SAFE_DELETE( mTexture[ i ] );
					mTexture.clear();

					SAFE_RELEASE_PTR( mCurrWindow->mContext );
				
					delete mCurrWindow;
					mCurrWindow = NULL;
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
		Buffer* CreateBuffer( void* data, UINT size, UINT stride, BufferType type )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( type == VERTEX_BUFFER || type == INDEX_BUFFER );

			D3D11_BUFFER_DESC bufferDesc;
			bufferDesc.Usage				= D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth			= size;
			bufferDesc.BindFlags			= type;
			bufferDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags			= 0;
			bufferDesc.StructureByteStride	= 0;

			D3D11_SUBRESOURCE_DATA resourceData;
			resourceData.pSysMem			= data;
			resourceData.SysMemPitch		= 0;
			resourceData.SysMemSlicePitch	= 0;

			BufferDX* buffer = new BufferDX();
			buffer->mContext = mCurrWindow->mContext;
			buffer->mType	 = type;
			buffer->mSize	 = size;
			buffer->mStride  = stride;
			buffer->mCount	 = size / stride;
			
			if( mCurrWindow->mDevice->CreateBuffer( &bufferDesc, &resourceData, &buffer->mBuffer ) == S_FALSE )
			{
				delete buffer;
				return NULL;
			}
			
			return buffer;
		}

		void SetVBO( Buffer* buffer )
		{
			_ASSERT( mCurrWindow );

			UINT offset = 0;

			mCurrWindow->mContext->IASetVertexBuffers( 0, 1, &static_cast< BufferDX* >(buffer)->mBuffer, &static_cast< BufferDX* >(buffer)->mStride, &offset );
		}

		void SetIBO( Buffer* buffer )
		{
			_ASSERT( mCurrWindow );

			mCurrWindow->mContext->IASetIndexBuffer( static_cast< BufferDX* >(buffer)->mBuffer, DXGI_FORMAT_R32_UINT, 0 );
		}

		// Textures.
		//
		Texture* CreateTextureFromFile( const char* filename )
		{
			if( strlen( filename ) == 0 )
			{
				return NULL;
			}

			// Determine if the same file is being loaded, and if so, only return it's index.
			//
			UINT index = 0;
			for( UINT i = 0; i < mTexture.size(); ++i )
			{
				if( mTexture[ i ]->mFilename == filename )
				{
					return mTexture[ i ];
				}
				++index;
			}

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
					mTexture.push_back( new TextureDX( filename, info.Width, info.Height, mTexture.size() ));
					mTexture.back()->mResource = image;
					return mTexture.back();
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
			unsigned char *pixels = stbi_load( filename,
											   &width, &height,
											   &nChannels, 4 );

			if( pixels == NULL )
			{
				REPORT_ERROR( filename << " failed to load.", "Texture load failure." );
				return NULL;
			}

			if( width > 4096 || height > 4096 )
			{
				REPORT_ERROR( filename << " is bigger than 4096x4096.", "Texture load failure." );
				return NULL;
			}

			Texture* result = CreateTextureFromMemory( pixels, width, height, IMAGE_FORMAT_RGBA );
			SAFE_RELEASE_PTR( ((TextureDX*)result)->mTexture );

			// Rename the texture because loading from memory sets a default.
			//
			result->mFilename = filename;

			stbi_image_free( pixels );

			return result;
		}

		Texture* CreateTextureFromMemory( void* data, UINT width, UINT height, ImageFormatType format, bool createMips = true )
		{
			char filename[ 256 ];
			sprintf_s( filename, "~Memory%d~", mTexture.size());

			// Anything larger could cause compatibility issues.
			//
			if( width > 4096 || height > 4096 )
			{
				REPORT_ERROR( filename << " is bigger than 4096x4096.", "Texture load failure." );
				return 0;
			}
			
			mTexture.push_back( new TextureDX( filename, width, height, mTexture.size() ));

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
						{
							newData[(i<<2)+j] = reinterpret_cast<UCHAR*>(data)[i*3+j];
						}

						newData[(i<<2)+3] = (UCHAR)(255);
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
					// Invalid image format.
					//
					_ASSERT( false );
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
			desc.MiscFlags		= D3D11_RESOURCE_MISC_GENERATE_MIPS;

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

				desc.Usage			= D3D11_USAGE_STAGING;
				desc.MipLevels		= 1;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				desc.BindFlags		= D3D11_USAGE_DEFAULT;
				desc.MiscFlags		= D3D11_USAGE_DEFAULT;

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

			HV_PTR( mCurrWindow->mDevice->CreateShaderResourceView( tex0, &rsv, &(mTexture.back())->mResource ));
			mCurrWindow->mContext->GenerateMips( (mTexture.back())->mResource );
			
			mTexture.back()->mTexture = tex0;

			if( format == IMAGE_FORMAT_RGB )
			{
				free( newData );
			}

			#ifndef NDEBUG
				SET_DEBUG_NAME( (mTexture.back())->mResource );
				SET_DEBUG_NAME( (mTexture.back())->mTexture );
			#endif

			return mTexture.back();
		}

		// Special Rendering.
		//
		UINT CreateBackbuffer()
		{
			_ASSERT( mCurrWindow );

			ID3D11Texture2D *tex = NULL;

			HV_PTR( mCurrWindow->mSwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (void **)&tex ));

			return CreateRenderTarget( tex );
		}

		UINT CreateRenderTarget( ID3D11Texture2D *backbuffer )
		{
			_ASSERT( mCurrWindow );

			ID3D11RenderTargetView* rendertarget = NULL;
			
			HV( mCurrWindow->mDevice->CreateRenderTargetView( backbuffer, NULL, &rendertarget ));
			mRenderTarget.push_back( rendertarget );
			SAFE_RELEASE_PTR( backbuffer );

			return mRenderTarget.size()-1;
		}

		UINT CreateRenderTarget( Texture* texture )
		{
			return CreateRenderTarget( ((TextureDX*)texture)->mTexture );
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

		UINT CreateViewport( UINT width, UINT height )
		{
			D3D11_VIEWPORT vp = 
			{
				0.0f, 0.0f,
				(float)width, (float)height,
				0.0f, 1.0f
			};

			mViewport.push_back( vp );

			return mViewport.size()-1;
		}

		UINT ResizeBuffers( UINT width, UINT height )
		{
			UNUSED( width );
			UNUSED( height );

			// Requires releasing all resources, and reloading them.
			//
			//mSwapChain->Release();
			//HV( mSwapChain->ResizeBuffers( 1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ));
			return 1;
		}

		void SetRenderType( PrimitiveType type )
		{
			_ASSERT( type < NUM_PRIMITIVES );

			mCurrShader->ApplyPass();
			mCurrShader->ApplyLayout();
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

		void SetDepthStencilState( UINT state )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( state < mDepthStencilState.size() );

			mCurrWindow->mContext->OMSetDepthStencilState( mDepthStencilState[ state ], 0 );
		}

		void SetViewport( UINT viewport )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( viewport < mViewport.size() );

			mCurrWindow->mContext->RSSetViewports( 1, &mViewport[ viewport ] );
		}

		UINT SetCull( CullType type )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( type < NUM_CULL_TYPES );

			SAFE_RELEASE_PTR( mCurrWindow->mRasterizerState );
			D3D11_RASTERIZER_DESC rasterizerstate = { D3D11_FILL_SOLID, (D3D11_CULL_MODE)CULL_TYPE[ type ], true, 0, 0, 0, true, false, true, false };

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
		Shader* CreateShader( const char* filename, const char* attrib, const char* uniform )
		{
			ShaderDX* shader = new ShaderDX();

			if( shader->Startup( filename, attrib, uniform, mCurrWindow->mDevice, mCurrWindow->mContext ) != S_OK )
			{
				delete shader;
				return NULL;
			}

			return shader;
		}

		void SetShader( Shader* shader )
		{
			mCurrShader = shader;
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
		}

		void Present()
		{
			_ASSERT( mCurrWindow );

			mCurrWindow->mSwapChain->Present( 0, 0 );
			
			// Unbind render targets.
			//
			mCurrWindow->mContext->OMSetRenderTargets( 0, 0, 0 );

			ID3D11ShaderResourceView* nullViews[ 8 ] = { 0, 0, 0, 0, 0, 0, 0, 0 };
			mCurrWindow->mContext->PSSetShaderResources( 0, 8, nullViews );
		}
	};


	Renderer* BuildRendererDX()
	{
		TRACE( "Creating DX11 Renderer..." );

		return new RendererDX();
	}
}