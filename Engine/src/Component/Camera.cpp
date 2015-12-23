#include "Component/Camera.h"
#include "Exception.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "Component/Transform.h"
#include "RenderManager.h"

namespace Sentinel {
namespace Component
{
	Camera::Camera() :
		mTransform(NULL),
		mRenderLayer(RenderLayer::LAYER0),
		mClearDepth(1.0f),
		mViewportOffsetX(0),
		mViewportOffsetY(0),
		mViewportWidth(Renderer::WINDOW_WIDTH_BASE),
		mViewportHeight(Renderer::WINDOW_HEIGHT_BASE),
		mDepthStencil(NULL),
		mRenderTexture(NULL)
	{
		mMatrixView.Identity();
		mMatrixProjection.Identity();
		mMatrixWVP.Identity();
	}

	void Camera::Startup()
	{
		_ASSERT(mOwner);

		mTransform = mOwner->mTransform;

		if (mTransform == NULL)
			throw AppException("Camera::Startup()\n" + std::string(mOwner->mName) + " does not contain Transform");

		Sentinel::GameWorld* world = mOwner->GetWorld();

		_ASSERT(world);

		world->AddCamera(this);
		world->mCurrentCamera = this;
	}

	void Camera::Shutdown()
	{
		mTransform = NULL;
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
		renderer->SetViewport(mViewportOffsetX, mViewportOffsetY,
			mViewportWidth, mViewportHeight);

		renderer->SetDepthStencil(mDepthStencil);
		renderer->SetRenderTexture(mRenderTexture);

		if (mDepthStencil != NULL && mRenderTexture != NULL)
		{
			renderer->Clear(mClearColor.Ptr(), mClearDepth);
		}
		else if (mDepthStencil != NULL)
		{
			renderer->ClearDepth(mClearDepth);
		}
		else if (mRenderTexture != NULL)
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
		archive.Write(mClearColor.Ptr(), ar_sizeof(mClearColor));
		archive.Write(&mClearDepth);
	}

	void Camera::Load(Archive& archive)
	{
		GameComponent::Load(archive);

		archive.Read(&mViewportOffsetX);
		archive.Read(&mViewportOffsetY);
		archive.Read(&mViewportWidth);
		archive.Read(&mViewportHeight);
		archive.Read(mClearColor.Ptr(), ar_sizeof(mClearColor));
		archive.Read(&mClearDepth);
	}

	////////////////////////////////

	void Camera::Copy(GameComponent* component)
	{
		Camera* camera = (Camera*)component;

		GameComponent::Copy(camera);

		camera->mViewportOffsetX = mViewportOffsetX;
		camera->mViewportOffsetY = mViewportOffsetY;
		camera->mViewportWidth = mViewportWidth;
		camera->mViewportHeight = mViewportHeight;
		camera->mClearColor = mClearColor;
		camera->mClearDepth = mClearDepth;
	}
}}
