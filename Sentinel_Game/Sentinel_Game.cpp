/*
Runs the Game loaded from a MAP file.

Use this program to create customized implementations.

To add functionality to the engine itself, use the
Sentinel_DLL portion.  The resulting SentinelEngine.DLL
copies automatically to the editor and game directory
upon compilation / linking.
*/
#include "stdafx.h"

#include <crtdbg.h>
#include <exception>
#include <time.h>

#include "Sentinel_Game.h"

#include "Renderer.h"
#include "PhysicsSystem.h"
#include "SpriteSystem.h"
#include "Network/Socket.h"
#include "AudioSystem.h"

#include "GameWindow.h"
#include "WindowInfo.h"

#include "Archive.h"

#include "TextureManager.h"
#include "ShaderManager.h"
#include "MaterialManager.h"
#include "SpriteManager.h"
#include "MeshManager.h"
#include "ModelManager.h"
#include "SoundManager.h"
#include "RenderManager.h"
#include "BlendStateManager.h"

#include "RenderTexture.h"

#include "Input.h"
#include "Timing.h"
#include "Profile.h"
#include "Util.h"
#include "Debug.h"

#include "MeshBuilder.h"
#include "Model.h"

#include "GameWorld.h"
#include "GameObject.h"

#include "Component/Transform.h"
#include "Component/Physics.h"
#include "Component/Light.h"
#include "Component/PerspectiveCamera.h"
#include "Component/OrthographicCamera.h"
#include "Component/PlayerController.h"
#include "Component/ParticleEmitter.h"
#include "Component/MeshDrawable.h"
#include "Component/ModelDrawable.h"
#include "Component/SpriteDrawable.h"

#include "Sound.h"
#include "Sprite.h"

#include "GUI/Button.h"

#include "RandomValue.h"

using namespace Sentinel;


class MainApp
{
	HACCEL mAccelTable;

	GameWindow* mGameWindow;
	Renderer* mRenderer;

	GameWorld* mGameWorld;

	RenderTexture* mRTMain;
	DepthStencil* mDSMain;

public:

	MainApp() :
		mGameWindow(nullptr),
		mRenderer(nullptr),
		mGameWorld(nullptr)
	{
		srand((UINT)time((time_t*)0));
	}

	~MainApp()
	{ }

	void Startup(HINSTANCE hInstance, int nCmdShow)
	{
		mGameWindow = new GameWindow(IDI_SENTINEL_GAME, IDI_SMALL, 0);

		mAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SENTINEL_GAME));

		// Load config file to setup windows for the renderer.
		//
		WindowInfo info;
		mRenderer = Renderer::Create("config.xml", info);
		if (!mRenderer)
		{
			Debug::ShowError("Failed to load 'config.xml'\nDefaulting to OpenGL", "Renderer Setup Failure");

			if (!(mRenderer = BuildRendererGL()))
				throw AppException("Failed to create OpenGL Renderer");
		}

		// Prepare main window.
		//
		mGameWindow->Startup(mRenderer, hInstance, nCmdShow, "Sentinel Game", "SentinelClass", info);

		mRTMain = mRenderer->CreateBackbuffer();
		mDSMain = mRenderer->CreateDepthStencil(WindowInfo::BASE_WIDTH, WindowInfo::BASE_HEIGHT);

		////////////////////////////////////

		Mouse::Get().SetPosition(CenterHandle((HWND)mGameWindow->GetInfo()->Handle()));
		//ShowCursor(FALSE);

		////////////////////////////////////
		// Prepare GameWorld
		//
		mGameWorld = new GameWorld();

		mGameWorld->mRenderer = mRenderer;
		mGameWorld->mTiming = std::unique_ptr<Timing>(new Timing());
		mGameWorld->mPhysicsSystem = std::unique_ptr<PhysicsSystem>(BuildPhysicsSystemBT());
		mGameWorld->mAudioSystem = std::unique_ptr<AudioSystem>(BuildAudioSystemAL());

		mGameWorld->mTextureManager = std::unique_ptr<TextureManager>(new TextureManager(mRenderer));
		mGameWorld->mShaderManager = std::unique_ptr<ShaderManager>(new ShaderManager());
		mGameWorld->mMaterialManager = std::unique_ptr<MaterialManager>(new MaterialManager());
		mGameWorld->mSpriteManager = std::unique_ptr<SpriteManager>(new SpriteManager());
		mGameWorld->mMeshManager = std::unique_ptr<MeshManager>(new MeshManager());
		mGameWorld->mModelManager = std::unique_ptr<ModelManager>(new ModelManager());
		mGameWorld->mSoundManager = std::unique_ptr<SoundManager>(new SoundManager());
		mGameWorld->mRenderManager = std::unique_ptr<RenderManager>(new RenderManager());
		mGameWorld->mBlendStateManager = std::unique_ptr<BlendStateManager>(new BlendStateManager());

		mGameWorld->mPhysicsSystem->Startup();

		Archive archive;
		if (!archive.Open("Default.MAP", "rb"))
			throw AppException("Failed to load Default.MAP");

		mGameWorld->mTextureManager->Load(archive, mRenderer);
		mGameWorld->mShaderManager->Load(archive, mRenderer);
		mGameWorld->mMaterialManager->Load(archive, mGameWorld->mShaderManager.get(), mGameWorld->mTextureManager.get(), mGameWorld->mBlendStateManager.get());
		mGameWorld->mSpriteManager->Load(archive);
		mGameWorld->mMeshManager->Load(archive, mRenderer, mGameWorld->mMaterialManager.get());
		mGameWorld->mModelManager->Load(archive, mRenderer, mGameWorld->mShaderManager.get(), mGameWorld->mTextureManager.get(), mGameWorld->mMaterialManager.get(), mGameWorld->mBlendStateManager.get());
		mGameWorld->mSoundManager->Load(archive, mGameWorld->mAudioSystem.get());

		mGameWorld->mSpriteSystem = std::unique_ptr<SpriteSystem>(new SpriteSystem(mRenderer, mGameWorld->mShaderManager->Get("GUI").lock()->Layout(), 256));

		mGameWorld->Load(archive);

		archive.Close();

		////////////////////////////////////

		mGameWorld->Startup();

		// needs to create the depth stencil and render texture automatically
		Component::Camera* camera = mGameWorld->mCurrentCamera;
		camera->mDepthStencil = mDSMain;
		camera->mRenderTexture = mRTMain;
		camera->mViewportWidth = mGameWindow->GetInfo()->Width();
		camera->mViewportHeight = mGameWindow->GetInfo()->Height();
		camera->mClearColor = ColorRGBA(0.0f, 0.2f, 0.8f, 1.0f);
	}

	void Update()
	{
		MSG msg;
		for (;;)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					return;

				if (!TranslateAccelerator(msg.hwnd, mAccelTable, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			else
			{
				static Timing* timing;
				timing = mGameWorld->mTiming.get();

				timing->Update();

				Keyboard::Get().ProcessMessages();

				BEGIN_PROFILE(timing);
				if (Keyboard::Get().DidGoDown(VK_ESCAPE))
					return;

				mGameWindow->Update();

				BEGIN_PROFILE(timing);
				mGameWorld->UpdateController();

				BEGIN_PROFILE(timing);
				mGameWorld->UpdatePhysics();
				END_PROFILE(timing, "Physics");

				mGameWorld->UpdateTransform();
				mGameWorld->UpdateComponents();
				mGameWorld->UpdateCamera();

				BEGIN_PROFILE(timing);
				mGameWorld->UpdateLight();
				END_PROFILE(timing, "Light");

				BEGIN_PROFILE(timing);
				mGameWorld->Present();
				END_PROFILE(timing, "Drawable");

				END_PROFILE(timing, "World");

				BEGIN_PROFILE(timing);
				mRenderer->Present();
				END_PROFILE(timing, "Renderer");

				Mouse::Get().Update();
				Keyboard::Get().Update();
				END_PROFILE(timing, "Update");

				SEPARATE_PROFILE("-----------------------");

				timing->Limit();
			}
		}
	}

	void Shutdown()
	{
		SAFE_DELETE(mDSMain);
		SAFE_DELETE(mRTMain);

		SHUTDOWN_DELETE(mGameWorld);
		SHUTDOWN_DELETE(mGameWindow);

		SAFE_DELETE(mRenderer);
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////

int APIENTRY _tWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine,
	int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	//_CrtSetBreakAlloc(776);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	int result = 0;
	MainApp* mainApp = 0;

	try
	{
		mainApp = new MainApp();
		mainApp->Startup(hInstance, SW_MAXIMIZE);
		mainApp->Update();
	}
	catch (AppException e)
	{
		Debug::ShowError(e.what(), "Application Error");
	}
	catch (std::exception e)
	{
		Debug::ShowError(e.what(), "Application Error");
	}
	catch (std::bad_alloc&)
	{
		Debug::ShowError("Failed to allocate memory.", "Application Error");
	}
	catch (...)
	{
		Debug::ShowError("Failed to initialize.", "Application Error");
	}

	if (mainApp)
	{
		mainApp->Shutdown();
		delete mainApp;
	}

	return result;
}
