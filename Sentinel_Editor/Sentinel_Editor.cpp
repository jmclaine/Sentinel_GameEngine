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
#include "ParticleSystem.h"
#include "SpriteSystem.h"
#include "Network/Socket.h"
#include "AudioSystem.h"
#include "FontSystem.h"

#include "GameWindow.h"

#include "Archive.h"

#include "TextureManager.h"
#include "ShaderManager.h"
#include "MaterialManager.h"
#include "SpriteManager.h"
#include "MeshManager.h"
#include "ModelManager.h"
#include "SoundManager.h"
#include "RenderManager.h"

#include "Input.h"
#include "Timing.h"
#include "Util.h"
#include "FileUtil.h"
#include "Profile.h"

#include "MeshBuilder.h"
#include "Model.h"

#include "GameWorld.h"
#include "GameObject.h"

#include "Component/Transform.h"
#include "Component/Physics.h"
//#include "DirectionalLightComponent.h"
#include "Component/PointLight.h"
#include "Component/PerspectiveCamera.h"
#include "Component/OrthographicCamera.h"
#include "Component/PlayerController.h"
#include "Component/ParticleEmitter.h"
#include "Component/SpriteDrawable.h"
#include "Component/MeshDrawable.h"
#include "Component/ModelDrawable.h"

#include "EditorControllerComponent.h"

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

// Main Application.
//
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
		mGameWindow(NULL),
		mRenderer(NULL),
		mGameWorld(NULL),
		mEditorWorld(NULL),
		mRTMain(NULL),
		mDSMain(NULL),
		mRTGameWorld(NULL),
		mDSGameWorld(NULL),
		mRTBackbufferMesh(NULL),
		mSelectedObject(NULL),
		mHoverObject(NULL),
		mHoverDist(0),
		mTranslateObject(NULL),
		mBoundsObject(NULL),
		mSelectedColorX(1, 0, 0, 0.5f),
		mSelectedColorY(0, 1, 0, 0.5f),
		mSelectedColorZ(0, 0, 1, 0.5f),
		mSelectedColorAxis(1, 1, 0, 0.5f),
		mDebug(NULL),
		mMouseToWorldRay(Vector3(), Vector3())
	{
		srand((UINT)time((time_t*)0));
	}

	~MainApp()
	{}

	void Startup(HINSTANCE hInstance, int nCmdShow)
	{
		mGameWindow = new GameWindow(IDI_SENTINEL_EDITOR, IDI_SMALL, 0);

		mAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SENTINEL_EDITOR));

		// Load config file to setup windows for the renderer.
		//
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

		// Prepare main window.
		//
		mGameWindow->Startup(mRenderer, hInstance, nCmdShow, "Sentinel Editor", "SentinelClass", info);

		mRTMain = mRenderer->CreateBackbuffer();
		mDSMain = mRenderer->CreateDepthStencil(Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE);

		////////////////////////////////////
		// Prepare Editor and World View.
		//
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
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
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

				timing = mEditorWorld->mTiming;

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
				mGameWorld->UpdateDrawable();
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
				mEditorWorld->UpdateDrawable();
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

		mGameWorld->mTextureManager->Save(archive, mRenderer);
		mGameWorld->mShaderManager->Save(archive);
		mGameWorld->mMaterialManager->Save(archive, mRenderer, mGameWorld->mShaderManager, mGameWorld->mTextureManager);
		mGameWorld->mSpriteManager->Save(archive);
		mGameWorld->mMeshManager->Save(archive, mRenderer, mGameWorld->mShaderManager, mGameWorld->mTextureManager, mGameWorld->mMaterialManager);
		mGameWorld->mModelManager->Save(archive, mRenderer, mGameWorld->mShaderManager, mGameWorld->mTextureManager, mGameWorld->mMaterialManager);
		mGameWorld->mSoundManager->Save(archive);

		mGameWorld->Save(archive);

		archive.Close();

		system("copy Default.MAP ..\\Sentinel_Game");
	}

	void PrepareEditorWorld()
	{
		MeshBuilder meshBuilder;
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;
		std::shared_ptr<Texture> texture;
		std::shared_ptr<Shader> shader;
		std::shared_ptr<Sprite> sprite;

		GameObject* obj;
		Transform* transform;
		Physics* physics;
		RigidBody* body;

		/////////////////////////////////////////////////////////////////////////

		mEditorWorld = new GameWorld();
		mEditorWorld->mRenderer = mRenderer;
		mEditorWorld->mTiming = new Timing();
		mEditorWorld->mPhysicsSystem = BuildPhysicsSystemBT();
		mEditorWorld->mAudioSystem = BuildAudioSystemAL();
		mEditorWorld->mFontSystem = BuildFontSystemFT();

		mEditorWorld->mPhysicsSystem->Startup();

		mEditorWorld->mTextureManager = new TextureManager(mRenderer);
		mEditorWorld->mShaderManager = new ShaderManager();
		mEditorWorld->mMaterialManager = new MaterialManager();
		mEditorWorld->mSpriteManager = new SpriteManager();
		mEditorWorld->mMeshManager = new MeshManager();
		mEditorWorld->mModelManager = new ModelManager();
		mEditorWorld->mSoundManager = new SoundManager();
		mEditorWorld->mRenderManager = new RenderManager();

		SetDirectory("Shaders");

		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir(GetDirectory().c_str())) != NULL)
		{
			while ((ent = readdir(dir)) != NULL)
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

					mEditorWorld->mShaderManager->Add(name.substr(0, len - 4), SHARED(shader));
				}
			}
			closedir(dir);
		}
		else
		{
			throw AppException("Failed to open 'Assets\\Shaders' directory.");
		}

		/*
		std::vector<std::string> names;
		std::vector<std::shared_ptr<Shader>> shaders;

		mShaderManager->GetAll(names, shaders);

		for (int x = 0; x < (int)names.size(); ++x)
		{
		mAssetShader->addChild(new AssetTreeItem<Shader>(names[x].c_str(), shaders[x]));
		}
		*/
		SetDirectory("..");

		shader = mEditorWorld->mShaderManager->Get("GUI");

		FontSystem* fontSystem = mEditorWorld->mFontSystem;
		fontSystem->mSpriteSystem = SHARED(new SpriteSystem(mRenderer, shader->Layout(), 256));
		fontSystem->Load("Font\\courbd.ttf");
		fontSystem->mFont = fontSystem->Build(12, 12);

		material = fontSystem->mFont->mMaterial;
		material->mShader = shader;
		material->mBlendState = mRenderer->BLEND_ALPHA;
		material->mCullMode = CullFormat::NONE;
		material->mDepthMode = DepthFormat::OFF;

		fontSystem->mSpriteSystem->mMaterial = material;


		mEditorWorld->mSpriteSystem = new SpriteSystem(mRenderer, shader->Layout(), 256);

		texture = mEditorWorld->mTextureManager->Add("GUI.png", SHARED(mRenderer->CreateTextureFromFile("Textures\\GUI.png")));

		material = mEditorWorld->mMaterialManager->Add("GUI", SHARED(new Material()));
		material->mShader = mEditorWorld->mShaderManager->Get("GUI");
		material->mBlendState = mRenderer->BLEND_ALPHA;
		material->mTexture[TextureIndex::DIFFUSE] = texture;
		material->mRenderQueue = RenderQueue::FOREGROUND;
		material->mCullMode = CullFormat::NONE;
		material->mDepthMode = DepthFormat::OFF;

		mEditorWorld->mSpriteSystem->mMaterial = material;

		mEditorWorld->mShaderManager->Get("GUI")->SetSampler(0, SamplerMode::WRAP, SamplerMode::WRAP, SamplerFilter::POINT, SamplerFilter::POINT);
		mEditorWorld->mShaderManager->Get("GUI_Mesh")->SetSampler(0, SamplerMode::WRAP, SamplerMode::WRAP, SamplerFilter::POINT, SamplerFilter::POINT);
		mEditorWorld->mShaderManager->Get("RT_Color")->SetSampler(0, SamplerMode::WRAP, SamplerMode::WRAP, SamplerFilter::POINT, SamplerFilter::POINT);
		mEditorWorld->mShaderManager->Get("Normal_Map")->SetSampler(0, SamplerMode::WRAP, SamplerMode::WRAP, SamplerFilter::POINT, SamplerFilter::POINT, SamplerFilter::LINEAR);

		//
		// Create World View Backbuffer
		//
		mRTGameWorld = mRenderer->CreateRenderTexture(mEditorWorld->mTextureManager->Add("Backbuffer", SHARED(mRenderer->CreateTexture(NULL, Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE, ImageFormat::RGB))).get());
		mDSGameWorld = mRenderer->CreateDepthStencil(Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE);

		material = mEditorWorld->mMaterialManager->Add("Backbuffer", std::shared_ptr< Material >(new Material()));
		material->mShader = mEditorWorld->mShaderManager->Get("RT_Color");
		material->mBlendState = mRenderer->BLEND_OFF;
		material->mTexture[TextureIndex::DIFFUSE] = mEditorWorld->mTextureManager->Get("Backbuffer");
		material->mRenderQueue = RenderQueue::FOREGROUND;

		mRTBackbufferMesh = MeshBuilder::BuildRenderTextureMesh(mRenderer, material->mShader->Layout());
		mRTBackbufferMesh->mMaterial = material;
		
		//
		// GUI Camera
		//
		GameObject* cameraGUI = mEditorWorld->AddGameObject(new GameObject());
		cameraGUI->mName = "GUI Camera";

		transform = (Transform*)cameraGUI->Attach(new Transform());

		mEditorCamera = (OrthographicCamera*)cameraGUI->Attach(new OrthographicCamera(Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE));
		mEditorCamera->mDepthStencil = mDSMain;
		mEditorCamera->mRenderTexture = mRTMain;
		mEditorCamera->mClearColor = ColorRGBA(0.2f, 0.2f, 0.2f, 1.0f);
		
		//
		// GUI Widgets
		//
		sprite = mEditorWorld->mSpriteManager->Add("GUI", SHARED(new Sprite()));
		sprite->AddFrame(Sprite::QUADtoTEXCOORD(Quad(0, 0, 1, 1), texture->Width(), texture->Height()));
		sprite->AddFrame(Sprite::QUADtoTEXCOORD(Quad(1, 0, 2, 1), texture->Width(), texture->Height()));
		sprite->AddFrame(Sprite::QUADtoTEXCOORD(Quad(2, 0, 3, 1), texture->Width(), texture->Height()));

		GameObject* widget;
		GUI::Label* label;
		GUI::SpriteController* controller;
		//GUI::SpriteButton* button;
		SpriteDrawable* view;

		//
		// Menu Bar
		//
		widget = mEditorWorld->AddGameObject(new GameObject());
		widget->mName = "Menu Bar";

		transform = (Transform*)widget->Attach(new Transform());
		transform->mPosition = Vector3(0, 0, 1);
		transform->mScale = Vector3((float)Renderer::WINDOW_WIDTH_BASE, 0, 1);

		view = (SpriteDrawable*)widget->Attach(new SpriteDrawable(sprite));
		view->mFrame = 0;
		view->mColor = ColorRGBA(0.7f, 0.7f, 0.7f, 1);

		controller = (GUI::SpriteController*)widget->Attach(new GUI::SpriteController());
		controller->mMargin.bottom = 30;
		controller->mScaleToWindowX = true;

		//
		// Menu Transform
		//
		GameObject* menu = mEditorWorld->AddGameObject(new GameObject());
		menu->mName = "Menu Transform";

		transform = (Transform*)menu->Attach(new Transform());
		transform->mPosition = Vector3(0, 0, 0.9f);
		transform->mScale = Vector3(1, 1, 1);

		//
		// Menu Labels
		//
		widget = mEditorWorld->AddGameObject(new GameObject());
		widget->mName = "Menu Labels";

		transform = (Transform*)widget->Attach(new Transform());
		transform->mPosition = Vector3(10, 20, 0);
		transform->mScale = Vector3(12, 12, 1);

		//label = (GUI::Label*)widget->Attach( new GUI::Label( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ{}[]!@#$%^&*()1234567890" ));
		label = (GUI::Label*)widget->Attach(new GUI::Label("File"));
		label->mColor = ColorRGBA(0, 0, 0, 1);

		controller = (GUI::SpriteController*)widget->Attach(new GUI::SpriteController());

		menu->AddChild(widget);

		//////////////////////////////////////

		widget = mEditorWorld->AddGameObject(new GameObject());
		widget->mName = "Edit";

		transform = (Transform*)widget->Attach(new Transform());
		transform->mPosition = Vector3(110, 20, 0);
		transform->mScale = Vector3(12, 12, 1);

		label = (GUI::Label*)widget->Attach(new GUI::Label("Edit"));
		label->mColor = ColorRGBA(0, 0, 0, 1);

		controller = (GUI::SpriteController*)widget->Attach(new GUI::SpriteController());

		menu->AddChild(widget);

		//////////////////////////////////////

		widget = mEditorWorld->AddGameObject(new GameObject());
		widget->mName = "Help";

		transform = (Transform*)widget->Attach(new Transform());
		transform->mPosition = Vector3(210, 20, 0);
		transform->mScale = Vector3(12, 12, 1);

		label = (GUI::Label*)widget->Attach(new GUI::Label("Help"));
		label->mColor = ColorRGBA(0, 0, 0, 1);

		controller = (GUI::SpriteController*)widget->Attach(new GUI::SpriteController());

		menu->AddChild(widget);

		//////////////////////////////////////

		widget = mEditorWorld->AddGameObject(new GameObject());
		widget->mName = "Debug";

		transform = (Transform*)widget->Attach(new Transform());
		transform->mPosition = Vector3(0, 40, 0.9f);
		transform->mScale = Vector3(10, 10, 1);

		mDebugText = new GUI::Label();
		mDebugText->mColor = ColorRGBA(0, 0, 0, 1);
		widget->Attach(mDebugText);

		controller = (GUI::SpriteController*)widget->Attach(new GUI::SpriteController());
		
		//
		// Toolbar
		//
		widget = mEditorWorld->AddGameObject(new GameObject());
		widget->mName = "Toolbar";

		transform = (Transform*)widget->Attach(new Transform());
		transform->mPosition = Vector3(0, 0, 1);
		transform->mScale = Vector3((float)Renderer::WINDOW_WIDTH_BASE, 0, 1);

		view = (SpriteDrawable*)widget->Attach(new SpriteDrawable(sprite));
		view->mFrame = 0;
		view->mColor = ColorRGBA(0.9f, 0.9f, 0.9f, 1);

		controller = (GUI::SpriteController*)widget->Attach(new GUI::SpriteController());
		controller->mMargin = Quad(0, 30, 0, 30);
		controller->mScaleToWindowX = true;

		//
		// Status Bar
		//
		widget = mEditorWorld->AddGameObject(new GameObject());
		widget->mName = "Status Bar";

		transform = (Transform*)widget->Attach(new Transform());
		transform->mPosition = Vector3(0, (float)Renderer::WINDOW_HEIGHT_BASE, 0.1f);
		transform->mScale = Vector3((float)Renderer::WINDOW_WIDTH_BASE, 0, 1);

		view = (SpriteDrawable*)widget->Attach(new SpriteDrawable(sprite));
		view->mFrame = 0;
		view->mColor = ColorRGBA(0.7f, 0.7f, 0.7f, 1);

		controller = (GUI::SpriteController*)widget->Attach(new GUI::SpriteController());
		controller->mMargin = Quad(0, -30, 0, 30);
		controller->mScaleToWindowX = true;
		controller->mPositionToWindowY = true;

		//
		// Label Button
		//
		/*GameObject* testButton = mEditorWorld->AddGameObject(new GameObject());
		testButton->mName = "Test Button";

		transform = (Transform*)testButton->Attach(new Transform());
		transform->mPosition = Vector3(200, 30, 0.7f);
		transform->mOrientation = Quaternion(0, 0, 45);
		transform->mScale = Vector3(1, 1, 1);

		//////////////////////////////////

		widget = testButton->AddChild(new GameObject());
		widget->mName = "Button";

		transform = (Transform*)widget->Attach(new Transform());
		transform->mPosition = Vector3(0, 0, 0);
		transform->mOrientation = Quaternion(0, 0, 0);
		transform->mScale = Vector3(128, 64, 1);

		view = (SpriteDrawable*)widget->Attach(new SpriteDrawable(sprite));

		button = (GUI::SpriteButton*)widget->Attach(new GUI::SpriteButton());
		button->mPositionToWindowX = true;
		button->mPositionToWindowY = true;
		button->mScaleToWindowX = true;
		button->mScaleToWindowY = true;

		//////////////////////////////////

		widget = testButton->AddChild(new GameObject());
		widget->mName = "Label";

		transform = (Transform*)widget->Attach(new Transform());
		transform->mPosition = Vector3(48, 32, 0);
		transform->mOrientation = Quaternion(0, 0, 0);
		transform->mScale = Vector3(16, 16, 1);

		label = (GUI::Label*)widget->Attach(new GUI::Label("Ok"));
		label->mColor = ColorRGBA(0, 0, 0, 1);

		controller = (GUI::SpriteController*)widget->Attach(new GUI::SpriteController());
		//controller->mPositionToWindowX	= true;
		//controller->mPositionToWindowY	= true;
		//controller->mScaleToWindowX		= true;
		//controller->mScaleToWindowY		= true;*/

		//
		// World View Camera
		//
		obj = mEditorWorld->AddGameObject(new GameObject());
		obj->mName = "Main Camera";

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(0, 15, 40);
		transform->mOrientation = Quaternion(-15, 0, 0);

		physics = (Physics*)obj->Attach(new Physics());
		physics->SetRigidBody(mEditorWorld->mPhysicsSystem->CreateRigidBody(mEditorWorld->mPhysicsSystem->CreateSphere(1.0f), transform->mPosition, transform->mOrientation, 1.0f));
		body = physics->GetRigidBody();
		body->SetFlags(DISABLE_GRAVITY);
		body->SetRestitution(1.0f);
		body->SetDamping(0.999f, 0.999f);

		mWorldCamera = (PerspectiveCamera*)obj->Attach(new PerspectiveCamera(Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE));
		mWorldCamera->mClearColor = ColorRGBA(0.0f, 0.2f, 0.8f, 1.0f);
		mWorldCamera->mViewportWidth = Renderer::WINDOW_WIDTH_BASE;
		mWorldCamera->mViewportHeight = Renderer::WINDOW_HEIGHT_BASE;
		mWorldCamera->mDepthStencil = mDSGameWorld;
		mWorldCamera->mRenderTexture = mRTGameWorld;

		EditorControllerComponent* editorController = (EditorControllerComponent*)obj->Attach(new EditorControllerComponent());
		editorController->mEditorCamera = mEditorCamera;
		editorController->mWorldCamera = mWorldCamera;

		//
		// World Drawable
		//
		mGameWorldWidget = mEditorWorld->AddGameObject(new GameObject());
		mGameWorldWidget->mName = "World Drawable";

		transform = (Transform*)mGameWorldWidget->Attach(new Transform());
		transform->mPosition = Vector3(0.0f, 0.0f, 1.0f);
		transform->mScale = Vector3((float)(Renderer::WINDOW_WIDTH_BASE - 500), (float)Renderer::WINDOW_HEIGHT_BASE, 1.0f);

		material = mEditorWorld->mMaterialManager->Add("World", SHARED(new Material()));
		material->mShader = mEditorWorld->mShaderManager->Get("GUI_Mesh");
		material->mBlendState = mRenderer->BLEND_OFF;
		material->mCullMode = CullFormat::NONE;
		material->mTexture[TextureIndex::DIFFUSE] = mEditorWorld->mTextureManager->Get("Backbuffer");
		material->mRenderQueue = RenderQueue::GEOMETRY;

		mesh = mEditorWorld->mMeshManager->Add("World", SHARED(MeshBuilder::BuildGUIMesh(mRenderer, material->mShader->Layout())));
		mesh->mMaterial = material;
		mGameWorldWidget->Attach(new MeshDrawable(mesh));

		controller = (GUI::SpriteController*)mGameWorldWidget->Attach(new GUI::SpriteController());
		controller->mScaleToWindowX = true;
		controller->mScaleToWindowY = true;
		controller->mMargin = Quad(0, 60, 0, -90);

		///////////////////////////////

		mEditorWorld->Startup();
	}

	void PrepareGameWorld()
	{
		MeshBuilder meshBuilder;
		std::shared_ptr<Mesh> mesh;
		std::shared_ptr<Material> material;

		GameObject* obj;
		Transform* transform;
		Physics* physics;
		RigidBody* body;
		MeshDrawable* meshComp;
		Camera* camera;

		/////////////////////////////////////////////////////////////////////////

		mGameWorld = new GameWorld();
		PhysicsSystem* physicsSystem = BuildPhysicsSystemSE();

		mGameWorld->mRenderer = mRenderer;

		mGameWorld->mTiming = new Timing();
		mGameWorld->mPhysicsSystem = physicsSystem;
		mGameWorld->mAudioSystem = NULL;

		mGameWorld->mTextureManager = new TextureManager(mRenderer);
		mGameWorld->mShaderManager = new ShaderManager();
		mGameWorld->mMaterialManager = new MaterialManager();
		mGameWorld->mSpriteManager = new SpriteManager();
		mGameWorld->mMeshManager = new MeshManager();
		mGameWorld->mModelManager = new ModelManager();
		mGameWorld->mSoundManager = new SoundManager();
		mGameWorld->mRenderManager = new RenderManager();

		physicsSystem->Startup();

		////////////////////////////////////

		std::shared_ptr<Shader> shaderGUI = mGameWorld->mShaderManager->Add("GUI", mEditorWorld->mShaderManager->Get("GUI"));

		mGameWorld->mSpriteSystem = new SpriteSystem(mRenderer, shaderGUI->Layout(), 256);

		////////////////////////////////////

		mGameWorld->mTextureManager->Add("default-alpha", SHARED(mRenderer->CreateTextureFromFile("Textures\\default-alpha.png")));
		mGameWorld->mTextureManager->Add("wall", SHARED(mRenderer->CreateTextureFromFile("Textures\\wall.png")));
		mGameWorld->mTextureManager->Add("wall-normal", SHARED(mRenderer->CreateTextureFromFile("Textures\\wall-normal.png")));
		mGameWorld->mTextureManager->Add("cobblestones", SHARED(mRenderer->CreateTextureFromFile("Textures\\cobblestones.bmp")));
		mGameWorld->mTextureManager->Add("cobblestones-normal", SHARED(mRenderer->CreateTextureFromFile("Textures\\cobblestones-normal.bmp")));
		mGameWorld->mTextureManager->Add("001-normal", SHARED(mRenderer->CreateTextureFromFile("Textures\\001-normal.png")));
		mGameWorld->mTextureManager->Add("test-normal", SHARED(mRenderer->CreateTextureFromFile("Textures\\test-normal.jpg")));
		mGameWorld->mTextureManager->Add("stone", SHARED(mRenderer->CreateTextureFromFile("Textures\\stone.jpg")));
		mGameWorld->mTextureManager->Add("stone-normal", SHARED(mRenderer->CreateTextureFromFile("Textures\\stone-normal.jpg")));
		
		mGameWorld->mShaderManager->Add("Color_Lit", mEditorWorld->mShaderManager->Get("Color_Lit"));
		mGameWorld->mShaderManager->Add("Color_Lit_Shadow", mEditorWorld->mShaderManager->Get("Color_Lit_Shadow"));
		mGameWorld->mShaderManager->Add("Texture_Lit", mEditorWorld->mShaderManager->Get("Texture_Lit"));
		mGameWorld->mShaderManager->Add("Texture_Lit_Shadow", mEditorWorld->mShaderManager->Get("Texture_Lit_Shadow"));
		mGameWorld->mShaderManager->Add("Sprite", mEditorWorld->mShaderManager->Get("Sprite"));
		mGameWorld->mShaderManager->Add("RT_Cube_Depth", mEditorWorld->mShaderManager->Get("RT_Cube_Depth"));
		mGameWorld->mShaderManager->Add("Normal_Map_Shadow", mEditorWorld->mShaderManager->Get("Normal_Map_Shadow"));

		std::shared_ptr<Shader> shaderColor = mGameWorld->mShaderManager->Get("Color_Lit_Shadow");
		std::shared_ptr<Shader> shaderTexture = mGameWorld->mShaderManager->Get("Texture_Lit_Shadow");
		std::shared_ptr<Shader> shaderNormalMap = mGameWorld->mShaderManager->Get("Normal_Map_Shadow");

		////////////////////////////////////

		//
		// Main Camera
		//
		obj = mGameWorld->AddGameObject(new GameObject());
		obj->mName = "Main Camera";

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(0, 15, 40);
		transform->mOrientation = Quaternion(-15, 0, 0);

		obj->Attach(new PlayerController());

		physics = (Physics*)obj->Attach(new Physics());
		physics->SetRigidBody(
			physicsSystem->CreateRigidBody(
			physicsSystem->CreateSphere(1.0f), transform->mPosition, transform->mOrientation, 1.0f));
		body = physics->GetRigidBody();
		body->SetFlags(DISABLE_GRAVITY);
		body->SetRestitution(1.0f);
		body->SetDamping(0.9f, 0.9f);
		body->SetAngularFactor(Vector3(0, 0, 0));

		camera = (Camera*)obj->Attach(new PerspectiveCamera(Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE));
		camera->mViewportWidth = Renderer::WINDOW_WIDTH_BASE;
		camera->mViewportHeight = Renderer::WINDOW_HEIGHT_BASE;

		//
		// Colored Box
		//
		meshBuilder.CreateCube(1.0f);
		meshBuilder.mLayout = shaderColor->Layout();
		meshBuilder.mPrimitive = PrimitiveFormat::TRIANGLES;

		material = mGameWorld->mMaterialManager->Add("Box", SHARED(new Material()));
		material->mShader = shaderColor;
		material->mBlendState = mRenderer->BLEND_OFF;
		material->mRenderQueue = RenderQueue::GEOMETRY;

		mesh = mGameWorld->mMeshManager->Add("Box", SHARED(meshBuilder.BuildMesh(mRenderer)));
		mesh->mMaterial = material;

		//
		// Ground object - Bottom
		//
		obj = mGameWorld->AddGameObject(new GameObject());
		obj->mName = "Ground_Bottom";

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(0, 0, 0);
		transform->mScale = Vector3(100, 1, 100);

		physics = (Physics*)obj->Attach(new Physics());
		physics->SetRigidBody(
			physicsSystem->CreateRigidBody(
			physicsSystem->CreateBox(transform->mScale), transform->mPosition, transform->mOrientation, 0.0f));
		body = physics->GetRigidBody();
		body->SetFlags(DISABLE_GRAVITY);

		meshComp = (MeshDrawable*)obj->Attach(new MeshDrawable(mesh));
		meshComp->mIsDynamic = true;
		
		//
		// Ground object - Back
		//
		obj = mGameWorld->AddGameObject(new GameObject());
		obj->mName = "Ground_Back";

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(0, 20, -20);
		transform->mScale = Vector3(20, 20, 1);

		physics = (Physics*)obj->Attach(new Physics());
		physics->SetRigidBody(
			physicsSystem->CreateRigidBody(
			physicsSystem->CreateBox(transform->mScale), transform->mPosition, transform->mOrientation, 0.0f));
		body = physics->GetRigidBody();
		body->SetFlags(DISABLE_GRAVITY);

		meshComp = (MeshDrawable*)obj->Attach(new MeshDrawable(mesh));
		meshComp->mIsDynamic = true;

		//
		// Ground object - Right
		//
		obj = mGameWorld->AddGameObject(new GameObject());
		obj->mName = "Ground_Right";

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(20, 20, 0);
		transform->mScale = Vector3(1, 20, 20);

		physics = (Physics*)obj->Attach(new Physics());
		physics->SetRigidBody(
			physicsSystem->CreateRigidBody(
			physicsSystem->CreateBox(transform->mScale), transform->mPosition, transform->mOrientation, 0.0f));
		body = physics->GetRigidBody();
		body->SetFlags(DISABLE_GRAVITY);

		meshComp = (MeshDrawable*)obj->Attach(new MeshDrawable(mesh));
		meshComp->mIsDynamic = true;

		//
		// Ground object - Left
		//
		obj = mGameWorld->AddGameObject(new GameObject());
		obj->mName = "Ground_Left";

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(-22, 10, 0);
		transform->mScale = Vector3(20, 1, 20);
		transform->mOrientation = Quaternion(0, 0, 0);

		physics = (Physics*)obj->Attach(new Physics());
		physics->SetRigidBody(
			physicsSystem->CreateRigidBody(
			physicsSystem->CreateBox(transform->mScale), transform->mPosition, transform->mOrientation, 0.0f));
		body = physics->GetRigidBody();
		body->SetFlags(DISABLE_GRAVITY);

		meshComp = (MeshDrawable*)obj->Attach(new MeshDrawable(mesh));
		meshComp->mIsDynamic = true;

		//
		// Ground object - Top
		//
		obj = mGameWorld->AddGameObject(new GameObject());
		obj->mName = "Ground_Top";

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(0, 20, 0);
		transform->mScale = Vector3(20, 1, 20);

		physics = (Physics*)obj->Attach(new Physics());
		physics->SetRigidBody(
			physicsSystem->CreateRigidBody(
			physicsSystem->CreateBox(transform->mScale), transform->mPosition, transform->mOrientation, 0.0f));
		body = physics->GetRigidBody();
		body->SetFlags(DISABLE_GRAVITY);

		meshComp = (MeshDrawable*)obj->Attach(new MeshDrawable(mesh));
		meshComp->mIsDynamic = true;
		
		//
		// Box object - Right
		//
		obj = mGameWorld->AddGameObject(new GameObject());
		obj->mName = "Box_Right";

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(10, 2, 5);
		transform->mScale = Vector3(1, 1, 1);

		physics = (Physics*)obj->Attach(new Physics());
		physics->SetRigidBody(
			physicsSystem->CreateRigidBody(
			physicsSystem->CreateBox(transform->mScale), transform->mPosition, transform->mOrientation, 1.0f));

		meshComp = (MeshDrawable*)obj->Attach(new MeshDrawable(mesh));
		meshComp->mIsDynamic = true;

		//
		// Box object - Left
		//
		obj = mGameWorld->AddGameObject(new GameObject());
		obj->mName = "Box_Left";

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(10, 5, 5);
		transform->mScale = Vector3(1, 2, 1);

		physics = (Physics*)obj->Attach(new Physics());
		physics->SetRigidBody(
			physicsSystem->CreateRigidBody(
			physicsSystem->CreateBox(transform->mScale), transform->mPosition, transform->mOrientation, 1.0f));

		meshComp = (MeshDrawable*)obj->Attach(new MeshDrawable(mesh));
		meshComp->mIsDynamic = true;

		//
		// Box object - Top
		//
		obj = mGameWorld->AddGameObject(new GameObject());
		obj->mName = "Box_Top";

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(0, 8, 5);
		transform->mScale = Vector3(1, 1, 1);

		physics = (Physics*)obj->Attach(new Physics());
		physics->SetRigidBody(
			physicsSystem->CreateRigidBody(
			physicsSystem->CreateBox(transform->mScale), transform->mPosition, transform->mOrientation, 1.0f));

		meshComp = (MeshDrawable*)obj->Attach(new MeshDrawable(mesh));
		meshComp->mIsDynamic = true;

		//
		// Box object - Back
		//
		obj = mGameWorld->AddGameObject(new GameObject());
		obj->mName = "Box_Back";

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(0, 3, -10);
		transform->mScale = Vector3(1, 1, 1);
		transform->mOrientation = Quaternion(45, 45, 45);

		physics = (Physics*)obj->Attach(new Physics());
		physics->SetRigidBody(
			physicsSystem->CreateRigidBody(
			physicsSystem->CreateBox(transform->mScale), transform->mPosition, transform->mOrientation, 1.0f));

		meshComp = (MeshDrawable*)obj->Attach(new MeshDrawable(mesh));
		meshComp->mIsDynamic = true;

		//
		// Dodecahedron
		//
		meshBuilder.ClearGeometry();
		meshBuilder.CreateDodecahedron(1.0f);
		meshBuilder.mLayout = shaderTexture->Layout();

		material = mGameWorld->mMaterialManager->Add("Dodecahedron", SHARED(new Material()));
		material->mShader = shaderColor; // intentionally set wrong shader to demonstrate vertex layout compatibility
		material->mBlendState = mRenderer->BLEND_ALPHA;
		material->mTexture[TextureIndex::DIFFUSE] = mGameWorld->mTextureManager->Get("default-alpha");
		material->mRenderQueue = RenderQueue::ALPHA_BLEND;

		mesh = mGameWorld->mMeshManager->Add("Dodecahedron", SHARED(meshBuilder.BuildMesh(mRenderer)));
		mesh->mMaterial = material;

		obj = new GameObject();
		obj->mName = "Dodecahedron";

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(10, 2, -10);
		transform->mScale = Vector3(1, 1, 1);
		transform->mOrientation = Quaternion(0, 0, 0);

		meshComp = (MeshDrawable*)obj->Attach(new MeshDrawable(mesh));
		meshComp->mIsDynamic = true;

		Buffer* meshVBO = mesh->mVertexBuffer;
		physics = (Physics*)obj->Attach(new Physics());
		physics->SetRigidBody(
			physicsSystem->CreateRigidBody(
			physicsSystem->CreateMesh((Vector3*)meshVBO->Lock(), meshVBO->Count(), meshVBO->Stride(), transform->mScale), transform->mPosition, transform->mOrientation, 1.0f));
		meshVBO->Unlock();

		mGameWorld->AddGameObject(obj);

		//
		// Sphere
		//
		meshBuilder.ClearGeometry();
		meshBuilder.CreateSphere(1.0f, 50, 50);
		meshBuilder.mLayout = shaderTexture->Layout();

		material = mGameWorld->mMaterialManager->Add("Sphere", SHARED(new Material()));
		material->mShader = shaderTexture;
		material->mBlendState = mRenderer->BLEND_ALPHA;
		material->mTexture[TextureIndex::DIFFUSE] = mGameWorld->mTextureManager->Get("default-alpha");
		material->mRenderQueue = RenderQueue::ALPHA_BLEND;

		mesh = mGameWorld->mMeshManager->Add("Sphere", SHARED(meshBuilder.BuildMesh(mRenderer)));
		mesh->mMaterial = material;

		obj = mGameWorld->AddGameObject(new GameObject());
		obj->mName = "Sphere";

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(15, 2, 10);
		transform->mScale = Vector3(1, 1, 1);

		meshComp = (MeshDrawable*)obj->Attach(new MeshDrawable(mesh));
		meshComp->mIsDynamic = true;

		physics = (Physics*)obj->Attach(new Physics());
		physics->SetRigidBody(
			physicsSystem->CreateRigidBody(
			physicsSystem->CreateSphere(transform->mScale.x), transform->mPosition, transform->mOrientation, 1.0f));

		//
		// Texture
		//
		meshBuilder.ClearAll();
		meshBuilder.CreateCube(1);
		meshBuilder.mLayout = shaderTexture->Layout();

		material = mGameWorld->mMaterialManager->Add("Texture", SHARED(new Material()));
		material->mShader = shaderTexture;
		material->mBlendState = mRenderer->BLEND_ALPHA;
		material->mTexture[TextureIndex::DIFFUSE] = mGameWorld->mTextureManager->Get("wall");
		material->mRenderQueue = RenderQueue::GEOMETRY;

		mesh = mGameWorld->mMeshManager->Add("Texture", SHARED(meshBuilder.BuildMesh(mRenderer)));
		mesh->mMaterial = material;

		obj = mGameWorld->AddGameObject(new GameObject("Texture"));

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(-17, 5, 16);
		transform->mOrientation = Quaternion(Vector3(0, 45, 0));
		transform->mScale = Vector3(3, 3, 3);

		meshComp = (MeshDrawable*)obj->Attach(new MeshDrawable(mesh));
		meshComp->mIsDynamic = true;

		physics = (Physics*)obj->Attach(new Physics());
		physics->SetRigidBody(
			physicsSystem->CreateRigidBody(
			physicsSystem->CreateBox(transform->mScale), transform->mPosition, transform->mOrientation, 1.0f));
		
		//
		// Normal Map
		//
		meshBuilder.ClearAll();
		meshBuilder.CreateCube(1);
		meshBuilder.CalculateTangents(true);
		meshBuilder.mLayout = shaderNormalMap->Layout();

		material = mGameWorld->mMaterialManager->Add("Normals", SHARED(new Material()));
		material->mShader = shaderNormalMap;
		material->mBlendState = mRenderer->BLEND_ALPHA;
		//material->mTexture[TextureIndex::DIFFUSE] = mGameWorld->mTextureManager->Get("wall");
		//material->mTexture[TextureIndex::NORMAL] = mGameWorld->mTextureManager->Get("wall-normal");
		//material->mTexture[TextureIndex::DIFFUSE] = mGameWorld->mTextureManager->Get("cobblestones");
		//material->mTexture[TextureIndex::NORMAL] = mGameWorld->mTextureManager->Get("cobblestones-normal");
		//material->mTexture[TextureIndex::DIFFUSE] = mRenderer->BASE_TEXTURE;
		//material->mTexture[TextureIndex::NORMAL] = mGameWorld->mTextureManager->Get("test-normal");
		material->mTexture[TextureIndex::DIFFUSE] = mGameWorld->mTextureManager->Get("stone");
		material->mTexture[TextureIndex::NORMAL] = mGameWorld->mTextureManager->Get("stone-normal");
		material->mRenderQueue = RenderQueue::GEOMETRY;
		//material->mSpecular = ColorRGBA(1, 1, 1, 1);
		//material->mSpecularComponent = 32;

		mesh = mGameWorld->mMeshManager->Add("Normals", SHARED(meshBuilder.BuildMesh(mRenderer)));
		mesh->mMaterial = material;

		obj = mGameWorld->AddGameObject(new GameObject("Normals"));

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(-15, 5, 5);
		transform->mOrientation = Quaternion(Vector3(90, 45, 0));
		transform->mScale = Vector3(3,3,3);
		normalMapTrans = transform;

		meshComp = (MeshDrawable*)obj->Attach(new MeshDrawable(mesh));
		meshComp->mIsDynamic = true;

		physics = (Physics*)obj->Attach(new Physics());
		physics->SetRigidBody(
			physicsSystem->CreateRigidBody(
			physicsSystem->CreateBox(transform->mScale), transform->mPosition, transform->mOrientation, 1.0f));

		//
		// Particles
		//
		std::shared_ptr<Texture> texture = mGameWorld->mTextureManager->Add("fire", SHARED(mRenderer->CreateTextureFromFile("Textures\\fire.png")));

		material = mGameWorld->mMaterialManager->Add("Fire", SHARED(new Material()));
		material->mShader = mGameWorld->mShaderManager->Get("Sprite");
		material->mBlendState = SHARED(mRenderer->CreateBlendState(BlendFormat::SRC_ALPHA, BlendFormat::ONE, BlendFormat::SRC_ALPHA, BlendFormat::ONE));
		material->mTexture[TextureIndex::DIFFUSE] = texture;
		material->mCullMode = CullFormat::NONE;
		material->mDepthMode = DepthFormat::OFF;
		material->mRenderQueue = RenderQueue::ALPHA_BLEND;

		std::shared_ptr< Sprite > sprite = mGameWorld->mSpriteManager->Add("Fire", SHARED(new Sprite()));
		sprite->AddFrame(Sprite::QUADtoTEXCOORD(Quad(0, 0, 64, 64), texture->Width(), texture->Height()));
		sprite->AddFrame(Sprite::QUADtoTEXCOORD(Quad(64, 0, 128, 64), texture->Width(), texture->Height()));

		ParticleSystem* particleSystem = BuildParticleSystemNormal(mRenderer, mGameWorld, material, sprite, 300);
		particleSystem->mSpawnRate = 0.025f;
		particleSystem->mMinLifetime = 3.0f;
		particleSystem->mMaxLifetime = 5.0f;
		particleSystem->mEffect.push_back(new TextureEffect(0, 0));
		particleSystem->mEffect.push_back(new AreaPositionEffect(0, Vector3(-0.125f, 1, 0), Vector3(0.125f, 1, 0)));
		particleSystem->mEffect.push_back(new RandomRotationEffect(0, Vector3(0, 0, -10), Vector3(0, 0, 10)));
		particleSystem->mEffect.push_back(new ScaleEffect(0, Vector3(1, 1, 1)));
		particleSystem->mEffect.push_back(new RandomVelocityEffect(0, Vector3(0, 2.0f, 0), Vector3(0, 2.8f, 0)));
		particleSystem->mEffect.push_back(new RandomColorEffect(0, ColorRGBA(0.75f, 0.25f, 0, 0.125f), ColorRGBA(0.75f, 0.75f, 0, 0.125f)));
		particleSystem->mEffect.push_back(new FadeToScaleEffect(0, 0.25f, 0.666f));
		particleSystem->mEffect.push_back(new FadeToScaleEffect(0.5f, 1.0f, 0.1f));
		particleSystem->mEffect.push_back(new TextureEffect(1.0f, 1));
		particleSystem->mEffect.push_back(new FadeToScaleEffect(1.0f, 2.0f, 1.0f));
		particleSystem->mEffect.push_back(new RandomColorEffect(1.0f, ColorRGBA(1, 1, 1, 0.125f), ColorRGBA(0.9f, 0.9f, 0.9f, 0.125f)));
		particleSystem->mEffect.push_back(new FadeToColorEffect(1.0f, 5.0f, ColorRGBA(0.9f, 0.9f, 0.9f, 0.025f)));
		particleSystem->mEffect.push_back(new RandomVelocityEffect(1.0f, Vector3(-0.5f, 1.4f, -0.5f), Vector3(0.5f, 3.0f, 0.5f)));

		obj = mGameWorld->AddGameObject(new GameObject());
		obj->mName = "Particle Emitter";

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(8, 1, 0);
		transform->mScale = Vector3(1, 1, 1);

		obj->Attach(new ParticleEmitter(particleSystem));

		//
		// Point Light attached to particle emitter
		//
		GameObject* peObject = obj;

		obj = new GameObject();
		obj->mName = "Point Light";

		transform = (Transform*)obj->Attach(new Transform());
		transform->mPosition = Vector3(0, 4, 0);

		PointLight* light = (PointLight*)obj->Attach(new PointLight(512));
		light->mColor = ColorRGBA(0.8f, 0.6f, 0.0f, 1.0f);
		light->mAttenuation = Vector4(1.0f, 0.0f, 0.0f, 40.0f);

		material = mGameWorld->mMaterialManager->Add("Point Light", SHARED(new Material()));
		material->mShader = mGameWorld->mShaderManager->Get("RT_Cube_Depth");
		material->mBlendState = SHARED(mRenderer->CreateBlendState(
			BlendFormat::DST_COLOR, BlendFormat::SRC_COLOR,
			BlendFormat::ZERO, BlendFormat::ZERO,
			BlendFunction::MIN, BlendFunction::ADD));
		material->mCullMode = CullFormat::CCW;
		material->mDepthMode = DepthFormat::ALWAYS;
		material->mRenderQueue = RenderQueue::BACKGROUND;

		light->mMaterial = material;

		peObject->AddChild(obj);

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
		std::shared_ptr<Shader> shaderLine = mEditorWorld->mShaderManager->Get("Color_Only");
		std::shared_ptr<Material> material = mEditorWorld->mMaterialManager->Add("Debug", SHARED(new Material()));
		material->mShader = shaderLine;
		material->mBlendState = mRenderer->BLEND_OFF;
		material->mDepthMode = DepthFormat::LEQUAL;
		material->mCullMode = CullFormat::NONE;
		material->mRenderQueue = RenderQueue::FOREGROUND;
	}

	void CreateTranslateObject()
	{
		mTranslateObject = new GameObject();
		mTranslateObject->mName = "Translate Object";
		const float tileSize = 5;

		std::shared_ptr<Model> model = SHARED(LoadModelM3DFromFile("Editor\\Translate.M3D", mRenderer, mGameWorld->mShaderManager, mGameWorld->mTextureManager, mGameWorld->mMaterialManager));

		std::vector<Mesh*> meshList;
		model->GetMeshList(&meshList);

		std::shared_ptr<BlendState> blend = SHARED(mRenderer->CreateBlendState(BlendFormat::SRC_ALPHA, BlendFormat::ONE_MINUS_SRC_ALPHA, BlendFormat::SRC_ALPHA, BlendFormat::ONE));

		TRAVERSE_VECTOR(x, meshList)
		{
			std::shared_ptr<Material> material = meshList[x]->mMaterial;

			material->mShader = mEditorWorld->mShaderManager->Get("Color_Only");
			material->mCullMode = CullFormat::NONE;
			material->mBlendState = blend;
			material->mDepthMode = DepthFormat::ALWAYS;
			material->mRenderQueue = RenderQueue::FOREGROUND;
			material->mAmbient.a = 0.5f;
		}

		//
		// Root Translate Object
		//
		mTranslateObject->Attach(new Transform());

		///////////////////////////////

		GameObject* obj = new GameObject();
		obj->mName = "X-Axis";

		Transform* transform = new Transform();
		transform->mPosition = Vector3(tileSize, 0, 0);
		transform->mOrientation = Quaternion(Vector3(0, 0, -90));
		obj->Attach(transform);

		ModelDrawable* modelComp = new ModelDrawable(model);
		//modelComp->SetMaterial(mSelectedMaterialX);
		obj->Attach(modelComp);

		mTranslateObject->AddChild(obj);

		///////////////////////////////

		obj = new GameObject();
		obj->mName = "Y-Axis";

		transform = new Transform();
		transform->mPosition = Vector3(0, tileSize, 0);
		transform->mOrientation = Quaternion(Vector3(0, 0, 0));
		obj->Attach(transform);

		modelComp = new ModelDrawable(model);
		//modelComp->SetMaterial(mSelectedMaterialY);
		obj->Attach(modelComp);

		mTranslateObject->AddChild(obj);

		///////////////////////////////

		obj = new GameObject();
		obj->mName = "Z-Axis";

		transform = new Transform();
		transform->mPosition = Vector3(0, 0, tileSize);
		transform->mOrientation = Quaternion(Vector3(90, 0, 0));
		obj->Attach(transform);

		modelComp = new ModelDrawable(model);
		//modelComp->SetMaterial(mSelectedMaterialZ);
		obj->Attach(modelComp);

		mTranslateObject->AddChild(obj);

		///////////////////////////////

		mTranslateObject->Startup();
		mTranslateObject->SetWorld(mGameWorld);

		mTranslateObject->SetActive(false);
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
			mTranslateObject->UpdateDrawable();
		}
	}

	void CreateRotateObject()
	{}

	void CreateScaleObject()
	{}

	void CreateBoundsObject()
	{
		MeshBuilder builder;

		std::shared_ptr< Shader > shaderLine = mEditorWorld->mShaderManager->Get("Color_Only");

		builder.mLayout = shaderLine->Layout();
		builder.CreateWireCube(1);

		Mesh* mesh = builder.BuildMesh(mRenderer);

		std::shared_ptr< Material > material = mEditorWorld->mMaterialManager->Add("Bounds", SHARED(new Material()));
		material->mShader = shaderLine;
		material->mBlendState = mRenderer->BLEND_OFF;
		material->mDepthMode = DepthFormat::LEQUAL;
		material->mCullMode = CullFormat::NONE;
		material->mRenderQueue = RenderQueue::FOREGROUND;

		mesh->mMaterial = material;

		mBoundsObject = new GameObject();
		mBoundsObject->mName = "Bounds";

		mBoundsObject->Attach(new Transform());
		mBoundsObject->Attach(new MeshDrawable(SHARED(mesh)));

		mBoundsObject->Startup();
		mBoundsObject->SetWorld(mGameWorld);
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

			MeshDrawable* mesh = mBoundsObject->GetComponent<MeshDrawable>();
			std::shared_ptr< Material > material = mesh->mMesh->mMaterial;

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

			mesh->mMesh->mBounds = BoundingBox();

			mBoundsObject->UpdateTransform();
			mBoundsObject->UpdateDrawable();

			if (mesh->CheckVisible(mWorldCamera))
				mesh->Draw();
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

		mHoverObject = NULL;
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

			mTranslateObject->SetActive((mSelectedObject != NULL) ? true : false);
		}
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	//_CrtSetBreakAlloc(242);

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
