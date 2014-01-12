#include "GUI/ModelWidget.h"
#include "GUI/WidgetObject.h"

namespace Sentinel { namespace GUI
{
	DEFINE_SERIAL_REGISTER( ModelWidget );
	DEFINE_SERIAL_CLONE( ModelWidget );

	DEFINE_SERIAL_REGISTER_SAVE( ModelWidget );

	ModelWidget::ModelWidget() :
		mParentModel( NULL ),
		mScale( 1, 1, 1 )
	{}

	ModelWidget::~ModelWidget()
	{}

	void ModelWidget::Startup()
	{
		if( mOwner->GetParent() )
			mParentModel = mOwner->GetParent()->GetModel();
		else
			mParentModel = NULL;
	}

	void ModelWidget::Update()
	{
		mMatrixWorld.World( mPosition, mOrientation, mScale );

		if( mParentModel )
			mMatrixWorld = mParentModel->GetMatrixWorld() * mMatrixWorld;
	}

	void ModelWidget::Shutdown()
	{
		mParentModel = NULL;
	}

	///////////////////////////////////

	const Matrix4f& ModelWidget::GetMatrixWorld()
	{
		return mMatrixWorld;
	}

	///////////////////////////////////

	void ModelWidget::Save( Archive& archive )
	{
		archive.Write( mPosition.Ptr(),		ar_sizeof( mPosition ));
		archive.Write( mOrientation.Ptr(),	ar_sizeof( mOrientation ));
		archive.Write( mScale.Ptr(),		ar_sizeof( mScale ));
	}

	void ModelWidget::Load( Archive& archive )
	{
		archive.Read( mPosition.Ptr(),		ar_sizeof( mPosition ));
		archive.Read( mOrientation.Ptr(),	ar_sizeof( mOrientation ));
		archive.Read( mScale.Ptr(),			ar_sizeof( mScale ));
	}
}}
