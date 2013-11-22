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
#include "SpriteSystem.h"
#include "ParticleSystem.h"
#include "SpriteSystem.h"
#include "NetworkSocket.h"
#include "AudioSystem.h"

#include "GameWindow.h"

#include "Archive.h"

#include "TextureManager.h"
#include "ShaderManager.h"
#include "SpriteManager.h"
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
#include "ParticleEmitterComponent.h"
#include "SpriteComponent.h"
#include "WidgetComponent.h"
#include "MeshComponent.h"
#include "ModelComponent.h"

#include "Sound.h"
#include "Sprite.h"

#include "GUI/Widget.h"
#include "GUI/Button.h"
#include "GUI/Drawable.h"

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
	SpriteManager*			mSpriteManager;
	MeshManager*			mMeshManager;
	ModelManager*			mModelManager;
	SoundManager*			mSoundManager;
	
	GameWorld*				mGameWorld;
	GameWorld*				mEditorWorld;

	Mesh*					mRTBackbufferMesh;

	struct FuncTest
	{
		int value;

		void output()
		{
			TRACE( "Func: " << value );
		}
	};
	FuncTest test0;
	FuncTest test1;

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
		mSpriteManager	= NULL;
		mMeshManager	= NULL;
		mModelManager	= NULL;
		mSoundManager   = NULL;

		mGameWorld		= NULL;

		mRTBackbufferMesh = NULL;

		test0.value = 0;
		test1.value = 1;
		
		REGISTER_SERIAL_FUNCTION( test1.output );
		REGISTER_SERIAL_FUNCTION( test0.output );
	}

	~MainApp()
	{}

	static void Quit()
	{
		PostQuitMessage(0);
	}

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
		
		mRenderer->CreateDepthStencil( Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE );
		mRenderer->CreateBackbuffer();

		////////////////////////////////////
		// Prepare GameWorld.
		//
		SetDirectory( "Assets" );

		PrepareGameWorld();
		
		SetDirectory( ".." );
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

				if( Keyboard::Get().DidGoDown( VK_F9 ))
				{
					SaveMAP( "Default.MAP" );
				}

				static float color[] = {0.0f, 0.2f, 0.8f, 1.0f};

				mGameWindow->Update();

				UINT width  = mGameWindow->GetInfo()->Width();
				UINT height = mGameWindow->GetInfo()->Height();

				mRenderer->SetViewport( 0, 0, Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE );
				mRenderer->SetDepthStencil( 1 );
				mRenderer->SetRenderTarget( 1 );

				mRenderer->Clear( color );

				BEGIN_PROFILE( mTiming );
				mGameWorld->UpdateController();
				mGameWorld->UpdatePhysics();
				mGameWorld->UpdateTransform();
				mGameWorld->UpdateComponents();
				mGameWorld->UpdateDrawable();
				END_PROFILE( mTiming, "World" );

				////////////////////////////////////
				
				mRenderer->SetViewport( 0, 0, width, height );
				mRenderer->SetDepthStencil( 0 );
				mRenderer->SetRenderTarget( 0 );

				mRenderer->Clear( color );

				BEGIN_PROFILE( mTiming );
				mEditorWorld->UpdateController();
				mEditorWorld->UpdatePhysics();
				mEditorWorld->UpdateTransform();
				mEditorWorld->UpdateComponents();
				mEditorWorld->UpdateDrawable();
				END_PROFILE( mTiming, "Editor" );
				
				//mRTBackbufferMesh->mMatrixWorld.Identity();
				//mRTBackbufferMesh->Draw( mRenderer, mGameWorld );

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
		SAFE_DELETE( mRTBackbufferMesh );

		SHUTDOWN_DELETE( mGameWorld );
		SHUTDOWN_DELETE( mEditorWorld );
		
		SAFE_DELETE( mTiming );
		SAFE_DELETE( mPhysicsSystem );
		SAFE_DELETE( mSpriteSystem );
		SAFE_DELETE( mAudioSystem );
		
		SAFE_DELETE( mSoundManager );
		SAFE_DELETE( mModelManager );
		SAFE_DELETE( mSpriteManager );
		SAFE_DELETE( mMeshManager );
		SAFE_DELETE( mShaderManager );
		SAFE_DELETE( mTextureManager );

		SHUTDOWN_DELETE( mGameWindow );
		
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
		mSpriteManager->Save( archive, mShaderManager, mTextureManager );
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
		mSpriteManager					= new SpriteManager();
		mMeshManager					= new MeshManager();
		mModelManager					= new ModelManager();
		mSoundManager					= new SoundManager();

		////////////////////////////////////

		mPhysicsSystem->Startup();

		mGameWorld = new GameWorld();

		mGameWorld->mRenderer			= mRenderer;
		mGameWorld->mTiming				= mTiming;
		mGameWorld->mPhysicsSystem		= mPhysicsSystem;

		mGameWorld->mTextureManager		= mTextureManager;
		mGameWorld->mShaderManager		= mShaderManager;
		mGameWorld->mSpriteManager		= mSpriteManager;
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

		mSpriteSystem = new SpriteSystem( mRenderer, mShaderManager->Get( "GUI" ), 256 );
		mGameWorld->mSpriteSystem		= mSpriteSystem;

		////////////////////////////////////

		// Create backbuffer for added effects.
		//
		mRenderer->CreateDepthStencil( Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE );
		mRenderer->CreateRenderTarget( mTextureManager->Add( "Backbuffer", mRenderer->CreateTexture( Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE )));
		
		mRTBackbufferMesh = mRenderer->CreateRenderTargetMesh( mShaderManager->Get( "RT_Normal" ));
		mRTBackbufferMesh->mTexture[ TEXTURE_DIFFUSE ] = mTextureManager->Get( "Backbuffer" );

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
			
		obj->AttachComponent( new PerspectiveCameraComponent( (float)Renderer::WINDOW_WIDTH_BASE, (float)Renderer::WINDOW_HEIGHT_BASE ), "PCamera" );
		
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
		meshBuilder.ClearGeometry();
		meshBuilder.CreateDodecahedron( 1.0f );
		meshBuilder.mShader = mShaderManager->Get( "Texture" );
		meshBuilder.mTexture[ TEXTURE_DIFFUSE ] = mTextureManager->Get( "default-alpha.png" );

		mesh = mMeshManager->Add( "Dodecahedron", std::shared_ptr< Mesh >(meshBuilder.BuildMesh( mRenderer )));
		//AddAsset( "Dodecahedron", mesh );
			
		GameObject* obj2 = new GameObject();
		obj2->mName = "Dodecahedron";

		transform = (TransformComponent*)obj2->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition	= Vector3f( 0, 4, 0 );
		transform->mScale		= Vector3f( 0.5f, 0.5f, 0.5f );
		transform->mOrientation	= Quatf( 90, 180, 270 );

		obj2->AttachComponent( new MeshComponent( mesh ), "Mesh" );
			
		// Parent test object.
		//
		meshBuilder.ClearGeometry();
		meshBuilder.CreateSphere( 1.0f, 10, 10 );
			
		mesh = mMeshManager->Add( "Sphere", std::shared_ptr< Mesh >(meshBuilder.BuildMesh( mRenderer )));
		//AddAsset( "Sphere", mesh );

		obj = mGameWorld->AddGameObject( new GameObject(), "Sphere" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition	= Vector3f( -10, 4, 0 );
		transform->mScale		= Vector3f( 2, 2, 2 );
		transform->mOrientation	= Quatf( 45, 45, 45 );

		obj->AttachComponent( new MeshComponent( mesh ), "Mesh" );

		obj->AddChild( obj2 );

		//AddObject( obj );

		// Testing particle effects.
		//
		/*std::shared_ptr< Texture > textureS = mTextureManager->Add( "fire.png", mRenderer->CreateTextureFromFile( "Textures\\fire.png" ));
		
		std::shared_ptr< Sprite > sprite( new Sprite( mShaderManager->Get( "Sprite" ), textureS ));
		sprite->AddFrame( sprite->GetTextureCoords( Quad( 0, 0, 64, 64 )));
		sprite->AddFrame( sprite->GetTextureCoords( Quad( 64, 0, 128, 64 )));

		ParticleSystem* particleSystem = BuildParticleSystemNormal( mRenderer, mGameWorld, sprite, 100 );
		particleSystem->mSpawnRate   = 0.025f;
		particleSystem->mMinLifetime = 3.0f;
		particleSystem->mMaxLifetime = 5.0f;
		particleSystem->mEffect.push_back( new TextureEffect( 0, 0 ));
		particleSystem->mEffect.push_back( new AreaPositionEffect( 0, Vector3f( -0.125f, 1, 0 ), Vector3f( 0.125f, 1, 0 )));
		particleSystem->mEffect.push_back( new RandomRotationEffect( 0, Vector3f( 0, 0, -10 ), Vector3f( 0, 0, 10 )));
		particleSystem->mEffect.push_back( new ScaleEffect( 0, Vector3f( 1, 1, 1 )));
		particleSystem->mEffect.push_back( new VelocityEffect( 0, Vector3f( 0, 1.2f, 0 )));
		particleSystem->mEffect.push_back( new RandomColorEffect( 0, ColorRGBA( 0.75f, 0.25f, 0, 0.125 ), ColorRGBA( 0.75f, 0.75f, 0, 0.125 )));
		particleSystem->mEffect.push_back( new FadeToScaleEffect( 0, 0.25f, 0.666f ));
		particleSystem->mEffect.push_back( new FadeToScaleEffect( 0.5f, 1.0f, 0.1f ));
		particleSystem->mEffect.push_back( new TextureEffect( 1.0f, 1 ));
		particleSystem->mEffect.push_back( new FadeToScaleEffect( 1.0f, 2.0f, 1.0f ));
		particleSystem->mEffect.push_back( new RandomColorEffect( 1.0f, ColorRGBA( 1, 1, 1, 0.125 ), ColorRGBA( 0.9f, 0.9f, 0.9f, 0.125 )));
		particleSystem->mEffect.push_back( new FadeToColorEffect( 1.0f, 5.0f, ColorRGBA( 0.9f, 0.9f, 0.9f, 0.025f )));
		//particleSystem->mEffect.push_back( new RandomVelocityEffect( 1, Vector3f( -1, 1, 0 ), Vector3f( 1, 1, 0 )));

		obj = mGameWorld->AddGameObject( new GameObject(), "Particle Emitter" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mScale = Vector3f( 512, 512, 1 );

		obj->AttachComponent( new ParticleEmitterComponent( particleSystem ), "Sprite" );
		*/

		/////////////////////////////////////////////////////////////////////////

		mEditorWorld = new GameWorld();
		mEditorWorld->mRenderer			= mRenderer;
		mEditorWorld->mTiming			= mTiming;
		mEditorWorld->mPhysicsSystem	= mPhysicsSystem;

		mEditorWorld->mTextureManager	= mTextureManager;
		mEditorWorld->mShaderManager	= mShaderManager;
		mEditorWorld->mSpriteManager	= mSpriteManager;
		mEditorWorld->mMeshManager		= mMeshManager;
		mEditorWorld->mModelManager		= mModelManager;
		mEditorWorld->mSoundManager		= mSoundManager;
		mEditorWorld->mSpriteSystem		= mSpriteSystem;

		////////////////////////////////////

		// GUI Camera.
		//
		obj = mEditorWorld->AddGameObject( new GameObject(), "GUI Camera" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( 0, 0, 0 );
	
		OrthographicCameraComponent* orthoCamera = (OrthographicCameraComponent*)obj->AttachComponent( new OrthographicCameraComponent( (float)Renderer::WINDOW_WIDTH_BASE, (float)Renderer::WINDOW_HEIGHT_BASE ), "OCamera" );
		orthoCamera->mScaleToWindow = true;

		//AddObject( obj );
		
		// Create game object for widget.
		//
		std::shared_ptr< Sprite > sprite( new Sprite( mShaderManager->Get( "GUI" ), mTextureManager->Get( "default-alpha.png" )));

		sprite->AddFrame( sprite->GetTextureCoords( Quad( 0, 0, 512, 512 )));
		sprite->AddFrame( sprite->GetTextureCoords( Quad( 0, 0, 64, 64 )));

		mSpriteManager->Add( "GUI Sprite", sprite );

		obj = mEditorWorld->AddGameObject( new GameObject(), "GUI Widget" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );

		WidgetComponent* widgetComp = (WidgetComponent*)obj->AttachComponent( new WidgetComponent( sprite, 0 ), "Widget" );
		
		// Create world drawable.
		//
		GUI::Drawable* drawable = (GUI::Drawable*)widgetComp->mRoot.AddChild( new GUI::Drawable() );
		drawable->mScaleToWindow = true;

		meshBuilder.ClearAll();
		meshBuilder.CreateQuad( 0.5f, Vector3f( 0, 0, -1 ));
		Matrix4f matTrans;
		matTrans.Translate( Vector3f( 0.5f, 0.5f, 0 ));
		meshBuilder.ApplyMatrix( matTrans );
		meshBuilder.mShader = mShaderManager->Get( "GUI Mesh" );
		meshBuilder.mTexture[ TEXTURE_DIFFUSE ] = mTextureManager->Get( "Backbuffer" );

		drawable->mMesh = mMeshManager->Add( "World", std::shared_ptr< Mesh >( meshBuilder.BuildMesh( mRenderer )));
		drawable->mPosition = Vector3f( 0, 30, 0.2f );
		drawable->mScale = Vector3f( (float)Renderer::WINDOW_WIDTH_BASE-500, (float)Renderer::WINDOW_HEIGHT_BASE-30, 1 );

		// Create button.
		//
		GUI::Button* button = (GUI::Button*)widgetComp->mRoot.AddChild( new GUI::Button() );
		button->mPosition = Vector3f( 200, 30, 0.1f );
		button->mScale    = Vector3f( 512, 512, 1 );
		button->mScaleToWindow = false;

		button->mActionClick = CREATE_SERIAL_FUNCTION( test1.output );
		
		///////////////////////////////

		mGameWorld->Startup();
		mEditorWorld->Startup();

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
