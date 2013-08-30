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

using namespace Sentinel;


// Main Application.
//
enum ShaderTypes
{
	SHADER_COLOR,
	SHADER_TEXTURE,
	SHADER_NORMAL_MAP,
	SHADER_SPRITE,

	NUM_SHADERS
};

enum ShapeTypes
{
	SHAPE_CUBE,
	SHAPE_CYLINDER,
	SHAPE_OCTAHEDRON,
	SHAPE_TETRAHEDRON,
	SHAPE_DODECAHEDRON,
	SHAPE_SPHERE,
	SHAPE_WIRE_SPHERE,

	NUM_SHAPES
};

//////////////////////////////////////////

class MainApp
{
	HACCEL					mAccelTable;
	
	Shader*					mShader[ NUM_SHADERS ];
	Texture*				mTexture;

	GameWindow*				mWindow0;
	GameWindow*				mWindow1;

public:

	MainApp()
	{
		srand( (UINT)time( (time_t*)0 ));

		for( UINT x = 0; x < NUM_SHADERS; ++x )
			mShader[ x ] = NULL;

		mTexture = NULL;

		mWindow0 = NULL;
		mWindow1 = NULL;
	}

	~MainApp()
	{}

	int Run( HINSTANCE hInstance, int nCmdShow )
	{
		mWindow0 = new GameWindow( IDI_SENTINEL_TEST, IDI_SMALL, IDC_SENTINEL_TEST );
		mWindow1 = new GameWindow( IDI_SENTINEL_TEST, IDI_SMALL, IDC_SENTINEL_TEST );

		//LoadString( hInstance, IDS_APP_TITLE, mTitle, GameWindow::MAX_LOADSTRING );
		//LoadString( hInstance, IDC_SENTINEL_TEST, mWindowClass, GameWindow::MAX_LOADSTRING );
		
		mAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_SENTINEL_TEST ));

		// Load config file to setup windows for the renderer.
		//
		WindowInfo info;
		if( !Renderer::Load( "config.xml", info ))
		{
			REPORT_ERROR( "Failed to load 'config.xml'\nDefaulting to OpenGL", "Renderer Setup Failure" );

			if( !Renderer::Inst( BuildRendererGL() ))
				throw AppException( "Failed to BuildRendererGL" );
		}

		// Prepare main window.
		//
		mWindow0->Startup( hInstance, nCmdShow, "Sentinel_Test", "SentinelClass0", info );

		Renderer::Inst()->CreateDepthStencil( info.Width(), info.Height() );
		Renderer::Inst()->CreateViewport( info.Width(), info.Height() );
		Renderer::Inst()->CreateBackbuffer();

		// Prepare second window for testing.
		//
		mWindow1->Startup( hInstance, nCmdShow, "Sentinel_Dup",  "SentinelClass1", info );

		Renderer::Inst()->CreateDepthStencil( info.Width(), info.Height() );
		Renderer::Inst()->CreateViewport( info.Width(), info.Height() );
		Renderer::Inst()->CreateBackbuffer();

		////////////////////////////////////

		SetFocus( mWindow0->GetHandle() );

		Mouse::Inst()->SetPosition( CenterHandle( mWindow0->GetHandle() ));
		ShowCursor( FALSE );

		mWindow0->SetActive();
		mWindow0->ShareResources( mWindow1 );
		
		////////////////////////////////////

		PhysicsSystem::Inst()->Startup();

		PrepareShaders();
		PrepareObjects();
		PrepareFont();

		ParticleSystem::Inst()->Startup( mShader[ SHADER_SPRITE ], 100 );

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

		mWindow0->SetActive();

		Renderer::Inst()->SetDepthStencil( 0 );
		Renderer::Inst()->SetViewport( 0 );
		Renderer::Inst()->SetRenderTarget( 0 );

		Renderer::Inst()->Clear( color );

		GameWorld::Inst()->Update();

		Renderer::Inst()->Present();

		/////////////////////////////////
		/*
		mWindow1->SetActive();

		Renderer::Inst()->SetDepthStencil( 1 );
		Renderer::Inst()->SetViewport( 1 );
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

		mShader[ SHADER_COLOR ] = Renderer::Inst()->CreateShader( "colnorm", "PN", "PpVML" );
		if( !mShader[ SHADER_COLOR ] )
			throw AppException( "Failed to load 'colnorm' shader." );

		mShader[ SHADER_TEXTURE ] = Renderer::Inst()->CreateShader( "texture", "PXN", "PpXVML" );
		if( !mShader[ SHADER_TEXTURE ] )
			throw AppException( "Failed to load 'texture' shader." );
		
		mShader[ SHADER_NORMAL_MAP ] = Renderer::Inst()->CreateShader( "normmap", "PXNT", "PpXXVML" );
		if( !mShader[ SHADER_NORMAL_MAP ] )
			throw AppException( "Failed to load 'normmap' shader." );

		mShader[ SHADER_SPRITE ] = Renderer::Inst()->CreateShader( "spriteGO", "XCx", "PXx" );
		if( !mShader[ SHADER_SPRITE ] )
			throw AppException( "Failed to load 'spriteGO' shader." );

		Model::SHADER_COLOR			= mShader[ SHADER_COLOR ];
		Model::SHADER_TEXTURE		= mShader[ SHADER_TEXTURE ];
		Model::SHADER_NORMAL_MAP	= mShader[ SHADER_NORMAL_MAP ];

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

		GameWorld::Load( "NoMap.MAP" );
		
		// Create main perspective camera.
		//
		info = mWindow0->GetInfo();
		camera = new PerspectiveCameraComponent( (float)info->Width(), (float)info->Height() );
		
		transform = new TransformComponent();
		transform->mPosition = Vector3f( 0, 10, 50 );
		
		controller = new PlayerControllerComponent();
		
		physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateSphere( transform->mPosition, transform->mOrientation, 1, 1 ));
		physics->GetRigidBody()->setGravity( btVector3( 0, 0, 0 ));
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
		mTexture = Renderer::Inst()->CreateTextureFromFile( "default-alpha.png" );

		MeshBuilder				meshBuilder;
		std::shared_ptr< Mesh >	mesh[ NUM_SHAPES ];

		meshBuilder.mShader = mShader[ SHADER_TEXTURE ];
		meshBuilder.mTexture[ TEXTURE_DIFFUSE ] = mTexture;

		meshBuilder.CreateCube( 1 );
		mesh[ SHAPE_CUBE ] = std::shared_ptr< Mesh >( meshBuilder.BuildMesh() );

		meshBuilder.ClearGeometry();
		meshBuilder.CreateCylinder( 1, 1, 10 );
		mesh[ SHAPE_CYLINDER ] = std::shared_ptr< Mesh >( meshBuilder.BuildMesh() );

		meshBuilder.ClearGeometry();
		meshBuilder.CreateOctahedron( 1 );
		mesh[ SHAPE_OCTAHEDRON ] = std::shared_ptr< Mesh >( meshBuilder.BuildMesh() );

		meshBuilder.ClearGeometry();
		meshBuilder.CreateTetrahedron( 1 );
		mesh[ SHAPE_TETRAHEDRON ] = std::shared_ptr< Mesh >( meshBuilder.BuildMesh() );

		meshBuilder.ClearGeometry();
		meshBuilder.CreateDodecahedron( 1 );
		mesh[ SHAPE_DODECAHEDRON ] = std::shared_ptr< Mesh >( meshBuilder.BuildMesh() );

		meshBuilder.ClearGeometry();
		meshBuilder.CreateSphere( 1, 10, 10 );
		mesh[ SHAPE_SPHERE ] = std::shared_ptr< Mesh >( meshBuilder.BuildMesh() );

		meshBuilder.ClearGeometry();
		meshBuilder.CreateWireSphere( 1, 10, 10 );
		meshBuilder.mShader = mShader[ SHADER_COLOR ];
		mesh[ SHAPE_WIRE_SPHERE ] = std::shared_ptr< Mesh >( meshBuilder.BuildMesh() );

		std::shared_ptr< Model > model = std::shared_ptr< Model >( Model::Load( "Player.M3D" ));

		if( !model )
			throw AppException( "Player.M3D failed to load." );

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
		obj->AttachComponent( new MeshComponent( mesh[ SHAPE_CUBE ] ), "Mesh" );

		////////////////////////////////
		
		transform = new TransformComponent();
		transform->mPosition	= Vector3f( -9.9f, 0, 0 );
		transform->mOrientation = Quatf( 0, 0, 1, -15 ).AxisAngle();
		transform->mScale		= Vector3f( 10, 1, 25 );
		
		physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateBox( transform->mPosition, transform->mOrientation, transform->mScale, 0 ));

		obj = GameWorld::Inst()->AddGameObject( new GameObject(), "Origin" );
		obj->AttachComponent( transform,	"Transform" );
		obj->AttachComponent( physics,		"Physics" );
		obj->AttachComponent( new MeshComponent( mesh[ SHAPE_CUBE ] ), "Mesh" );
		
		////////////////////////////////
		
		// Create some random objects from the mesh list.
		//
		char name[32];
		int  count = 50;
		for( int x = 0; x < count; ++x )
		{
			Vector3f scale = Vector3f( RandomValue( 0.25f, 0.5f ), RandomValue( 0.25f, 0.5f ), RandomValue( 0.25f, 0.5f ));
			float mass  = scale.Length();
			UINT  shape = RandomValue( 0, NUM_SHAPES + 1 );

			transform = new TransformComponent();
			transform->mPosition = Vector3f( RandomValue( -20.0f, 20.0f ), 50.0f, RandomValue( -20.0f, 20.0f ));
			transform->mScale    = scale;
				
			if( shape == SHAPE_CUBE )
			{
				physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateBox( transform->mPosition, Quatf( 0, 0, 0, 1 ), transform->mScale, mass ));
			}
			else
			if( shape == SHAPE_CYLINDER )
			{
				transform->mScale.z = transform->mScale.x;
				transform->mScale.y = transform->mScale.y * 5;
				physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateCylinder( transform->mPosition, Quatf( 0, 0, 0, 1 ), transform->mScale, mass ));
			}
			else
			if( shape == SHAPE_SPHERE || shape == SHAPE_WIRE_SPHERE )
			{
				transform->mScale = Vector3f( mass, mass, mass );
				physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateSphere( transform->mPosition, Quatf( 0, 0, 0, 1 ), mass, mass ));
			}
			else
			{
				if( shape < NUM_SHAPES )
					physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateMesh( transform->mPosition, Quatf( 0, 0, 0, 1 ), transform->mScale, mesh[ shape ].get(), mass ));
				else
				{
					// Create a sphere for models...for now.
					//
					transform->mScale = Vector3f( mass, mass, mass ) * 0.025f;
					physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateSphere( transform->mPosition, Quatf( 0, 0, 0, 1 ), mass * 0.6f, mass ));
				}
			}

			sprintf_s( name, "Object %d", x );

			obj = GameWorld::Inst()->AddGameObject( new GameObject(), name );
			obj->AttachComponent( transform,	"Transform" );
			obj->AttachComponent( physics,		"Physics" );

			if( shape < NUM_SHAPES )
				obj->AttachComponent( new MeshComponent( mesh[ shape ] ),	"Mesh" );
			else
				obj->AttachComponent( new ModelComponent( model ),			"Model" );
		}

		//////////////////////////////

		for( UINT x = 0; x < NUM_SHAPES; ++x )
			mesh[ x ].reset();
		
		model.reset();

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
		SAFE_DELETE( mTexture );

		for( UINT x = 0; x < NUM_SHADERS; ++x )
		{
			if( mShader[ x ] )
			{
				mShader[ x ]->Release();
				delete mShader[ x ];
			}
			mShader[ x ] = NULL;
		}

		Mouse::Destroy();
		Keyboard::Destroy();
		Timing::Destroy();
		PhysicsSystem::Destroy();
		ParticleSystem::Destroy();
		GameWorld::Destroy();

		mWindow0->Shutdown();
		delete mWindow0;
		
		mWindow1->Shutdown();
		delete mWindow1;

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
	//_CrtSetBreakAlloc( 246 );

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
