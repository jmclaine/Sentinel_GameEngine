#include "Component/Transform.h"
#include "GameObject.h"
#include "Archive.h"

namespace Sentinel {
namespace Component
{
	DEFINE_SERIAL_REGISTER(Transform);
	DEFINE_SERIAL_CLONE(Transform);

	Transform::Transform() :
		mParentTransform(NULL),
		mScale(Vector3(1, 1, 1))
	{}

	Transform::~Transform()
	{
		if (mOwner->mTransform == this)
			mOwner->mTransform = NULL;
	}

	void Transform::Startup()
	{
		_ASSERT(mOwner);

		GameObject* parent = mOwner->GetParent();

		mParentTransform = (parent) ? parent->mTransform : NULL;
	}

	void Transform::Update()
	{}

	void Transform::Shutdown()
	{
		mParentTransform = NULL;
	}

	///////////////////////////////////

	void Transform::Execute()
	{
		mMatrixWorld.World(mPosition, mOrientation, mScale);

		if (mParentTransform)
			mMatrixWorld = mParentTransform->GetMatrixWorld() * mMatrixWorld;
	}

	void Transform::SetOwner(GameObject* owner)
	{
		GameComponent::SetOwner(owner);

		if (owner->mTransform == NULL)
			owner->mTransform = this;
	}

	const Matrix4x4& Transform::GetMatrixWorld() const
	{
		return mMatrixWorld;
	}

	const Matrix4x4& Transform::GetMatrixWorld(const Vector3& offset)
	{
		mMatrixWorld.World(mPosition, mOrientation, mScale, offset);

		if (mParentTransform)
			mMatrixWorld = mParentTransform->GetMatrixWorld() * mMatrixWorld;

		return mMatrixWorld;
	}

	///////////////////////////////////

	void Transform::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		GameComponent::Save(archive);

		archive.Write(mPosition.Ptr(), ar_sizeof(mPosition));
		archive.Write(mOrientation.Ptr(), ar_sizeof(mOrientation));
		archive.Write(mScale.Ptr(), ar_sizeof(mScale));
	}

	void Transform::Load(Archive& archive)
	{
		GameComponent::Load(archive);

		archive.Read(mPosition.Ptr(), ar_sizeof(mPosition));
		archive.Read(mOrientation.Ptr(), ar_sizeof(mOrientation));
		archive.Read(mScale.Ptr(), ar_sizeof(mScale));
	}

	///////////////////////////////////

	GameComponent* Transform::Copy()
	{
		Transform* transform = new Transform();

		GameComponent::Copy(transform);

		transform->mParentTransform = mParentTransform;
		transform->mPosition = mPosition;
		transform->mOrientation = mOrientation;
		transform->mScale = mScale;

		return transform;
	}
}}
