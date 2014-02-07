#pragma once

#include "ShaderDX.h"
#include "TextureDX.h"
#include "VertexLayoutDX.h"
#include "Util.h"
#include "Archive.h"

namespace Sentinel
{
	ShaderDX::SamplerDX::SamplerDX() :
		mContext( NULL ),
		mSampler( NULL )
	{}

	ShaderDX::SamplerDX::~SamplerDX()
	{
		if( mSampler )
			mSampler->Release();
	}

	bool ShaderDX::SamplerDX::Create( ID3D11Device* device, ID3D11DeviceContext* context, 
									  SamplerMode modeU, SamplerMode modeV, 
									  SamplerFilter minFilter, 
									  SamplerFilter magFilter, 
									  SamplerFilter mipFilter )
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

	void ShaderDX::SamplerDX::Apply()
	{
		_ASSERT( mSampler );

		mContext->PSSetSamplers( mStartSlot, 1, &mSampler );
	}

	//////////////////////////////////////////////////////////////////////

	ShaderDX::ShaderDX( ID3D11Device* device, ID3D11DeviceContext* context ) :
		mDevice( device ),
		mContext( context ),
		mVertexShader( NULL ),
		mGeometryShader( NULL ),
		mPixelShader( NULL ),
		mConstantBuffer( NULL ),
		mTextureLevel( 0 )
	{}

	ShaderDX::~ShaderDX()
	{
		Release();
	}

	void ShaderDX::Release()
	{
		mDevice			= NULL;
		mContext		= NULL;

		mVertexShader	= NULL;
		mGeometryShader = NULL;
		mPixelShader	= NULL;

		SAFE_RELEASE_PTR( mConstantBuffer );
		
		mUniform.clear();
	}
		
	///////////////////////////////////

	bool ShaderDX::CreateUniform( char* name, ID3D11ShaderReflectionConstantBuffer* reflect )
	{
		ID3D11ShaderReflectionVariable* var = reflect->GetVariableByName( name );
			
		D3D11_SHADER_VARIABLE_DESC varDesc;

		if( var->GetDesc( &varDesc ) == E_FAIL )
			return false;

		mUniformDX.push_back( varDesc.StartOffset );

		return true;
	}

	///////////////////////////////////

	// Use the filename without the extension.
	// DirectX uses .fx files.
	//
	UINT ShaderDX::CreateFromFile( std::string filename )
	{
		filename.append( ".xsh" );

		if( Archive::ToBuffer( filename.c_str(), mSource ) == 0 )
		{
			REPORT_ERROR( "Could not open '" << filename << "'", "Shader Loader Error" );

			return S_FALSE;
		}

		TRACE( "Compiling '" << filename << "'..." );

		return CreateFromMemory( mSource );
	}

	UINT ShaderDX::CreateFromMemory( char* source )
	{
		mSource = source;
			
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
		if( strstr( mSource, "GS_Main" ) != NULL )
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

		TRACE( "Shader Compiled Successfully!" );
		
		// Get shader reflection info.
		//
		ID3D11ShaderReflection* reflect = NULL;
		if( D3DReflect( blobVS->GetBufferPointer(), blobVS->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflect ) == S_FALSE )
		{
			SAFE_RELEASE_PTR( blobVS );
			SAFE_RELEASE_PTR( blobPS );

			Release();

			return S_FALSE;
		}

		// Create vertex attributes.
		//
		D3D11_SHADER_DESC desc;
		reflect->GetDesc( &desc );

		for( UINT x = 0; x < desc.InputParameters; ++x )
		{
			D3D11_SIGNATURE_PARAMETER_DESC varDesc;

			reflect->GetInputParameterDesc( x, &varDesc );

			if( strcmp( varDesc.SemanticName, "POSITION" ) == 0 )
			{
				mAttribute.push_back( ATTRIB_POSITION );
			}
			else
			if( strcmp( varDesc.SemanticName, "TEXCOORD" ) == 0 )
			{
				if( varDesc.SemanticIndex == 0 )
					mAttribute.push_back( ATTRIB_TEXCOORD0 );
				else
				if( varDesc.SemanticIndex == 1 )
					mAttribute.push_back( ATTRIB_TEXCOORD1 );
				else
				if( varDesc.SemanticIndex == 2 )
					mAttribute.push_back( ATTRIB_TEXCOORD2 );
			}
			else
			if( strcmp( varDesc.SemanticName, "QUADCOORD" ) == 0 )
			{
				if( varDesc.SemanticIndex == 0 )
					mAttribute.push_back( ATTRIB_QUADCOORD0 );
				else
				if( varDesc.SemanticIndex == 1 )
					mAttribute.push_back( ATTRIB_QUADCOORD1 );
				else
				if( varDesc.SemanticIndex == 2 )
					mAttribute.push_back( ATTRIB_QUADCOORD2 );
			}
			else
			if( strcmp( varDesc.SemanticName, "NORMAL" ) == 0 )
			{
				mAttribute.push_back( ATTRIB_NORMAL );
			}
			else
			if( strcmp( varDesc.SemanticName, "COLOR" ) == 0 )
			{
				mAttribute.push_back( ATTRIB_COLOR );
			}
			else
			if( strcmp( varDesc.SemanticName, "TANGENT" ) == 0 )
			{
				mAttribute.push_back( ATTRIB_COLOR );
			}
			else
			if( strcmp( varDesc.SemanticName, "BONE_COUNT" ) == 0 )
			{
				mAttribute.push_back( ATTRIB_BONE_COUNT );
			}
			else
			if( strcmp( varDesc.SemanticName, "BONE_INDEX" ) == 0 )
			{
				mAttribute.push_back( ATTRIB_BONE_INDEX );
			}
			else
			if( strcmp( varDesc.SemanticName, "BONE_WEIGHT" ) == 0 )
			{
				mAttribute.push_back( ATTRIB_BONE_WEIGHT );
			}
			else
			if( strcmp( varDesc.SemanticName, "MATRIX" ) == 0 )
			{
				mAttribute.push_back( ATTRIB_MATRIX );
				x += 3;
			}
		}

		// Create layout.
		//
		VertexLayoutDX* layout = new VertexLayoutDX();

		UINT size = (UINT)mAttribute.size();

		for( UINT x = 0; x < size; ++x )
		{
			layout->AddAttribute( mAttribute[ x ] );
		}

		layout->Create( mDevice, blobVS );

		mLayout = std::shared_ptr< VertexLayout >(layout);

		// Create uniforms / constant buffers.
		//
		ID3D11ShaderReflectionConstantBuffer* cbuffer = NULL;

		if( desc.ConstantBuffers > 0 )
		{
			cbuffer = reflect->GetConstantBufferByName( "Uniforms" );

			D3D11_SHADER_BUFFER_DESC shaderDesc;
			cbuffer->GetDesc( &shaderDesc );
			
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

				SAFE_RELEASE_PTR( blobVS );
				SAFE_RELEASE_PTR( blobPS );

				Release();

				return S_FALSE;
			}

			free( bufferData );
		}
			
		// Create uniforms.
		//
		if( cbuffer )
		{
			if( CreateUniform( "_WVP", cbuffer ))
				mUniform.push_back( UNIFORM_WVP );

			if( CreateUniform( "_World", cbuffer ))
				mUniform.push_back( UNIFORM_WORLD );

			if( CreateUniform( "_InvWorld", cbuffer ))
				mUniform.push_back( UNIFORM_INV_WORLD );

			if( CreateUniform( "_View", cbuffer ))
				mUniform.push_back( UNIFORM_VIEW );

			if( CreateUniform( "_InvView", cbuffer ))
				mUniform.push_back( UNIFORM_INV_VIEW );

			if( CreateUniform( "_Proj", cbuffer ))
				mUniform.push_back( UNIFORM_PROJ );

			if( CreateUniform( "_InvProj", cbuffer ))
				mUniform.push_back( UNIFORM_INV_PROJ );

			if( CreateUniform( "_Ambient", cbuffer ))
				mUniform.push_back( UNIFORM_AMBIENT );

			if( CreateUniform( "_Diffuse", cbuffer ))
				mUniform.push_back( UNIFORM_DIFFUSE );

			if( CreateUniform( "_Specular", cbuffer ))
				mUniform.push_back( UNIFORM_SPECULAR );

			if( CreateUniform( "_SpecComp", cbuffer ))
				mUniform.push_back( UNIFORM_SPEC_COMP );

			if( CreateUniform( "_LightPos", cbuffer ))
				mUniform.push_back( UNIFORM_LIGHT_POS );

			if( CreateUniform( "_LightDir", cbuffer ))
				mUniform.push_back( UNIFORM_LIGHT_DIR );

			if( CreateUniform( "_LightColor", cbuffer ))
				mUniform.push_back( UNIFORM_LIGHT_COLOR );

			if( CreateUniform( "_LightAttn", cbuffer ))
				mUniform.push_back( UNIFORM_LIGHT_ATTN );

			if( CreateUniform( "_LightMatrix", cbuffer ))
				mUniform.push_back( UNIFORM_LIGHT_MATRIX );

			if( CreateUniform( "_LightCubeMatrix", cbuffer ))
				mUniform.push_back( UNIFORM_LIGHT_CUBE_MATRIX );

			if( CreateUniform( "_CameraPos", cbuffer ))
				mUniform.push_back( UNIFORM_CAMERA_POS );

			if( CreateUniform( "_Bones", cbuffer ))
				mUniform.push_back( UNIFORM_BONES );

			if( CreateUniform( "_DeltaTime", cbuffer ))
				mUniform.push_back( UNIFORM_DELTA_TIME );
		}

		// Create texture uniforms.
		//
		if( D3DReflect( blobPS->GetBufferPointer(), blobPS->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflect ) == S_FALSE )
		{
			SAFE_RELEASE_PTR( blobVS );
			SAFE_RELEASE_PTR( blobPS );

			Release();

			return S_FALSE;
		}

		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		UINT numTexture = 0;
			
		for( int x = 0; x < MAX_TEXTURES; ++x )
		{
			char texName[16];
			sprintf_s( texName, "_Texture%d", x );

			if( reflect->GetResourceBindingDescByName( texName, &bindDesc ) == S_OK )
			{
				mUniform.push_back( UNIFORM_TEXTURE );
				++numTexture;
			}
		}

		// Create texture samplers.
		//
		if( numTexture > 0 )
		{
			mNumSamplers = numTexture;

			mSampler = new Sampler*[ numTexture ];

			for( UINT i = 0; i < numTexture; ++i )
			{
				SamplerDX* sampler = new SamplerDX();
					
				sampler->Create( mDevice, mContext, MODE_WRAP, MODE_WRAP, FILTER_LINEAR, FILTER_LINEAR, FILTER_LINEAR );
				sampler->mStartSlot = i;

				mSampler[ i ] = sampler;
			}
		}

		SAFE_RELEASE_PTR( blobVS );
		SAFE_RELEASE_PTR( blobPS );

		TRACE( "Shader Created Successfully!" );

		return S_OK;
	}

	///////////////////////////////////

	ID3D10Blob* ShaderDX::Compile( ShaderType type )
	{
		ID3D10Blob* shaderBlob = NULL;
		ID3D10Blob* errorBlob  = NULL;

		D3D10_SHADER_MACRO macro[2] = {{ "VERSION_DX", 0 }, { 0, 0 }};

		if( FAILED( D3DX11CompileFromMemory( mSource, strlen( mSource ), 0, macro, NULL, 
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

	///////////////////////////////////

	void ShaderDX::SetFloat( UINT uniform, float* data, UINT count )
	{
		memcpy( mConstantBufferData + mUniformDX[ uniform ], &data, sizeof( float ));
	}

	void ShaderDX::SetFloat2( UINT uniform, float* data, UINT count )
	{
		memcpy( mConstantBufferData + mUniformDX[ uniform ], data, 2 * sizeof( float ));
	}

	void ShaderDX::SetFloat3( UINT uniform, float* data, UINT count )
	{
		memcpy( mConstantBufferData + mUniformDX[ uniform ], data, 3 * sizeof( float ));
	}

	void ShaderDX::SetFloat4( UINT uniform, float* data, UINT count )
	{
		memcpy( mConstantBufferData + mUniformDX[ uniform ], data, 4 * sizeof( float ));
	}

	void ShaderDX::SetMatrix( UINT uniform, float* data, UINT count )
	{
		memcpy( mConstantBufferData + mUniformDX[ uniform ], data, 16 * sizeof( float ));
	}

	void ShaderDX::SetTexture( UINT uniform, Texture* texture )
	{
		if( texture )
		{
			mContext->PSSetShaderResources( mTextureLevel, 1, &((TextureDX*)texture)->mResource );
				
			static_cast< SamplerDX* >(mSampler[ mTextureLevel ])->Apply();
		}
		else
		{
			mContext->PSSetShaderResources( mTextureLevel, 1, NULL );
		}

		++mTextureLevel;
	}

	void ShaderDX::SetTextureCube( UINT uniform, Texture* texture )
	{
		_ASSERT(0); // not implemented
	}

	void ShaderDX::SetSampler( UINT index, 
							   SamplerMode modeU, SamplerMode modeV, 
							   SamplerFilter minFilter, 
							   SamplerFilter magFilter, 
							   SamplerFilter mipFilter )
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

	///////////////////////////////////

	void ShaderDX::Enable()
	{
		mTextureLevel = 0;

		mContext->VSSetShader( mVertexShader, NULL, 0 );
		mContext->GSSetShader( mGeometryShader, NULL, 0 );
		mContext->PSSetShader( mPixelShader, NULL, 0 );

		if( mConstantBuffer )
		{
			D3D11_MAPPED_SUBRESOURCE mapRes;
				
			mContext->Map( mConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapRes );
			
			mConstantBufferData = (BYTE*)mapRes.pData;

			mContext->Unmap( mConstantBuffer, 0 );

			mContext->VSSetConstantBuffers( 0, 1, &mConstantBuffer );
			mContext->GSSetConstantBuffers( 0, 1, &mConstantBuffer );
			mContext->PSSetConstantBuffers( 0, 1, &mConstantBuffer );
		}
	}

	void ShaderDX::Disable()
	{}
}
