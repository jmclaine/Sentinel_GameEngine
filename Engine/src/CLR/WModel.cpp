#include "WModel.h"
#include "WString.h"

using namespace Sentinel::Utilities;

namespace Sentinel { namespace Assets
{
	DEFINE_REF_SHARED( Model );

	DEFINE_OP_SHARED( Model );

	////////////////////////////////

	DEFINE_PROPERTY_R( Model, Matrix4f, MatrixWorld );

	////////////////////////////////

	WModel^ WModel::Load( System::String^ filename )
	{
		return gcnew WModel( std::shared_ptr< Model >( Model::Load( msclr::interop::marshal_as< std::string >(filename).c_str() )));
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

	void WModel::Update()
	{
		mRef->Update();
	}

	void WModel::Draw()
	{
		mRef->Draw();
	}

	////////////////////////////////

	DEFINE_CLASS_REF_SHARED( Model );
}}
