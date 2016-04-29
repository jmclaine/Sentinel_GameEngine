#include "Component/OrthographicCamera.h"
#include "Component/Transform.h"
#include "Archive.h"
#include "Renderer.h"
#include "GameObject.h"
#include "GameWorld.h"

namespace Sentinel
{
	namespace Component
	{
		SerialRegister OrthographicCamera::SERIAL_REGISTER("OrthographicCamera", OrthographicCamera::Clone);
		Serializable* OrthographicCamera::Clone() { return new OrthographicCamera(); }

		OrthographicCamera::OrthographicCamera()
		{ }

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

			archive.Write(&mMatrixProjection);
		}

		void OrthographicCamera::Load(Archive& archive)
		{
			Camera::Load(archive);

			archive.Read(&mMatrixProjection);
		}

		//////////////////////////////

		GameComponent* OrthographicCamera::Copy()
		{
			OrthographicCamera* camera = new OrthographicCamera();

			Copy(camera);

			return camera;
		}

		void OrthographicCamera::Copy(GameComponent* component)
		{
			Camera::Copy(component);

			auto camera = static_cast<OrthographicCamera*>(component);
			camera->mMatrixProjection = mMatrixProjection;
		}
	}
}
