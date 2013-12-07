#include "GUI/MeshViewWidget.h"
#include "GUI/WidgetWorld.h"
#include "GUI/WidgetObject.h"
#include "GUI/ModelWidget.h"
#include "Mesh.h"
#include "GameWorld.h"
#include "Renderer.h"

namespace Sentinel { namespace GUI
{
	DEFINE_SERIAL_REGISTER( MeshViewWidget );
	DEFINE_SERIAL_CLONE( MeshViewWidget );

	MeshViewWidget::MeshViewWidget()
	{}

	MeshViewWidget::~MeshViewWidget()
	{}

	///////////////////////////////////

	void MeshViewWidget::Startup()
	{}

	void MeshViewWidget::Update()
	{
		_ASSERT( mOwner->GetWorld()->mGameWorld );
		_ASSERT( mOwner->GetWorld()->mGameWorld->mRenderer );

		mMesh->mMatrixWorld = mOwner->GetWorld()->mMatrixWVP * ((ModelWidget*)(mOwner->GetModel()))->GetMatrixWorld();
		mMesh->Draw( mOwner->GetWorld()->mGameWorld->mRenderer, mOwner->GetWorld()->mGameWorld );
	}

	void MeshViewWidget::Shutdown()
	{}

	///////////////////////////////////

	DEFINE_SERIAL_REGISTER_SAVE( MeshViewWidget );

	void MeshViewWidget::Save( Archive& archive )
	{
		ViewWidget::Save( archive );
	}

	void MeshViewWidget::Load( Archive& archive )
	{
		ViewWidget::Load( archive );
	}
}}
