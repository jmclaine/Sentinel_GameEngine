#pragma once

#include "GameComponent.h"
#include "Matrix4f.h"
#include "Shape.h"
#include "ColorRGBA.h"
#include "Renderer.h"

#ifndef WIN32
#define HWND void*
#define GetClientRect( x, y );

struct RECT
{
	int right;
	int top;
	int left;
	int bottom;
};
#endif

namespace Sentinel {
namespace Component
{
	class Transform;

	class SENTINEL_DLL Camera : public GameComponent
	{
	protected:

		Transform* mTransform;

		Matrix4f mMatrixView;
		Matrix4f mMatrixProjection;
		Matrix4f mMatrixWVP;

		Vector3f mLookAt;

		BoundingFrustum mFrustum;

	public:

		WORD mRenderLayer;

		ColorRGBA mClearColor;
		float mClearDepth;

		int mViewportOffsetX;
		int mViewportOffsetY;

		UINT mViewportWidth;
		UINT mViewportHeight;

		DepthStencil* mDepthStencil;
		RenderTexture* mRenderTexture;

	protected:

		Camera();

	public:

		virtual void Startup();
		virtual void Shutdown();

		////////////////////////////////

		virtual void Execute();

		virtual void SetOwner(GameObject* owner);

		const Transform* GetTransform();

		const Matrix4f& GetMatrixWorld();
		const Matrix4f& GetMatrixView();
		const Matrix4f& GetMatrixProjection();
		const Matrix4f& GetMatrixWVP();

		const BoundingFrustum& GetFrustum();

		virtual Ray ScreenPointToRay(UINT mouseX, UINT mouseY, UINT screenWidth = 0, UINT screenHeight = 0) = 0;

		void Apply(Renderer* renderer);

		////////////////////////////////

		virtual void Save(Archive& archive);
		virtual void Load(Archive& archive);

		////////////////////////////////

		void Copy(GameComponent* component);
	};
}}
