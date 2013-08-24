#include "RModel.h"
#include "RString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Assets
{
	RModel::RModel( Model* model )
	{
		mRef = model;
	}

	RModel::RModel( std::shared_ptr< Model > model )
	{
		mRef = model.get();
	}

	RModel::~RModel()
	{}

	m_shared_ptr< Model > RModel::GetRef()
	{
		return mRef;
	}

	////////////////////////////////

	RMatrix4f^ RModel::MatrixWorld()
	{
		return gcnew RMatrix4f( &mRef->mMatrixWorld );
	}

	////////////////////////////////

	RModel^ RModel::Load( System::String^ filename )
	{
		return gcnew RModel( Model::Load( RString::ToString( filename ).c_str() ));
	}

	bool RModel::Create( System::String^ filename )
	{
		return mRef->Create( RString::ToString( filename ).c_str() );
	}

	void RModel::Release()
	{
		mRef->Release();
	}

	void RModel::SetMaterial( WMaterial^ material )
	{
		mRef->SetMaterial( *material->GetRef() );
	}

	void RModel::SetShader( WShader^ shader )
	{
		mRef->SetShader( shader->GetRef() );
	}

	//void RModel::SetKeyFrame( const KeyFrame& key, int keyIndex = -1, int objIndex = 0 )
	//{}

	//void RModel::SetTime( float _time, UINT objIndex = 0 )
	//{}

	//float RModel::GetTime( UINT objIndex = 0 )
	//{}

	void RModel::Update()
	{
		mRef->Update();
	}

	void RModel::Draw()
	{
		mRef->Draw();
	}
}}
