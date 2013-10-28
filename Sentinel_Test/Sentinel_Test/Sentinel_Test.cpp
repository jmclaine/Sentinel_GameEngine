// Test a component based engine system.
//
#include "stdafx.h"

#include <crtdbg.h>
#include <exception>
#include <time.h>

#include "Sentinel_Test.h"

#include "Renderer.h"
#include "PhysicsSystem.h"
#include "ParticleSystem.h"
#include "NetworkSocket.h"
#include "AudioSystem.h"

#include "GameWindow.h"

#include "Archive.h"

#include "TextureManager.h"
#include "ShaderManager.h"
#include "MeshManager.h"
#include "ModelManager.h"
#include "SoundManager.h"

#include "Input.h"
#include "Timing.h"
#include "Util.h"

#include "MeshBuilder.h"
#include "Model.h"

#include "GameWorld.h"
#include "GameObject.h"

#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "LightComponent.h"
#include "PerspectiveCameraComponent.h"
#include "OrthographicCameraComponent.h"
#include "PlayerControllerComponent.h"
#include "PhysicsComponent.h"
#include "MeshComponent.h"
#include "ModelComponent.h"

#include "Sound.h"

#include "RandomValue.h"

using namespace Sentinel;


// Main Application.
//
class MainApp
{
	HACCEL					mAccelTable;
	
	GameWindow*				mGameWindow;
	Renderer*				mRenderer;

	Timing*					mTiming;
	PhysicsSystem*			mPhysicsSystem;
	AudioSystem*			mAudioSystem;
	ParticleSystem*			mParticleSystem;

	TextureManager*			mTextureManager;
	ShaderManager*			mShaderManager;
	MeshManager*			mMeshManager;
	ModelManager*			mModelManager;
	SoundManager*			mSoundManager;
	
	GameWorld*				mGameWorld;

public:

	MainApp()
	{
		srand( (UINT)time( (time_t*)0 ));

		mGameWindow		= NULL;
		mRenderer		= NULL;

		mTiming			= new Timing();
		mPhysicsSystem	= NULL;

		mTextureManager	= new TextureManager();
		mShaderManager	= new ShaderManager();
		mMeshManager	= new MeshManager();
		mModelManager	= new ModelManager();
		mSoundManager = new SoundManager();

		mAudioSystem	= NULL;
		
		mGameWorld		= new GameWorld();
	}

	~MainApp()
	{}

	int Run( HINSTANCE hInstance, int nCmdShow )
	{
		mGameWindow = new GameWindow( IDI_SENTINEL_TEST, IDI_SMALL, IDC_SENTINEL_TEST );
		
		//LoadString( hInstance, IDS_APP_TITLE, mTitle, GameWindow::MAX_LOADSTRING );
		//LoadString( hInstance, IDC_SENTINEL_TEST, mGameWindowClass, GameWindow::MAX_LOADSTRING );
		
		mAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_SENTINEL_TEST ));

		// Load config file to setup windows for the renderer.
		//
		WindowInfo info;
		mRenderer = Renderer::Create( "config.xml", info );
		if( !mRenderer )
		{
			REPORT_ERROR( "Failed to load 'config.xml'\nDefaulting to OpenGL", "Renderer Setup Failure" );

			if( !(mRenderer = BuildRendererGL()) )
				throw AppException( "Failed to create OpenGL Renderer" );
		}

		// Prepare main window.
		//
		mGameWindow->Startup( mRenderer, hInstance, nCmdShow, "Sentinel_Test", "SentinelClass0", info );

		mRenderer->CreateDepthStencil( info.Width(), info.Height() );
		mRenderer->CreateBackbuffer();

		////////////////////////////////////

		Mouse::Get().SetPosition( CenterHandle( mGameWindow->GetHandle() ));
		ShowCursor( FALSE );

		////////////////////////////////////

		mPhysicsSystem = BuildPhysicsSystemBT();
		mPhysicsSystem->Startup();

		mAudioSystem = BuildAudioSystemAL();

		Archive archive;
		archive.Open( "Default.MAP", "rb" );

		mTextureManager->Load( archive, mRenderer );
		mShaderManager->Load( archive, mRenderer );
		mMeshManager->Load( archive, mRenderer, mShaderManager, mTextureManager );
		mModelManager->Load( archive, mRenderer, mShaderManager, mTextureManager );
		mSoundManager->Load( archive, mAudioSystem );
		
		mGameWorld->mTiming			= mTiming;
		mGameWorld->mRenderer		= mRenderer;
		mGameWorld->mPhysicsSystem	= mPhysicsSystem;
		mGameWorld->mTextureManager = mTextureManager;
		mGameWorld->mShaderManager	= mShaderManager;
		mGameWorld->mMeshManager	= mMeshManager;
		mGameWorld->mModelManager	= mModelManager;
		
		mGameWorld->Load( archive );

		//std::shared_ptr< Sound > source = mSoundManager->Get( "rtsoundthrow.wav" );
		//source->mLoop = true;
		//source->Play();

		archive.Close();
		/*
		PrepareShaders();
		PrepareObjects();
		PrepareFont();
		//*/

		mGameWorld->Startup();

		SetDirectory( "Objects" );

		std::shared_ptr< Texture > texture = mTextureManager->Add( "fire.png", mRenderer->CreateTextureFromFile( "fire.png" ));
		std::shared_ptr< Shader >  shader  = mShaderManager->Add( "sprite", mRenderer->CreateShaderFromFile( "sprite", "XCx", "PXx" ));

		std::shared_ptr< Sprite > sprite( new Sprite( texture, shader, Point2i( 64, 64 )));

		mParticleSystem = BuildParticleSystemNormal( mRenderer, mGameWorld, sprite, 300 );
		mParticleSystem->mSpawnRate   = 0.025f;
		mParticleSystem->mMinLifetime = 3.0f;
		mParticleSystem->mMaxLifetime = 5.0f;
		mParticleSystem->mEffect.push_back( new TextureEffect( 0, 0 ));
		mParticleSystem->mEffect.push_back( new AreaPositionEffect( 0, Vector3f( -0.125f, 1, 0 ), Vector3f( 0.125f, 1, 0 )));
		mParticleSystem->mEffect.push_back( new RandomRotationEffect( 0, Vector3f( 0, 0, -10 ), Vector3f( 0, 0, 10 )));
		mParticleSystem->mEffect.push_back( new ScaleEffect( 0, Vector3f( 1, 1, 1 )));
		mParticleSystem->mEffect.push_back( new VelocityEffect( 0, Vector3f( 0, 1.2f, 0 )));
		mParticleSystem->mEffect.push_back( new RandomColorEffect( 0, ColorRGBA( 0.75f, 0.25f, 0, 0.125 ), ColorRGBA( 0.75f, 0.75f, 0, 0.125 )));
		mParticleSystem->mEffect.push_back( new FadeToScaleEffect( 0, 0.25f, 0.666f ));
		mParticleSystem->mEffect.push_back( new FadeToScaleEffect( 0.5f, 1.0f, 0.1f ));
		mParticleSystem->mEffect.push_back( new TextureEffect( 1.0f, 1 ));
		mParticleSystem->mEffect.push_back( new FadeToScaleEffect( 1.0f, 2.0f, 1.0f ));
		mParticleSystem->mEffect.push_back( new RandomColorEffect( 1.0f, ColorRGBA( 1, 1, 1, 0.125 ), ColorRGBA( 0.9f, 0.9f, 0.9f, 0.125 )));
		mParticleSystem->mEffect.push_back( new FadeToColorEffect( 1.0f, 5.0f, ColorRGBA( 0.9f, 0.9f, 0.9f, 0.025f )));
		//mParticleSystem->mEffect.push_back( new RandomVelocityEffect( 1, Vector3f( -1, 1, 0 ), Vector3f( 1, 1, 0 )));

		mParticleSystem->Startup();

		SetDirectory( ".." );
		
		// Enter main game loop.
		//
		MSG msg;
		for(;;)
		{
			if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
			{
				switch( msg.message )
				{
					case WM_QUIT:
						return (int)msg.wParam;
				}
				
				if( !TranslateAccelerator( msg.hwnd, mAccelTable, &msg ))
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
			}
			else
			{
				Update();
			}
		}
	}

	void Update()
	{
		mTiming->Update();

		if( Keyboard::Get().DidGoDown( VK_ESCAPE ))
		{
			PostQuitMessage( 0 );
		}

		static float color[] = {0.0f, 0.2f, 0.8f, 1.0f};

		mRenderer->SetDepthStencil( 0 );
		mRenderer->SetViewport( 0, 0, mGameWindow->GetInfo()->Width(), mGameWindow->GetInfo()->Height() );
		mRenderer->SetRenderTarget( 0 );

		mRenderer->Clear( color );

		mGameWorld->UpdateController();

		BEGIN_PROFILE( mTiming );
		mPhysicsSystem->Update( mTiming->DeltaTime() );
		END_PROFILE( mTiming, "Particle" );

		mGameWorld->UpdatePhysics();
		mGameWorld->UpdateTransform();
		mGameWorld->UpdateComponents();
		mGameWorld->UpdateDrawable();

		mParticleSystem->Update( mTiming->DeltaTime() );

		mRenderer->Present();

		Mouse::Get().Update();
		Keyboard::Get().Update();

		mTiming->Limit();
	}

	void PrepareShaders()
	{
		SetDirectory( "Shaders" );
		
		ShaderManager::LoadConfig( "config.xml", mRenderer, mShaderManager );

		SetDirectory( ".." );
	}
	
	void PrepareObjects()
	{
		SetDirectory( "Objects" );
		
		const WindowInfo*		info;
		TransformComponent*		transform;
		ControllerComponent*	controller;
		PhysicsComponent*		physics;
		CameraComponent*		camera;
		LightComponent*			light;
		GameObject*				obj;

		//////////////////////////////

		// Create main perspective camera.
		//
		info = mGameWindow->GetInfo();
		camera = new PerspectiveCameraComponent( (float)info->Width(), (float)info->Height() );
		
		transform = new TransformComponent();
		transform->mPosition = Vector3f( 0, 10, 50 );
		
		controller = new PlayerControllerComponent();
		
		physics = new PhysicsComponent( mPhysicsSystem->CreateSphere( transform->mPosition, transform->mOrientation, 1, 1 ));
		physics->GetRigidBody()->SetFlags( DISABLE_GRAVITY );
		physics->GetRigidBody()->SetDamping( 0.9f, 0.9f );
		physics->GetRigidBody()->SetAngularFactor( Vector3f() );

		obj = mGameWorld->AddGameObject( new GameObject(), "MainCamera" );
		obj->AttachComponent( transform,	"Transform" );
		obj->AttachComponent( controller,	"Player" );
		obj->AttachComponent( physics,		"Physics" );
		obj->AttachComponent( camera,		"Camera" );

		//////////////////////////////

		// Create sprite orthographic camera.
		//
		camera = new OrthographicCameraComponent( (float)info->Width(), (float)info->Height() );
		
		transform = new TransformComponent();
		transform->mPosition = Vector3f( 0, 0, 0 );
		
		obj = mGameWorld->AddGameObject( new GameObject(), "SpriteCamera" );
		obj->AttachComponent( transform,	"Transform" );
		obj->AttachComponent( camera,		"Camera" );
		
		//////////////////////////////

		// Create a point light.
		//
		transform = new TransformComponent();
		transform->mPosition = Vector3f( 0, 2000, 0 );

		light = new LightComponent();
		light->mAttenuation = Vector4f( 1, 1, 1, 2000 );
		light->mColor		= ColorRGBA( 1, 1, 1, 1 );

		obj = mGameWorld->AddGameObject( new GameObject(), "PointLight" );
		obj->AttachComponent( transform,	"Transform" );
		obj->AttachComponent( light,		"Light" );

		//////////////////////////////

		// Create meshes and model for object instancing.
		//
		MeshBuilder meshBuilder;
		
		meshBuilder.mShader = mShaderManager->Get( "Texture" );
		meshBuilder.mTexture[ TEXTURE_DIFFUSE ] = mTextureManager->Add( "DEFAULT", mRenderer->CreateTextureFromFile( "default-alpha.png" ));

		meshBuilder.CreateCube( 1 );
		mMeshManager->Add( "Cube", std::shared_ptr< Mesh >( meshBuilder.BuildMesh( mRenderer )));
		
		meshBuilder.ClearGeometry();
		meshBuilder.CreateCylinder( 1, 1, 10 );
		mMeshManager->Add( "Cylinder", std::shared_ptr< Mesh >( meshBuilder.BuildMesh( mRenderer )));

		meshBuilder.ClearGeometry();
		meshBuilder.CreateOctahedron( 1 );
		mMeshManager->Add( "Octahedron", std::shared_ptr< Mesh >( meshBuilder.BuildMesh( mRenderer )));

		meshBuilder.ClearGeometry();
		meshBuilder.CreateTetrahedron( 1 );
		mMeshManager->Add( "Tetrahedron", std::shared_ptr< Mesh >( meshBuilder.BuildMesh( mRenderer )));

		meshBuilder.ClearGeometry();
		meshBuilder.CreateDodecahedron( 1 );
		mMeshManager->Add( "Dodecahedron", std::shared_ptr< Mesh >( meshBuilder.BuildMesh( mRenderer )));

		meshBuilder.ClearGeometry();
		meshBuilder.CreateSphere( 1, 10, 10 );
		mMeshManager->Add( "Sphere", std::shared_ptr< Mesh >( meshBuilder.BuildMesh( mRenderer )));

		meshBuilder.ClearGeometry();
		meshBuilder.CreateWireSphere( 1, 10, 10 );
		meshBuilder.mShader = mShaderManager->Get( "Color" );
		mMeshManager->Add( "Wire Sphere", std::shared_ptr< Mesh >( meshBuilder.BuildMesh( mRenderer )));

		std::shared_ptr< Model > model = std::shared_ptr< Model >( Model::Load( "Player.M3D", mRenderer, mShaderManager, mTextureManager ));

		if( !model )
			throw AppException( "Player.M3D failed to load." );

		mModelManager->Add( "Player", model );

		// Create simple box in center of the world.
		//
		transform = new TransformComponent();
		transform->mPosition	= Vector3f( 9.9f, 0, 0 );
		transform->mOrientation = Quatf( 0, 0, 1, 15 ).AxisAngle();
		transform->mScale		= Vector3f( 10, 1, 25 );
		
		physics = new PhysicsComponent( mPhysicsSystem->CreateBox( transform->mPosition, transform->mOrientation, transform->mScale, 0 ));

		obj = mGameWorld->AddGameObject( new GameObject(), "Origin" );
		obj->AttachComponent( transform,	"Transform" );
		obj->AttachComponent( physics,		"Physics" );
		obj->AttachComponent( new MeshComponent( mMeshManager->Get( "Cube" )), "Mesh" );

		////////////////////////////////
		
		transform = new TransformComponent();
		transform->mPosition	= Vector3f( -9.9f, 0, 0 );
		transform->mOrientation = Quatf( 0, 0, 1, -15 ).AxisAngle();
		transform->mScale		= Vector3f( 10, 1, 25 );
		
		physics = new PhysicsComponent( mPhysicsSystem->CreateBox( transform->mPosition, transform->mOrientation, transform->mScale, 0 ));

		obj = mGameWorld->AddGameObject( new GameObject(), "Origin" );
		obj->AttachComponent( transform,	"Transform" );
		obj->AttachComponent( physics,		"Physics" );
		obj->AttachComponent( new MeshComponent( mMeshManager->Get( "Cube" )), "Mesh" );
		
		////////////////////////////////
		
		// Create some random objects from the mesh list.
		//
		char name[32];
		int  count = 50;
		for( int x = 0; x < count; ++x )
		{
			Vector3f scale = Vector3f( RandomValue( 0.25f, 0.5f ), RandomValue( 0.25f, 0.5f ), RandomValue( 0.25f, 0.5f ));
			float mass     = scale.Length();
			UINT  shape    = RandomValue( 0, 7 );
			std::shared_ptr< Mesh >	mesh;

			transform = new TransformComponent();
			transform->mPosition = Vector3f( RandomValue( -20.0f, 20.0f ), 50.0f, RandomValue( -20.0f, 20.0f ));
			transform->mScale    = scale;
				
			switch( shape )
			{
				// Cube
				//
				case 0:
					mesh = mMeshManager->Get( "Cube" );

					physics = new PhysicsComponent( mPhysicsSystem->CreateBox( transform->mPosition, Quatf( 0, 0, 0, 1 ), transform->mScale, mass ));

					break;

				// Cylinder
				//
				case 1:
					mesh = mMeshManager->Get( "Cylinder" );

					transform->mScale.z = transform->mScale.x;
					transform->mScale.y = transform->mScale.y * 5;
					physics = new PhysicsComponent( mPhysicsSystem->CreateCylinder( transform->mPosition, Quatf( 0, 0, 0, 1 ), transform->mScale, mass ));

					break;

				// Sphere
				//
				case 2:
					mesh = mMeshManager->Get( "Sphere" );

					transform->mScale = Vector3f( mass, mass, mass );
					physics = new PhysicsComponent( mPhysicsSystem->CreateSphere( transform->mPosition, Quatf( 0, 0, 0, 1 ), mass, mass ));

					break;

				// Wire Sphere
				//
				case 3:
					mesh = mMeshManager->Get( "Wire Sphere" );

					transform->mScale = Vector3f( mass, mass, mass );
					physics = new PhysicsComponent( mPhysicsSystem->CreateSphere( transform->mPosition, Quatf( 0, 0, 0, 1 ), mass, mass ));

					break;

				// Tetrahedron
				//
				case 4:
					mesh = mMeshManager->Get( "Tetrahedron" );

					physics = new PhysicsComponent( mPhysicsSystem->CreateMesh( transform->mPosition, Quatf( 0, 0, 0, 1 ), transform->mScale, mesh.get(), mass ));

					break;

				// Octahedron
				//
				case 5:
					mesh = mMeshManager->Get( "Octahedron" );

					physics = new PhysicsComponent( mPhysicsSystem->CreateMesh( transform->mPosition, Quatf( 0, 0, 0, 1 ), transform->mScale, mesh.get(), mass ));

					break;

				// Dodecahedron
				//
				case 6:
					mesh = mMeshManager->Get( "Dodecahedron" );

					physics = new PhysicsComponent( mPhysicsSystem->CreateMesh( transform->mPosition, Quatf( 0, 0, 0, 1 ), transform->mScale, mesh.get(), mass ));

					break;

				// All others
				//
				default:
					transform->mScale = Vector3f( mass, mass, mass ) * 0.025f;

					physics = new PhysicsComponent( mPhysicsSystem->CreateSphere( transform->mPosition, Quatf( 0, 0, 0, 1 ), mass * 0.6f, mass ));

					break;
			}

			sprintf_s( name, "Object %d", x );

			obj = mGameWorld->AddGameObject( new GameObject(), name );
			obj->AttachComponent( transform,	"Transform" );
			obj->AttachComponent( physics,		"Physics" );

			if( shape < 7 )
				obj->AttachComponent( new MeshComponent( mesh ),	"Mesh" );
			else
				obj->AttachComponent( new ModelComponent( model ),	"Model" );
		}

		SetDirectory( ".." );
	}

	void PrepareFont()
	{
		SetDirectory( "Font" );
		
		// No font...yet.

		SetDirectory( ".." );
	}

	void Shutdown()
	{
		if( mGameWorld )
		{
			mGameWorld->Shutdown();
			delete mGameWorld;
		}

		SAFE_DELETE( mTiming );
		SAFE_DELETE( mPhysicsSystem );
		SAFE_DELETE( mParticleSystem );
		SAFE_DELETE( mAudioSystem );
		
		SAFE_DELETE( mSoundManager );
		SAFE_DELETE( mModelManager );
		SAFE_DELETE( mMeshManager );
		SAFE_DELETE( mShaderManager );
		SAFE_DELETE( mTextureManager );

		if( mGameWindow )
		{
			mGameWindow->Shutdown();
			delete mGameWindow;
		}
		
		SAFE_DELETE( mRenderer );
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////

int APIENTRY _tWinMain( HINSTANCE hInstance,
					    HINSTANCE hPrevInstance,
					    LPTSTR    lpCmdLine,
					    int       nCmdShow )
{
	// Check for memory leaks.
	//
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
	//_CrtSetBreakAlloc( 185 );

	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	int result = 0;
	MainApp* mainApp = 0;

	try
	{
		mainApp = new MainApp();
		result  = mainApp->Run( hInstance, nCmdShow );
	}
	catch( AppException e )
	{
		REPORT_ERROR( e.what(), "Application Error" );
	}
	catch( std::exception e )
	{
		REPORT_ERROR( e.what(), "Application Error" );
	}
	catch( std::bad_alloc& )
	{
		REPORT_ERROR( "Failed to allocate memory.", "Application Error" );
	}
	catch( ... )
	{
		REPORT_ERROR( "Failed to initialize.", "Application Error" );
	}

	if( mainApp )
	{
		mainApp->Shutdown();
		delete mainApp;
	}
	
	return result;
}
