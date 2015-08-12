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
#include "Network/Socket.h"
#include "AudioSystem.h"

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

#include "MeshBuilder.h"
#include "Model.h"

#include "GameWorld.h"
#include "GameObject.h"

#include "Component/Transform.h"
#include "Component/Physics.h"
#include "Component/Light.h"
#include "Component/PerspectiveCamera.h"
#include "Component/OrthographicCamera.h"
#include "Component/PlayerController.h"
#include "Component/ParticleEmitter.h"
#include "Component/MeshDrawable.h"
#include "Component/ModelDrawable.h"
#include "Component/SpriteDrawable.h"

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

	GameWorld*				mGameWorld;

	RenderTexture*			mRTMain;
	DepthStencil*			mDSMain;

public:

	MainApp() :
		mGameWindow( NULL ),
		mRenderer( NULL ),
		mGameWorld( NULL )
	{
		srand( (UINT)time( (time_t*)0 ));
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
		
		mDSMain = mRenderer->CreateDepthStencil( Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE );
		mRTMain = mRenderer->CreateBackbuffer();

		////////////////////////////////////

		Mouse::Get().SetPosition( CenterHandle( (HWND)mGameWindow->GetInfo()->Handle() ));
		//ShowCursor( FALSE );

		////////////////////////////////////
		// Prepare GameWorld.
		//
		mGameWorld = new GameWorld();

		mGameWorld->mRenderer			= mRenderer;
		mGameWorld->mTiming				= new Timing();
		mGameWorld->mPhysicsSystem		= BuildPhysicsSystemBT();
		mGameWorld->mAudioSystem		= BuildAudioSystemAL();
		
		mGameWorld->mTextureManager		= new TextureManager( mRenderer );
		mGameWorld->mShaderManager		= new ShaderManager();
		mGameWorld->mMaterialManager	= new MaterialManager();
		mGameWorld->mSpriteManager		= new SpriteManager();
		mGameWorld->mMeshManager		= new MeshManager();
		mGameWorld->mModelManager		= new ModelManager();
		mGameWorld->mSoundManager		= new SoundManager();
		mGameWorld->mRenderManager		= new RenderManager();

		mGameWorld->mPhysicsSystem->Startup();

		Archive archive;
		if( !archive.Open( "Default.MAP", "rb" ))
			throw AppException( "Failed to load Default.MAP" );

		mGameWorld->mTextureManager->Load( archive, mRenderer );
		mGameWorld->mShaderManager->Load( archive, mRenderer );
		mGameWorld->mMaterialManager->Load( archive, mRenderer, mGameWorld->mShaderManager, mGameWorld->mTextureManager );
		mGameWorld->mSpriteManager->Load( archive );
		mGameWorld->mMeshManager->Load( archive, mRenderer, mGameWorld->mShaderManager, mGameWorld->mTextureManager, mGameWorld->mMaterialManager );
		mGameWorld->mModelManager->Load( archive, mRenderer, mGameWorld->mShaderManager, mGameWorld->mTextureManager, mGameWorld->mMaterialManager );
		mGameWorld->mSoundManager->Load( archive, mGameWorld->mAudioSystem );

		mGameWorld->mSpriteSystem = new SpriteSystem( mRenderer, mGameWorld->mShaderManager->Get( "GUI" )->Layout(), 256 );
		
		mGameWorld->Load( archive );

		archive.Close();

		////////////////////////////////////

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
				static Timing* timing;
				timing = mGameWorld->mTiming;

				timing->Update();

				mGameWindow->Update();

				Keyboard::Get().ProcessMessages();

				BEGIN_PROFILE( timing );
				if( Keyboard::Get().DidGoDown( VK_ESCAPE ))
				{
					return;
				}

				static float color[] = {0.0f, 0.2f, 0.8f, 1.0f};

				UINT width  = mGameWindow->GetInfo()->Width();
				UINT height = mGameWindow->GetInfo()->Height();

				BEGIN_PROFILE( timing );
				mGameWorld->UpdateController();

				BEGIN_PROFILE( timing );
				mGameWorld->UpdatePhysics();
				END_PROFILE( timing, "Physics" );

				mGameWorld->UpdateTransform();
				mGameWorld->UpdateComponents();
				mGameWorld->UpdateCamera();

				BEGIN_PROFILE( timing );
				mGameWorld->UpdateLight();
				END_PROFILE( timing, "Light" );

				mRenderer->SetViewport( 0, 0, width, height );
//				mRenderer->SetViewport( ((int)width - (int)Renderer::WINDOW_WIDTH_BASE) >> 1, ((int)height - (int)Renderer::WINDOW_HEIGHT_BASE) >> 1,
//										Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE );
				mRenderer->SetDepthStencil( mDSMain );
				mRenderer->SetRenderTexture( mRTMain );
				mRenderer->Clear( color );

				BEGIN_PROFILE( timing );
				mGameWorld->UpdateDrawable();
				mGameWorld->Present();
				END_PROFILE( timing, "Drawable" );

				END_PROFILE( timing, "World" );

				BEGIN_PROFILE( timing );
				mRenderer->Present();
				END_PROFILE( timing, "Renderer" );

				Mouse::Get().Update();
				Keyboard::Get().Update();
				END_PROFILE( timing, "Update" );

				SEPARATE_PROFILE( "-----------------------" );

				timing->Limit();
			}
		}
	}

	void Shutdown()
	{
		SAFE_DELETE( mDSMain );
		SAFE_DELETE( mRTMain );

		SHUTDOWN_DELETE( mGameWorld );
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
	//_CrtSetBreakAlloc( 776 );

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
