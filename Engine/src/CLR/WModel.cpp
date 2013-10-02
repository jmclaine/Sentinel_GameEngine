#include "WModel.h"
#include "WString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Assets
{
	DEFINE_REF_SHARED( Model );

	////////////////////////////////

	DEFINE_PROPERTY_R( Model, Matrix4f, MatrixWorld );

	////////////////////////////////

	WModel^ WModel::Load( System::String^ filename )
	{
		return gcnew WModel( std::shared_ptr< Model >( Model::Load( msclr::interop::marshal_as< std::string >(filename).c_str() )));
	}

	void WModel::SetShaderColor( WShader^ shader )
	{
		Model::SetShaderColor( shader->GetRef() );
	}

	WShader^ WModel::GetShaderColor()
	{
		return gcnew RShader( Model::GetShaderColor() );
	}

	void WModel::SetShaderTexture( WShader^ shader )
	{
		Model::SetShaderTexture( shader->GetRef() );
	}

	WShader^ WModel::GetShaderTexture()
	{
		return gcnew RShader( Model::GetShaderTexture() );
	}
	
	void WModel::SetShaderNormalMap( WShader^ shader )
	{
		Model::SetShaderNormalMap( shader->GetRef() );
	}

	WShader^ WModel::GetShaderNormalMap()
	{
		return gcnew RShader( Model::GetShaderNormalMap() );
	}
	
	void WModel::SetShaderParallax( WShader^ shader )
	{
		Model::SetShaderParallax( shader->GetRef() );
	}

	WShader^ WModel::GetShaderParallax()
	{
		return gcnew RShader( Model::GetShaderParallax() );
	}
	
	void WModel::SetShaderSkinning( WShader^ shader )
	{
		Model::SetShaderSkinning( shader->GetRef() );
	}

	WShader^ WModel::GetShaderSkinning()
	{
		return gcnew RShader( Model::GetShaderSkinning() );
	}

	bool WModel::Create( System::String^ filename )
	{
		return mRef->Create( msclr::interop::marshal_as< std::string >(filename).c_str() );
	}

	void WModel::Release()
	{
		mRef->Release();
	}

	void WModel::SetMaterials( WStdVector_Material^ material )
	{
		mRef->SetMaterials( material );
	}

	void WModel::GetMaterials( WStdVector_Material^ material )
	{
		mRef->GetMaterials( material->GetRef() );
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
