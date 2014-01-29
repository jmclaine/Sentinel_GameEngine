#include "VertexLayoutDX.h"
#include "Util.h"

namespace Sentinel
{
	VertexLayoutDX::VertexLayoutDX() :
		mInputLayout( NULL )
	{}

	VertexLayoutDX::~VertexLayoutDX()
	{}

	////////////////////////////////////

	void VertexLayoutDX::SetAttribute( LPCSTR name, DXGI_FORMAT format, UINT size, UINT index )
	{
		mInputDesc.push_back( D3D11_INPUT_ELEMENT_DESC() );

		D3D11_INPUT_ELEMENT_DESC &desc = mInputDesc.back();

		desc.SemanticName		= name;
		desc.Format				= format;
		desc.SemanticIndex		= index;
		desc.AlignedByteOffset	= mVertexSize;
		desc.InputSlot			= 0;
		desc.InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = 0;

		mVertexSize += size;
	}

	void VertexLayoutDX::AddAttribute( AttributeType type )
	{
		switch( type )
		{
		case ATTRIB_POSITION:
			SetAttribute( "POSITION", DXGI_FORMAT_R32G32B32_FLOAT, 12 );
			break;

		case ATTRIB_NORMAL:
			SetAttribute( "NORMAL", DXGI_FORMAT_R32G32B32_FLOAT, 12 );
			break;

		case ATTRIB_COLOR:
			SetAttribute( "COLOR", DXGI_FORMAT_R8G8B8A8_UNORM, 4 );
			break;

		case ATTRIB_TEXCOORD0:
			SetAttribute( "TEXCOORD", DXGI_FORMAT_R32G32_FLOAT, 8 );
			break;

		case ATTRIB_TEXCOORD1:
			SetAttribute( "TEXCOORD", DXGI_FORMAT_R32G32_FLOAT, 8, 1 );
			break;

		case ATTRIB_TEXCOORD2:
			SetAttribute( "TEXCOORD", DXGI_FORMAT_R32G32_FLOAT, 8, 2 );
			break;

		case ATTRIB_QUADCOORD0:
			SetAttribute( "QUADCOORD", DXGI_FORMAT_R32G32B32A32_FLOAT, 16 );
			break;

		case ATTRIB_QUADCOORD1:
			SetAttribute( "QUADCOORD", DXGI_FORMAT_R32G32B32A32_FLOAT, 16, 1 );
			break;

		case ATTRIB_QUADCOORD2:
			SetAttribute( "QUADCOORD", DXGI_FORMAT_R32G32B32A32_FLOAT, 16, 2 );
			break;

		case ATTRIB_TANGENT:
			SetAttribute( "TANGENT", DXGI_FORMAT_R32G32B32A32_FLOAT, 16 );
			break;

		case ATTRIB_BONE_COUNT:
			SetAttribute( "BONE_COUNT", DXGI_FORMAT_R32_SINT, 4 );
			break;

		case ATTRIB_BONE_INDEX:
			SetAttribute( "BONE_INDEX", DXGI_FORMAT_R32G32B32A32_SINT, 16 );
			break;
						
		case ATTRIB_BONE_WEIGHT:
			SetAttribute( "BONE_WEIGHT", DXGI_FORMAT_R32G32B32A32_FLOAT, 16 );
			break;

		case ATTRIB_MATRIX:
			for( UINT x = 0; x < 4; ++x )
			{
				SetAttribute( "MATRIX", DXGI_FORMAT_R32G32B32A32_FLOAT, 16, x );
			}
			break;

		default:
			TRACE( "Attempted to add unknown Attribute: " << type );
			return;
		}

		mLayout.push_back( type );
	}

	HRESULT VertexLayoutDX::Create( ID3D11Device* device )
	{
		// Creating a dummy shader source is the only
		// method of creating a valid vertex layout.
		//
		std::string source;

		source = "struct VSInput {\n";
		
		UINT size = (UINT)mLayout.size();
		for( UINT x = 0; x < size; ++x )
		{
			switch( mLayout[ x ] )
			{
			case ATTRIB_POSITION:
				source += "float4 Position:POSITION;\n";
				break;

			case ATTRIB_TEXCOORD0:
				source += "float2 TexCoord0:TEXCOORD0;\n";
				break;

			case ATTRIB_TEXCOORD1:
				source += "float2 TexCoord1:TEXCOORD1;\n";
				break;

			case ATTRIB_TEXCOORD2:
				source += "float2 TexCoord2:TEXCOORD2;\n";
				break;

			case ATTRIB_QUADCOORD0:
				source += "float4 QuadCoord0:QUADCOORD0;\n";
				break;

			case ATTRIB_QUADCOORD1:
				source += "float4 QuadCoord1:QUADCOORD1;\n";
				break;

			case ATTRIB_QUADCOORD2:
				source += "float4 QuadCoord2:QUADCOORD2;\n";
				break;

			case ATTRIB_NORMAL:
				source += "float3 Normal:NORMAL;\n";
				break;

			case ATTRIB_COLOR:
				source += "float4 Color:COLOR;\n";
				break;

			case ATTRIB_TANGENT:
				source += "float4 Tangent:TANGENT;\n";
				break;

			case ATTRIB_BONE_COUNT:
				source += "int BoneCount:BONE_COUNT;\n";
				break;

			case ATTRIB_BONE_INDEX:
				source += "int4 BoneIndex:BONE_INDEX;\n";
				break;
						
			case ATTRIB_BONE_WEIGHT:
				source += "float4 BoneWeight:BONE_WEIGHT;\n";
				break;

			case ATTRIB_MATRIX:
				source += "matrix Matrix:MATRIX;\n";
				break;
			}
		}

		source += "};\n\n";
		source += "VSInput VS_Main( VSInput input ) { return input; }\n";

		ID3D10Blob* shaderBlob = NULL;
		
		if( D3DX11CompileFromMemory( source.c_str(), source.size(), 0, 0, NULL, 
									 "VS_Main", "vs_4_0", 
									 D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &shaderBlob, NULL, NULL ) == S_FALSE )
		{
			SAFE_RELEASE_PTR( shaderBlob );

			return S_FALSE;
		}
			
		HRESULT result = Create( device, shaderBlob );

		SAFE_RELEASE_PTR( shaderBlob );

		return result;
	}

	HRESULT VertexLayoutDX::Create( ID3D11Device* device, ID3D10Blob* shaderBlob )
	{
		return device->CreateInputLayout( mInputDesc.data(), mInputDesc.size(), 
										  shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), 
										  &mInputLayout );
	}
}
