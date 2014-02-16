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
#include "NetworkSocket.h"
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

#include "Input.h"
#include "Timing.h"
#include "Util.h"

#include "MeshBuilder.h"
#include "Model.h"

#include "GameWorld.h"
#include "GameObject.h"

#include "TransformComponent.h"
#include "PhysicsComponent.h"
//#include "DirectionalLightComponent.h"
#include "PointLightComponent.h"
#include "PerspectiveCameraComponent.h"
#include "OrthographicCameraComponent.h"
#include "PlayerControllerComponent.h"
#include "PhysicsComponent.h"
#include "ParticleEmitterComponent.h"
#include "SpriteComponent.h"
#include "WidgetComponent.h"
#include "MeshComponent.h"
#include "ModelComponent.h"

#include "EditorControllerComponent.h"

#include "Sound.h"
#include "Sprite.h"
#include "Shape.h"

#include "GUI/Widget.h"
#include "GUI/WidgetObject.h"
#include "GUI/WidgetWorld.h"
#include "GUI/ModelWidget.h"
#include "GUI/SpriteViewWidget.h"
#include "GUi/SpriteControllerWidget.h"
#include "GUI/SpriteButtonWidget.h"
#include "GUI/LabelWidget.h"
#include "GUI/MeshViewWidget.h"

#include "RandomValue.h"

#include "Debug.h"

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

	RenderTexture*			mRTMain;
	DepthStencil*			mDSMain;

	RenderTexture*			mRTGameWorld;
	DepthStencil*			mDSGameWorld;

	Mesh*					mRTBackbufferMesh;
	Mesh*					mBoundsMesh;		// wire cube

	GameObject*				mSelectedObject;
	GameObject*				mHoverObject;
	float					mHoverDist;

	GameObject*				mTranslateObject;

	ColorRGBA				mSelectedColorX;
	ColorRGBA				mSelectedColorY;
	ColorRGBA				mSelectedColorZ;
	ColorRGBA				mSelectedColorAxis;

	FontSystem*				mFontSystem;
	GUI::LabelWidget*		mDebugText;
	GUI::WidgetObject*		mGameWorldWidget;

	Debug*					mDebug;

	Ray						mMouseToWorldRay;

public:

	MainApp() :
		mGameWindow( NULL ),
		mRenderer( NULL ),
		mGameWorld( NULL ),
		mEditorWorld( NULL ),
		mRTMain( NULL ),
		mDSMain( NULL ),
		mRTGameWorld( NULL ),
		mDSGameWorld( NULL ),
		mRTBackbufferMesh( NULL ),
		mBoundsMesh( NULL ),
		mSelectedObject( NULL ),
		mHoverObject( NULL ),
		mHoverDist( 0 ),
		mTranslateObject( NULL ),
		mSelectedColorX( 1, 0, 0, 0.5f ),
		mSelectedColorY( 0, 1, 0, 0.5f ),
		mSelectedColorZ( 0, 0, 1, 0.5f ),
		mSelectedColorAxis( 1, 1, 0, 0.5f ),
		mFontSystem( NULL ),
		mDebug( NULL ),
		mMouseToWorldRay( Vector3f(), Vector3f() )
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
		
		mRTMain = mRenderer->CreateBackbuffer();
		mDSMain = mRenderer->CreateDepthStencil( Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE );

		////////////////////////////////////
		// Prepare Editor and World View.
		//
		SetDirectory( "Assets" );

		PrepareEditorWorld();
		PrepareGameWorld();

		MeshBuilder builder;
		static Matrix4f matTrans, matScale;

		// Create wire cube from solid cubes.
		// Adjust spaces using scale of object.
		//
		std::shared_ptr< Shader > shaderLine = mEditorWorld->mShaderManager->Get( "Color_Only" );

		builder.mLayout = shaderLine->Layout();
		builder.CreateWireCube( 1 );
		
		mBoundsMesh = builder.BuildMesh( mRenderer );

		std::shared_ptr< Material > material = mEditorWorld->mMaterialManager->Add( "Bounds", SHARED( new Material() ));
		material->mShader     = shaderLine;
		material->mBlendState = mRenderer->BLEND_OFF;
		material->mDepthMode  = DEPTH_LEQUAL;
		material->mCullMode   = CULL_NONE;

		mBoundsMesh->mMaterial = material;

		mGameWorld->SetCamera( mEditorWorld->GetCamera( 1 ));

		mDebug = new Debug( mRenderer, mGameWorld, material );

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
				static char drawnText[ 256 ];
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

				static float colorWorld[] = {0.0f, 0.2f, 0.8f, 1.0f};

				mGameWindow->Update();

				BEGIN_PROFILE( timing );
				mGameWorld->UpdateTransform();
				mGameWorld->UpdatePhysics();
				mGameWorld->UpdateLight();

				mRenderer->SetViewport( 0, 0, Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE );
				mRenderer->SetDepthStencil( mDSGameWorld );
				mRenderer->SetRenderTexture( mRTGameWorld );
				mRenderer->Clear( colorWorld );

				mGameWorld->UpdateDrawable();

				SelectGameObject();
				DrawTranslateObject();

				END_PROFILE( timing, "World" );

				////////////////////////////////////
				
				static float colorEditor[] = {0.2f, 0.2f, 0.2f, 1.0f};

				const WindowInfo* info = mGameWindow->GetInfo();

				//mDebugText->mText = drawnText;

				BEGIN_PROFILE( timing );
				mEditorWorld->UpdateController();
				mEditorWorld->UpdatePhysics();
				mEditorWorld->UpdateTransform();
				mEditorWorld->UpdateComponents();

				mRenderer->SetViewport( 0, 0, info->Width(), info->Height() );
				mRenderer->SetDepthStencil( mDSMain );
				mRenderer->SetRenderTexture( mRTMain );
				mRenderer->Clear( colorEditor );

				mEditorWorld->UpdateDrawable();
				END_PROFILE( timing, "Editor" );

				// Draw GameWorld over Backbuffer.
				//
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
		if( mFontSystem )
			SAFE_DELETE( mFontSystem->mSpriteSystem );

		SAFE_DELETE( mFontSystem );

		SAFE_DELETE( mRTMain );
		SAFE_DELETE( mDSMain );

		SAFE_DELETE( mRTGameWorld );
		SAFE_DELETE( mDSGameWorld );

		SAFE_DELETE( mBoundsMesh );
		SAFE_DELETE( mRTBackbufferMesh );

		SAFE_DELETE( mTranslateObject );

		SAFE_DELETE( mDebug );

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
		mGameWorld->mMaterialManager->Save( archive, mRenderer, mGameWorld->mShaderManager, mGameWorld->mTextureManager );
		mGameWorld->mSpriteManager->Save( archive );
		mGameWorld->mMeshManager->Save( archive, mRenderer, mGameWorld->mShaderManager, mGameWorld->mTextureManager, mGameWorld->mMaterialManager );
		mGameWorld->mModelManager->Save( archive, mRenderer, mGameWorld->mShaderManager, mGameWorld->mTextureManager, mGameWorld->mMaterialManager );
		mGameWorld->mSoundManager->Save( archive );
		
		mGameWorld->Save( archive );

		archive.Close();
	}

	void PrepareEditorWorld()
	{
		MeshBuilder						meshBuilder;
		std::shared_ptr< Mesh >			mesh;
		std::shared_ptr< Material >		material;
		std::shared_ptr< Texture >		texture;
		std::shared_ptr< Shader >		shader;
		std::shared_ptr< Sprite >		sprite;

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

		mEditorWorld->mPhysicsSystem->Startup();
		
		mEditorWorld->mTextureManager	= new TextureManager( mRenderer );
		mEditorWorld->mShaderManager	= new ShaderManager();
		mEditorWorld->mMaterialManager	= new MaterialManager();
		mEditorWorld->mSpriteManager	= new SpriteManager();
		mEditorWorld->mMeshManager		= new MeshManager();
		mEditorWorld->mModelManager		= new ModelManager();
		mEditorWorld->mSoundManager		= new SoundManager();

		SetDirectory( "Shaders" );

		DIR *dir;
		struct dirent *ent;
		if( (dir = opendir( GetDirectory().c_str() )) != NULL )
		{
			while( (ent = readdir( dir )) != NULL )
			{
				std::string name( ent->d_name );
				size_t len = name.size();

				if( name.find( ".xsh" ) != std::string::npos )
				{
					Shader* shader = mRenderer->CreateShaderFromFile( name.c_str() );

					if( !shader )
					{
						closedir( dir );
						throw AppException( "Failed to load 'Assets\\Shaders\\" + name );
					}

					mEditorWorld->mShaderManager->Add( name.substr( 0, len-4 ), SHARED( shader ));
				}
			}
			closedir( dir );
		}
		else
		{
			throw AppException( "Failed to open 'Assets\\Shaders' directory." );
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

		shader = mEditorWorld->mShaderManager->Get( "GUI" );

		mFontSystem = BuildFontSystemFT();
		mFontSystem->mSpriteSystem = new SpriteSystem( mRenderer, shader->Layout(), 256 );
		mFontSystem->Load( "Font\\courbd.ttf" );
		mFontSystem->mFont = mFontSystem->Build( 16, 16 );
		mFontSystem->mFont->mMaterial->mShader = shader;
		mFontSystem->mFont->mMaterial->mBlendState = mRenderer->BLEND_ALPHA;

		mEditorWorld->mSpriteSystem = new SpriteSystem( mRenderer, shader->Layout(), 256 );

		material = std::shared_ptr< Material >(new Material());
		material->mShader = shader;
		material->mBlendState = mRenderer->BLEND_ALPHA;

		mEditorWorld->mSpriteSystem->mMaterial = material;

		mEditorWorld->mShaderManager->Get( "GUI" )->SetSampler( 0, MODE_WRAP, MODE_WRAP, FILTER_POINT, FILTER_POINT );
		mEditorWorld->mShaderManager->Get( "GUI_Mesh" )->SetSampler( 0, MODE_WRAP, MODE_WRAP, FILTER_LINEAR, FILTER_LINEAR );
		mEditorWorld->mShaderManager->Get( "RT_Color" )->SetSampler( 0, MODE_WRAP, MODE_WRAP, FILTER_LINEAR, FILTER_LINEAR );

		////////////////////////////////////

		// Create backbuffer for world view.
		//
		mRTGameWorld = mRenderer->CreateRenderTexture( mEditorWorld->mTextureManager->Add( "Backbuffer", SHARED( mRenderer->CreateTexture( Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE, IMAGE_FORMAT_RGBA ))).get() );
		mDSGameWorld = mRenderer->CreateDepthStencil( Renderer::WINDOW_WIDTH_BASE, Renderer::WINDOW_HEIGHT_BASE );
		
		material = mEditorWorld->mMaterialManager->Add( "Backbuffer", std::shared_ptr< Material >( new Material() ));
		material->mShader = mEditorWorld->mShaderManager->Get( "RT_Color" );
		material->mBlendState = mRenderer->BLEND_OFF;
		material->mTexture[ TEXTURE_DIFFUSE ] = mEditorWorld->mTextureManager->Get( "Backbuffer" );

		mRTBackbufferMesh = MeshBuilder::BuildRenderTextureMesh( mRenderer, material->mShader->Layout() );
		mRTBackbufferMesh->mMaterial = material;

		// Add GUI texture.
		//
		texture = mEditorWorld->mTextureManager->Add( "GUI.png", SHARED( mRenderer->CreateTextureFromFile( "Textures\\GUI.png" )));
		//AssetTreeItem< Texture >* asset = new AssetTreeItem< Texture >( "default-alpha.png", texture );
		//mAssetTexture->addChild( asset );

		////////////////////////////////////

		// GUI Camera.
		//
		obj = mEditorWorld->AddGameObject( new GameObject(), "GUI Camera" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( 0, 0, 0 );
	
		OrthographicCameraComponent* orthoCamera = (OrthographicCameraComponent*)obj->AttachComponent( new OrthographicCameraComponent( (float)Renderer::WINDOW_WIDTH_BASE, (float)Renderer::WINDOW_HEIGHT_BASE ), "OCamera" );
		
		//AddObject( obj );
		
		// Create game object for widget.
		//
		texture = mEditorWorld->mTextureManager->Get( "GUI.png" );

		material = mEditorWorld->mMaterialManager->Add( "GUI", SHARED( new Material() ));
		material->mShader = mEditorWorld->mShaderManager->Get( "GUI" );
		material->mBlendState = mRenderer->BLEND_ALPHA;
		material->mTexture[ TEXTURE_DIFFUSE ] = texture;

		sprite = mEditorWorld->mSpriteManager->Add( "GUI", SHARED( new Sprite() ));
		sprite->AddFrame( Sprite::QUADtoTEXCOORD( Quad( 0, 0, 1, 1 ), texture->Width(), texture->Height() ));
		sprite->AddFrame( Sprite::QUADtoTEXCOORD( Quad( 1, 0, 2, 1 ), texture->Width(), texture->Height() ));
		sprite->AddFrame( Sprite::QUADtoTEXCOORD( Quad( 2, 0, 3, 1 ), texture->Width(), texture->Height() ));

		obj = mEditorWorld->AddGameObject( new GameObject(), "GUI Widget" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );

		WidgetComponent* widgetComp = (WidgetComponent*)obj->AttachComponent( new WidgetComponent( sprite, material, mFontSystem, 0 ), "Widget" );
		
		GUI::WidgetObject*				widget;
		GUI::ModelWidget*				model;
		GUI::SpriteViewWidget*			view;
		GUI::MeshViewWidget*			drawableMesh;
		GUI::LabelWidget*				label;
		GUI::SpriteControllerWidget*	controller;
		GUI::SpriteButtonWidget*		button;

		// Menu Bar.
		//
		widget = (GUI::WidgetObject*)widgetComp->mWidgetWorld->AddWidgetObject( new GUI::WidgetObject() );

		model = new GUI::ModelWidget();
		model->mPosition	= Vector3f( 0, 0, 1 );
		model->mScale		= Vector3f( (float)Renderer::WINDOW_WIDTH_BASE, 0, 1 );
		
		view = new GUI::SpriteViewWidget();
		view->mFrame		= 0;
		view->mColor		= ColorRGBA( 0.7f, 0.7f, 0.7f, 1 );

		controller = new GUI::SpriteControllerWidget();
		controller->mMargin.bottom = 30;
		controller->mScaleToWindowX = true;
		
		widget->SetModel( model );
		widget->SetView( view );
		widget->SetController( controller );

		// Menu Labels.
		//
		widget = (GUI::WidgetObject*)widgetComp->mWidgetWorld->AddWidgetObject( new GUI::WidgetObject() );

		model = new GUI::ModelWidget();
		model->mPosition = Vector3f( 10, 20, 0.9f );
		model->mScale    = Vector3f( 16, 16, 1 );

		//label = (GUI::Label*)widgetComp->mRoot.AddChild( new GUI::Label( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ{}[]!@#$%^&*()1234567890" ));
		label = new GUI::LabelWidget( "File" );
		label->mColor = ColorRGBA( 0, 0, 0, 1 );

		controller = new GUI::SpriteControllerWidget();
		
		widget->SetModel( model );
		widget->SetView( label );
		widget->SetController( controller );

		//////////////////////////////////////

		widget = (GUI::WidgetObject*)widgetComp->mWidgetWorld->AddWidgetObject( new GUI::WidgetObject() );

		model = new GUI::ModelWidget();
		model->mPosition = Vector3f( 110, 20, 0.9f );
		model->mScale    = Vector3f( 16, 16, 1 );

		label = new GUI::LabelWidget( "Edit" );
		label->mColor = ColorRGBA( 0, 0, 0, 1 );

		controller = new GUI::SpriteControllerWidget();
		
		widget->SetModel( model );
		widget->SetView( label );
		widget->SetController( controller );

		//////////////////////////////////////

		widget = (GUI::WidgetObject*)widgetComp->mWidgetWorld->AddWidgetObject( new GUI::WidgetObject() );

		model = new GUI::ModelWidget();
		model->mPosition = Vector3f( 210, 20, 0.9f );
		model->mScale = Vector3f( 16, 16, 1 );

		label = new GUI::LabelWidget( "Help" );
		label->mColor = ColorRGBA( 0, 0, 0, 1 );

		controller = new GUI::SpriteControllerWidget();
		
		widget->SetModel( model );
		widget->SetView( label );
		widget->SetController( controller );

		//////////////////////////////////////

		widget = (GUI::WidgetObject*)widgetComp->mWidgetWorld->AddWidgetObject( new GUI::WidgetObject() );

		model = new GUI::ModelWidget();
		model->mPosition = Vector3f( 0, 40, 0.9f );
		model->mScale = Vector3f( 16, 16, 1 );

		mDebugText = new GUI::LabelWidget();
		mDebugText->mColor = ColorRGBA( 0, 0, 0, 1 );

		controller = new GUI::SpriteControllerWidget();

		widget->SetModel( model );
		widget->SetView( mDebugText );
		widget->SetController( controller );

		// Toolbar.
		//
		widget = (GUI::WidgetObject*)widgetComp->mWidgetWorld->AddWidgetObject( new GUI::WidgetObject() );

		model = new GUI::ModelWidget();
		model->mPosition	= Vector3f( 0, 0, 1 );
		model->mScale		= Vector3f( (float)Renderer::WINDOW_WIDTH_BASE, 0, 1 );
		
		view = new GUI::SpriteViewWidget();
		view->mFrame		= 0;
		view->mColor		= ColorRGBA( 0.9f, 0.9f, 0.9f, 1 );

		controller = new GUI::SpriteControllerWidget();
		controller->mMargin = Quad( 0, 30, 0, 30 );
		controller->mScaleToWindowX = true;

		widget->SetModel( model );
		widget->SetView( view );
		widget->SetController( controller );

		// Status Bar.
		//
		widget = (GUI::WidgetObject*)widgetComp->mWidgetWorld->AddWidgetObject( new GUI::WidgetObject() );

		model = new GUI::ModelWidget();
		model->mPosition	= Vector3f( 0, (float)Renderer::WINDOW_HEIGHT_BASE, 0.1f );
		model->mScale		= Vector3f( (float)Renderer::WINDOW_WIDTH_BASE, 0, 1 );
		
		view = new GUI::SpriteViewWidget();
		view->mFrame		= 0;
		view->mColor		= ColorRGBA( 0.7f, 0.7f, 0.7f, 1 );

		controller = new GUI::SpriteControllerWidget();
		controller->mMargin				= Quad( 0, -30, 0, 30 );
		controller->mScaleToWindowX		= true;
		controller->mPositionToWindowY	= true;

		widget->SetModel( model );
		widget->SetView( view );
		widget->SetController( controller );

		// Button.
		//
		widget = (GUI::WidgetObject*)widgetComp->mWidgetWorld->AddWidgetObject( new GUI::WidgetObject() );

		model = new GUI::ModelWidget();
		model->mPosition    = Vector3f( 200, 0, 0.7f );
		model->mOrientation = Quatf( 0, 0, 45 );
		model->mScale       = Vector3f( 128, 64, 1 );
		
		view = new GUI::SpriteViewWidget();
		
		button = new GUI::SpriteButtonWidget();
		button->mMargin     = Quad( 0, 30, 0, 0 );
		button->mPositionToWindowX = true;
		button->mPositionToWindowY = true;
		button->mScaleToWindowX = true;
		button->mScaleToWindowY = true;

		widget->SetModel( model );
		widget->SetView( view );
		widget->SetController( button );

		// World Drawable.
		//
		mGameWorldWidget = (GUI::WidgetObject*)widgetComp->mWidgetWorld->AddWidgetObject( new GUI::WidgetObject() );

		model = new GUI::ModelWidget();
		model->mPosition = Vector3f( 0, 0, 0.8f );
		model->mScale    = Vector3f( (float)Renderer::WINDOW_WIDTH_BASE-500, (float)Renderer::WINDOW_HEIGHT_BASE, 1 );
		
		material = mEditorWorld->mMaterialManager->Add( "World", SHARED( new Material() ));
		material->mShader = mEditorWorld->mShaderManager->Get( "GUI_Mesh" );
		material->mBlendState = mRenderer->BLEND_OFF;
		material->mTexture[ TEXTURE_DIFFUSE ] = mEditorWorld->mTextureManager->Get( "Backbuffer" );

		drawableMesh = new GUI::MeshViewWidget();
		drawableMesh->mMesh = mEditorWorld->mMeshManager->Add( "World", SHARED( MeshBuilder::BuildGUIMesh( mRenderer, material->mShader->Layout() )));
		drawableMesh->mMesh->mMaterial = material;
		
		controller = new GUI::SpriteControllerWidget();
		controller->mScaleToWindowX = true;
		controller->mScaleToWindowY = true;
		controller->mMargin = Quad( 0, 60, 0, -90 );

		mGameWorldWidget->SetModel( model );
		mGameWorldWidget->SetView( drawableMesh );
		mGameWorldWidget->SetController( controller );

		///////////////////////////////

		// World View Camera.
		//
		obj = mEditorWorld->AddGameObject( new GameObject(), "Main Camera" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition    = Vector3f( 0, 15, 40 );
		transform->mOrientation = Quatf( -15, 0, 0 );

		EditorControllerComponent* editorController = (EditorControllerComponent*)obj->AttachComponent( new EditorControllerComponent(), "Controller" );
		editorController->mWorldWidget = model;
			
		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mEditorWorld->mPhysicsSystem->CreateRigidBody( mEditorWorld->mPhysicsSystem->CreateSphere( 1.0f ), transform->mPosition, transform->mOrientation, 1.0f ));
		body = physics->GetRigidBody();
		body->SetFlags( DISABLE_GRAVITY );
		body->SetRestitution( 1.0f );
		body->SetDamping( 0.9f, 0.9f );

		obj->AttachComponent( new PerspectiveCameraComponent( (float)Renderer::WINDOW_WIDTH_BASE, (float)Renderer::WINDOW_HEIGHT_BASE ), "PCamera" );

		///////////////////////////////

		mEditorWorld->Startup();
	}

	void PrepareGameWorld()
	{
		MeshBuilder						meshBuilder;
		std::shared_ptr< Mesh >			mesh;

		std::shared_ptr< Material >		material;

		GameObject*						obj;
		TransformComponent*				transform;
		PhysicsComponent*				physics;
		RigidBody*						body;
		MeshComponent*					meshComp;

		/////////////////////////////////////////////////////////////////////////

		mGameWorld = new GameWorld();

		mGameWorld->mRenderer			= mRenderer;

		mGameWorld->mTiming				= new Timing();
		mGameWorld->mPhysicsSystem		= BuildPhysicsSystemSE();
		mGameWorld->mAudioSystem		= NULL;

		mGameWorld->mTextureManager		= new TextureManager( mRenderer );
		mGameWorld->mShaderManager		= new ShaderManager();
		mGameWorld->mMaterialManager	= new MaterialManager();
		mGameWorld->mSpriteManager		= new SpriteManager();
		mGameWorld->mMeshManager		= new MeshManager();
		mGameWorld->mModelManager		= new ModelManager();
		mGameWorld->mSoundManager		= new SoundManager();

		mGameWorld->mPhysicsSystem->Startup();

		////////////////////////////////////

		std::shared_ptr< Shader > shaderGUI = mGameWorld->mShaderManager->Add( "GUI", mEditorWorld->mShaderManager->Get( "GUI" ));

		mGameWorld->mSpriteSystem = new SpriteSystem( mRenderer, shaderGUI->Layout(), 256 );

		////////////////////////////////////

		mGameWorld->mTextureManager->Add( "default-alpha.png", SHARED( mRenderer->CreateTextureFromFile( "Textures\\default-alpha.png" )));

		mGameWorld->mShaderManager->Add( "Color_Lit",			mEditorWorld->mShaderManager->Get( "Color_Lit" ));
		mGameWorld->mShaderManager->Add( "Color_Lit_Shadow",	mEditorWorld->mShaderManager->Get( "Color_Lit_Shadow" ));
		mGameWorld->mShaderManager->Add( "Texture_Lit",			mEditorWorld->mShaderManager->Get( "Texture_Lit" ));
		mGameWorld->mShaderManager->Add( "Texture_Lit_Shadow",	mEditorWorld->mShaderManager->Get( "Texture_Lit_Shadow" ));
		mGameWorld->mShaderManager->Add( "Sprite",				mEditorWorld->mShaderManager->Get( "Sprite" ));
		mGameWorld->mShaderManager->Add( "RT_Cube_Depth",		mEditorWorld->mShaderManager->Get( "RT_Cube_Depth" ));

		std::shared_ptr< Shader > shaderColor   = mGameWorld->mShaderManager->Get( "Color_Lit_Shadow" );
		std::shared_ptr< Shader > shaderTexture = mGameWorld->mShaderManager->Get( "Texture_Lit_Shadow" );

		////////////////////////////////////

		// Main Camera.
		//
		obj = mGameWorld->AddGameObject( new GameObject(), "Main Camera" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition    = Vector3f( 0, 15, 40 );
		transform->mOrientation = Quatf( -15, 0, 0 );

		obj->AttachComponent( new PlayerControllerComponent(), "Controller" );
			
		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mGameWorld->mPhysicsSystem->CreateRigidBody( mGameWorld->mPhysicsSystem->CreateSphere( 1.0f ), transform->mPosition, transform->mOrientation, 1.0f ));
		body = physics->GetRigidBody();
		body->SetFlags( DISABLE_GRAVITY );
		body->SetRestitution( 1.0f );
		body->SetDamping( 0.9f, 0.9f );
		body->SetAngularFactor( Vector3f( 0, 0, 0 ));
			
		obj->AttachComponent( new PerspectiveCameraComponent( (float)Renderer::WINDOW_WIDTH_BASE, (float)Renderer::WINDOW_HEIGHT_BASE ), "PCamera" );
		
		// Point Light.
		//
		obj = mGameWorld->AddGameObject( new GameObject(), "Point_Light" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( 0, 4, 0 );

		PointLightComponent* light = (PointLightComponent*)obj->AttachComponent( new PointLightComponent( 1024 ), "Light" );
		light->mColor         = ColorRGBA( 0.8f, 0.6f, 0.0f, 1.0f );
		light->mAttenuation   = Vector4f( 1.0f, 0.0f, 0.0f, 25.0f );
		light->mShader        = mGameWorld->mShaderManager->Get( "RT_Cube_Depth" );

		// Create color only box.
		//
		meshBuilder.CreateCube( 1.0f );
		meshBuilder.mLayout    = shaderColor->Layout();
		meshBuilder.mPrimitive = TRIANGLE_LIST;

		material = mGameWorld->mMaterialManager->Add( "Box", SHARED( new Material() ));
		material->mShader      = shaderColor;
		material->mBlendState  = mRenderer->BLEND_OFF;

		mesh = mGameWorld->mMeshManager->Add( "Box", SHARED( meshBuilder.BuildMesh( mRenderer )));
		mesh->mMaterial        = material;
		
		// Ground object - Bottom.
		//
		obj = mGameWorld->AddGameObject( new GameObject(), "Ground_Bottom" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( 0, 0, 0 );
		transform->mScale    = Vector3f( 100, 1, 100 );
		
		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mGameWorld->mPhysicsSystem->CreateRigidBody( mGameWorld->mPhysicsSystem->CreateBox( transform->mScale ), transform->mPosition, transform->mOrientation, 0.0f ));
		body = physics->GetRigidBody();
		body->SetFlags( DISABLE_GRAVITY );
			
		meshComp = (MeshComponent*)obj->AttachComponent( new MeshComponent( mesh ), "Mesh" );
		meshComp->mIsDynamic = true;
		
		// Ground object - Back.
		//
		obj = mGameWorld->AddGameObject( new GameObject(), "Ground_Back" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( 0, 20, -20 );
		transform->mScale    = Vector3f( 20, 20, 1 );
		
		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mGameWorld->mPhysicsSystem->CreateRigidBody( mGameWorld->mPhysicsSystem->CreateBox( transform->mScale ), transform->mPosition, transform->mOrientation, 0.0f ));
		body = physics->GetRigidBody();
		body->SetFlags( DISABLE_GRAVITY );
			
		meshComp = (MeshComponent*)obj->AttachComponent( new MeshComponent( mesh ), "Mesh" );
		meshComp->mIsDynamic = true;

		// Ground object - Right.
		//
		obj = mGameWorld->AddGameObject( new GameObject(), "Ground_Right" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( 20, 20, 0 );
		transform->mScale    = Vector3f( 1, 20, 20 );
		
		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mGameWorld->mPhysicsSystem->CreateRigidBody( mGameWorld->mPhysicsSystem->CreateBox( transform->mScale ), transform->mPosition, transform->mOrientation, 0.0f ));
		body = physics->GetRigidBody();
		body->SetFlags( DISABLE_GRAVITY );
			
		meshComp = (MeshComponent*)obj->AttachComponent( new MeshComponent( mesh ), "Mesh" );
		meshComp->mIsDynamic = true;

		// Ground object - Left.
		//
		obj = mGameWorld->AddGameObject( new GameObject(), "Ground_Left" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( -22, 10, 0 );
		transform->mScale    = Vector3f( 20, 1, 20 );
		transform->mOrientation = Quatf( 0, 0, 0 );

		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mGameWorld->mPhysicsSystem->CreateRigidBody( mGameWorld->mPhysicsSystem->CreateBox( transform->mScale ), transform->mPosition, transform->mOrientation, 0.0f ));
		body = physics->GetRigidBody();
		body->SetFlags( DISABLE_GRAVITY );

		meshComp = (MeshComponent*)obj->AttachComponent( new MeshComponent( mesh ), "Mesh" );
		meshComp->mIsDynamic = true;

		// Ground object - Top.
		//
		obj = mGameWorld->AddGameObject( new GameObject(), "Ground_Top" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( 0, 20, 0 );
		transform->mScale    = Vector3f( 20, 1, 20 );
		
		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mGameWorld->mPhysicsSystem->CreateRigidBody( mGameWorld->mPhysicsSystem->CreateBox( transform->mScale ), transform->mPosition, transform->mOrientation, 0.0f ));
		body = physics->GetRigidBody();
		body->SetFlags( DISABLE_GRAVITY );
			
		meshComp = (MeshComponent*)obj->AttachComponent( new MeshComponent( mesh ), "Mesh" );
		meshComp->mIsDynamic = true;

		// Box object - Right.
		//
		obj = mGameWorld->AddGameObject( new GameObject(), "Box_Right" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( 10, 2, 5 );
		transform->mScale    = Vector3f( 1, 1, 1 );
		
		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mGameWorld->mPhysicsSystem->CreateRigidBody( mGameWorld->mPhysicsSystem->CreateBox( transform->mScale ), transform->mPosition, transform->mOrientation, 1.0f ));
		body = physics->GetRigidBody();
			
		meshComp = (MeshComponent*)obj->AttachComponent( new MeshComponent( mesh ), "Mesh" );
		meshComp->mIsDynamic = true;

		// Box object - Left.
		//
		obj = mGameWorld->AddGameObject( new GameObject(), "Box_Left" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( 10, 5, 5 );
		transform->mScale    = Vector3f( 1, 2, 1 );
		
		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mGameWorld->mPhysicsSystem->CreateRigidBody( mGameWorld->mPhysicsSystem->CreateBox( transform->mScale ), transform->mPosition, transform->mOrientation, 1.0f ));
		body = physics->GetRigidBody();
			
		meshComp = (MeshComponent*)obj->AttachComponent( new MeshComponent( mesh ), "Mesh" );
		meshComp->mIsDynamic = true;

		// Box object - Top.
		//
		obj = mGameWorld->AddGameObject( new GameObject(), "Box_Top" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( 0, 8, 5 );
		transform->mScale    = Vector3f( 1, 1, 1 );
		
		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mGameWorld->mPhysicsSystem->CreateRigidBody( mGameWorld->mPhysicsSystem->CreateBox( transform->mScale ), transform->mPosition, transform->mOrientation, 1.0f ));
		body = physics->GetRigidBody();
			
		meshComp = (MeshComponent*)obj->AttachComponent( new MeshComponent( mesh ), "Mesh" );
		meshComp->mIsDynamic = true;

		// Box object - Back.
		//
		obj = mGameWorld->AddGameObject( new GameObject(), "Box_Back" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition = Vector3f( 0, 3, -10 );
		transform->mScale    = Vector3f( 1, 1, 1 );
		transform->mOrientation = Quatf( 45, 45, 45 );

		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mGameWorld->mPhysicsSystem->CreateRigidBody( mGameWorld->mPhysicsSystem->CreateBox( transform->mScale ), transform->mPosition, transform->mOrientation, 1.0f ));
		body = physics->GetRigidBody();
			
		meshComp = (MeshComponent*)obj->AttachComponent( new MeshComponent( mesh ), "Mesh" );
		meshComp->mIsDynamic = true;
		
		// Dodecahedron.
		//
		meshBuilder.ClearGeometry();
		meshBuilder.CreateDodecahedron( 1.0f );
		meshBuilder.mLayout = shaderTexture->Layout();

		material = mGameWorld->mMaterialManager->Add( "Dodecahedron", SHARED( new Material() ));
		material->mShader = shaderColor; // intentionally set wrong shader to demonstrate vertex layout compatibility
		material->mBlendState = mRenderer->BLEND_ALPHA;
		material->mTexture[ TEXTURE_DIFFUSE ] = mGameWorld->mTextureManager->Get( "default-alpha.png" );

		mesh = mGameWorld->mMeshManager->Add( "Dodecahedron", SHARED( meshBuilder.BuildMesh( mRenderer )));
		mesh->mMaterial = material;
		
		GameObject* obj2 = new GameObject();
		obj2->mName = "Dodecahedron";

		transform = (TransformComponent*)obj2->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition	= Vector3f( 10, 2, -10 );
		transform->mScale		= Vector3f( 1, 1, 1 );
		transform->mOrientation	= Quatf( 0, 0, 0 );

		meshComp = (MeshComponent*)obj2->AttachComponent( new MeshComponent( mesh ), "Mesh" );
		meshComp->mIsDynamic = true;

		//physics = (PhysicsComponent*)obj2->AttachComponent( new PhysicsComponent(), "Physics" );
		//physics->SetRigidBody( mGameWorld->mPhysicsSystem->CreateMesh( mesh->mVBO, transform->mOrientation, transform->mScale, mesh, 1.0f ));
		//body = physics->GetRigidBody();
		
		mGameWorld->AddGameObject( obj2 );
		
		// Sphere.
		//
		meshBuilder.ClearGeometry();
		meshBuilder.CreateSphere( 1.0f, 10, 10 );
		mesh->mLayout = shaderTexture->Layout();

		material = mGameWorld->mMaterialManager->Add( "Sphere", SHARED( new Material() ));
		material->mShader = shaderTexture;
		material->mBlendState = mRenderer->BLEND_ALPHA;
		material->mTexture[ TEXTURE_DIFFUSE ] = mGameWorld->mTextureManager->Get( "default-alpha.png" );

		mesh = mGameWorld->mMeshManager->Add( "Sphere", SHARED( meshBuilder.BuildMesh( mRenderer )));
		mesh->mMaterial = material;
		
		obj = mGameWorld->AddGameObject( new GameObject(), "Sphere" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mPosition	= Vector3f( 20, 2, 30 );
		transform->mScale		= Vector3f( 1, 1, 1 );
		
		meshComp = (MeshComponent*)obj->AttachComponent( new MeshComponent( mesh ), "Mesh" );
		meshComp->mIsDynamic = true;

		physics = (PhysicsComponent*)obj->AttachComponent( new PhysicsComponent(), "Physics" );
		physics->SetRigidBody( mGameWorld->mPhysicsSystem->CreateRigidBody( mGameWorld->mPhysicsSystem->CreateSphere( transform->mScale.x ), transform->mPosition, transform->mOrientation, 1.0f ));
		body = physics->GetRigidBody();
		
		//obj->AddChild( obj2 );
		
		// Testing particle effects.
		//
		std::shared_ptr< Texture > texture = mGameWorld->mTextureManager->Add( "fire.png", SHARED( mRenderer->CreateTextureFromFile( "Textures\\fire.png" )));
		
		material = mGameWorld->mMaterialManager->Add( "Fire", SHARED( new Material() ));
		material->mShader = mGameWorld->mShaderManager->Get( "Sprite" );
		material->mBlendState = SHARED( mRenderer->CreateBlendState( BLEND_SRC_ALPHA, BLEND_ONE, BLEND_SRC_ALPHA, BLEND_ONE ));
		material->mTexture[ TEXTURE_DIFFUSE ] = texture;
		material->mCullMode  = CULL_NONE;
		material->mDepthMode = DEPTH_OFF;

		std::shared_ptr< Sprite > sprite = mGameWorld->mSpriteManager->Add( "Fire", SHARED( new Sprite() ));
		sprite->AddFrame( Sprite::QUADtoTEXCOORD( Quad( 0, 0, 64, 64 ), texture->Width(), texture->Height() ));
		sprite->AddFrame( Sprite::QUADtoTEXCOORD( Quad( 64, 0, 128, 64 ), texture->Width(), texture->Height() ));

		ParticleSystem* particleSystem = BuildParticleSystemNormal( mRenderer, mGameWorld, material, sprite, 300 );
		particleSystem->mSpawnRate   = 0.025f;
		particleSystem->mMinLifetime = 3.0f;
		particleSystem->mMaxLifetime = 5.0f;
		particleSystem->mEffect.push_back( new TextureEffect( 0, 0 ));
		particleSystem->mEffect.push_back( new AreaPositionEffect( 0, Vector3f( -0.125f, 1, 0 ), Vector3f( 0.125f, 1, 0 )));
		particleSystem->mEffect.push_back( new RandomRotationEffect( 0, Vector3f( 0, 0, -10 ), Vector3f( 0, 0, 10 )));
		particleSystem->mEffect.push_back( new ScaleEffect( 0, Vector3f( 1, 1, 1 )));
		particleSystem->mEffect.push_back( new VelocityEffect( 0, Vector3f( 0, 1.2f, 0 )));
		particleSystem->mEffect.push_back( new RandomColorEffect( 0, ColorRGBA( 0.75f, 0.25f, 0, 0.125f ), ColorRGBA( 0.75f, 0.75f, 0, 0.125f )));
		particleSystem->mEffect.push_back( new FadeToScaleEffect( 0, 0.25f, 0.666f ));
		particleSystem->mEffect.push_back( new FadeToScaleEffect( 0.5f, 1.0f, 0.1f ));
		particleSystem->mEffect.push_back( new TextureEffect( 1.0f, 1 ));
		particleSystem->mEffect.push_back( new FadeToScaleEffect( 1.0f, 2.0f, 1.0f ));
		particleSystem->mEffect.push_back( new RandomColorEffect( 1.0f, ColorRGBA( 1, 1, 1, 0.125f ), ColorRGBA( 0.9f, 0.9f, 0.9f, 0.125f )));
		particleSystem->mEffect.push_back( new FadeToColorEffect( 1.0f, 5.0f, ColorRGBA( 0.9f, 0.9f, 0.9f, 0.025f )));
		
		obj = mGameWorld->AddGameObject( new GameObject(), "Particle Emitter" );

		transform = (TransformComponent*)obj->AttachComponent( new TransformComponent(), "Transform" );
		transform->mScale = Vector3f( 512, 512, 1 );

		obj->AttachComponent( new ParticleEmitterComponent( particleSystem ), "Sprite" );

		///////////////////////////////

		mGameWorld->Startup();

		///////////////////////////////
		// CREATE EDITOR OBJECTS
		//
		CreateTranslateObject();

		CreateRotateMesh();
			
		CreateScaleMesh();
	}
		
	void CreateTranslateObject()
	{
		mTranslateObject = new GameObject();
		const float tileSize = 5;

		std::shared_ptr< Model > model = SHARED( LoadModelM3DFromFile( "Editor\\Translate.M3D", mRenderer, mGameWorld->mShaderManager, mGameWorld->mTextureManager, mGameWorld->mMaterialManager ));
		
		std::vector< std::shared_ptr< Material >> material;
		model->GetMaterials( &material );

		material[ 0 ]->mShader = mEditorWorld->mShaderManager->Get( "Color_Only" );
		material[ 0 ]->mCullMode = CULL_NONE;
		material[ 0 ]->mBlendState = SHARED( mRenderer->CreateBlendState( BLEND_SRC_ALPHA, BLEND_ONE, BLEND_SRC_ALPHA, BLEND_ONE ));
		material[ 0 ]->mDepthMode = DEPTH_ALWAYS;
		
		model->SetMaterials( material );

		// Root Translate Object.
		//
		mTranslateObject->AttachComponent( new TransformComponent(), "Transform" );
			
		///////////////////////////////

		GameObject* obj = new GameObject();
			
		TransformComponent* transform = new TransformComponent();
		transform->mPosition    = Vector3f( tileSize, 0, 0 );
		transform->mOrientation = Quatf( Vector3f( 0, 0, -90 ));
		obj->AttachComponent( transform, "Tile_X" );

		ModelComponent* modelComp = new ModelComponent( model );
		//modelComp->SetMaterial( mSelectedMaterialX );
		obj->AttachComponent( modelComp, "Model_X" );

		mTranslateObject->AddChild( obj );

		///////////////////////////////
			
		obj = new GameObject();

		transform = new TransformComponent();
		transform->mPosition    = Vector3f( 0, tileSize, 0 );
		transform->mOrientation = Quatf( Vector3f( 0, 0, 0 ));
		obj->AttachComponent( transform, "Tile_Y" );

		modelComp = new ModelComponent( model );
		//modelComp->SetMaterial( mSelectedMaterialY );
		obj->AttachComponent( modelComp, "Model_Y" );

		mTranslateObject->AddChild( obj );

		///////////////////////////////

		obj = new GameObject();

		transform = new TransformComponent();
		transform->mPosition    = Vector3f( 0, 0, tileSize );
		transform->mOrientation = Quatf( Vector3f( 90, 0, 0 ));
		obj->AttachComponent( transform, "Tile_Z" );

		modelComp = new ModelComponent( model );
		//modelComp->SetMaterial( mSelectedMaterialZ );
		obj->AttachComponent( modelComp, "Model_Z" );

		mTranslateObject->AddChild( obj );

		///////////////////////////////

		mTranslateObject->SetWorld( mGameWorld );
		mTranslateObject->Startup();
	}

	void DrawTranslateObject()
	{
		if( mSelectedObject )
		{
			TransformComponent* transformObject = (TransformComponent*)mTranslateObject->FindComponent( GameComponent::TRANSFORM );
			TransformComponent* transformSelect = (TransformComponent*)mSelectedObject->FindComponent( GameComponent::TRANSFORM );

			transformObject->mPosition = transformSelect->mPosition;
			transformObject->mScale    = Vector3f::ONE * (mGameWorld->GetCamera()->GetTransform()->mPosition - transformSelect->mPosition).Length() * 0.005f;
			
			mTranslateObject->UpdateTransform();
			mTranslateObject->UpdateDrawable();
		}
	}

	void CreateRotateMesh()
	{}

	void CreateScaleMesh()
	{}

	void CheckBounds( GameObject* obj )
	{
		DrawableComponent* drawable = (DrawableComponent*)obj->FindComponent( GameComponent::DRAWABLE );

		if( drawable )
		{
			const BoundingBox& box = drawable->GetBounds();

			Vector3f center = (box.GetMaxBounds() + box.GetMinBounds()) * 0.5f;
			Vector3f scale  = (box.GetMaxBounds() - center);
			Vector3f intersection;

			if( box.Intersects( mMouseToWorldRay, &intersection ))
			{
				float dist = (mMouseToWorldRay.mPosition - intersection).LengthSquared();

				if( mHoverDist > dist )
				{
					mHoverObject = obj;
					mHoverDist = dist;
				}
			}
		}

		UINT count = obj->NumChildren();

		for( UINT x = 0; x < count; ++x )
		{
			CheckBounds( obj->GetChild( x ));
		}
	}

	void DrawBounds( GameObject* obj )
	{
		DrawableComponent* drawable = (DrawableComponent*)obj->FindComponent( GameComponent::DRAWABLE );

		if( drawable )
		{
			const BoundingBox& box = drawable->GetBounds();

			Vector3f center = (box.GetMaxBounds() + box.GetMinBounds()) * 0.5f;
			Vector3f scale  = (box.GetMaxBounds() - center);

			if( obj != mHoverObject )
			{
				mBoundsMesh->mMaterial->mAmbient = ColorRGBA::WHITE;

				if( obj == mSelectedObject )
					mBoundsMesh->mMaterial->mAmbient = ColorRGBA::RED;
			}
			else
			if( obj == mSelectedObject )
				mBoundsMesh->mMaterial->mAmbient = ColorRGBA::YELLOW;
			else
				mBoundsMesh->mMaterial->mAmbient = ColorRGBA::GREEN;

			mBoundsMesh->mMatrixWorld.World( center, Quatf::IDENTITY, scale );
			
			mBoundsMesh->Draw( mRenderer, mGameWorld );
		}

		UINT count = obj->NumChildren();

		for( UINT x = 0; x < count; ++x )
		{
			DrawBounds( obj->GetChild( x ));
		}
	}

	void SelectGameObject()
	{
		// Cast a ray into the world to select objects.
		//
		POINT mousePos = Mouse::Get().GetPosition( (HWND)mGameWindow->GetInfo()->Handle() );

		GUI::ModelWidget* model = (GUI::ModelWidget*)mGameWorldWidget->GetModel();
		const Vector3f& worldPos   = model->mPosition;
		const Vector3f& worldScale = model->mScale;

		mMouseToWorldRay = mGameWorld->GetCamera()->ScreenPointToRay( mousePos.x - (UINT)worldPos.x, mousePos.y - (UINT)worldPos.y, (UINT)worldScale.x, (UINT)worldScale.y );
				
		UINT count = mGameWorld->NumGameObjects();

		mHoverObject = NULL;
		mHoverDist = FLT_MAX;

		for( UINT x = 0; x < count; ++x )
		{
			CheckBounds( mGameWorld->GetGameObject( x ));
		}

		if( Mouse::Get().DidGoDown( BUTTON_LEFT ))
		{
			mSelectedObject = mHoverObject;
		}

		for( UINT x = 0; x < count; ++x )
		{
			DrawBounds( mGameWorld->GetGameObject( x ));
		}
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
	//_CrtSetBreakAlloc( 1260 );

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
