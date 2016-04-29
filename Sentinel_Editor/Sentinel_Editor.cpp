/*
Edits levels for a game created with the Sentinel Engine.
*/
#include "stdafx.h"

#include <crtdbg.h>
#include <exception>
#include <time.h>

#include "dirent.h"

#include "Sentinel_Editor.h"

#include "Renderer.h"
#include "PhysicsSystem.h"
#include "SpriteSystem.h"
#include "Network/Socket.h"
#include "AudioSystem.h"
#include "FontSystem.h"

#include "Archive.h"
#include "WindowInfo.h"
#include "RenderTexture.h"

#include "TextureManager.h"
#include "ShaderManager.h"
#include "MaterialManager.h"
#include "SpriteManager.h"
#include "MeshManager.h"
#include "ModelManager.h"
#include "SoundManager.h"
#include "FontManager.h"
#include "RenderManager.h"
#include "BlendStateManager.h"

#include "Input.h"
#include "Timing.h"
#include "Util.h"
#include "FileUtil.h"
#include "Profile.h"

#include "GameWindow.h"
#include "GameWorld.h"
#include "GameObject.h"

#include "Component/Transform.h"
#include "Component/Physics.h"
//#include "DirectionalLightComponent.h"
#include "Component/PointLight.h"
#include "Component/PerspectiveCamera.h"
#include "Component/OrthographicCamera.h"
#include "Component/PlayerController.h"
#include "Component/SpriteParticleEmitter.h"
#include "Component/SpriteDrawable.h"
#include "Component/MeshDrawable.h"
#include "Component/ModelDrawable.h"

#include "Particle.h"
#include "ParticleEffect.h"

#include "EditorControllerComponent.h"

#include "MeshBuilder.h"
#include "Model.h"
#include "Sound.h"
#include "Sprite.h"
#include "Shape.h"
#include "Buffer.h"

#include "GUI/SpriteController.h"
#include "GUI/SpriteButton.h"
#include "GUI/Label.h"

#include "RandomValue.h"

#include "Debug.h"

using namespace Sentinel;
using namespace Sentinel::Component;


class MainApp
{
	HACCEL mAccelTable;

	GameWindow* mGameWindow;
	Renderer* mRenderer;

	GameWorld* mGameWorld;
	GameWorld* mEditorWorld;

	RenderTexture* mRTMain;
	DepthStencil* mDSMain;

	RenderTexture* mRTGameWorld;
	DepthStencil* mDSGameWorld;

	Mesh* mRTBackbufferMesh;

	GameObject* mSelectedObject;
	GameObject* mHoverObject;
	float mHoverDist;

	GameObject* mTranslateObject;
	GameObject* mBoundsObject;

	ColorRGBA mSelectedColorX;
	ColorRGBA mSelectedColorY;
	ColorRGBA mSelectedColorZ;
	ColorRGBA mSelectedColorAxis;

	GameObject* mGameWorldWidget;
	GUI::Label* mDebugText;

	Debug* mDebug;

	Ray mMouseToWorldRay;

	OrthographicCamera* mEditorCamera;
	PerspectiveCamera* mWorldCamera;

	Transform* normalMapTrans;

public:

	MainApp() :
		mGameWindow(nullptr),
		mRenderer(nullptr),
		mGameWorld(nullptr),
		mEditorWorld(nullptr),
		mRTMain(nullptr),
		mDSMain(nullptr),
		mRTGameWorld(nullptr),
		mDSGameWorld(nullptr),
		mRTBackbufferMesh(nullptr),
		mSelectedObject(nullptr),
		mHoverObject(nullptr),
		mHoverDist(0),
		mTranslateObject(nullptr),
		mBoundsObject(nullptr),
		mSelectedColorX(1, 0, 0, 0.5f),
		mSelectedColorY(0, 1, 0, 0.5f),
		mSelectedColorZ(0, 0, 1, 0.5f),
		mSelectedColorAxis(1, 1, 0, 0.5f),
		mDebug(nullptr),
		mMouseToWorldRay(Vector3(), Vector3())
	{
		srand((UINT)time((time_t*)0));
	}

	~MainApp()
	{ }

	void Startup(HINSTANCE hInstance, int nCmdShow)
	{
		mGameWindow = new GameWindow(IDI_SENTINEL_EDITOR, IDI_SMALL, 0);

		mAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SENTINEL_EDITOR));

		WindowInfo info;
		mRenderer = Renderer::Create("config.xml", info);

		if (!mRenderer)
		{
			Debug::ShowError(
				"Failed to load 'config.xml'\nDefaulting to OpenGL",
				"Renderer Setup Failure");

			if (!(mRenderer = BuildRendererGL()))
				throw AppException("Failed to create OpenGL Renderer");
		}

		mGameWindow->Startup(mRenderer, hInstance, nCmdShow, "Sentinel Editor", "SentinelClass", info);

		mRTMain = mRenderer->CreateBackbuffer();
		mDSMain = mRenderer->CreateDepthStencil(WindowInfo::BASE_WIDTH, WindowInfo::BASE_HEIGHT);

		SetDirectory("Assets");

		PrepareEditorWorld();
		PrepareGameWorld();
		PrepareDebug();

		GUI::SpriteController* controller = mGameWorldWidget->GetComponent<GUI::SpriteController>();
		controller->mActionOver += BIND(MainApp::SelectGameObject);

		SetDirectory("..");
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
				static char drawnText[256];
				static Timing* timing;

				timing = mEditorWorld->mTiming.get();

				timing->Update();
				mGameWorld->mTiming->Update();

				Keyboard::Get().ProcessMessages();

				BEGIN_PROFILE(timing);
				if (Keyboard::Get().DidGoDown(VK_ESCAPE))
					return;

				if (Keyboard::Get().DidGoDown(VK_F9))
					SaveMAP("Default.MAP");

				mGameWindow->Update();

				static float rotY = 0;
				rotY += 0.25f;
				normalMapTrans->mOrientation = Quaternion(Vector3(0, rotY, 0));

				//BEGIN_PROFILE(timing);
				//mGameWorld->UpdatePhysics();
				//END_PROFILE(timing, "Physics");

				mGameWorld->UpdateTransform();

				BEGIN_PROFILE(timing);
				mGameWorld->UpdateLight();
				END_PROFILE(timing, "Light");

				BEGIN_PROFILE(timing);
				mGameWorld->UpdateComponents();
				mGameWorld->Present(mWorldCamera);

				DrawBounds();
				DrawTranslateObject();
				END_PROFILE(timing, "Drawable");

				////////////////////////////////////

				const WindowInfo* info = mGameWindow->GetInfo();

				BEGIN_PROFILE(timing);
				mEditorWorld->UpdateController();
				mEditorWorld->UpdatePhysics();
				mEditorWorld->UpdateTransform();
				mEditorWorld->UpdateComponents();

				mEditorCamera->Set(info->Width(), info->Height());
				mEditorCamera->mViewportWidth = info->Width();
				mEditorCamera->mViewportHeight = info->Height();

				mEditorWorld->UpdateCamera();
				mEditorWorld->Present(mEditorCamera);
				END_PROFILE(timing, "Editor");

				// ** DEBUG **
				// Draw GameWorld over Backbuffer
				//
				//mRTBackbufferMesh->mMatrixWorld.Identity();
				//mRTBackbufferMesh->Draw(mRenderer, mGameWorld, mWorldCamera);

				BEGIN_PROFILE(timing);
				mRenderer->Present();
				END_PROFILE(timing, "Renderer");

				Mouse::Get().Update();
				Keyboard::Get().Update();
				END_PROFILE(timing, "Update");

				SEPARATE_PROFILE("-----------------------");

				timing->Limit();
				mGameWorld->mTiming->Limit(0);
			}
		}
	}

	void Shutdown()
	{
		SAFE_DELETE(mRTMain);
		SAFE_DELETE(mDSMain);

		SAFE_DELETE(mRTGameWorld);
		SAFE_DELETE(mDSGameWorld);

		SAFE_DELETE(mRTBackbufferMesh);

		SAFE_DELETE(mTranslateObject);
		SAFE_DELETE(mBoundsObject);

		SAFE_DELETE(mDebug);

		SHUTDOWN_DELETE(mGameWorld);
		SHUTDOWN_DELETE(mEditorWorld);

		SHUTDOWN_DELETE(mGameWindow);

		SAFE_DELETE(mRenderer);
	}

	void SaveMAP(const char* filename)
	{
		Archive archive;
		if (!archive.Open(filename, "wb+"))
		{
			Debug::ShowError(
				STREAM("Failed to save " << filename),
				STREAM("File IO Error"));

			return;
		}

		auto shaderManager = mGameWorld->mShaderManager.get();
		auto textureManager = mGameWorld->mTextureManager.get();
		auto materialManager = mGameWorld->mMaterialManager.get();
		auto blendManager = mGameWorld->mBlendStateManager.get();

		mGameWorld->mTextureManager->Save(archive, mRenderer);
		mGameWorld->mShaderManager->Save(archive);
		mGameWorld->mMaterialManager->Save(archive, shaderManager, textureManager, blendManager);
		mGameWorld->mSpriteManager->Save(archive);
		mGameWorld->mMeshManager->Save(archive, mRenderer, materialManager);
		mGameWorld->mModelManager->Save(archive, mRenderer, shaderManager, textureManager, materialManager, blendManager);
		mGameWorld->mSoundManager->Save(archive);

		mGameWorld->Save(archive);

		archive.Close();

		system("copy Default.MAP ..\\Sentinel_Game");
	}

	void CreateDefaultBlend(GameWorld* world)
	{
		{
			auto state = std::shared_ptr<BlendState>(
				mRenderer->CreateBlendState(
					BlendFormat::ONE, BlendFormat::ZERO, BlendFunction::ADD,
					BlendFormat::ONE, BlendFormat::ZERO, BlendFunction::ADD));
			world->mBlendStateManager->Add("DEFAULT", state);
		}
		{
			auto state = std::shared_ptr<BlendState>(
				mRenderer->CreateBlendState(
					BlendFormat::SRC_ALPHA, BlendFormat::ONE_MINUS_SRC_ALPHA, BlendFunction::ADD,
					BlendFormat::SRC_ALPHA, BlendFormat::ONE_MINUS_SRC_ALPHA, BlendFunction::ADD));
			world->mBlendStateManager->Add("ALPHA", state);
		}
	}

	void CreateDefaultTexture(GameWorld* world)
	{
		UCHAR* newTex = new UCHAR[4];

		newTex[0] = 255;
		newTex[1] = 255;
		newTex[2] = 255;
		newTex[3] = 255;

		auto texture = std::shared_ptr<Texture>(
			mRenderer->CreateTexture(newTex, 1, 1, ImageFormat::RGBA, false));

		delete newTex;

		world->mTextureManager->Add("NULL", texture);
	}

	void PrepareEditorWorld()
	{
		mEditorWorld = new GameWorld();

		mEditorWorld->mRenderer = mRenderer;
		mEditorWorld->mRenderManager = std::unique_ptr<RenderManager>(new RenderManager());

		mEditorWorld->mTiming = std::unique_ptr<Timing>(new Timing());
		mEditorWorld->mPhysicsSystem = std::unique_ptr<PhysicsSystem>(BuildPhysicsSystemBT());

		mEditorWorld->mAudioSystem = std::unique_ptr<AudioSystem>(BuildAudioSystemAL());
		mEditorWorld->mSoundManager = std::unique_ptr<SoundManager>(new SoundManager());

		mEditorWorld->mFontSystem = std::unique_ptr<FontSystem>(BuildFontSystemFT());
		mEditorWorld->mFontManager = std::unique_ptr<FontManager>(new FontManager());

		mEditorWorld->mShaderManager = std::unique_ptr<ShaderManager>(new ShaderManager());
		mEditorWorld->mTextureManager = std::unique_ptr<TextureManager>(new TextureManager(mRenderer));
		
		mEditorWorld->mMaterialManager = std::unique_ptr<MaterialManager>(new MaterialManager());
		mEditorWorld->mSpriteManager = std::unique_ptr<SpriteManager>(new SpriteManager());
		mEditorWorld->mMeshManager = std::unique_ptr<MeshManager>(new MeshManager());
		mEditorWorld->mModelManager = std::unique_ptr<ModelManager>(new ModelManager());
		mEditorWorld->mBlendStateManager = std::unique_ptr<BlendStateManager>(new BlendStateManager());

		mEditorWorld->mPhysicsSystem->Startup();

		// Create Default Blend
		CreateDefaultBlend(mEditorWorld);

		// Create Default Texture
		CreateDefaultTexture(mEditorWorld);
		
		// Create Shaders
		{
			SetDirectory("Shaders");

			DIR *dir;
			struct dirent *ent;
			if ((dir = opendir(GetDirectory().c_str())) != nullptr)
			{
				while ((ent = readdir(dir)) != nullptr)
				{
					std::string name(ent->d_name);
					size_t len = name.size();

					if (name.find(".xsh") != std::string::npos)
					{
						Shader* shader = mRenderer->CreateShaderFromFile(name.c_str());

						if (!shader)
						{
							closedir(dir);
							throw AppException("Failed to load 'Assets\\Shaders\\" + name);
						}

						mEditorWorld->mShaderManager->Add(name.substr(0, name.find_first_of('.')), std::shared_ptr<Shader>(shader));
					}
				}
				closedir(dir);
			}
			else
			{
				throw AppException("Failed to open 'Assets\\Shaders' directory.");
			}

			SetDirectory("..");
		}
		// Create Font
		{
			auto shader = mEditorWorld->mShaderManager->Get("GUI").lock();

			auto fontSystem = mEditorWorld->mFontSystem.get();
			fontSystem->mSpriteSystem = std::unique_ptr<SpriteSystem>(new SpriteSystem(mRenderer, shader->Layout(), 256));
			fontSystem->Load("Font\\courbd.ttf");

			auto font = fontSystem->Build(12, 12);
			auto material = font->mMaterial;
			material->mShader = shader;
			material->mBlendState = mEditorWorld->mBlendStateManager->Get("ALPHA");
			material->mCullMode = CullFormat::NONE;
			material->mDepthMode = DepthFormat::OFF;

			mEditorWorld->mFontManager->Add("courbd.ttf", std::shared_ptr<Font>(font));

			fontSystem->mSpriteSystem->mMaterial = material;
		}
		// Create GUI
		{
			auto shader = mEditorWorld->mShaderManager->Get("GUI").lock();
			mEditorWorld->mSpriteSystem = std::unique_ptr<SpriteSystem>(new SpriteSystem(mRenderer, shader->Layout(), 256));

			auto texture = std::shared_ptr<Texture>(mRenderer->CreateTextureFromFile("Textures\\GUI.png"));
			mEditorWorld->mTextureManager->Add("GUI", texture);

			auto material = std::shared_ptr<Material>(new Material());
			mEditorWorld->mMaterialManager->Add("GUI", material);

			material->mShader = mEditorWorld->mShaderManager->Get("GUI");
			material->mBlendState = mEditorWorld->mBlendStateManager->Get("ALPHA");
			material->mTexture[(BYTE)TextureIndex::DIFFUSE] = texture;
			material->mRenderQueue = (WORD)RenderQueue::FOREGROUND;
			material->mCullMode = CullFormat::NONE;
			material->mDepthMode = DepthFormat::OFF;

			mEditorWorld->mSpriteSystem->mMaterial = material;

			mEditorWorld->mShaderManager->Get("GUI").lock()->SetSampler(0, SamplerMode::WRAP, SamplerMode::WRAP, SamplerFilter::POINT, SamplerFilter::POINT);
			mEditorWorld->mShaderManager->Get("GUI_Mesh").lock()->SetSampler(0, SamplerMode::WRAP, SamplerMode::WRAP, SamplerFilter::POINT, SamplerFilter::POINT);
			mEditorWorld->mShaderManager->Get("RT_Color").lock()->SetSampler(0, SamplerMode::WRAP, SamplerMode::WRAP, SamplerFilter::POINT, SamplerFilter::POINT);
			mEditorWorld->mShaderManager->Get("Normal_Map").lock()->SetSampler(0, SamplerMode::WRAP, SamplerMode::WRAP, SamplerFilter::POINT, SamplerFilter::POINT, SamplerFilter::LINEAR);
		}
		// Create World View Backbuffer
		{
			auto texture = std::shared_ptr<Texture>(mRenderer->CreateTexture(nullptr, WindowInfo::BASE_WIDTH, WindowInfo::BASE_HEIGHT, ImageFormat::RGB, false));
			mEditorWorld->mTextureManager->Add("Backbuffer", texture);

			mRTGameWorld = mRenderer->CreateRenderTexture(texture.get());
			mDSGameWorld = mRenderer->CreateDepthStencil(WindowInfo::BASE_WIDTH, WindowInfo::BASE_HEIGHT);

			auto shader = mEditorWorld->mShaderManager->Get("RT_Color");
			auto material = std::shared_ptr<Material>(new Material());
			material->mShader = shader;
			material->mBlendState = mEditorWorld->mBlendStateManager->Get("DEFAULT");
			material->mTexture[(BYTE)TextureIndex::DIFFUSE] = texture;
			material->mRenderQueue = (WORD)RenderQueue::FOREGROUND;
			mEditorWorld->mMaterialManager->Add("Backbuffer", material);

			mRTBackbufferMesh = MeshBuilder::BuildRenderTextureMesh(mRenderer, shader.lock()->Layout());
			mRTBackbufferMesh->mMaterial = material;
		}
		// GUI Camera
		{
			auto cameraGUI = new GameObject("GUI Camera");

			cameraGUI->AddComponent(new Transform());

			mEditorCamera = new OrthographicCamera(WindowInfo::BASE_WIDTH, WindowInfo::BASE_HEIGHT);
			mEditorCamera->mDepthStencil = mDSMain;
			mEditorCamera->mRenderTexture = mRTMain;
			mEditorCamera->mClearColor = ColorRGBA(0.2f, 0.2f, 0.2f, 1.0f);

			cameraGUI->AddComponent(mEditorCamera);

			mEditorWorld->AddGameObject(cameraGUI);
		}
		// GUI Widgets
		{
			auto texture = mEditorWorld->mTextureManager->Get("Backbuffer").lock();

			auto sprite = std::shared_ptr<Sprite>(new Sprite());
			sprite->AddFrame(Sprite::QUADtoTEXCOORD(Quad(0, 0, 1, 1), texture->Width(), texture->Height()));
			sprite->AddFrame(Sprite::QUADtoTEXCOORD(Quad(1, 0, 2, 1), texture->Width(), texture->Height()));
			sprite->AddFrame(Sprite::QUADtoTEXCOORD(Quad(2, 0, 3, 1), texture->Width(), texture->Height()));

			mEditorWorld->mSpriteManager->Add("GUI", sprite);
		}
		// Menu Bar
		{
			auto widget = new GameObject("Menu Bar");

			auto transform = new Transform();
			transform->mPosition = Vector3(0, 0, 1);
			transform->mScale = Vector3((float)WindowInfo::BASE_WIDTH, 0, 1);
			widget->AddComponent(transform);

			auto view = new SpriteDrawable(mEditorWorld->mSpriteManager->Get("GUI"));
			view->mFrame = 0;
			view->mColor = ColorRGBA(0.7f, 0.7f, 0.7f, 1);
			widget->AddComponent(view);

			auto controller = new GUI::SpriteController();
			controller->mMargin.bottom = 30;
			controller->mScaleToWindowX = true;
			widget->AddComponent(controller);

			mEditorWorld->AddGameObject(widget);
		}
		// Menu Transform
		{
			auto menu = new GameObject("Menu Transform");
			{
				auto transform = new Transform();
				transform->mPosition = Vector3(0, 0, 0.9f);
				transform->mScale = Vector3(1, 1, 1);
				menu->AddComponent(transform);
			}
			// Menu Labels
			{
				auto widget = new GameObject("Menu Labels");

				auto transform = new Transform();
				transform->mPosition = Vector3(10, 20, 0);
				transform->mScale = Vector3(12, 12, 1);
				widget->AddComponent(transform);

				//label = (GUI::Label*)widget->AddComponent( new GUI::Label( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ{ }[]!@#$%^&*()1234567890" ));
				auto label = new GUI::Label("File");
				label->mColor = ColorRGBA(0, 0, 0, 1);
				label->mFont = mEditorWorld->mFontManager->Get("courbd.ttf");
				widget->AddComponent(label);

				auto controller = new GUI::SpriteController();
				widget->AddComponent(controller);

				menu->AddChild(widget);
			}
			//////////////////////////////////////
			{
				auto widget = new GameObject("Edit");

				auto transform = new Transform();
				transform->mPosition = Vector3(110, 20, 0);
				transform->mScale = Vector3(12, 12, 1);
				widget->AddComponent(transform);

				auto label = new GUI::Label("Edit");
				label->mColor = ColorRGBA(0, 0, 0, 1);
				label->mFont = mEditorWorld->mFontManager->Get("courbd.ttf");
				widget->AddComponent(label);

				auto controller = new GUI::SpriteController();
				widget->AddComponent(controller);

				menu->AddChild(widget);
			}
			//////////////////////////////////////
			{
				auto widget = new GameObject("Help");

				auto transform = new Transform();
				transform->mPosition = Vector3(210, 20, 0);
				transform->mScale = Vector3(12, 12, 1);
				widget->AddComponent(transform);

				auto label = new GUI::Label("Help");
				label->mColor = ColorRGBA(0, 0, 0, 1);
				label->mFont = mEditorWorld->mFontManager->Get("courbd.ttf");
				widget->AddComponent(label);

				auto controller = new GUI::SpriteController();
				widget->AddComponent(controller);

				menu->AddChild(widget);
			}
			mEditorWorld->AddGameObject(menu);
		}
		//////////////////////////////////////
		{
			auto widget = new GameObject("Debug");

			auto transform = new Transform();
			transform->mPosition = Vector3(0, 40, 0.9f);
			transform->mScale = Vector3(10, 10, 1);
			widget->AddComponent(transform);

			mDebugText = new GUI::Label();
			mDebugText->mColor = ColorRGBA(0, 0, 0, 1);
			mDebugText->mFont = mEditorWorld->mFontManager->Get("courbd.ttf");
			widget->AddComponent(mDebugText);

			auto controller = new GUI::SpriteController();
			widget->AddComponent(controller);

			mEditorWorld->AddGameObject(widget);
		}
		// Toolbar
		{
			auto widget = new GameObject("Toolbar");

			auto transform = new Transform();
			transform->mPosition = Vector3(0, 0, 1);
			transform->mScale = Vector3((float)WindowInfo::BASE_WIDTH, 0, 1);
			widget->AddComponent(transform);

			auto view = new SpriteDrawable(mEditorWorld->mSpriteManager->Get("GUI"));
			view->mFrame = 0;
			view->mColor = ColorRGBA(0.9f, 0.9f, 0.9f, 1);
			widget->AddComponent(view);

			auto controller = new GUI::SpriteController();
			controller->mMargin = Quad(0, 30, 0, 30);
			controller->mScaleToWindowX = true;
			widget->AddComponent(controller);

			mEditorWorld->AddGameObject(widget);
		}
		// Status Bar
		{
			auto widget = new GameObject("Status Bar");

			auto transform = new Transform();
			transform->mPosition = Vector3(0, (float)WindowInfo::BASE_HEIGHT, 0.1f);
			transform->mScale = Vector3((float)WindowInfo::BASE_WIDTH, 0, 1);
			widget->AddComponent(transform);

			auto view = new SpriteDrawable(mEditorWorld->mSpriteManager->Get("GUI"));
			view->mFrame = 0;
			view->mColor = ColorRGBA(0.7f, 0.7f, 0.7f, 1);
			widget->AddComponent(view);

			auto controller = new GUI::SpriteController();
			controller->mMargin = Quad(0, -30, 0, 30);
			controller->mScaleToWindowX = true;
			controller->mPositionToWindowY = true;
			widget->AddComponent(controller);

			mEditorWorld->AddGameObject(widget);
		}
		// World View Camera
		{
			auto obj = new GameObject("Main Camera");

			auto transform = new Transform();
			transform->mPosition = Vector3(0, 15, 40);
			transform->mOrientation = Quaternion(-15, 0, 0);
			obj->AddComponent(transform);

			auto physicsSystem = mEditorWorld->mPhysicsSystem.get();
			auto body = physicsSystem->CreateRigidBody(
				physicsSystem->CreateSphere(1.0f), 
				transform->mPosition, transform->mOrientation, 1.0f);
			body->SetFlags(DISABLE_GRAVITY);
			body->SetRestitution(1.0f);
			body->SetDamping(0.999f, 0.999f);
			obj->AddComponent(new Physics(body));

			mWorldCamera = new PerspectiveCamera(WindowInfo::BASE_WIDTH, WindowInfo::BASE_HEIGHT);
			mWorldCamera->mClearColor = ColorRGBA(0.0f, 0.2f, 0.8f, 1.0f);
			mWorldCamera->mViewportWidth = WindowInfo::BASE_WIDTH;
			mWorldCamera->mViewportHeight = WindowInfo::BASE_HEIGHT;
			mWorldCamera->mDepthStencil = mDSGameWorld;
			mWorldCamera->mRenderTexture = mRTGameWorld;
			obj->AddComponent(mWorldCamera);

			auto editorController = new EditorControllerComponent();
			editorController->mEditorCamera = mEditorCamera;
			editorController->mWorldCamera = mWorldCamera;
			obj->AddComponent(editorController);

			mEditorWorld->AddGameObject(obj);
		}
		// World Drawable
		{
			mGameWorldWidget = new GameObject("World Drawable");

			auto transform = new Transform();
			transform->mPosition = Vector3(0.0f, 0.0f, 1.0f);
			transform->mScale = Vector3((float)(WindowInfo::BASE_WIDTH - 500), (float)WindowInfo::BASE_HEIGHT, 1.0f);
			mGameWorldWidget->AddComponent(transform);

			auto material = std::shared_ptr<Material>(new Material());
			material->mShader = mEditorWorld->mShaderManager->Get("GUI_Mesh");
			material->mBlendState = mEditorWorld->mBlendStateManager->Get("DEFAULT");
			material->mCullMode = CullFormat::NONE;
			material->mTexture[(BYTE)TextureIndex::DIFFUSE] = mEditorWorld->mTextureManager->Get("Backbuffer");
			material->mRenderQueue = (WORD)RenderQueue::GEOMETRY;
			mEditorWorld->mMaterialManager->Add("World", material);

			auto mesh = std::shared_ptr<Mesh>(MeshBuilder::BuildGUIMesh(mRenderer, material->mShader.lock()->Layout()));
			mesh->mMaterial = material;
			mEditorWorld->mMeshManager->Add("World", mesh);

			mGameWorldWidget->AddComponent(new MeshDrawable(std::weak_ptr<Mesh>(mesh)));

			auto controller = new GUI::SpriteController();
			controller->mScaleToWindowX = true;
			controller->mScaleToWindowY = true;
			controller->mMargin = Quad(0, 60, 0, -90);
			mGameWorldWidget->AddComponent(controller);

			mEditorWorld->AddGameObject(mGameWorldWidget);
		}
		///////////////////////////////

		mEditorWorld->Startup();
	}

	void PrepareGameWorld()
	{
		mGameWorld = new GameWorld();

		mGameWorld->mRenderer = mRenderer;
		mGameWorld->mTiming = std::unique_ptr<Timing>(new Timing());
		mGameWorld->mPhysicsSystem = std::unique_ptr<PhysicsSystem>(BuildPhysicsSystemSE());
		mGameWorld->mAudioSystem = nullptr;

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

		////////////////////////////////////

		mGameWorld->mTextureManager->Add("default-alpha", std::shared_ptr<Texture>(mRenderer->CreateTextureFromFile("Textures\\default-alpha.png")));
		mGameWorld->mTextureManager->Add("wall", std::shared_ptr<Texture>(mRenderer->CreateTextureFromFile("Textures\\wall.png")));
		mGameWorld->mTextureManager->Add("wall-normal", std::shared_ptr<Texture>(mRenderer->CreateTextureFromFile("Textures\\wall-normal.png")));
		mGameWorld->mTextureManager->Add("cobblestones", std::shared_ptr<Texture>(mRenderer->CreateTextureFromFile("Textures\\cobblestones.bmp")));
		mGameWorld->mTextureManager->Add("cobblestones-normal", std::shared_ptr<Texture>(mRenderer->CreateTextureFromFile("Textures\\cobblestones-normal.bmp")));
		mGameWorld->mTextureManager->Add("001-normal", std::shared_ptr<Texture>(mRenderer->CreateTextureFromFile("Textures\\001-normal.png")));
		mGameWorld->mTextureManager->Add("test-normal", std::shared_ptr<Texture>(mRenderer->CreateTextureFromFile("Textures\\test-normal.jpg")));
		mGameWorld->mTextureManager->Add("stone", std::shared_ptr<Texture>(mRenderer->CreateTextureFromFile("Textures\\stone.jpg")));
		mGameWorld->mTextureManager->Add("stone-normal", std::shared_ptr<Texture>(mRenderer->CreateTextureFromFile("Textures\\stone-normal.jpg")));

		mGameWorld->mShaderManager->Add("Color_Lit", mEditorWorld->mShaderManager->Get("Color_Lit").lock());
		mGameWorld->mShaderManager->Add("Color_Lit_Shadow", mEditorWorld->mShaderManager->Get("Color_Lit_Shadow").lock());
		mGameWorld->mShaderManager->Add("Texture_Lit", mEditorWorld->mShaderManager->Get("Texture_Lit").lock());
		mGameWorld->mShaderManager->Add("Texture_Lit_Shadow", mEditorWorld->mShaderManager->Get("Texture_Lit_Shadow").lock());
		mGameWorld->mShaderManager->Add("Sprite", mEditorWorld->mShaderManager->Get("Sprite").lock());
		mGameWorld->mShaderManager->Add("RT_Cube_Depth", mEditorWorld->mShaderManager->Get("RT_Cube_Depth").lock());
		mGameWorld->mShaderManager->Add("Normal_Map_Shadow", mEditorWorld->mShaderManager->Get("Normal_Map_Shadow").lock());
		{
			auto shaderGUI = mEditorWorld->mShaderManager->Get("GUI").lock();
			mGameWorld->mShaderManager->Add("GUI", shaderGUI);
			mGameWorld->mSpriteSystem = std::unique_ptr<SpriteSystem>(new SpriteSystem(mRenderer, shaderGUI->Layout(), 256));
		}

		// Create Default Blend
		CreateDefaultBlend(mGameWorld);

		// Create Default Texture
		CreateDefaultTexture(mGameWorld);

		// Main Camera
		{
			auto obj = new GameObject("Main Camera");

			auto transform = new Transform();
			transform->mPosition = Vector3(0, 15, 40);
			transform->mOrientation = Quaternion(-15, 0, 0);
			obj->AddComponent(transform);

			obj->AddComponent(new PlayerController());

			auto physicsSystem = mGameWorld->mPhysicsSystem.get();
			auto body = physicsSystem->CreateRigidBody(
				physicsSystem->CreateSphere(1.0f), 
				transform->mPosition, transform->mOrientation, 1.0f);
			body->SetFlags(DISABLE_GRAVITY);
			body->SetRestitution(1.0f);
			body->SetDamping(0.9f, 0.9f);
			body->SetAngularFactor(Vector3(0, 0, 0));
			obj->AddComponent(new Physics(body));

			auto camera = new PerspectiveCamera(WindowInfo::BASE_WIDTH, WindowInfo::BASE_HEIGHT);
			camera->mViewportWidth = WindowInfo::BASE_WIDTH;
			camera->mViewportHeight = WindowInfo::BASE_HEIGHT;
			obj->AddComponent(camera);

			mGameWorld->AddGameObject(obj);
		}
		// Colored Box Prefab
		{
			auto shader = mGameWorld->mShaderManager->Get("Color_Lit_Shadow");

			MeshBuilder builder;
			builder.CreateCube(1.0f);
			builder.mLayout = shader.lock()->Layout();
			builder.mPrimitive = PrimitiveFormat::TRIANGLES;

			auto material = std::shared_ptr<Material>(new Material());
			material->mShader = shader;
			material->mBlendState = mEditorWorld->mBlendStateManager->Get("DEFAULT");
			material->mRenderQueue = (WORD)RenderQueue::GEOMETRY;
			mGameWorld->mMaterialManager->Add("Box", material);

			auto mesh = std::shared_ptr<Mesh>(builder.BuildMesh(mRenderer));
			mesh->mMaterial = material;
			mGameWorld->mMeshManager->Add("Box", mesh);
		}
		// Ground object - Bottom
		{
			auto obj = new GameObject("Ground_Bottom");

			auto transform = new Transform();
			transform->mPosition = Vector3(0, 0, 0);
			transform->mScale = Vector3(100, 1, 100);
			obj->AddComponent(transform);

			auto physicsSystem = mGameWorld->mPhysicsSystem.get();
			auto body = physicsSystem->CreateRigidBody(
				physicsSystem->CreateBox(transform->mScale), 
				transform->mPosition, transform->mOrientation, 0.0f);
			body->SetFlags(DISABLE_GRAVITY);
			obj->AddComponent(new Physics(body));

			auto meshComp = new MeshDrawable(mGameWorld->mMeshManager->Get("Box"));
			meshComp->mIsDynamic = true;
			obj->AddComponent(meshComp);

			mGameWorld->AddGameObject(obj);
		}
		// Ground object - Back
		{
			auto obj = new GameObject("Ground_Back");

			auto transform = new Transform();
			transform->mPosition = Vector3(0, 20, -20);
			transform->mScale = Vector3(20, 20, 1);
			obj->AddComponent(transform);

			auto physicsSystem = mGameWorld->mPhysicsSystem.get();
			auto body = physicsSystem->CreateRigidBody(
				physicsSystem->CreateBox(transform->mScale), 
				transform->mPosition, transform->mOrientation, 0.0f);
			body->SetFlags(DISABLE_GRAVITY);
			obj->AddComponent(new Physics(body));

			auto meshComp = new MeshDrawable(mGameWorld->mMeshManager->Get("Box"));
			meshComp->mIsDynamic = true;
			obj->AddComponent(meshComp);

			mGameWorld->AddGameObject(obj);
		}
		// Ground object - Right
		{
			auto obj = new GameObject("Ground_Right");

			auto transform = new Transform();
			transform->mPosition = Vector3(20, 20, 0);
			transform->mScale = Vector3(1, 20, 20);
			obj->AddComponent(transform);

			auto physicsSystem = mGameWorld->mPhysicsSystem.get();
			auto body = physicsSystem->CreateRigidBody(
				physicsSystem->CreateBox(transform->mScale), 
				transform->mPosition, transform->mOrientation, 0.0f);
			body->SetFlags(DISABLE_GRAVITY);
			obj->AddComponent(new Physics(body));

			auto meshComp = new MeshDrawable(mGameWorld->mMeshManager->Get("Box"));
			meshComp->mIsDynamic = true;
			obj->AddComponent(meshComp);

			mGameWorld->AddGameObject(obj);
		}
		// Ground object - Left
		{
			auto obj = new GameObject("Ground_Left");

			auto transform = new Transform();
			transform->mPosition = Vector3(-22, 10, 0);
			transform->mScale = Vector3(20, 1, 20);
			transform->mOrientation = Quaternion(0, 0, 0);
			obj->AddComponent(transform);

			auto physicsSystem = mGameWorld->mPhysicsSystem.get();
			auto body = physicsSystem->CreateRigidBody(
				physicsSystem->CreateBox(transform->mScale), 
				transform->mPosition, transform->mOrientation, 0.0f);
			body->SetFlags(DISABLE_GRAVITY);
			obj->AddComponent(new Physics(body));

			auto meshComp = new MeshDrawable(mGameWorld->mMeshManager->Get("Box"));
			meshComp->mIsDynamic = true;
			obj->AddComponent(meshComp);

			mGameWorld->AddGameObject(obj);
		}
		// Ground object - Top
		{
			auto obj = new GameObject("Ground_Top");

			auto transform = new Transform();
			transform->mPosition = Vector3(0, 20, 0);
			transform->mScale = Vector3(20, 1, 20);
			obj->AddComponent(transform);

			auto physicsSystem = mGameWorld->mPhysicsSystem.get();
			auto body = physicsSystem->CreateRigidBody(
				physicsSystem->CreateBox(transform->mScale), 
				transform->mPosition, transform->mOrientation, 0.0f);
			body->SetFlags(DISABLE_GRAVITY);
			obj->AddComponent(new Physics(body));

			auto meshComp = new MeshDrawable(mGameWorld->mMeshManager->Get("Box"));
			meshComp->mIsDynamic = true;
			obj->AddComponent(meshComp);

			mGameWorld->AddGameObject(obj);
		}
		// Box object - Right
		{
			auto obj = new GameObject("Box_Right");

			auto transform = new Transform();
			transform->mPosition = Vector3(10, 2, 5);
			transform->mScale = Vector3(1, 1, 1);
			obj->AddComponent(transform);

			auto physicsSystem = mGameWorld->mPhysicsSystem.get();
			auto body = physicsSystem->CreateRigidBody(
				physicsSystem->CreateBox(transform->mScale), 
				transform->mPosition, transform->mOrientation, 0.0f);
			obj->AddComponent(new Physics(body));

			auto meshComp = new MeshDrawable(mGameWorld->mMeshManager->Get("Box"));
			meshComp->mIsDynamic = true;
			obj->AddComponent(meshComp);

			mGameWorld->AddGameObject(obj);
		}
		// Box object - Left
		{
			auto obj = new GameObject("Box_Left");

			auto transform = new Transform();
			transform->mPosition = Vector3(10, 5, 5);
			transform->mScale = Vector3(1, 2, 1);
			obj->AddComponent(transform);

			auto physicsSystem = mGameWorld->mPhysicsSystem.get();
			auto body = physicsSystem->CreateRigidBody(
				physicsSystem->CreateBox(transform->mScale), 
				transform->mPosition, transform->mOrientation, 0.0f);
			obj->AddComponent(new Physics(body));

			auto meshComp = new MeshDrawable(mGameWorld->mMeshManager->Get("Box"));
			meshComp->mIsDynamic = true;
			obj->AddComponent(meshComp);

			mGameWorld->AddGameObject(obj);
		}
		// Box object - Top
		{
			auto obj = new GameObject("Box_Top");

			auto transform = new Transform();
			transform->mPosition = Vector3(-2, 7, 3);
			transform->mScale = Vector3(1, 1, 1);
			obj->AddComponent(transform);

			auto physicsSystem = mGameWorld->mPhysicsSystem.get();
			auto body = physicsSystem->CreateRigidBody(
				physicsSystem->CreateBox(transform->mScale), 
				transform->mPosition, transform->mOrientation, 0.0f);
			obj->AddComponent(new Physics(body));

			auto meshComp = new MeshDrawable(mGameWorld->mMeshManager->Get("Box"));
			meshComp->mIsDynamic = true;
			obj->AddComponent(meshComp);

			mGameWorld->AddGameObject(obj);
		}
		// Box object - Back
		{
			auto obj = new GameObject("Box_Back");

			auto transform = new Transform();
			transform->mPosition = Vector3(0, 3, -10);
			transform->mScale = Vector3(1, 1, 1);
			transform->mOrientation = Quaternion(45, 45, 45);
			obj->AddComponent(transform);

			auto physicsSystem = mGameWorld->mPhysicsSystem.get();
			auto body = physicsSystem->CreateRigidBody(
				physicsSystem->CreateBox(transform->mScale), 
				transform->mPosition, transform->mOrientation, 0.0f);
			obj->AddComponent(new Physics(body));

			auto meshComp = new MeshDrawable(mGameWorld->mMeshManager->Get("Box"));
			meshComp->mIsDynamic = true;
			obj->AddComponent(meshComp);

			mGameWorld->AddGameObject(obj);
		}
		// Dodecahedron
		{
			MeshBuilder builder;
			builder.CreateDodecahedron(1.0f);
			builder.mLayout = mGameWorld->mShaderManager->Get("Texture_Lit_Shadow").lock()->Layout();

			auto material = std::shared_ptr<Material>(new Material());
			material->mShader = mGameWorld->mShaderManager->Get("Color_Lit_Shadow"); // intentionally set wrong shader to demonstrate vertex layout compatibility
			material->mBlendState = mEditorWorld->mBlendStateManager->Get("ALPHA");
			material->mTexture[(BYTE)TextureIndex::DIFFUSE] = mGameWorld->mTextureManager->Get("default-alpha");
			material->mRenderQueue = (WORD)RenderQueue::TRANSPARENCY;
			mGameWorld->mMaterialManager->Add("Dodecahedron", material);

			auto mesh = std::shared_ptr<Mesh>(builder.BuildMesh(mRenderer));
			mesh->mMaterial = material;
			mGameWorld->mMeshManager->Add("Dodecahedron", mesh);


			auto obj = new GameObject("Dodecahedron");

			auto transform = new Transform();
			transform->mPosition = Vector3(10, 2, -10);
			transform->mScale = Vector3(1, 1, 1);
			transform->mOrientation = Quaternion(0, 0, 0);
			obj->AddComponent(transform);

			auto meshComp = new MeshDrawable(std::weak_ptr<Mesh>(mesh));
			meshComp->mIsDynamic = true;
			obj->AddComponent(meshComp);

			auto meshVBO = mesh->mVertexBuffer;

			auto physicsSystem = mGameWorld->mPhysicsSystem.get();
			auto body = physicsSystem->CreateRigidBody(
				physicsSystem->CreateMesh((Vector3*)meshVBO->Lock(), meshVBO->Count(), meshVBO->Stride(), transform->mScale),
				transform->mPosition, transform->mOrientation, 1.0f);
			meshVBO->Unlock();
			obj->AddComponent(new Physics(body));

			mGameWorld->AddGameObject(obj);
		}
		// Sphere
		{
			auto shader = mGameWorld->mShaderManager->Get("Texture_Lit_Shadow");

			MeshBuilder builder;
			builder.CreateSphere(1.0f, 50, 50);
			builder.mLayout = shader.lock()->Layout();

			auto material = std::shared_ptr<Material>(new Material());
			material->mShader = shader;
			material->mBlendState = mEditorWorld->mBlendStateManager->Get("ALPHA");
			material->mTexture[(BYTE)TextureIndex::DIFFUSE] = mGameWorld->mTextureManager->Get("default-alpha");
			material->mRenderQueue = (WORD)RenderQueue::TRANSPARENCY;
			mGameWorld->mMaterialManager->Add("Sphere", material);

			auto mesh = std::shared_ptr<Mesh>(builder.BuildMesh(mRenderer));
			mesh->mMaterial = material;
			mGameWorld->mMeshManager->Add("Sphere", mesh);


			auto obj = new GameObject();
			obj->mName = "Sphere";

			auto transform = new Transform();
			transform->mPosition = Vector3(15, 2, 10);
			transform->mScale = Vector3(1, 1, 1);
			obj->AddComponent(transform);

			auto meshComp = new MeshDrawable(std::weak_ptr<Mesh>(mesh));
			meshComp->mIsDynamic = true;
			obj->AddComponent(meshComp);

			auto physicsSystem = mGameWorld->mPhysicsSystem.get();
			auto body = physicsSystem->CreateRigidBody(
				physicsSystem->CreateSphere(transform->mScale.x), 
				transform->mPosition, transform->mOrientation, 1.0f);
			obj->AddComponent(new Physics(body));

			mGameWorld->AddGameObject(obj);
		}
		// Texture
		{
			auto shader = mGameWorld->mShaderManager->Get("Texture_Lit_Shadow");

			MeshBuilder builder;
			builder.CreateCube(1);
			builder.mLayout = shader.lock()->Layout();

			auto material = std::shared_ptr<Material>(new Material());
			material->mShader = shader;
			material->mBlendState = mEditorWorld->mBlendStateManager->Get("ALPHA");
			material->mTexture[(BYTE)TextureIndex::DIFFUSE] = mGameWorld->mTextureManager->Get("wall");
			material->mRenderQueue = (WORD)RenderQueue::GEOMETRY;

			mGameWorld->mMaterialManager->Add("Texture", material);

			auto mesh = std::shared_ptr<Mesh>(builder.BuildMesh(mRenderer));
			mesh->mMaterial = material;
			mGameWorld->mMeshManager->Add("Texture", mesh);


			auto obj = new GameObject("Texture");

			auto transform = new Transform();
			transform->mPosition = Vector3(-17, 5, 16);
			transform->mOrientation = Quaternion(Vector3(0, 45, 0));
			transform->mScale = Vector3(3, 3, 3);
			obj->AddComponent(transform);

			auto meshComp = new MeshDrawable(std::weak_ptr<Mesh>(mesh));
			meshComp->mIsDynamic = true;
			obj->AddComponent(meshComp);

			auto physicsSystem = mGameWorld->mPhysicsSystem.get();
			auto body = physicsSystem->CreateRigidBody(
				physicsSystem->CreateBox(transform->mScale),
				transform->mPosition, transform->mOrientation, 1.0f);
			obj->AddComponent(new Physics(body));

			mGameWorld->AddGameObject(obj);
		}
		// Normal Map
		{
			auto shader = mGameWorld->mShaderManager->Get("Normal_Map_Shadow");

			MeshBuilder builder;
			builder.CreateCube(1);
			builder.CalculateTangents(true);
			builder.mLayout = shader.lock()->Layout();

			auto material = std::shared_ptr<Material>(new Material());
			material->mShader = shader;
			material->mBlendState = mEditorWorld->mBlendStateManager->Get("ALPHA");
			//material->mTexture[TextureIndex::DIFFUSE] = mGameWorld->mTextureManager->Get("wall");
			//material->mTexture[TextureIndex::NORMAL] = mGameWorld->mTextureManager->Get("wall-normal");
			//material->mTexture[TextureIndex::DIFFUSE] = mGameWorld->mTextureManager->Get("cobblestones");
			//material->mTexture[TextureIndex::NORMAL] = mGameWorld->mTextureManager->Get("cobblestones-normal");
			//material->mTexture[TextureIndex::DIFFUSE] = mRenderer->BASE_TEXTURE;
			//material->mTexture[TextureIndex::NORMAL] = mGameWorld->mTextureManager->Get("test-normal");
			material->mTexture[(BYTE)TextureIndex::DIFFUSE] = mGameWorld->mTextureManager->Get("stone");
			material->mTexture[(BYTE)TextureIndex::NORMAL] = mGameWorld->mTextureManager->Get("stone-normal");
			material->mRenderQueue = (WORD)RenderQueue::GEOMETRY;
			//material->mSpecular = ColorRGBA(1, 1, 1, 1);
			//material->mSpecularComponent = 32;

			mGameWorld->mMaterialManager->Add("Normals", material);

			auto mesh = std::shared_ptr<Mesh>(builder.BuildMesh(mRenderer));
			mesh->mMaterial = material;
			mGameWorld->mMeshManager->Add("Normals", mesh);

			
			auto obj = new GameObject("Normals");

			auto transform = new Transform();
			transform->mPosition = Vector3(-15, 5, 5);
			transform->mOrientation = Quaternion(Vector3(90, 45, 0));
			transform->mScale = Vector3(3, 3, 3);
			obj->AddComponent(transform);

			normalMapTrans = transform;

			auto meshComp = new MeshDrawable(mesh);
			meshComp->mIsDynamic = true;
			obj->AddComponent(meshComp);

			auto physicsSystem = mGameWorld->mPhysicsSystem.get();
			auto body = physicsSystem->CreateRigidBody(
				physicsSystem->CreateBox(transform->mScale), 
				transform->mPosition, transform->mOrientation, 1.0f);
			obj->AddComponent(new Physics(body));

			mGameWorld->AddGameObject(obj);
		}
		// Particles
		{
			auto particleObject = new GameObject("Particle_Emitter");
			{
				auto texture = std::shared_ptr<Texture>(mRenderer->CreateTextureFromFile("Textures\\fire.png"));
				mGameWorld->mTextureManager->Add("fire", texture);

				auto state = std::shared_ptr<BlendState>(mRenderer->CreateBlendState(
					BlendFormat::SRC_ALPHA, BlendFormat::ONE, BlendFunction::ADD,
					BlendFormat::SRC_ALPHA, BlendFormat::ONE, BlendFunction::ADD));
				mEditorWorld->mBlendStateManager->Add("PARTICLE", state);

				auto material = std::shared_ptr<Material>(new Material());
				material->mShader = mGameWorld->mShaderManager->Get("Sprite");
				material->mBlendState = mEditorWorld->mBlendStateManager->Get("PARTICLE");
				material->mTexture[(BYTE)TextureIndex::DIFFUSE] = texture;
				material->mCullMode = CullFormat::NONE;
				material->mDepthMode = DepthFormat::OFF;
				material->mRenderQueue = (WORD)RenderQueue::TRANSPARENCY;
				mGameWorld->mMaterialManager->Add("Fire", material);

				auto sprite = std::shared_ptr<Sprite>(new Sprite());
				sprite->AddFrame(Sprite::QUADtoTEXCOORD(Quad(0, 0, 64, 64), texture->Width(), texture->Height()));
				sprite->AddFrame(Sprite::QUADtoTEXCOORD(Quad(64, 0, 128, 64), texture->Width(), texture->Height()));
				mGameWorld->mSpriteManager->Add("Fire", sprite);


				auto transform = new Transform();
				transform->mPosition = Vector3(8, 1, 0);
				transform->mScale = Vector3(1, 1, 1);
				particleObject->AddComponent(transform);

				auto particles = new SpriteParticleEmitter(300);
				particles->mMaterial = material;
				particles->mSprite = sprite;
				particles->mSpawnRate = 0.025f;
				particles->mMinLifetime = 3.0f;
				particles->mMaxLifetime = 5.0f;
				particles->mEffects.push_back(new TextureEffect(0, 0));
				particles->mEffects.push_back(new AreaPositionEffect(0, Vector3(-0.125f, 1, 0), Vector3(0.125f, 1, 0)));
				particles->mEffects.push_back(new RandomRotationEffect(0, Vector3(0, 0, -10), Vector3(0, 0, 10)));
				particles->mEffects.push_back(new ScaleEffect(0, Vector3(1, 1, 1)));
				particles->mEffects.push_back(new RandomVelocityEffect(0, Vector3(0, 2.0f, 0), Vector3(0, 2.8f, 0)));
				particles->mEffects.push_back(new RandomColorEffect(0, ColorRGBA(0.75f, 0.25f, 0, 0.125f), ColorRGBA(0.75f, 0.75f, 0, 0.125f)));
				particles->mEffects.push_back(new FadeToScaleEffect(0, 0.25f, 0.666f));
				particles->mEffects.push_back(new FadeToScaleEffect(0.5f, 1.0f, 0.1f));
				particles->mEffects.push_back(new TextureEffect(1.0f, 1));
				particles->mEffects.push_back(new FadeToScaleEffect(1.0f, 2.0f, 1.0f));
				particles->mEffects.push_back(new RandomColorEffect(1.0f, ColorRGBA(1, 1, 1, 0.125f), ColorRGBA(0.9f, 0.9f, 0.9f, 0.125f)));
				particles->mEffects.push_back(new FadeToColorEffect(1.0f, 5.0f, ColorRGBA(0.9f, 0.9f, 0.9f, 0.025f)));
				particles->mEffects.push_back(new RandomVelocityEffect(1.0f, Vector3(-0.5f, 1.4f, -0.5f), Vector3(0.5f, 3.0f, 0.5f)));
				particleObject->AddComponent(particles);
			}
			// Point Light
			{
				auto state = std::shared_ptr<BlendState>(mRenderer->CreateBlendState(
					BlendFormat::DST_COLOR, BlendFormat::SRC_COLOR, BlendFunction::MIN,
					BlendFormat::ZERO, BlendFormat::ZERO, BlendFunction::ADD));
				mEditorWorld->mBlendStateManager->Add("SHADOW", state);

				auto material = std::shared_ptr<Material>(new Material());
				material->mShader = mGameWorld->mShaderManager->Get("RT_Cube_Depth");
				material->mBlendState = mEditorWorld->mBlendStateManager->Get("SHADOW");
				material->mCullMode = CullFormat::CCW;
				material->mDepthMode = DepthFormat::ALWAYS;
				material->mRenderQueue = (WORD)RenderQueue::BACKGROUND;
				mGameWorld->mMaterialManager->Add("Point Light", material);


				auto obj = new GameObject("Point Light");

				auto transform = new Transform();
				transform->mPosition = Vector3(0, 4, 0);
				obj->AddComponent(transform);

				auto light = new PointLight(512);
				light->mColor = ColorRGBA(0.8f, 0.6f, 0.0f, 1.0f);
				light->mAttenuation = Vector4(1.0f, 0.0f, 0.0f, 40.0f);
				light->mMaterial = material;
				obj->AddComponent(light);

				particleObject->AddChild(obj);
			}
			mGameWorld->AddGameObject(particleObject);
		}
		///////////////////////////////

		mGameWorld->Startup();

		///////////////////////////////
		// CREATE EDITOR OBJECTS
		//
		CreateTranslateObject();
		CreateRotateObject();
		CreateScaleObject();
		CreateBoundsObject();
	}

	void PrepareDebug()
	{
		auto material = std::shared_ptr<Material>(new Material());
		material->mShader = mEditorWorld->mShaderManager->Get("Color_Only").lock();
		material->mBlendState = mEditorWorld->mBlendStateManager->Get("DEFAULT");
		material->mDepthMode = DepthFormat::LEQUAL;
		material->mCullMode = CullFormat::NONE;
		material->mRenderQueue = (WORD)RenderQueue::FOREGROUND;

		mEditorWorld->mMaterialManager->Add("Debug", material);
	}

	void CreateTranslateObject()
	{
		const float tileSize = 5;

		auto model = std::shared_ptr<Model>(LoadModelM3DFromFile("Editor\\Translate.M3D",
			mRenderer,
			mGameWorld->mShaderManager.get(),
			mGameWorld->mTextureManager.get(),
			mGameWorld->mMaterialManager.get(),
			mGameWorld->mBlendStateManager.get()));
		mEditorWorld->mModelManager->Add("Translate", model);

		std::vector<Mesh*> meshList;
		model->GetMeshList(&meshList);

		auto state = std::shared_ptr<BlendState>(mRenderer->CreateBlendState(
			BlendFormat::SRC_ALPHA, BlendFormat::ONE_MINUS_SRC_ALPHA, BlendFunction::ADD,
			BlendFormat::SRC_ALPHA, BlendFormat::ONE, BlendFunction::ADD));
		mEditorWorld->mBlendStateManager->Add("TRANSLATE", state);

		for (auto mesh : meshList)
		{
			auto material = mesh->mMaterial.lock();
			material->mShader = mEditorWorld->mShaderManager->Get("Color_Only");
			material->mCullMode = CullFormat::NONE;
			material->mBlendState = mEditorWorld->mBlendStateManager->Get("TRANSLATE");
			material->mDepthMode = DepthFormat::ALWAYS;
			material->mRenderQueue = (WORD)RenderQueue::FOREGROUND;
			material->mAmbient.a = 0.5f;
		}

		
		// Root Translate Object
		{
			mTranslateObject = new GameObject("Translate Object");
			mTranslateObject->AddComponent(new Transform());
			{
				auto obj = new GameObject();
				obj->mName = "X-Axis";

				Transform* transform = new Transform();
				transform->mPosition = Vector3(tileSize, 0, 0);
				transform->mOrientation = Quaternion(Vector3(0, 0, -90));
				obj->AddComponent(transform);

				auto modelComp = new ModelDrawable(model);
				//modelComp->SetMaterial(mSelectedMaterialX);
				obj->AddComponent(modelComp);

				mTranslateObject->AddChild(obj);
			}
			{
				auto obj = new GameObject("Y-Axis");

				auto transform = new Transform();
				transform->mPosition = Vector3(0, tileSize, 0);
				transform->mOrientation = Quaternion(Vector3(0, 0, 0));
				obj->AddComponent(transform);

				auto modelComp = new ModelDrawable(model);
				//modelComp->SetMaterial(mSelectedMaterialY);
				obj->AddComponent(modelComp);

				mTranslateObject->AddChild(obj);
			}
			{
				auto obj = new GameObject("Z-Axis");

				auto transform = new Transform();
				transform->mPosition = Vector3(0, 0, tileSize);
				transform->mOrientation = Quaternion(Vector3(90, 0, 0));
				obj->AddComponent(transform);

				auto modelComp = new ModelDrawable(model);
				//modelComp->SetMaterial(mSelectedMaterialZ);
				obj->AddComponent(modelComp);

				mTranslateObject->AddChild(obj);
			}
			mTranslateObject->SetWorld(mGameWorld);
			mTranslateObject->Startup();
			mTranslateObject->SetActive(false);
		}
	}

	void DrawTranslateObject()
	{
		if (mSelectedObject)
		{
			Transform* transformObject = mTranslateObject->GetComponent<Transform>();
			Transform* transformSelect = mSelectedObject->GetComponent<Transform>();

			transformObject->mPosition = transformSelect->mPosition;
			transformObject->mScale = Vector3::ONE * (mWorldCamera->GetTransform()->mPosition - transformSelect->mPosition).Length() * 0.005f;

			mTranslateObject->UpdateTransform();
		}
	}

	void CreateRotateObject()
	{ }

	void CreateScaleObject()
	{ }

	void CreateBoundsObject()
	{
		auto shaderLine = mEditorWorld->mShaderManager->Get("Color_Only").lock();

		MeshBuilder builder;
		builder.mLayout = shaderLine->Layout();
		builder.CreateWireCube(1);

		auto material = std::shared_ptr<Material>(new Material());
		material->mShader = shaderLine;
		material->mBlendState = mEditorWorld->mBlendStateManager->Get("DEFAULT");
		material->mDepthMode = DepthFormat::LEQUAL;
		material->mCullMode = CullFormat::NONE;
		material->mRenderQueue = (WORD)RenderQueue::FOREGROUND;
		mEditorWorld->mMaterialManager->Add("Bounds", material);

		auto mesh = std::shared_ptr<Mesh>(builder.BuildMesh(mRenderer));
		mesh->mMaterial = material;
		mEditorWorld->mMeshManager->Add("Bounds", mesh);

		mBoundsObject = new GameObject("Bounds");
		mBoundsObject->AddComponent(new Transform());
		mBoundsObject->AddComponent(new MeshDrawable(mesh));
		
		mBoundsObject->SetWorld(mGameWorld);
		mBoundsObject->Startup();
	}

	void CheckBounds(GameObject* obj)
	{
		Drawable* drawable = obj->GetComponent<Drawable>();

		if (drawable)
		{
			const BoundingBox& box = drawable->mBounds;

			Vector3 center = (box.GetMaxBounds() + box.GetMinBounds()) * 0.5f;
			Vector3 scale = (box.GetMaxBounds() - center);
			Vector3 intersection;

			if (box.Intersects(mMouseToWorldRay, &intersection))
			{
				float dist = (mMouseToWorldRay.mPosition - intersection).LengthSquared();

				if (mHoverDist > dist)
				{
					mHoverObject = obj;
					mHoverDist = dist;
				}
			}
		}

		UINT count = obj->NumChildren();

		for (UINT x = 0; x < count; ++x)
		{
			CheckBounds(obj->GetChild(x));
		}
	}

	void DrawBounds(GameObject* obj)
	{
		Drawable* drawable = obj->GetComponent<Drawable>();

		if (drawable)
		{
			const BoundingBox& box = drawable->mBounds;

			Vector3 center = (box.GetMaxBounds() + box.GetMinBounds()) * 0.5f;
			Vector3 scale = (box.GetMaxBounds() - center);

			MeshDrawable* drawable = mBoundsObject->GetComponent<MeshDrawable>();

			auto mesh = drawable->mMesh.lock();
			auto material = mesh->mMaterial.lock();

			if (obj != mHoverObject)
			{
				material->mAmbient = ColorRGBA::WHITE * 0.5f;

				if (obj == mSelectedObject)
					material->mAmbient = ColorRGBA::RED;
			}
			else material->mAmbient = (obj == mSelectedObject) ? ColorRGBA::YELLOW : ColorRGBA::GREEN;

			Transform* transform = mBoundsObject->GetComponent<Transform>();
			transform->mPosition = center;
			transform->mOrientation = Quaternion::IDENTITY;
			transform->mScale = scale;

			mesh->mBounds = BoundingBox();

			mBoundsObject->UpdateTransform();

			if (drawable->CheckVisible(mWorldCamera))
				drawable->Draw(mWorldCamera);
		}

		UINT count = obj->NumChildren();

		for (UINT x = 0; x < count; ++x)
		{
			DrawBounds(obj->GetChild(x));
		}
	}

	void DrawBounds()
	{
		UINT count = mGameWorld->NumGameObjects();

		for (UINT x = 0; x < count; ++x)
		{
			DrawBounds(mGameWorld->GetGameObject(x));
		}

		mHoverObject = nullptr;
		mHoverDist = FLT_MAX;
	}

	void SelectGameObject()
	{
		POINT mousePos = Mouse::Get().GetPosition((HWND)mGameWindow->GetInfo()->Handle());

		Transform* transform = mGameWorldWidget->GetComponent<Transform>();
		const Vector3& worldPos = transform->mPosition;
		const Vector3& worldScale = transform->mScale;

		mMouseToWorldRay = mWorldCamera->ScreenPointToRay(mousePos.x - (UINT)worldPos.x, mousePos.y - (UINT)worldPos.y, (UINT)worldScale.x, (UINT)worldScale.y);

		UINT count = mGameWorld->NumGameObjects();

		for (UINT x = 0; x < count; ++x)
		{
			CheckBounds(mGameWorld->GetGameObject(x));
		}

		if (Mouse::Get().DidGoDown(BUTTON_LEFT))
		{
			mSelectedObject = mHoverObject;

			mTranslateObject->SetActive((mSelectedObject != nullptr) ? true : false);
		}
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////

int APIENTRY _tWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	//_CrtSetBreakAlloc(4679);

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
