// Test a component based engine system.
//
#include "stdafx.h"

#include <crtdbg.h>
#include <exception>
#include <time.h>

#include "Sentinel_Test.h"

#include "GameWindow.h"
#include "PhysicsSystem.h"
#include "ParticleSystem.h"
#include "NetworkSocket.h"
#include "AudioSystem.h"

#include "TextureManager.h"
#include "ShaderManager.h"
#include "MeshManager.h"
#include "ModelManager.h"

#include "Input.h"
#include "Timing.h"
#include "Util.h"

#include "MeshBuilder.h"
#include "Model.h"

#include "GameWorld.h"
#include "GameObject.h"

#include "PerspectiveCameraComponent.h"
#include "OrthographicCameraComponent.h"
#include "PlayerControllerComponent.h"
#include "PhysicsComponent.h"
#include "MeshComponent.h"
#include "ModelComponent.h"

#include "RandomValue.h"

using namespace Sentinel;


// Main Application.
//
class MainApp
{
	HACCEL					mAccelTable;
	
	GameWindow*				mWindow;

	AudioSource*			mSound;
	
public:

	MainApp()
	{
		srand( (UINT)time( (time_t*)0 ));

		mWindow = NULL;
	
		TextureManager::Create();
		ShaderManager::Create();
		MeshManager::Create();
		ModelManager::Create();
	}

	~MainApp()
	{}

	int Run( HINSTANCE hInstance, int nCmdShow )
	{
		mWindow = new GameWindow( IDI_SENTINEL_TEST, IDI_SMALL, IDC_SENTINEL_TEST );
		
		//LoadString( hInstance, IDS_APP_TITLE, mTitle, GameWindow::MAX_LOADSTRING );
		//LoadString( hInstance, IDC_SENTINEL_TEST, mWindowClass, GameWindow::MAX_LOADSTRING );
		
		mAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_SENTINEL_TEST ));

		// Load config file to setup windows for the renderer.
		//
		WindowInfo info;
		if( !Renderer::Create( "config.xml", info ))
		{
			REPORT_ERROR( "Failed to load 'config.xml'\nDefaulting to OpenGL", "Renderer Setup Failure" );

			if( !Renderer::Inst( BuildRendererGL() ))
				throw AppException( "Failed to BuildRendererGL" );
		}

		// Prepare main window.
		//
		mWindow->Startup( hInstance, nCmdShow, "Sentinel_Test", "SentinelClass0", info );

		Renderer::Inst()->CreateDepthStencil( info.Width(), info.Height() );
		Renderer::Inst()->CreateBackbuffer();

		////////////////////////////////////

		Mouse::Inst()->SetPosition( CenterHandle( mWindow->GetHandle() ));
		ShowCursor( FALSE );

		////////////////////////////////////

		PhysicsSystem::Inst()->Startup();

		SetDirectory( "Sounds" );
		AudioSystem::Inst( AudioSystem::Create() )->Startup();
		mSound = AudioSystem::Inst()->CreateSound( "rtsoundthrow.wav" );
		mSound->Play();
		SetDirectory( ".." );

		/*
		PrepareShaders();
		PrepareObjects();
		PrepareFont();
		//*/
		
		Archive archive;
		archive.Open( "Default.MAP", "rb" );

		TextureManager::Inst()->Load( archive );
		ShaderManager::Inst()->Load( archive );
		MeshManager::Inst()->Load( archive );
		ModelManager::Inst()->Load( archive );
		GameWorld::Inst()->Load( archive );

		archive.Close();

		GameWorld::Inst()->Startup();
		//*/
		ParticleSystem::Inst()->Startup( ShaderManager::Inst()->Get( "Sprite" ), 100 );

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
		Timing::Inst()->Update();

		if( Keyboard::Inst()->DidGoDown( VK_ESCAPE ))
		{
			PostQuitMessage( 0 );
		}

		static float color[] = {0.0f, 0.2f, 0.8f, 1.0f};

		Renderer::Inst()->SetDepthStencil( 0 );
		Renderer::Inst()->SetViewport( 0, 0, mWindow->GetInfo()->Width(), mWindow->GetInfo()->Height() );
		Renderer::Inst()->SetRenderTarget( 0 );

		Renderer::Inst()->Clear( color );

		GameWorld::Inst()->Update();

		Renderer::Inst()->Present();

		/////////////////////////////////
		/*
		mWindow1->SetActive();

		Renderer::Inst()->SetDepthStencil( 1 );
		Renderer::Inst()->SetViewport( 0, 0, mWindow1->GetInfo()->Width(), mWindow1->GetInfo()->Height() );
		Renderer::Inst()->SetRenderTarget( 1 );

		Renderer::Inst()->Clear( color );

		Renderer::Inst()->Present();
		*/
		/////////////////////////////////

		Mouse::Inst()->Update();
		Keyboard::Inst()->Update();

		Timing::Inst()->Limit();
	}

	void PrepareShaders()
	{
		SetDirectory( "Shaders" );
		
		if( !(ShaderManager::Inst()->LoadConfig( "config.xml" )))
			throw AppException( "Failed to load 'Shaders\\config.xml'" );

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

		//GameWorld::Inst()->Load( "Default.MAP" );
		
		// Create main perspective camera.
		//
		info = mWindow->GetInfo();
		camera = new PerspectiveCameraComponent( (float)info->Width(), (float)info->Height() );
		
		transform = new TransformComponent();
		transform->mPosition = Vector3f( 0, 10, 50 );
		
		controller = new PlayerControllerComponent();
		
		physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateSphere( transform->mPosition, transform->mOrientation, 1, 1 ));
		physics->GetRigidBody()->setFlags( BT_DISABLE_WORLD_GRAVITY );
		physics->GetRigidBody()->setDamping( 0.9f, 0.9f );
		physics->GetRigidBody()->setRestitution( 1 );
		physics->GetRigidBody()->setAngularFactor( 0 );

		obj = GameWorld::Inst()->AddGameObject( new GameObject(), "MainCamera" );
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
		
		obj = GameWorld::Inst()->AddGameObject( new GameObject(), "SpriteCamera" );
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

		obj = GameWorld::Inst()->AddGameObject( new GameObject(), "PointLight" );
		obj->AttachComponent( transform,	"Transform" );
		obj->AttachComponent( light,		"Light" );

		//////////////////////////////

		// Create meshes and model for object instancing.
		//
		MeshBuilder meshBuilder;
		
		meshBuilder.mShader = ShaderManager::Inst()->Get( "Texture" );
		meshBuilder.mTexture[ TEXTURE_DIFFUSE ] = TextureManager::Inst()->Add( "DEFAULT", Renderer::Inst()->CreateTextureFromFile( "default-alpha.png" ));

		meshBuilder.CreateCube( 1 );
		MeshManager::Inst()->Add( "Cube", std::shared_ptr< Mesh >( meshBuilder.BuildMesh() ));
		
		meshBuilder.ClearGeometry();
		meshBuilder.CreateCylinder( 1, 1, 10 );
		MeshManager::Inst()->Add( "Cylinder", std::shared_ptr< Mesh >( meshBuilder.BuildMesh() ));

		meshBuilder.ClearGeometry();
		meshBuilder.CreateOctahedron( 1 );
		MeshManager::Inst()->Add( "Octahedron", std::shared_ptr< Mesh >( meshBuilder.BuildMesh() ));

		meshBuilder.ClearGeometry();
		meshBuilder.CreateTetrahedron( 1 );
		MeshManager::Inst()->Add( "Tetrahedron", std::shared_ptr< Mesh >( meshBuilder.BuildMesh() ));

		meshBuilder.ClearGeometry();
		meshBuilder.CreateDodecahedron( 1 );
		MeshManager::Inst()->Add( "Dodecahedron", std::shared_ptr< Mesh >( meshBuilder.BuildMesh() ));

		meshBuilder.ClearGeometry();
		meshBuilder.CreateSphere( 1, 10, 10 );
		MeshManager::Inst()->Add( "Sphere", std::shared_ptr< Mesh >( meshBuilder.BuildMesh() ));

		meshBuilder.ClearGeometry();
		meshBuilder.CreateWireSphere( 1, 10, 10 );
		meshBuilder.mShader = ShaderManager::Inst()->Get( "Color" );
		MeshManager::Inst()->Add( "Wire Sphere", std::shared_ptr< Mesh >( meshBuilder.BuildMesh() ));

		std::shared_ptr< Model > model = std::shared_ptr< Model >( Model::Load( "Player.M3D" ));

		if( !model )
			throw AppException( "Player.M3D failed to load." );

		ModelManager::Inst()->Add( "Player", model );

		// Create simple box in center of the world.
		//
		transform = new TransformComponent();
		transform->mPosition	= Vector3f( 9.9f, 0, 0 );
		transform->mOrientation = Quatf( 0, 0, 1, 15 ).AxisAngle();
		transform->mScale		= Vector3f( 10, 1, 25 );
		
		physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateBox( transform->mPosition, transform->mOrientation, transform->mScale, 0 ));

		obj = GameWorld::Inst()->AddGameObject( new GameObject(), "Origin" );
		obj->AttachComponent( transform,	"Transform" );
		obj->AttachComponent( physics,		"Physics" );
		obj->AttachComponent( new MeshComponent( MeshManager::Inst()->Get( "Cube" )), "Mesh" );

		////////////////////////////////
		
		transform = new TransformComponent();
		transform->mPosition	= Vector3f( -9.9f, 0, 0 );
		transform->mOrientation = Quatf( 0, 0, 1, -15 ).AxisAngle();
		transform->mScale		= Vector3f( 10, 1, 25 );
		
		physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateBox( transform->mPosition, transform->mOrientation, transform->mScale, 0 ));

		obj = GameWorld::Inst()->AddGameObject( new GameObject(), "Origin" );
		obj->AttachComponent( transform,	"Transform" );
		obj->AttachComponent( physics,		"Physics" );
		obj->AttachComponent( new MeshComponent( MeshManager::Inst()->Get( "Cube" )), "Mesh" );
		
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
					mesh = MeshManager::Inst()->Get( "Cube" );

					physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateBox( transform->mPosition, Quatf( 0, 0, 0, 1 ), transform->mScale, mass ));

					break;

				// Cylinder
				//
				case 1:
					mesh = MeshManager::Inst()->Get( "Cylinder" );

					transform->mScale.z = transform->mScale.x;
					transform->mScale.y = transform->mScale.y * 5;
					physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateCylinder( transform->mPosition, Quatf( 0, 0, 0, 1 ), transform->mScale, mass ));

					break;

				// Sphere
				//
				case 2:
					mesh = MeshManager::Inst()->Get( "Sphere" );

					transform->mScale = Vector3f( mass, mass, mass );
					physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateSphere( transform->mPosition, Quatf( 0, 0, 0, 1 ), mass, mass ));

					break;

				// Wire Sphere
				//
				case 3:
					mesh = MeshManager::Inst()->Get( "Wire Sphere" );

					transform->mScale = Vector3f( mass, mass, mass );
					physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateSphere( transform->mPosition, Quatf( 0, 0, 0, 1 ), mass, mass ));

					break;

				// Tetrahedron
				//
				case 4:
					mesh = MeshManager::Inst()->Get( "Tetrahedron" );

					physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateMesh( transform->mPosition, Quatf( 0, 0, 0, 1 ), transform->mScale, mesh.get(), mass ));

					break;

				// Octahedron
				//
				case 5:
					mesh = MeshManager::Inst()->Get( "Octahedron" );

					physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateMesh( transform->mPosition, Quatf( 0, 0, 0, 1 ), transform->mScale, mesh.get(), mass ));

					break;

				// Dodecahedron
				//
				case 6:
					mesh = MeshManager::Inst()->Get( "Dodecahedron" );

					physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateMesh( transform->mPosition, Quatf( 0, 0, 0, 1 ), transform->mScale, mesh.get(), mass ));

					break;

				// All others
				//
				default:
					transform->mScale = Vector3f( mass, mass, mass ) * 0.025f;

					physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateSphere( transform->mPosition, Quatf( 0, 0, 0, 1 ), mass * 0.6f, mass ));

					break;
			}

			sprintf_s( name, "Object %d", x );

			obj = GameWorld::Inst()->AddGameObject( new GameObject(), name );
			obj->AttachComponent( transform,	"Transform" );
			obj->AttachComponent( physics,		"Physics" );

			if( shape < 7 )
				obj->AttachComponent( new MeshComponent( mesh ),	"Mesh" );
			else
				obj->AttachComponent( new ModelComponent( model ),	"Model" );
		}

		GameWorld::Inst()->Startup();

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
		delete mSound;

		Mouse::Destroy();
		Keyboard::Destroy();

		GameWorld::Inst()->Shutdown();
		GameWorld::Destroy();

		ModelManager::Destroy();
		MeshManager::Destroy();
		ShaderManager::Destroy();
		TextureManager::Destroy();

		mWindow->Shutdown();
		delete mWindow;
		
		AudioSystem::Inst()->Shutdown();
		AudioSystem::Destroy();

		Timing::Destroy();
		PhysicsSystem::Destroy();
		ParticleSystem::Destroy();

		Renderer::Destroy();
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
	//_CrtSetBreakAlloc( 565 );

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
