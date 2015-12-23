#pragma once

#include "GameComponent.h"
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Quaternion.h"

namespace Sentinel {
namespace Component
{
	class SENTINEL_DLL Transform : public GameComponent
	{
		DECLARE_SERIAL();

	protected:

		Transform* mParentTransform;

		Matrix4x4 mMatrixWorld;

	public:

		Vector3 mPosition;
		Quaternion mOrientation;
		Vector3 mScale;

		///////////////////////////////////

		Transform();
		~Transform();

		virtual void Startup();

		virtual void Update();

		virtual void Shutdown();

		///////////////////////////////////

		virtual void Execute();

		virtual void SetOwner(GameObject* owner);

		const Matrix4x4&	GetMatrixWorld() const;
		const Matrix4x4&	GetMatrixWorld(const Vector3& offset);

		///////////////////////////////////

		GameComponent* Copy();
	};
}}
