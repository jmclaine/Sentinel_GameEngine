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

	GameWorld*				mGameWorld;
	GameWorld*				mEditorWorld;

	Mesh*					mRTBackbufferMesh;

public:

	MainApp() :
		mGameWindow( NULL ),
		mRenderer( NULL ),
		mGameWorld( NULL ),
		mEditorWorld( NULL ),
		mRTBackbufferMesh( NULL )
	{
		srand( (UINT)time( (time_t*)0 ));
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
		
		mRenderer->CreateDepthStencil( Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE );
		mRenderer->CreateBackbuffer();

		////////////////////////////////////
		// Prepare Editor and World View.
		//
		SetDirectory( "Assets" );

		PrepareEditorWorld();
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
				static Timing* timing;

				timing = mEditorWorld->mTiming;

				timing->Update();
				mGameWorld->mTiming->Update();

				Keyboard::Get().ProcessMessages();

				BEGIN_PROFILE( timing );
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

				BEGIN_PROFILE( timing );
				mGameWorld->UpdateTransform();
				mGameWorld->UpdateDrawable();
				END_PROFILE( timing, "World" );

				////////////////////////////////////
				
				mRenderer->SetViewport( 0, 0, width, height );
				mRenderer->SetDepthStencil( 0 );
				mRenderer->SetRenderTarget( 0 );

				mRenderer->Clear( color );

				BEGIN_PROFILE( timing );
				mEditorWorld->UpdateController();
				mEditorWorld->UpdatePhysics();
				mEditorWorld->UpdateTransform();
				mEditorWorld->UpdateComponents();
				mEditorWorld->UpdateDrawable();
				END_PROFILE( timing, "Editor" );
				
				//mRTBackbufferMesh->mMatrixWorld.Identity();
				//mRTBackbufferMesh->Draw( mRenderer, mGameWorld );

				BEGIN_PROFILE( timing );
				mRenderer->Present();
				END_PROFILE( timing, "Renderer" );

				Mouse::Get().Update();
				Keyboard::Get().Update();
				END_PROFILE( timing, "Update" );

				SEPARATE_PROFILE( "-----------------------" );

				timing->Limit();
				mGameWorld->mTiming->Limit(0);
			}
		}
	}

	void Shutdown()
	{
		SAFE_DELETE( mRTBackbufferMesh );

		SHUTDOWN_DELETE( mGameWorld );
		SHUTDOWN_DELETE( mEditorWorld );
		
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

		mGameWorld->mTextureManager->Save( archive, mRenderer );
		mGameWorld->mShaderManager->Save( archive );
		mGameWorld->mSpriteManager->Save( archive, mGameWorld->mShaderManager, mGameWorld->mTextureManager );
		mGameWorld->mMeshManager->Save( archive, mRenderer, mGameWorld->mShaderManager, mGameWorld->mTextureManager );
		mGameWorld->mModelManager->Save( archive, mRenderer, mGameWorld->mShaderManager, mGameWorld->mTextureManager );
		mGameWorld->mSoundManager->Save( archive );
		
		mGameWorld->Save( archive );

		archive.Close();
	}

	void PrepareEditorWorld()
	{
		MeshBuilder						meshBuilder;
		std::shared_ptr< Mesh >			mesh;

		GameObject*						obj;
		TransformComponent*				transform;
		PhysicsComponent*				physics;
		RigidBody*						body;

		/////////////////////////////////////////////////////////////////////////

		mEditorWorld = new GameWorld();
		mEditorWorld->mRenderer			= mRenderer;
		mEditorWorld->mTiming			= new Timing();
		mEditorWorld->mPhysicsSystem	= BuildPhysicsSystemBT();
		mEditorWorld->mAudioSystem		= BuildAudioSystemAL();
		
		mEditorWorld->mTextureManager	= new TextureManager();
		mEditorWorld->mShaderManager	= new ShaderManager();
		mEditorWorld->mSpriteManager	= new SpriteManager();
		mEditorWorld->mMeshManager		= new MeshManager();
		mEditorWorld->mModelManager		= new ModelManager();
		mEditorWorld->mSoundManager		= new SoundManager();

		SetDirectory( "Shaders" );

		if( !ShaderManager::LoadConfig( "config.xml", mRenderer, mEditorWorld->mShaderManager ))
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

		mEditorWorld->mSpriteSystem = new SpriteSystem( mRenderer, mEditorWorld->mShaderManager->Get( "GUI" ), 256 );

		////////////////////////////////////

		// Create backbuffer for world view.
		//
		mRenderer->CreateDepthStencil( Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE );
		mRenderer->CreateRenderTarget( mEditorWorld->mTextureManager->Add( "Backbuffer", mRenderer->CreateTexture( Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE )));
		
		mRTBackbufferMesh = mRenderer->CreateRenderTargetMesh( mEditorWorld->mShaderManager->Get( "RT_Normal" ));
		mRTBackbufferMesh->mTexture[ TEXTURE_DIFFUSE ] = mEditorWorld->mTextureManager->Get( "Backbuffer" );

		// Add GUI texture.
		//
		std::shared_ptr< Texture > texture = mEditorWorld->mTextureManager->Add( "default-alpha.png", mRenderer->CreateTextureFromFile( "Textures\\default-alpha.png" ));
		//AssetTreeItem< Texture >* asset = new AssetTreeItem< Texture >( "default-alpha.png", texture );
		//mAssetTexture->addChild( asset );

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
		std::shared_ptr< Sprite > sprite( new Sprite( mEditorWorld->mShaderManager->Get( "GUI" ), mEditorWorld->mTextureManager->Get( "default-alpha.png" )));

		sprite->AddFrame( sprite->GetTextureCoords( Quad( 0, 0, 512, 512 )));
		sprite->AddFrame( sprite->GetTextureCoords( Quad( 0, 0, 64, 64 )));

		mEditorWorld->mSpriteManager->Add( "GUI Sprite", sprite );

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
		meshBuilder.mShader = mEditorWorld->mShaderManager->Get( "GUI Mesh" );
		meshBuilder.mTexture[ TEXTURE_DIFFUSE ] = mEditorWorld->mTextureManager->Get( "Backbuffer" );

		drawable->mMesh = mEditorWorld->mMeshManager->Add( "World", std::shared_ptr< Mesh >( meshBuilder.BuildMesh( mRenderer )));
		drawable->mPosition = Vector3f( 0, 30, 0.2f );
		drawable->mScale = Vector3f( (float)Renderer::WINDOW_WIDTH_BASE-500, (float)Renderer::WINDOW_HEIGHT_BASE-60, 1 );

		// Create button.
		//
		/*GUI::Button* button = (GUI::Button*)widgetComp->mRoot.AddChild( new GUI::Button() );
		button->mPosition = Vector3f( 200, 30, 0.1f );
		button->mScale    = Vector3f( 512, 512, 1 );
		button->mScaleToWindow = false;
		*/
		///////////////////////////////

		mEditorWorld->Startup();

		///////////////////////////////
		// CREATE EDITOR OBJECTS

		CreateTranslateMesh();

		CreateRotateMesh();
			
		CreateScaleMesh();
	}

	void PrepareGameWorld()
	{
		MeshBuilder						meshBuilder;
		std::shared_ptr< Mesh >			mesh;

		GameObject*						obj;
		TransformComponent*				transform;
		PhysicsComponent*				physics;
		RigidBody*						body;

		/////////////////////////////////////////////////////////////////////////

		mGameWorld = new GameWorld();

		mGameWorld->mRenderer			= mRenderer;

		mGameWorld->mTiming				= new Timing();
		mGameWorld->mPhysicsSystem		= BuildPhysicsSystemSE();
		mGameWorld->mAudioSystem		= NULL;

		mGameWorld->mTextureManager		= new TextureManager();
		mGameWorld->mShaderManager		= new ShaderManager();
		mGameWorld->mSpriteManager		= new SpriteManager();
		mGameWorld->mMeshManager		= new MeshManager();
		mGameWorld->mModelManager		= new ModelManager();
		mGameWorld->mSoundManager		= new SoundManager();

		mGameWorld->mPhysicsSystem->Startup();

		////////////////////////////////////

		mGameWorld->mShaderManager->Add( "GUI", mEditorWorld->mShaderManager->Get( "GUI" ));

		mGameWorld->mSpriteSystem = new SpriteSystem( mRenderer, mGameWorld->mShaderManager->Get( "GUI" ), 256 );
		
		////////////////////////////////////

		mGameWorld->mTextureManager->Add( "default-alpha.png", mRenderer->CreateTextureFromFile( "Textures\\default-alpha.png" ));

		mGameWorld->mShaderManager->Add( "Color",   mEditorWorld->mShaderManager->Get( "Color" ));
		mGameWorld->mShaderManager->Add( "Texture", mEditorWorld->mShaderManager->Get( "Texture" ));
		mGameWorld->mShaderManager->Add( "Sprite",  mEditorWorld->mShaderManager->Get( "Sprite" ));

		////////////////////////////////////

		// Main Camera.
		//
		obj = mGameWorld->AddGameObject( new GameObject(), "Main Camera" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition    = Vector3f( 0, 25, 25 );
		transform->mOrientation = Quatf( -45, 0, 0 );

		obj->AttachComponent( new PlayerControllerComponent(), "Controller" );
			
		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mGameWorld->mPhysicsSystem->CreateSphere( transform->mPosition, transform->mOrientation, 1.0f, 1.0f ));
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
		meshBuilder.mShader    = mGameWorld->mShaderManager->Get( "Color" );
		meshBuilder.mPrimitive = TRIANGLE_LIST;

		mesh = mGameWorld->mMeshManager->Add( "Ground", std::shared_ptr< Mesh >(meshBuilder.BuildMesh( mRenderer )));
		//mAssetMesh->addChild( new AssetTreeItem< Mesh >( "Ground", mesh ));
	
		obj = mGameWorld->AddGameObject( new GameObject(), "Ground" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( 0, 0, 0 );
		transform->mScale    = Vector3f( 100, 1, 100 );

		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mGameWorld->mPhysicsSystem->CreateBox( transform->mPosition, transform->mOrientation, transform->mScale, 0.0f ));
		body = physics->GetRigidBody();
		body->SetShapeType( PHYSICS_BOX );
		body->SetFlags( DISABLE_GRAVITY );
			
		obj->AttachComponent( new MeshComponent( mesh ), "Mesh" );
			
		//AddObject( obj );
			
		// Test object.
		//
		meshBuilder.ClearGeometry();
		meshBuilder.CreateDodecahedron( 1.0f );
		meshBuilder.mShader = mGameWorld->mShaderManager->Get( "Texture" );
		meshBuilder.mTexture[ TEXTURE_DIFFUSE ] = mGameWorld->mTextureManager->Get( "default-alpha.png" );

		mesh = mGameWorld->mMeshManager->Add( "Dodecahedron", std::shared_ptr< Mesh >(meshBuilder.BuildMesh( mRenderer )));
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
			
		mesh = mGameWorld->mMeshManager->Add( "Sphere", std::shared_ptr< Mesh >(meshBuilder.BuildMesh( mRenderer )));
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
		std::shared_ptr< Texture > textureS = mGameWorld->mTextureManager->Add( "fire.png", mRenderer->CreateTextureFromFile( "Textures\\fire.png" ));
		std::shared_ptr< Sprite >  sprite( new Sprite( mGameWorld->mShaderManager->Get( "Sprite" ), textureS ));

		mGameWorld->mSpriteManager->Add( "Fire", sprite );
		
		sprite->AddFrame( sprite->GetTextureCoords( Quad( 0, 0, 64, 64 )));
		sprite->AddFrame( sprite->GetTextureCoords( Quad( 64, 0, 128, 64 )));

		ParticleSystem* particleSystem = BuildParticleSystemNormal( mRenderer, mGameWorld, sprite, 300 );
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

		///////////////////////////////

		mGameWorld->Startup();
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
