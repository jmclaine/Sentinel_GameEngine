/*
Edits levels for a game created with the Sentinel Engine.
*/
#include "stdafx.h"

#include <crtdbg.h>
#include <exception>
#include <time.h>

#include "Sentinel_Editor.h"

#include "Renderer.h"
#include "PhysicsSystem.h"
#include "ParticleSystem.h"
#include "SpriteSystem.h"
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
#include "Sprite.h"

#include "GUI/Button.h"

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
	SpriteSystem*			mSpriteSystem;

	TextureManager*			mTextureManager;
	ShaderManager*			mShaderManager;
	MeshManager*			mMeshManager;
	ModelManager*			mModelManager;
	SoundManager*			mSoundManager;
	
	GameWorld*				mGameWorld;

	std::shared_ptr< Texture >  mRTBackbufferTexture;
	Mesh*						mRTBackbufferMesh;

public:

	MainApp()
	{
		srand( (UINT)time( (time_t*)0 ));

		mGameWindow		= NULL;

		mRenderer		= NULL;
		mTiming			= NULL;
		mPhysicsSystem	= NULL;

		mSpriteSystem   = NULL;
		
		mAudioSystem    = NULL;

		mTextureManager	= NULL;
		mShaderManager	= NULL;
		mMeshManager	= NULL;
		mModelManager	= NULL;
		mSoundManager   = NULL;

		mGameWorld		= NULL;

		mRTBackbufferMesh = NULL;
	}

	~MainApp()
	{}

	void Startup( HINSTANCE hInstance, int nCmdShow )
	{
		mGameWindow = new GameWindow( IDI_SENTINEL_EDITOR, IDI_SMALL, 0 );
		
		mAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_SENTINEL_EDITOR ));

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
		mGameWindow->Startup( mRenderer, hInstance, nCmdShow, "Sentinel Editor", "SentinelClass", info );
		//mGameWindow->AddChild( new GUI::Button() );

		mRenderer->CreateDepthStencil( info.Width(), info.Height() );
		mRenderer->CreateBackbuffer();

		////////////////////////////////////
		// Prepare GameWorld.
		//
		mGameWorld = new GameWorld();

		SetDirectory( "Assets" );

		PrepareGameWorld();
		
		SetDirectory( ".." );

		mGameWorld->Startup();
	}

	void Update()
	{
		// Enter main game loop.
		//
		MSG msg;
		for(;;)
		{
			// Translate windows messages.
			//
			if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ))
			{
				if( msg.message == WM_QUIT )
					return;

				if( !TranslateAccelerator( msg.hwnd, mAccelTable, &msg ))
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
			}
			// Update game.
			//
			else
			{
				static CameraComponent* camera;

				mTiming->Update();

				Keyboard::Get().ProcessMessages();

				BEGIN_PROFILE( mTiming );
				if( Keyboard::Get().DidGoDown( VK_ESCAPE ))
				{
					return;
				}

				static float color[] = {0.0f, 0.2f, 0.8f, 1.0f};

				static RECT rect;
				GetClientRect( (HWND)mGameWindow->GetInfo()->Handle(), &rect );

				mRenderer->SetViewport( 0, 0, rect.right, rect.bottom );
				mRenderer->SetDepthStencil( 0 );
				mRenderer->SetRenderTarget( 0 );

				mRenderer->Clear( color );

				camera = mGameWorld->GetCamera( 0 );
				((PerspectiveCameraComponent*)camera)->Set( (float)rect.right, (float)rect.bottom );
				mGameWorld->SetCamera( camera );
				
				BEGIN_PROFILE( mTiming );
				mGameWorld->UpdateController();
				mGameWorld->UpdatePhysics();
				mGameWorld->UpdateTransform();
				mGameWorld->UpdateComponents();
				mGameWorld->UpdateDrawable();
				END_PROFILE( mTiming, "World" );

				camera = mGameWorld->GetCamera( 1 );
				((OrthographicCameraComponent*)camera)->Set( (float)rect.right, (float)rect.bottom );
				mGameWorld->SetCamera( camera );

				mGameWindow->Update();

				//mSpriteSystem->Present();
				
				BEGIN_PROFILE( mTiming );
				mRenderer->Present();
				END_PROFILE( mTiming, "Renderer" );

				Mouse::Get().Update();
				Keyboard::Get().Update();
				END_PROFILE( mTiming, "Update" );

				SEPARATE_PROFILE( "-----------------------" );

				mTiming->Limit();
			}
		}
	}

	void Shutdown()
	{
		SaveMAP( "Default.MAP" );

		SAFE_DELETE( mRTBackbufferMesh );

		if( mGameWorld )
		{
			mGameWorld->Shutdown();
			delete mGameWorld;
		}

		SAFE_DELETE( mTiming );
		SAFE_DELETE( mPhysicsSystem );
		SAFE_DELETE( mAudioSystem );
		SAFE_DELETE( mSpriteSystem );
		
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

	void SaveMAP( const char* filename )
	{
		Archive archive;
		if( !archive.Open( filename, "wb+" ))
		{
			REPORT_ERROR( "Failed to save " << filename, "File IO Error" );
			return;
		}

		mTextureManager->Save( archive, mRenderer );
		mShaderManager->Save( archive );
		mMeshManager->Save( archive, mRenderer, mShaderManager, mTextureManager );
		mModelManager->Save( archive, mRenderer, mShaderManager, mTextureManager );
		mSoundManager->Save( archive );
		
		mGameWorld->Save( archive );

		archive.Close();
	}

	void PrepareGameWorld()
	{
		MeshBuilder						meshBuilder;
		std::shared_ptr< Mesh >			mesh;

		GameObject*						obj;
		TransformComponent*				transform;
		PhysicsComponent*				physics;
		RigidBody*						body;

		mTiming							= new Timing();
		mPhysicsSystem					= BuildPhysicsSystemSE();
		mAudioSystem					= BuildAudioSystemAL();

		mTextureManager					= new TextureManager();
		mShaderManager					= new ShaderManager();
		mMeshManager					= new MeshManager();
		mModelManager					= new ModelManager();
		mSoundManager					= new SoundManager();

		////////////////////////////////////

		mPhysicsSystem->Startup();

		mGameWorld->mRenderer			= mRenderer;
		mGameWorld->mTiming				= mTiming;
		mGameWorld->mPhysicsSystem		= mPhysicsSystem;

		mGameWorld->mTextureManager		= mTextureManager;
		mGameWorld->mShaderManager		= mShaderManager;
		mGameWorld->mMeshManager		= mMeshManager;
		mGameWorld->mModelManager		= mModelManager;
		mGameWorld->mSoundManager		= mSoundManager;

		////////////////////////////////////

		SetDirectory( "Shaders" );

		if( !mShaderManager->LoadConfig( "config.xml", mRenderer, mShaderManager ))
		{
			REPORT_ERROR( "Failed to load 'Assets\\Shaders\\config.xml'", "Shader Load Failure" );
			throw AppException( "Failed to load 'Assets\\Shaders\\config.xml'" );
		}
		/*	
		std::vector< std::string >					names;
		std::vector< std::shared_ptr< Shader >>     shaders;

		mShaderManager->GetAll( names, shaders );

		for( int x = 0; x < (int)names.size(); ++x )
		{
			mAssetShader->addChild( new AssetTreeItem< Shader >( names[ x ].c_str(), shaders[ x ] ));
		}
		*/
		SetDirectory( ".." );

		////////////////////////////////////

		std::shared_ptr< Texture > texture = mTextureManager->Add( "default-alpha.png", mRenderer->CreateTextureFromFile( "Textures\\default-alpha.png" ));
		//AssetTreeItem< Texture >* asset = new AssetTreeItem< Texture >( "default-alpha.png", texture );
		//mAssetTexture->addChild( asset );

		// Main Camera.
		//
		obj = mGameWorld->AddGameObject( new GameObject(), "Main Camera" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition    = Vector3f( 0, 25, 25 );
		transform->mOrientation = Quatf( -45, 0, 0 );

		obj->AttachComponent( new PlayerControllerComponent(), "Controller" );
			
		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mPhysicsSystem->CreateSphere( transform->mPosition, transform->mOrientation, 1.0f, 1.0f ));
		body = physics->GetRigidBody();
		body->SetShapeType( PHYSICS_SPHERE );
		body->SetFlags( DISABLE_GRAVITY );
		body->SetRestitution( 1.0f );
		body->SetDamping( 0.9f, 0.9f );
			
		obj->AttachComponent( new PerspectiveCameraComponent( (float)mGameWindow->GetInfo()->Width(), (float)mGameWindow->GetInfo()->Height() ), "PCamera" );

		//AddObject( obj );

		// GUI Camera.
		//
		obj = mGameWorld->AddGameObject( new GameObject(), "GUI Camera" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( 0, 0, 0 );
	
		obj->AttachComponent( new OrthographicCameraComponent( (float)mGameWindow->GetInfo()->Width(), (float)mGameWindow->GetInfo()->Height() ), "OCamera" );

		//AddObject( obj );
			
		// Point Light.
		//
		obj = mGameWorld->AddGameObject( new GameObject(), "Point_Light" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( 0, 10, 0 );

		LightComponent* light = (LightComponent*)obj->AttachComponent( new LightComponent(), "Light" );
		light->mAttenuation  = Vector4f( 1, 1, 1, 25 );

		//AddObject( obj );
			
		// Ground object.
		//
		meshBuilder.CreateCube( 1.0f );
		meshBuilder.mShader    = mShaderManager->Get( "Color" );
		meshBuilder.mPrimitive = TRIANGLE_LIST;

		mesh = mMeshManager->Add( "Ground", std::shared_ptr< Mesh >(meshBuilder.BuildMesh( mRenderer )));
		//mAssetMesh->addChild( new AssetTreeItem< Mesh >( "Ground", mesh ));
	
		obj = mGameWorld->AddGameObject( new GameObject(), "Ground" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( 0, 0, 0 );
		transform->mScale    = Vector3f( 100, 1, 100 );

		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mPhysicsSystem->CreateBox( transform->mPosition, transform->mOrientation, transform->mScale, 0.0f ));
		body = physics->GetRigidBody();
		body->SetShapeType( PHYSICS_BOX );
		body->SetFlags( DISABLE_GRAVITY );
			
		obj->AttachComponent( new MeshComponent( mesh ), "Mesh" );
			
		//AddObject( obj );
			
		// Test object.
		//
		/*
		meshBuilder.ClearGeometry();
		meshBuilder.CreateDodecahedron( 1.0f );
		meshBuilder.Shader = mShaderManager.Get( "Texture" );
		meshBuilder.Texture( (int)TextureType.DIFFUSE ).Set( mTextureManager.Get( "default-alpha.png" ));

		mesh = mMeshManager.Add( "Dodecahedron", meshBuilder.BuildMesh( mRenderer ));
		AddAsset( "Dodecahedron", mesh );
			
		WGameObject obj2 = new WGameObject();
		obj2.Name = "Dodecahedron";

		transform = (WTransformComponent)obj2.AttachComponent( new WTransformComponent(), "Transform" );
		transform.Position	= new WVector3f( 0, 4, 0 );
		transform.Scale		= new WVector3f( 0.5f, 0.5f, 0.5f );
		transform.Rotation	= new WVector3f( 90, 180, 270 );

		obj2.AttachComponent( new WMeshComponent( mesh ), "Mesh" );
			
		// Parent test object.
		//
		meshBuilder.ClearGeometry();
		meshBuilder.CreateSphere( 1.0f, 10, 10 );
			
		mesh = mMeshManager.Add( "Sphere", meshBuilder.BuildMesh( mRenderer ));
		AddAsset( "Sphere", mesh );

		obj = mGameWorld.AddGameObject( new WGameObject(), "Sphere" );

		transform = (WTransformComponent)obj.AttachComponent( new WTransformComponent(), "Transform" );
		transform.Position	= new WVector3f( -10, 4, 0 );
		transform.Scale		= new WVector3f( 2, 2, 2 );
		transform.Rotation	= new WVector3f( 45, 45, 45 );

		obj.AttachComponent( new WMeshComponent( mesh ), "Mesh" );

		obj.AddChild( obj2 );

		AddObject( obj );
			
		///////////////////////////////
			
		mGameWorld.Startup();
		*/
		///////////////////////////////
		// CREATE EDITOR OBJECTS

		CreateTranslateMesh();

		CreateRotateMesh();
			
		CreateScaleMesh();
	}
		
	void CreateTranslateMesh()
	{
		/*mTranslateObject = new WGameObject();
		const float tileSize = 5;

		WModel model = WModel.Load( "Assets\\Editor\\Translate.M3D", mRenderer, mShaderManager, mTextureManager );
			
		// Root Translate Object.
		//
		mTranslateObject.AttachComponent( new WTransformComponent(), "Transform" );
			
		///////////////////////////////

		WGameObject obj = new WGameObject();
			
		WTransformComponent transform = new WTransformComponent();
		transform.Position = new WVector3f( tileSize, 0, 0 );
		transform.Rotation = new WVector3f( 0, 0, -90 );
		obj.AttachComponent( transform, "Tile_X" );

		WModelComponent modelComp = new WModelComponent( model );
		modelComp.SetMaterial( mMaterial_X_Axis );
		obj.AttachComponent( modelComp, "Model_X" );

		mTranslateObject.AddChild( obj );

		///////////////////////////////
			
		obj = new WGameObject();

		transform = new WTransformComponent();
		transform.Position = new WVector3f( 0, tileSize, 0 );
		transform.Rotation = new WVector3f( 0, 0, 0 );
		obj.AttachComponent( transform, "Tile_Y" );

		modelComp = new WModelComponent( model );
		modelComp.SetMaterial( mMaterial_Y_Axis );
		obj.AttachComponent( modelComp, "Model_Y" );

		mTranslateObject.AddChild( obj );

		///////////////////////////////

		obj = new WGameObject();

		transform = new WTransformComponent();
		transform.Position = new WVector3f( 0, 0, tileSize );
		transform.Rotation = new WVector3f( 90, 0, 0 );
		obj.AttachComponent( transform, "Tile_Z" );

		modelComp = new WModelComponent( model );
		modelComp.SetMaterial( mMaterial_Z_Axis );
		obj.AttachComponent( modelComp, "Model_Z" );

		mTranslateObject.AddChild( obj );

		///////////////////////////////

		mTranslateObject.SetWorld( mGameWorld );
		mTranslateObject.Startup();

		model.Dispose();
		*/
	}

	void CreateRotateMesh()
	{
		/*
		mRotateObject = new WGameObject();

		mRotateObject.SetWorld( mGameWorld );
		mRotateObject.Startup();
		*/
	}

	void CreateScaleMesh()
	{
		/*
		mScaleObject = new WGameObject();

		mScaleObject.SetWorld( mGameWorld );
		mScaleObject.Startup();
		*/
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
		mainApp->Startup( hInstance, nCmdShow );
		mainApp->Update();
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
