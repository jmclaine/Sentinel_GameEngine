#include "Component/Drawable.h"
#include "Component/Transform.h"
#include "GameObject.h"
#include "Archive.h"
#include "Exception.h"

namespace Sentinel {
namespace Component
{
	Drawable::Drawable() :
		mTransform(NULL),
		mIsDynamic(false)
	{}

	void Drawable::Startup()
	{
		mTransform = mOwner->GetComponent<Transform>();

		if (mTransform == NULL)
			throw AppException("Drawable::Startup()\n" + std::string(mOwner->mName) + " does not contain Transform");
	}

	void Drawable::Update()
	{}

	void Drawable::Shutdown()
	{
		mTransform = NULL;
	}

	///////////////////////////////////

	void Drawable::Execute()
	{
		CalculateBounds();
	}

	void Drawable::SetOwner(GameObject* owner)
	{
		GameComponent::SetOwner(owner);

		mOwner->mDrawable = this;
	}

	void Drawable::Save(Archive& archive)
	{
		GameComponent::Save(archive);

		archive.Write(&mIsDynamic);
	}

	void Drawable::Load(Archive& archive)
	{
		GameComponent::Load(archive);

		archive.Read(&mIsDynamic);
	}

	void Drawable::Copy(GameComponent* component)
	{
		GameComponent::Copy(component);

		((Drawable*)component)->mIsDynamic = mIsDynamic;
	}
}}
