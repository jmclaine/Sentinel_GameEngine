#include "GUI/DrawableMesh.h"
#include "Mesh.h"
#include "CameraComponent.h"

namespace Sentinel { namespace GUI
{
	DEFINE_SERIAL_REGISTER( DrawableMesh );
	DEFINE_SERIAL_CLONE( DrawableMesh );

	DrawableMesh::DrawableMesh()
	{}

	DrawableMesh::~DrawableMesh()
	{}

	void DrawableMesh::Update()
	{
		Widget::PreUpdate();

		mMesh->mMatrixWorld = MATRIX_WVP * mMatrixWorld;
		mMesh->Draw( GAME_WORLD->mRenderer, GAME_WORLD );

		Widget::PostUpdate();
	}

	///////////////////////////////////

	void DrawableMesh::Save( Archive& archive )
	{
		mSerialRegistry.Save( archive );

		Widget::Save( archive );
	}

	void DrawableMesh::Load( Archive& archive )
	{
		Widget::Load( archive );
	}
}}
