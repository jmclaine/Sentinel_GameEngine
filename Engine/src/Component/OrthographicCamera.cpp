#include "Component/OrthographicCamera.h"
#include "Component/Transform.h"
#include "Archive.h"
#include "Renderer.h"
#include "GameObject.h"
#include "GameWorld.h"

namespace Sentinel {
namespace Component
{
	DEFINE_SERIAL_REGISTER(OrthographicCamera);
	DEFINE_SERIAL_CLONE(OrthographicCamera);

	OrthographicCamera::OrthographicCamera()
	{}

	OrthographicCamera::OrthographicCamera(UINT windowWidth, UINT windowHeight)
	{
		Set(windowWidth, windowHeight);
	}

	//////////////////////////////

	void OrthographicCamera::Set(UINT windowWidth, UINT windowHeight)
	{
		mMatrixProjection.ProjectionOrthographic((float)windowWidth, (float)windowHeight);
	}

	Ray OrthographicCamera::ScreenPointToRay(UINT mouseX, UINT mouseY, UINT screenWidth, UINT screenHeight)
	{
		return Ray(Vector3((float)mouseX, (float)mouseY, 0), Vector3(0, 0, -1));
	}

	//////////////////////////////

	void OrthographicCamera::Save(Archive& archive)
	{
		SERIAL_REGISTER.Save(archive);

		Camera::Save(archive);

		archive.Write(mMatrixProjection.Ptr(), ar_sizeof(mMatrixProjection));
	}

	void OrthographicCamera::Load(Archive& archive)
	{
		Camera::Load(archive);

		archive.Read(mMatrixProjection.Ptr(), ar_sizeof(mMatrixProjection));
	}

	//////////////////////////////

	GameComponent* OrthographicCamera::Copy()
	{
		OrthographicCamera* camera = new OrthographicCamera();

		Camera::Copy(camera);

		camera->mMatrixProjection = mMatrixProjection;

		return camera;
	}
}}
