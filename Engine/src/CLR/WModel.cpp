#include "WModel.h"
#include "RString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { 

	Shader*  Model::SHADER_COLOR = NULL;

namespace Assets
{
	WModel::WModel( Model* model )
	{
		mRef = model;
	}

	WModel::WModel( std::shared_ptr< Model > model )
	{
		mRef = model.get();
	}

	WModel::~WModel()
	{
		//mRef.reset();
	}

	WModel::!WModel()
	{
		//mRef.reset();
	}

	m_shared_ptr< Model > WModel::GetRef()
	{
		return mRef;
	}

	////////////////////////////////

	DEFINE_PROPERTY_RS( Model, Matrix4f, MatrixWorld );

	////////////////////////////////

	WModel^ WModel::Load( System::String^ filename )
	{
		return gcnew WModel( Model::Load( RString::ToString( filename ).c_str() ));
	}

	void WModel::SetShaderColor( WShader^ shader )
	{
		Model::SetShaderColor( shader->GetRef() );
	}

	WShader^ WModel::GetShaderColor()
	{
		return gcnew RShader( Model::SHADER_COLOR );
	}

	bool WModel::Create( System::String^ filename )
	{
		return mRef->Create( RString::ToString( filename ).c_str() );
	}

	void WModel::Release()
	{
		mRef->Release();
	}

	void WModel::SetMaterial( WMaterial^ material )
	{
		mRef->SetMaterial( *material->GetRef() );
	}

	void WModel::SetShader( WShader^ shader )
	{
		mRef->SetShader( shader->GetRef() );
	}

	//void WModel::SetKeyFrame( const KeyFrame& key, int keyIndex = -1, int objIndex = 0 )
	//{}

	//void WModel::SetTime( float _time, UINT objIndex = 0 )
	//{}

	//float WModel::GetTime( UINT objIndex = 0 )
	//{}

	void WModel::Update()
	{
		mRef->Update();
	}

	void WModel::Draw()
	{
		mRef->Draw();
	}
}}
