#include "WModel.h"
#include "WShader.h"
#include "WMatrix4f.h"
#include "WMaterial.h"
#include "WString.h"
#include "WRenderer.h"
#include "WShaderManager.h"
#include "WTextureManager.h"
#include "WGameWorld.h"

namespace Sentinel { namespace Wrapped
{
	DEFINE_REF_SHARED( Model );

	DEFINE_OP_SHARED( Model );

	////////////////////////////////

	DEFINE_PROPERTY_R( Model, Matrix4f, MatrixWorld );

	////////////////////////////////

	WModel^ WModel::Load( System::String^	filename,
						  WRenderer^		renderer, 
						  WShaderManager^	shaderManager, 
						  WTextureManager^	textureManager )
	{
		return gcnew WModel( std::shared_ptr< Model >( Model::Load( WString::Cast( filename ).c_str(), renderer, shaderManager, textureManager )));
	}

	void WModel::SetMaterials( WStdVector_Material^ material )
	{
		mRef->SetMaterials( material );
	}

	void WModel::GetMaterials( WStdVector_Material^ material )
	{
		mRef->GetMaterials( material->GetRef() );
	}

	//void WModel::SetTime( float _time, UINT objIndex = 0 )
	//{}

	//float WModel::GetTime( UINT objIndex = 0 )
	//{}

	void WModel::Update( float DT )
	{
		mRef->Update( DT );
	}

	void WModel::Draw( WRenderer^ renderer, WGameWorld^ world )
	{
		mRef->Draw( renderer, world );
	}

	////////////////////////////////

	DEFINE_CLASS_REF_SHARED( Model );
}}
