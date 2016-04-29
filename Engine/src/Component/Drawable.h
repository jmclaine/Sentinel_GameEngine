#pragma once

#include "Sentinel.h"
#include "Component/Camera.h"
#include "Shape.h"

namespace Sentinel {
namespace Component
{
	class Transform;

	class SENTINEL_DLL Drawable : public GameComponent
	{
	protected:
		Transform* mTransform;

	public:
		BoundingBox mBounds;

		bool mIsDynamic;

	protected:
		Drawable();

	public:
		virtual void Startup();
		virtual void Update();
		virtual void Shutdown();

		/////////////////////////////

		void SetOwner(GameObject* owner);

		virtual void CalculateBounds();
		virtual bool CheckVisible(Camera* camera);

		virtual void Draw(Camera* camera) = 0;

	protected:
		virtual void Save(Archive& archive);
		virtual void Load(Archive& archive);

		virtual void Copy(GameComponent* component);
	};
}}
