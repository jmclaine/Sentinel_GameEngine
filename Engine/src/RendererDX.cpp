#include <d3d11.h>
#include <d3dx11.h>
#include <D3D11Shader.h>
#include <D3DCompiler.h>
//#include <D3DX11Effect.h>

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
//#pragma comment (lib, "effects11.lib")

#include <vector>
#include <crtdbg.h>

#include "Renderer.h"
#include "Archive.h"
#include "Buffer.h"
#include "Texture.h"
#include "Shader.h"

namespace Sentinel
{
	#define HV( func )\
		if( (func) != S_OK )\
			return S_FALSE;

	#define HV_PTR( func )\
		if( (func) != S_OK )\
			return NULL;

	////////////////////////////////////////////////////////////////////////////////////

	class RendererDX;
	class ShaderDX;

	////////////////////////////////////////////////////////////////////////////////////
	// The DirectX 11 Buffer cannot be read for saving of data.
	// The reason is that the Buffer must be created as a DYNAMIC resource. The solution
	// is to store and modify the buffer data using the CPU, and then transfer that
	// data over to the DYNAMIC buffer.
	//
	// The solution is memory consuming, and remains unimplemented due to the only
	// purpose to use it would be to save modified vertex buffers for DX11 only.
	//
	class BufferDX : public Buffer
	{
		friend class RendererDX;
		friend class ShaderDX;

	private:

		ID3D11DeviceContext*	mContext;
		ID3D11Buffer*			mBuffer;

		BufferDX()
		{
			mBuffer = NULL;
			mStride = 0;
		}

	public:

		~BufferDX()
		{
			Release();
		}

		void* Lock()
		{
			D3D11_MAPPED_SUBRESOURCE mapRes;
				
			mContext->Map( mBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mapRes );
			
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

	class TextureDX : public Texture
	{
		friend class RendererDX;
		friend class ShaderDX;

	private:

		ID3D11Texture2D*			mTexture;
		ID3D11ShaderResourceView*	mResource;

	public:

		TextureDX( UINT width, UINT height )
		{
			mTexture	= NULL;
			mResource	= NULL;
			
			mWidth		= width;
			mHeight		= height;
		}

		~TextureDX()
		{
			Release();
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

		enum ShaderType
		{
			VERTEX_SHADER,
			GEOMETRY_SHADER,
			PIXEL_SHADER,
		};

	public:

		class SamplerDX : public Sampler
		{
		private:

			ID3D11DeviceContext*	mContext;
			
		public:

			ID3D11SamplerState*		mSampler;

			UINT					mStartSlot;

		public:

			SamplerDX() :
				mContext( NULL ),
				mSampler( NULL )
			{}

			~SamplerDX()
			{
				if( mSampler )
					mSampler->Release();
			}

			bool Create( ID3D11Device* device, ID3D11DeviceContext*	context, SamplerMode modeU, SamplerMode modeV, SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter )
			{
				mContext = context;

				D3D11_SAMPLER_DESC desc;
				ZeroMemory( &desc, sizeof( desc ));

				if( minFilter == FILTER_LINEAR )
				{
					if( magFilter == FILTER_LINEAR )
					{
						if( mipFilter == FILTER_LINEAR )
						{
							desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
						}
						else
						{
							desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
						}
					}
					else
					{
						if( mipFilter == FILTER_LINEAR )
						{
							desc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
						}
						else
						{
							desc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
						}
					}
				}
				else
				{
					if( magFilter == FILTER_LINEAR )
					{
						if( mipFilter == FILTER_LINEAR )
						{
							desc.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
						}
						else
						{
							desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
						}
					}
					else
					{
						if( mipFilter == FILTER_LINEAR )
						{
							desc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
						}
						else
						{
							desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
						}
					}
				}

				desc.AddressU		= (modeU == MODE_WRAP) ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_CLAMP;
				desc.AddressV		= (modeV == MODE_WRAP) ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_CLAMP;
				desc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
				desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
				desc.MinLOD			= 0;
				desc.MaxLOD			= (mipFilter != NUM_FILTERS) ? D3D11_FLOAT32_MAX : 0;
				desc.MipLODBias		= 0;
				desc.MaxAnisotropy	= 1;
				desc.BorderColor[0] = 0;
				desc.BorderColor[1] = 0;
				desc.BorderColor[2] = 0;
				desc.BorderColor[3] = 0;
				
				if( device->CreateSamplerState( &desc, &mSampler ) == S_FALSE )
					return false;

				return true;
			}

			void Apply()
			{
				_ASSERT( mSampler );

				mContext->PSSetSamplers( mStartSlot, 1, &mSampler );
			}
		};

	private:
		
		ID3D11Device*			mDevice;
		ID3D11DeviceContext*	mContext;
		ID3D11ShaderReflectionConstantBuffer* mReflectionBuffer;

		ID3D11VertexShader*		mVertexShader;
		ID3D11GeometryShader*	mGeometryShader;
		ID3D11PixelShader*		mPixelShader;

		ID3D11Buffer*			mConstantBuffer;
		BYTE*					mBufferData;	// locked buffer data
		
		ID3D11InputLayout*		mLayout;		// vertex format

		std::vector< UINT >		mUniformDX;		// stores offsets

		UINT					mTextureLevel;

	public:

		ShaderDX( ID3D11Device* device, ID3D11DeviceContext* context ) :
			mDevice( device ),
			mContext( context ),
			mReflectionBuffer( NULL ),
			mVertexShader( NULL ),
			mGeometryShader( NULL ),
			mPixelShader( NULL ),
			mConstantBuffer( NULL ),
			mBufferData( NULL ),
			mLayout( NULL ),
			mTextureLevel( 0 )
		{
			mVertexSize = 0;
		}

		~ShaderDX()
		{
			Release();
		}
			
		// Use the filename without the extension.
		// DirectX uses .fx files.
		//
		UINT CreateFromFile( std::string filename, const std::string& attrib, const std::string& uniform )
		{
			filename.append( ".xsh" );

			if( Archive::ToBuffer( filename.c_str(), mShaderSource ) == 0 )
			{
				REPORT_ERROR( "Could not open '" << filename << "'", "Shader Loader Error" );

				return S_FALSE;
			}

			TRACE( "Compiling '" << filename << "'..." );

			return CreateFromMemory( mShaderSource, attrib, uniform );
		}

		UINT CreateFromMemory( char* source, const std::string& attrib, const std::string& uniform )
		{
			mShaderSource	= source;
			mAttribute		= attrib;
			mUniform		= uniform;

			// Compile Vertex Shader.
			//
			ID3D10Blob* blobVS = Compile( VERTEX_SHADER );

			if( blobVS )
				mDevice->CreateVertexShader( blobVS->GetBufferPointer(), blobVS->GetBufferSize(), 0, &mVertexShader );
			else
			{
				SAFE_RELEASE_PTR( blobVS );
				return S_FALSE;
			}

			// Compile Geometry Shader.
			//
			if( strstr( mShaderSource, "GS_Main" ) != NULL )
			{
				ID3D10Blob* blobGS = Compile( GEOMETRY_SHADER );

				if( blobGS )
					mDevice->CreateGeometryShader( blobGS->GetBufferPointer(), blobGS->GetBufferSize(), 0, &mGeometryShader );

				SAFE_RELEASE_PTR( blobGS );
			}

			// Compile Pixel Shader.
			//
			ID3D10Blob* blobPS = Compile( PIXEL_SHADER );

			if( blobPS )
				mDevice->CreatePixelShader( blobPS->GetBufferPointer(), blobPS->GetBufferSize(), 0, &mPixelShader );
			else
			{
				SAFE_RELEASE_PTR( blobVS );
				SAFE_RELEASE_PTR( blobPS );
				return S_FALSE;
			}

			SAFE_RELEASE_PTR( blobPS );

			TRACE( "Shader Compiled Successfully!" );
			
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
				if( attrib[ i ] == 'M' )
				{
					len += 3;
					break;
				}
			}

			D3D11_INPUT_ELEMENT_DESC *decl = new D3D11_INPUT_ELEMENT_DESC[ len ];
			memset( decl, 0, sizeof( D3D11_INPUT_ELEMENT_DESC ) * len );
			UINT texCount = 0;

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
					d.SemanticIndex = texCount++;
					mVertexSize += 8;
					break;

				// Quad Texture Coordinates.
				//
				case 'x':
					d.SemanticName = "TEXCOORD";
					d.Format =  DXGI_FORMAT_R32G32B32A32_FLOAT;
					d.SemanticIndex = texCount++;
					mVertexSize += 16;
					break;

				// Vertex Matrix.
				//
				case 'M':
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

			HRESULT hr = mDevice->CreateInputLayout( decl, len, blobVS->GetBufferPointer(), blobVS->GetBufferSize(), &mLayout );

			delete[] decl;

			if( hr != S_OK  )
			{
				SAFE_RELEASE_PTR( blobVS );

				Release();
				return S_FALSE;
			}

			// Create texture samplers.
			//
			if( texCount > 0 )
			{
				mNumSamplers = texCount;

				mSampler = new Sampler*[ texCount ];

				for( UINT i = 0; i < texCount; ++i )
				{
					SamplerDX* sampler = new SamplerDX();
					
					sampler->Create( mDevice, mContext, MODE_WRAP, MODE_WRAP, FILTER_LINEAR, FILTER_LINEAR, FILTER_LINEAR );
					sampler->mStartSlot = i;

					mSampler[ i ] = sampler;
				}
			}

			// Create uniforms / constant buffers.
			//
			ID3D11ShaderReflection* reflect = NULL;
			if( D3DReflect( blobVS->GetBufferPointer(), blobVS->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflect ) == S_FALSE )
			{
				SAFE_RELEASE_PTR( blobVS );
				Release();

				return S_FALSE;
			}

			SAFE_RELEASE_PTR( blobVS );

			D3D11_SHADER_DESC desc;
			reflect->GetDesc( &desc );

			if( desc.ConstantBuffers > 0 )
			{
				mReflectionBuffer = reflect->GetConstantBufferByName( "Uniforms" );

				D3D11_SHADER_BUFFER_DESC shaderDesc;
				mReflectionBuffer->GetDesc( &shaderDesc );
			
				void* bufferData = malloc( shaderDesc.Size );
				ZeroMemory( bufferData, shaderDesc.Size );

				D3D11_BUFFER_DESC bufferDesc;
				bufferDesc.Usage				= D3D11_USAGE_DYNAMIC;
				bufferDesc.ByteWidth			= shaderDesc.Size;
				bufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
				bufferDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
				bufferDesc.MiscFlags			= 0;
				bufferDesc.StructureByteStride	= 0;

				D3D11_SUBRESOURCE_DATA resourceData;
				resourceData.pSysMem			= bufferData;
				resourceData.SysMemPitch		= 0;
				resourceData.SysMemSlicePitch	= 0;

				if( mDevice->CreateBuffer( &bufferDesc, &resourceData, &mConstantBuffer ) == S_FALSE )
				{
					free( bufferData );
					Release();

					return S_FALSE;
				}

				free( bufferData );
			}

			ProcessUniforms();

			TRACE( "Shader Created Successfully!" );

			return S_OK;
		}

	private:

		ID3D10Blob* Compile( ShaderType type )
		{
			ID3D10Blob* shaderBlob = NULL;
			ID3D10Blob* errorBlob  = NULL;

			D3D10_SHADER_MACRO macro[2] = {{ "VERSION_DX", 0 }, { 0, 0 }};

			if( FAILED( D3DX11CompileFromMemory( mShaderSource, strlen( mShaderSource ), 0, macro, NULL, 
												 (type == VERTEX_SHADER) ? "VS_Main" : (type == GEOMETRY_SHADER) ? "GS_Main" : (type == PIXEL_SHADER) ? "PS_Main" : 0, 
												 (type == VERTEX_SHADER) ? "vs_4_0"  : (type == GEOMETRY_SHADER) ? "gs_4_0"  : (type == PIXEL_SHADER) ? "ps_4_0"  : 0, 
												 D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &shaderBlob, &errorBlob, NULL )))
			{
				if( errorBlob )
					TRACE( (char*)errorBlob->GetBufferPointer() );

				SAFE_RELEASE_PTR( shaderBlob );
				SAFE_RELEASE_PTR( errorBlob );

				return NULL;
			}
			
			SAFE_RELEASE_PTR( errorBlob );

			return shaderBlob;
		}

		void CreateUniform( const char* name )
		{
			if( strstr( name, "tex" ) == NULL )
			{
				ID3D11ShaderReflectionVariable* var = mReflectionBuffer->GetVariableByName( name );
				D3D11_SHADER_VARIABLE_DESC varDesc;
				var->GetDesc( &varDesc );

				mUniformDX.push_back( varDesc.StartOffset );
			}
			else
			{
				mUniformDX.push_back( 0 );
			}
		}

	public:

		void ApplyPass()
		{
			mTextureLevel = 0;

			if( mConstantBuffer )
			{
				D3D11_MAPPED_SUBRESOURCE mapRes;
				
				mContext->Map( mConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapRes );
			
				mBufferData = (BYTE*)mapRes.pData;
			}
		}

		void ApplyLayout()
		{
			mContext->IASetInputLayout( mLayout );

			mContext->VSSetShader( mVertexShader, NULL, 0 );
			mContext->GSSetShader( mGeometryShader, NULL, 0 );
			mContext->PSSetShader( mPixelShader, NULL, 0 );

			if( mConstantBuffer )
			{
				mContext->Unmap( mConstantBuffer, 0 );

				mContext->VSSetConstantBuffers( 0, 1, &mConstantBuffer );

				if( mGeometryShader )
					mContext->GSSetConstantBuffers( 0, 1, &mConstantBuffer );
			
				mContext->PSSetConstantBuffers( 0, 1, &mConstantBuffer );
			}
		}
		
		void Release()
		{
			mDevice		= NULL;
			mContext	= NULL;

			SAFE_RELEASE_PTR( mConstantBuffer );
			SAFE_RELEASE_PTR( mLayout );

			mUniform.clear();
		}

		void SetFloat( UINT uniform, float data )
		{
			memcpy( mBufferData + mUniformDX[ uniform ], &data, sizeof( float ));
		}

		void SetFloat2( UINT uniform, float* data )
		{
			memcpy( mBufferData + mUniformDX[ uniform ], data, 2 * sizeof( float ));
		}

		void SetFloat3( UINT uniform, float* data )
		{
			memcpy( mBufferData + mUniformDX[ uniform ], data, 3 * sizeof( float ));
		}

		void SetFloat4( UINT uniform, float* data )
		{
			memcpy( mBufferData + mUniformDX[ uniform ], data, 4 * sizeof( float ));
		}

		void SetMatrix( UINT uniform, float* data )
		{
			memcpy( mBufferData + mUniformDX[ uniform ], data, 16 * sizeof( float ));
		}

		void SetTexture( UINT uniform, Texture* texture )
		{
			if( texture )
			{
				mContext->PSSetShaderResources( mTextureLevel, 1, &static_cast< TextureDX* >(texture)->mResource );
				
				static_cast< SamplerDX* >(mSampler[ mTextureLevel ])->Apply();
			}
			else
			{
				mContext->PSSetShaderResources( mTextureLevel, 1, NULL );
			}

			++mTextureLevel;
		}

		void SetSampler( UINT index, SamplerMode modeU, SamplerMode modeV, 
						 SamplerFilter minFilter, SamplerFilter magFilter, SamplerFilter mipFilter )
		{
			_ASSERT( index < mNumSamplers );

			SamplerDX* sampler = new SamplerDX();

			if( !sampler->Create( mDevice, mContext, modeU, modeV, minFilter, magFilter, mipFilter ))
			{
				delete sampler;
				return;
			}

			sampler->mStartSlot = index;

			delete mSampler[ index ];

			mSampler[ index ] = sampler;
		}
	};

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
		Buffer* CreateBuffer( void* data, UINT size, UINT stride, BufferType type )
		{
			_ASSERT( mCurrWindow );
			_ASSERT( type == VERTEX_BUFFER || type == INDEX_BUFFER );
			
			D3D11_BUFFER_DESC bufferDesc;
			bufferDesc.Usage				= D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth			= size;
			bufferDesc.BindFlags			= (type == VERTEX_BUFFER) ? D3D11_BIND_VERTEX_BUFFER : D3D11_BIND_INDEX_BUFFER;
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
					TextureDX* texture = new TextureDX( info.Width, info.Height );
					texture->mResource = image;

					return std::shared_ptr< Texture >( texture );
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
			TextureDX* texture = new TextureDX( width, height );

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
			
			HV_PTR( mCurrWindow->mDevice->CreateShaderResourceView( tex0, &rsv, &texture->mResource ));
			mCurrWindow->mContext->GenerateMips( texture->mResource );
			
			texture->mTexture = tex0;

			if( format == IMAGE_FORMAT_RGB )
				free( newData );

			#ifndef NDEBUG
				SET_DEBUG_NAME( texture->mResource );
				SET_DEBUG_NAME( texture->mTexture );
			#endif

			return std::shared_ptr< Texture >( texture );
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
				
				UINT  size = (texDX->mWidth << 2) * texDX->mHeight;
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

			mCurrShader->ApplyLayout();
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
		std::shared_ptr< Shader > CreateShaderFromFile( const char* filename, const char* attrib, const char* uniform )
		{
			ShaderDX* shader = new ShaderDX( mCurrWindow->mDevice, mCurrWindow->mContext );

			if( shader->CreateFromFile( filename, attrib, uniform ) != S_OK )
			{
				delete shader;
				return NULL;
			}

			return std::shared_ptr< Shader >( shader );
		}

		std::shared_ptr< Shader > CreateShaderFromMemory( const char* source, const char* attrib, const char* uniform )
		{
			ShaderDX* shader = new ShaderDX( mCurrWindow->mDevice, mCurrWindow->mContext );

			if( shader->CreateFromMemory( const_cast< char* >(source), attrib, uniform ) != S_OK )
			{
				delete shader;
				return NULL;
			}

			return std::shared_ptr< Shader >( shader );
		}

		void SetShader( const std::shared_ptr< Shader >& shader )
		{
			mCurrShader = shader;

			shader->ApplyPass();
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
		TRACE( "Creating DX11 Renderer..." );

		return new RendererDX();
	}
}