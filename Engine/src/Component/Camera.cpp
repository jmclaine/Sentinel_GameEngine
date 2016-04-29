#include "Component/Camera.h"
#include "Exception.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Component/Transform.h"
#include "RenderManager.h"
#include "WindowInfo.h"

namespace Sentinel
{
	namespace Component
	{
		Camera::Camera() :
			mTransform(nullptr),
			mRenderLayer((WORD)RenderLayer::LAYER0),
			mClearDepth(1.0f),
			mViewportOffsetX(0),
			mViewportOffsetY(0),
			mViewportWidth(WindowInfo::BASE_WIDTH),
			mViewportHeight(WindowInfo::BASE_HEIGHT),
			mDepthStencil(nullptr),
			mRenderTexture(nullptr)
		{
			mMatrixView.Identity();
			mMatrixProjection.Identity();
			mMatrixWVP.Identity();
		}

		void Camera::Startup()
		{
			_ASSERT(mOwner);

			mTransform = mOwner->mTransform;

			if (mTransform == nullptr)
				throw AppException("Camera::Startup()\n" + std::string(mOwner->mName) + " does not contain Transform");

			Sentinel::GameWorld* world = mOwner->GetWorld();

			_ASSERT(world);

			world->AddCamera(this);
			world->mCurrentCamera = this;
		}

		void Camera::Shutdown()
		{
			mTransform = nullptr;
		}

		////////////////////////////////

		void Camera::Execute()
		{
			mMatrixView.Rotate(mTransform->mOrientation);
			mLookAt = mTransform->mPosition + mMatrixView.Forward();

			mMatrixView.LookAtView(mTransform->mPosition, mLookAt, mMatrixView.Up());
			mMatrixWVP = mMatrixProjection * mMatrixView;
		}

		void Camera::SetOwner(GameObject* owner)
		{
			GameComponent::SetOwner(owner);

			mOwner->mCamera = this;
		}

		const Transform* Camera::GetTransform()
		{
			return mTransform;
		}

		const Matrix4x4& Camera::GetMatrixWorld()
		{
			return mTransform->GetMatrixWorld();
		}

		const Matrix4x4& Camera::GetMatrixView()
		{
			return mMatrixView;
		}

		const Matrix4x4& Camera::GetMatrixProjection()
		{
			return mMatrixProjection;
		}

		const Matrix4x4& Camera::GetMatrixWVP()
		{
			return mMatrixWVP;
		}

		const BoundingFrustum& Camera::GetFrustum()
		{
			return mFrustum;
		}

		void Camera::Apply(Renderer* renderer)
		{
			renderer->SetViewport(
				mViewportOffsetX, mViewportOffsetY,
				mViewportWidth, mViewportHeight);

			renderer->SetDepthStencil(mDepthStencil);
			renderer->SetRenderTexture(mRenderTexture);

			if (mDepthStencil != nullptr && mRenderTexture != nullptr)
			{
				renderer->Clear(mClearColor.Ptr(), mClearDepth);
			}
			else if (mDepthStencil != nullptr)
			{
				renderer->ClearDepth(mClearDepth);
			}
			else if (mRenderTexture != nullptr)
			{
				renderer->ClearColor(mClearColor.Ptr());
			}
		}

		////////////////////////////////

		void Camera::Save(Archive& archive)
		{
			GameComponent::Save(archive);

			archive.Write(&mViewportOffsetX);
			archive.Write(&mViewportOffsetY);
			archive.Write(&mViewportWidth);
			archive.Write(&mViewportHeight);
			archive.Write(&mClearColor);
			archive.Write(&mClearDepth);
		}

		void Camera::Load(Archive& archive)
		{
			GameComponent::Load(archive);

			archive.Read(&mViewportOffsetX);
			archive.Read(&mViewportOffsetY);
			archive.Read(&mViewportWidth);
			archive.Read(&mViewportHeight);
			archive.Read(&mClearColor);
			archive.Read(&mClearDepth);
		}

		////////////////////////////////

		void Camera::Copy(GameComponent* component)
		{
			GameComponent::Copy(component);

			auto camera = static_cast<Camera*>(component);
			camera->mViewportOffsetX = mViewportOffsetX;
			camera->mViewportOffsetY = mViewportOffsetY;
			camera->mViewportWidth = mViewportWidth;
			camera->mViewportHeight = mViewportHeight;
			camera->mClearColor = mClearColor;
			camera->mClearDepth = mClearDepth;
		}
	}
}
