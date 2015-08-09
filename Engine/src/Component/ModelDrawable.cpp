#include "Component/ModelDrawable.h"
#include "Model.h"
#include "ModelManager.h"
#include "Component/Transform.h"
#include "Material.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Archive.h"
#include "RenderManager.h"
#include "Timing.h"

namespace Sentinel { namespace Component
{
	DEFINE_SERIAL_REGISTER( ModelDrawable );
	DEFINE_SERIAL_CLONE( ModelDrawable );

	ModelDrawable::ModelDrawable() :
		mModel( NULL )
	{}

	ModelDrawable::ModelDrawable( std::shared_ptr< Model > model )
	{
		Set( model );
	}

	void ModelDrawable::Set( std::shared_ptr< Model > model )
	{
		mModel = model;
	}

	void ModelDrawable::Startup()
	{
		Drawable::Startup();

		if( mOwner->GetWorld() )
			mOwner->GetWorld()->mRenderManager->Draw( this, RenderQueue::GEOMETRY, mOwner->mLayer );
	}

	void ModelDrawable::Update()
	{
		mModel->Update( mOwner->GetWorld()->mTiming->DeltaTime() );

		Drawable::Update();

		Draw();
	}

	void ModelDrawable::Shutdown()
	{
		Drawable::Shutdown();
	}

	///////////////////////////////////

	void ModelDrawable::CalculateBounds()
	{}

	bool ModelDrawable::CheckVisible( Camera* camera )
	{
		return true;
	}

	void ModelDrawable::Draw()
	{
		mModel->mMatrixWorld = mTransform->GetMatrixWorld();

		GameWorld* world = mOwner->GetWorld();

		mModel->Draw( world->mRenderer, world, world->mCurrentCamera );
	}

	///////////////////////////////////

	void ModelDrawable::Save( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mModelManager );

		SERIAL_REGISTER.Save( archive );

		Drawable::Save( archive );

		GameWorld* world = mOwner->GetWorld();

		archive.Write( &world->mModelManager->Get( mModel ));
	}

	void ModelDrawable::Load( Archive& archive )
	{
		_ASSERT( mOwner );
		_ASSERT( mOwner->GetWorld() );
		_ASSERT( mOwner->GetWorld()->mModelManager );

		Drawable::Load( archive );

		GameWorld* world = mOwner->GetWorld();

		std::string name;
		archive.Read( &name );

		mModel = world->mModelManager->Get( name );
	}

	///////////////////////////////////

	GameComponent* ModelDrawable::Copy()
	{
		ModelDrawable* drawable = new ModelDrawable( mModel );
		
		Drawable::Copy( drawable );

		return drawable;
	}
}}
