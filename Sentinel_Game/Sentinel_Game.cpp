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
#include "ParticleEmitterComponent.h"
#include "PhysicsComponent.h"
#include "MeshComponent.h"
#include "ModelComponent.h"
#include "SpriteComponent.h"
#include "WidgetComponent.h"

#include "Sound.h"
#include "Sprite.h"

#include "GUI/Widget.h"
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
	SpriteManager*			mSpriteManager;
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
	}

	~MainApp()
	{}

	void Startup( HINSTANCE hInstance, int nCmdShow )
	{
		mGameWindow = new GameWindow( IDI_SENTINEL_GAME, IDI_SMALL, 0 );
		
		mAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( IDC_SENTINEL_GAME ));

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
		mGameWindow->Startup( mRenderer, hInstance, nCmdShow, "Sentinel Game", "SentinelClass", info );
		
		mRenderer->CreateDepthStencil( Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE );
		mRenderer->CreateBackbuffer();

		////////////////////////////////////

		Mouse::Get().SetPosition( CenterHandle( (HWND)mGameWindow->GetInfo()->Handle() ));
		//ShowCursor( FALSE );

		////////////////////////////////////
		// Prepare and load GameWorld.
		//
		mTiming							= new Timing();
		mPhysicsSystem					= BuildPhysicsSystemBT();
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

		Archive archive;
		if( !archive.Open( "Default.MAP", "rb" ))
			throw AppException( "Failed to load Default.MAP" );

		mTextureManager->Load( archive, mRenderer );
		mShaderManager->Load( archive, mRenderer );
		mSpriteManager->Load( archive, mShaderManager, mTextureManager );
		mMeshManager->Load( archive, mRenderer, mShaderManager, mTextureManager );
		mModelManager->Load( archive, mRenderer, mShaderManager, mTextureManager );
		mSoundManager->Load( archive, mAudioSystem );

		mSpriteSystem = new SpriteSystem( mRenderer, mShaderManager->Get( "GUI" ), 256 );
		mGameWorld->mSpriteSystem		= mSpriteSystem;

		mGameWorld->Load( archive );

		archive.Close();

		// Testing area.
		//
		GameObject*			obj;
		TransformComponent* transform;
		
		// Testing particle effects.
		//
		std::shared_ptr< Texture > texture = mTextureManager->Add( "fire.png", mRenderer->CreateTextureFromFile( "fire.png" ));
		
		std::shared_ptr< Sprite > sprite( new Sprite( mShaderManager->Get( "Sprite" ), texture ));
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
		//mParticleSystem->mEffect.push_back( new RandomVelocityEffect( 1, Vector3f( -1, 1, 0 ), Vector3f( 1, 1, 0 )));

		obj = mGameWorld->AddGameObject( new GameObject(), "Particle Emitter" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mScale = Vector3f( 512, 512, 1 );

		obj->AttachComponent( new ParticleEmitterComponent( particleSystem ), "Sprite" );

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
				mTiming->Update();

				mGameWindow->Update();

				Keyboard::Get().ProcessMessages();

				BEGIN_PROFILE( mTiming );
				if( Keyboard::Get().DidGoDown( VK_ESCAPE ))
				{
					return;
				}

				static float color[] = {0.0f, 0.2f, 0.8f, 1.0f};

				UINT width  = mGameWindow->GetInfo()->Width();
				UINT height = mGameWindow->GetInfo()->Height();

				//mRenderer->SetViewport( 0, 0, width, height );
				mRenderer->SetViewport( ((int)width - (int)Renderer::WINDOW_WIDTH_BASE) >> 1, ((int)height - (int)Renderer::WINDOW_HEIGHT_BASE) >> 1, 
										Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE );
				mRenderer->SetDepthStencil( 0 );
				mRenderer->SetRenderTarget( 0 );

				mRenderer->Clear( color );

				BEGIN_PROFILE( mTiming );
				mGameWorld->UpdateController();
				mGameWorld->UpdatePhysics();
				mGameWorld->UpdateTransform();
				mGameWorld->UpdateComponents();
				mGameWorld->UpdateDrawable();
				END_PROFILE( mTiming, "World" );

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
		SHUTDOWN_DELETE( mGameWorld );
		
		SAFE_DELETE( mTiming );
		SAFE_DELETE( mPhysicsSystem );
		SAFE_DELETE( mSpriteSystem );
		SAFE_DELETE( mAudioSystem );
		
		SAFE_DELETE( mSoundManager );
		SAFE_DELETE( mModelManager );
		SAFE_DELETE( mMeshManager );
		SAFE_DELETE( mSpriteManager );
		SAFE_DELETE( mShaderManager );
		SAFE_DELETE( mTextureManager );

		SHUTDOWN_DELETE( mGameWindow );
		
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
