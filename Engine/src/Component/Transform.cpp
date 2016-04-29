#include "Component/Transform.h"
#include "GameObject.h"
#include "Archive.h"

namespace Sentinel 
{
	namespace Component
	{
		SerialRegister Transform::SERIAL_REGISTER("Transform", Transform::Clone);
		Serializable* Transform::Clone() { return new Transform(); }

		Transform::Transform() :
			mParentTransform(nullptr),
			mScale(Vector3(1, 1, 1))
		{ }

		Transform::~Transform()
		{
			if (mOwner->mTransform == this)
				mOwner->mTransform = nullptr;
		}

		void Transform::Startup()
		{
			_ASSERT(mOwner);

			GameObject* parent = mOwner->GetParent();

			mParentTransform = (parent) ? parent->mTransform : nullptr;
		}

		void Transform::Update()
		{ }

		void Transform::Shutdown()
		{
			mParentTransform = nullptr;
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

			if (owner->mTransform == nullptr)
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

			archive.Write(&mPosition);
			archive.Write(&mOrientation);
			archive.Write(&mScale);
		}

		void Transform::Load(Archive& archive)
		{
			GameComponent::Load(archive);

			archive.Read(&mPosition);
			archive.Read(&mOrientation);
			archive.Read(&mScale);
		}

		///////////////////////////////////

		GameComponent* Transform::Copy()
		{
			Transform* transform = new Transform();

			Copy(transform);

			return transform;
		}

		void Transform::Copy(GameComponent* component)
		{
			GameComponent::Copy(component);

			auto transform = static_cast<Transform*>(component);
			transform->mParentTransform = mParentTransform;
			transform->mPosition = mPosition;
			transform->mOrientation = mOrientation;
			transform->mScale = mScale;
		}
	}
}
