// Test a component based engine system.
//
#include "stdafx.h"

#include <crtdbg.h>
#include <exception>
#include <time.h>

#include "Sentinel_Test.h"

#include "Renderer.h"
#include "PhysicsSystem.h"
#include "NetworkSocket.h"

#include "Input.h"
#include "Timing.h"
#include "Util.h"

#include "MeshBuilder.h"
#include "Model.h"
#include "Sprite.h"

#include "GameWorld.h"
#include "GameObject.h"

#include "PerspectiveCameraComponent.h"
#include "PlayerControllerComponent.h"
#include "PhysicsComponent.h"
#include "MeshComponent.h"
#include "ModelComponent.h"

using namespace Sentinel;


// Visual Studio generated.
//
#define MAX_LOADSTRING 100

HINSTANCE hInst;								// current instance
TCHAR szTitle[ MAX_LOADSTRING ];				// title bar text
TCHAR szWindowClass[ MAX_LOADSTRING ];			// main window class name

ATOM				MyRegisterClass(HINSTANCE hInstance);
HWND				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


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
	HACCEL			mAccelTable;
	HWND			mHWND;

	Shader*			mShader[ NUM_SHADERS ];
	Texture*		mTexture;

public:

	MainApp( HINSTANCE hInstance, int nCmdShow )
	{
		srand( (UINT)time( (time_t*)0 ));

		for( UINT x = 0; x < NUM_SHADERS; ++x )
			mShader[ x ] = NULL;

		//////////////////////////////////////////

		LoadString( hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING );
		LoadString( hInstance, IDC_SENTINEL_TEST, szWindowClass, MAX_LOADSTRING );
		MyRegisterClass( hInstance );

		mAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_SENTINEL_TEST ));

		if( !CreateRenderer( "config.xml" ))
			REPORT_ERROR( "Failed to load 'config.xml'\nDefaulting to OpenGL", "Renderer Setup Failure" );

		mHWND = InitInstance( hInstance, nCmdShow );
		TV( mHWND );

		SetFocus( mHWND );

		Mouse::mHWND = mHWND;
		Mouse::Inst()->SetPosition( CenterHandle( Mouse::mHWND ));
		ShowCursor( FALSE );
	}

	~MainApp()
	{}

	int Run()
	{
		if( Renderer::Inst()->Startup( mHWND, Renderer::FULLSCREEN, Renderer::SCREEN_WIDTH, Renderer::SCREEN_HEIGHT ) != S_OK )
			throw AppException( "Failed Renderer::Startup()" );

		PhysicsSystem::Inst()->Startup();

		PrepareShaders();
		PrepareObjects();
		PrepareFont();

		Sprite::Inst()->Startup( mShader[ SHADER_SPRITE ], 100 );

		GameWorld::Inst()->Startup( "NoMap.MAP" );

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
		Renderer::Inst()->Clear( color );
		Renderer::Inst()->SetRenderTarget( 0 );
		Renderer::Inst()->SetDepthStencil( 0 );
		Renderer::Inst()->SetViewport( 0 );

		GameWorld::Inst()->Update();

		Renderer::Inst()->Present();

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
		
		TransformComponent*		transform;
		ControllerComponent*	controller;
		PhysicsComponent*		physics;
		CameraComponent*		camera;
		LightComponent*			light;
		GameObject*				obj;

		//////////////////////////////

		// Create perspective camera.
		//
		camera = new PerspectiveCameraComponent( mHWND );
		
		transform = new TransformComponent();
		transform->mPosition = vec3f( 0, 0, 50 );
		
		controller = new PlayerControllerComponent();
		
		physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateSphere( transform->mPosition, transform->mOrientation, 1, 1 ));
		physics->mRigidBody->setGravity( btVector3( 0, 0, 0 ));
		physics->mRigidBody->setDamping( 0.9f, 0.9f );
		physics->mRigidBody->setRestitution( 1 );
		physics->mRigidBody->setAngularFactor( 0 );

		obj = GameWorld::Inst()->AddGameObject( new GameObject(), "MainCamera" );
		obj->AttachComponent( transform,	"Transform" );
		obj->AttachComponent( controller,	"Player" );
		obj->AttachComponent( physics,		"Physics" );
		obj->AttachComponent( camera,		"Camera" );

		//////////////////////////////
		
		// Create a point light.
		//
		transform = new TransformComponent();
		transform->mPosition = vec3f( 0, 2000, 0 );

		light = new LightComponent();
		light->mAttenuation = vec4f( 1, 1, 1, 2000 );
		light->mColor = ColorRGBA( 1, 1, 1, 1 );

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
		transform->mPosition	= vec3f( 9.9f, 0, 0 );
		transform->mOrientation = quatf( 0, 0, 1, 15 ).AxisAngle();
		transform->mScale		= vec3f( 10, 1, 25 );
		
		physics   = new PhysicsComponent( PhysicsSystem::Inst()->CreateBox( transform->mPosition, transform->mOrientation, transform->mScale, 0 ));

		obj = GameWorld::Inst()->AddGameObject( new GameObject(), "Origin" );
		obj->AttachComponent( transform,	"Transform" );
		obj->AttachComponent( physics,		"Physics" );
		obj->AttachComponent( new MeshComponent( mesh[ SHAPE_CUBE ] ), "Mesh" );

		////////////////////////////////
		
		transform = new TransformComponent();
		transform->mPosition	= vec3f( -9.9f, 0, 0 );
		transform->mOrientation = quatf( 0, 0, 1, -15 ).AxisAngle();
		transform->mScale		= vec3f( 10, 1, 25 );
		
		physics   = new PhysicsComponent( PhysicsSystem::Inst()->CreateBox( transform->mPosition, transform->mOrientation, transform->mScale, 0 ));

		obj = GameWorld::Inst()->AddGameObject( new GameObject(), "Origin" );
		obj->AttachComponent( transform,	"Transform" );
		obj->AttachComponent( physics,		"Physics" );
		obj->AttachComponent( new MeshComponent( mesh[ SHAPE_CUBE ] ), "Mesh" );
		
		////////////////////////////////
		
		// Create some random objects from the mesh list.
		//
		char name[32];
		int  count = 25;
		for( int x = 0; x < count; ++x )
		{
			vec3f scale = vec3f( RandomValue( 0.25f, 1.0f ), RandomValue( 0.25f, 1.0f ), RandomValue( 0.25f, 1.0f ));
			float mass  = scale.Length();
			UINT  shape = RandomValue( 0, NUM_SHAPES + 1 );

			transform = new TransformComponent();
			transform->mPosition = vec3f( RandomValue( -20.0f, 20.0f ), 50.0f, RandomValue( -20.0f, 20.0f ));
			transform->mScale    = scale;
				
			if( shape == SHAPE_CUBE )
			{
				physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateBox( transform->mPosition, quatf( 0, 0, 0, 1 ), transform->mScale, mass ));
			}
			else
			if( shape == SHAPE_CYLINDER )
			{
				transform->mScale.z = transform->mScale.x;
				transform->mScale.y *= 5;
				physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateCylinder( transform->mPosition, quatf( 0, 0, 0, 1 ), transform->mScale, mass ));
			}
			else
			if( shape == SHAPE_SPHERE || shape == SHAPE_WIRE_SPHERE )
			{
				transform->mScale = vec3f( mass, mass, mass );
				physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateSphere( transform->mPosition, quatf( 0, 0, 0, 1 ), mass, mass ));
			}
			else
			{
				if( shape < NUM_SHAPES )
					physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateMesh( transform->mPosition, quatf( 0, 0, 0, 1 ), transform->mScale, mesh[ shape ].get(), mass ));
				else
				{
					// Create a sphere for models...for now.
					//
					transform->mScale = vec3f( mass, mass, mass ) * 0.025f;
					physics = new PhysicsComponent( PhysicsSystem::Inst()->CreateSphere( transform->mPosition, quatf( 0, 0, 0, 1 ), mass * 0.6f, mass ));
				}
			}

			sprintf_s( name, "Object %d", x );

			obj = GameWorld::Inst()->AddGameObject( new GameObject(), name );
			obj->AttachComponent( transform,	"Transform" );

			if( physics )
				obj->AttachComponent( physics,	"Physics" );

			if( shape < NUM_SHAPES )
				obj->AttachComponent( new MeshComponent( mesh[ shape ] ),	"Mesh" );
			else
				obj->AttachComponent( new ModelComponent( model ),			"Model" );
		}

		//////////////////////////////

		for( UINT x = 0; x < NUM_SHAPES; ++x )
			mesh[ x ].reset();

		model.reset();

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
		for( UINT x = 0; x < NUM_SHADERS; ++x )
		{
			if( mShader[ x ] )
			{
				mShader[ x ]->Release();
				delete mShader[ x ];
			}
			mShader[ x ] = NULL;
		}

		PhysicsSystem::Inst()->Shutdown();

		Sprite::Inst()->Shutdown();

		GameWorld::Inst()->Shutdown();

		Renderer::Inst()->Shutdown();
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////

int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	// Check for memory leaks.
	//
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
	//_CrtSetBreakAlloc( 1073 );

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	int result = 0;
	MainApp* mainApp = 0;

	try
	{
		mainApp = new MainApp( hInstance, nCmdShow );
		result  = mainApp->Run();
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

	mainApp->Shutdown();
	SAFE_DELETE( mainApp );
	
	return result;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SENTINEL_TEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SENTINEL_TEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND InitInstance( HINSTANCE hInstance, int nCmdShow )
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow( szWindowClass, "Sentinel", (!Renderer::FULLSCREEN) ? WS_OVERLAPPEDWINDOW : WS_POPUP,
						0, 0, Renderer::SCREEN_WIDTH, Renderer::SCREEN_HEIGHT,
						NULL, NULL, hInstance, NULL );

   if( !hWnd )
      return NULL;
   
   ShowWindow( hWnd, nCmdShow );
   UpdateWindow( hWnd );

   return hWnd;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Mouse::Inst()->ProcessMessages( message );
	Keyboard::Inst()->ProcessMessages();

	switch( message )
	{
		case WM_COMMAND:
			return DefWindowProc( hWnd, message, wParam, lParam );
			
		case WM_PAINT:
			{
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			}
			break;

		case WM_DESTROY:
			PostQuitMessage( 0 );
			break;
	}

	return DefWindowProc( hWnd, message, wParam, lParam );
}
