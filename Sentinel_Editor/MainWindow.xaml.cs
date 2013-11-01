using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Interop;
using System.Windows.Threading;
using System.ComponentModel;
using System.Collections.ObjectModel;

using System.Runtime.InteropServices;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	///
	/// Editor Window
	///
	public partial class MainWindow : Window
	{
		private static WColorRGBA   CLEAR_COLOR = new WColorRGBA( 0.0f, 0.2f, 0.8f, 1.0f );

		private static TreeViewItem	ASSET_TEXTURE;
		private static TreeViewItem	ASSET_SHADER;
		private static TreeViewItem	ASSET_MESH;
		private static TreeViewItem	ASSET_MODEL;
		private static TreeViewItem	ASSET_AUDIO;

		private GameWindow			mGameWindow;

		private EditorAsset			mSelectedAsset;

		private WGameObject			mSelectedObject;

		private WGameObject			mTranslateObject;
		private WGameObject			mRotateObject;
		private WGameObject			mScaleObject;
		
		private WMaterial			mMaterial_X_Axis;
		private WMaterial			mMaterial_Y_Axis;
		private WMaterial			mMaterial_Z_Axis;
		private WMaterial			mMaterial_Selected;

		private const float			TRANSFORM_OBJECT_ALPHA = 0.75f;
		private const float			TRANSFORM_OBJECT_SCALE = 0.0025f;

		private String				mMapName;

		/// 
		/// Necessary objects to create a Game World.
		/// 
		private WRenderer			mRenderer;
		private WTiming				mTiming;
		private WPhysicsSystem		mPhysicsSystem;
		private WAudioSystem		mAudioSystem;

		private WTextureManager		mTextureManager;
		private WShaderManager		mShaderManager;
		private WMeshManager		mMeshManager;
		private WModelManager		mModelManager;
		private WSoundManager		mSoundManager;

		private WGameWorld			mGameWorld;

		///
		/// Starting Point of Application.
		///
		public MainWindow()
		{
			try
			{
				InitializeComponent();
			}
			catch( Exception e )
			{
				MessageBox.Show( e.Message );
			}
		}

		#region Window Initialization and Shutdown
		private void Window_Loaded( Object sender, RoutedEventArgs e )
		{
			// Initialize Resources.
			//
			Inspector.TreeStyle   = (Style)FindResource( "Inspector_TreeStyle" );
			Inspector.TextStyle   = (Style)FindResource( "Inspector_TextStyle" );

			EditorAsset.TreeStyle = (Style)FindResource( "Assets_TreeStyle" );

			ATexture.DefaultImage = new BitmapImage( new Uri( "pack://application:,,,/Resources/Asset_Image.png" ));
			AShader.DefaultImage  = new BitmapImage( new Uri( "pack://application:,,,/Resources/Asset_Shader.png" ));
			AMesh.DefaultImage    = new BitmapImage( new Uri( "pack://application:,,,/Resources/Asset_Mesh.png" ));
			AModel.DefaultImage   = new BitmapImage( new Uri( "pack://application:,,,/Resources/Asset_Model.png" ));
			AAudio.DefaultImage   = new BitmapImage( new Uri( "pack://application:,,,/Resources/Asset_Audio.png" ));

			// Initialize Renderer.
			//
			WWindowInfo info = new WWindowInfo();
			mRenderer = WRenderer.Create( "config.xml", ref info );

			if( mRenderer == null )
			{
				MessageBox.Show( "Failed to load Renderer config.xml", "Application Failure" );

				System.Environment.Exit( 0 );
			}

			mGameWorld = new WGameWorld();
			
			mTiming        = new WTiming();
			mPhysicsSystem = WPhysicsSystem.BuildSE();
			mAudioSystem   = WAudioSystem.BuildAL();
			
			mGameWindow = new GameWindow( mGameWorld );
			mGameWindow.Startup( mRenderer, "World", "WorldClass", info );
			mGameWindow.SetCameraPosition( new WVector3f( 0, 25, 25 ));
			mGameWindow.SetCameraRotation( new WVector3f( -45, 0, 0 ));
			
			Window_World.Child = mGameWindow;
			
			mMapName = "Default.MAP";
			Window_Main.Title = "Sentinel Editor - " + mMapName;

			CompositionTarget.Rendering += Update;
			
			// Create editor materials.
			//
			mMaterial_X_Axis   = new WMaterial( new WColorRGBA( 1, 0, 0, TRANSFORM_OBJECT_ALPHA ), new WColorRGBA( 0, 0, 0, 0 ), new WColorRGBA( 0, 0, 0, 0 ), 0 );
			mMaterial_Y_Axis   = new WMaterial( new WColorRGBA( 0, 1, 0, TRANSFORM_OBJECT_ALPHA ), new WColorRGBA( 0, 0, 0, 0 ), new WColorRGBA( 0, 0, 0, 0 ), 0 );
			mMaterial_Z_Axis   = new WMaterial( new WColorRGBA( 0, 0, 1, TRANSFORM_OBJECT_ALPHA ), new WColorRGBA( 0, 0, 0, 0 ), new WColorRGBA( 0, 0, 0, 0 ), 0 );

			mMaterial_Selected = new WMaterial( new WColorRGBA( 1, 0.5f, 1, 1 ), new WColorRGBA( 0, 0, 0, 0 ), new WColorRGBA( 0, 0, 0, 0 ), 0 );

			// Create asset trees.
			//
			CreateAssetTree( ref ASSET_TEXTURE, "Texture" );
			CreateAssetTree( ref ASSET_SHADER,	"Shader" );
			CreateAssetTree( ref ASSET_MESH,	"Mesh" );
			CreateAssetTree( ref ASSET_MODEL,	"Model" );
			CreateAssetTree( ref ASSET_AUDIO,	"Audio" );

			// Prepare Game World.
			//
			PrepareShaders();
			PrepareObjects();
		}

		private void Window_Closing( Object sender, CancelEventArgs e )
		{
			MessageBoxResult result = MessageBox.Show( "Save before Exiting?",
													   "Exit Application",
													   MessageBoxButton.YesNoCancel,
													   MessageBoxImage.Question );

			switch( result )
			{
				case MessageBoxResult.Yes:
					Save_Click( sender, null );
					break;

				case MessageBoxResult.No:
					break;

				case MessageBoxResult.Cancel:
					e.Cancel = true;
					break;
			}
		}

		private void Window_Closed( Object sender, EventArgs e )
		{
			mTranslateObject.Shutdown();
			mTranslateObject.Release();

			mRotateObject.Shutdown();
			mRotateObject.Release();

			mScaleObject.Shutdown();
			mScaleObject.Release();

			foreach( AAudio item in ASSET_AUDIO.Items )
				item.Data.Dispose();

			foreach( AModel item in ASSET_MODEL.Items )
				item.Data.Dispose();

			foreach( AMesh item in ASSET_MESH.Items )
				item.Data.Dispose();

			foreach( AShader item in ASSET_SHADER.Items )
				item.Data.Dispose();

			foreach( ATexture item in ASSET_TEXTURE.Items )
				item.Data.Dispose();

			mGameWorld.Shutdown();
			mGameWorld.Release();

			mModelManager.Release();
			mMeshManager.Release();
			mShaderManager.Release();
			mTextureManager.Release();
			mSoundManager.Release();

			mTiming.Release();
			mAudioSystem.Release();
			mPhysicsSystem.Release();
			
			mGameWindow.Shutdown();

			mRenderer.Shutdown();
			mRenderer.Release();
		}
		#endregion

		#region Game World Rendering
		private void Update( Object sender, EventArgs e )
		{
			WMouse.Update();

			mRenderer.SetDepthStencil( 0 );
			mRenderer.SetViewport( 0, 0, mGameWindow.GetInfo().Width(), mGameWindow.GetInfo().Height() );
			mRenderer.SetRenderTarget( 0 );

			mRenderer.Clear( CLEAR_COLOR );

			mRenderer.SetCull( CullType.CCW );
			mRenderer.SetBlend( BlendType.ALPHA );
			mRenderer.SetDepthStencilState( StencilType.DEFAULT );

			mGameWindow.Update();

			DrawSelection();

			mRenderer.Present();
		}

		private void DrawSelection()
		{
			// Draw Object Transform Tool.
			//
			if( mSelectedObject != null )
			{
				mRenderer.SetBlend( BlendType.PARTICLE );
				mRenderer.SetDepthStencilState( StencilType.NO_ZBUFFER );

				WTransformComponent selectedObjectTransform = WTransformComponent.Cast( mSelectedObject.FindComponent( ComponentType.TRANSFORM ));

				if( selectedObjectTransform != null )
				{
					WVector3f selectedObjectPosition = selectedObjectTransform.GetMatrixWorld().Transform( new WVector3f( 0, 0, 0 ));

					if( Select_Object.IsChecked == true )
					{
						// Draw wireframe around the selected object.
						//
						WGameComponent component = mSelectedObject.FindComponent( ComponentType.DRAWABLE );
						
						if( component != null )
						{
							mRenderer.SetFill( FillType.WIREFRAME );

							WMeshComponent meshComp = WMeshComponent.Cast( component );
							
							if( meshComp != null )
							{
								WMaterial material = new WMaterial( meshComp.Material );
								meshComp.Material  = mMaterial_Selected;

								WMesh   mesh   = meshComp.Mesh;
								WShader shader = mesh.Shader;
								WShader color  = mShaderManager.Get( "Color Only" );

								mesh.Shader = color;

								mSelectedObject.UpdateDrawable( false );

								meshComp.Material = material;
								mesh.Shader = shader;

								color.Dispose();
								shader.Dispose();
								mesh.Dispose();
							}
							else
							{
								WModelComponent modelComp = WModelComponent.Cast( component );

								if( modelComp != null )
								{
									WStdVector_Material material = modelComp.GetMaterial();
									modelComp.SetMaterial( mMaterial_Selected );
									
									WModel model = modelComp.Model;
									//WShader shader = model.Shader;
									
									mSelectedObject.UpdateDrawable( false );

									modelComp.SetMaterial( material );

									model.Dispose();
								}
							}

							mRenderer.SetFill( FillType.SOLID );
						}
					}
					else
					if( Translate_Object.IsChecked == true )
					{
						// Draw the translate object.
						//
						WTransformComponent transform = WTransformComponent.Cast( mTranslateObject.FindComponent( ComponentType.TRANSFORM ));
						transform.Position = selectedObjectPosition;

						// Scale object based on distance so it remains of equal size throughout the world view.
						//
						WVector3f distance = mGameWindow.GetCamera().GetTransform().Position - transform.Position;
						distance.x = Math.Abs( distance.x );
						distance.y = Math.Abs( distance.y );
						distance.z = Math.Abs( distance.z );
						float d = distance.Length() * TRANSFORM_OBJECT_SCALE;
						transform.Scale = new WVector3f( d, d, d );

						mRenderer.SetCull( CullType.NONE );
						
						//WMeshComponent.Cast( mTranslateObject.GetChild( 0 ).FindComponent( ComponentType.DRAWABLE )).Material = mMaterial_Selected;
						mTranslateObject.UpdateTransform();
						mTranslateObject.UpdateDrawable();
					}
				}
			}
		}
		#endregion

		#region Game Objects Tree
		///
		/// Game Objects Tree.
		/// 
		private void AddObject( WGameObject obj, OGameObject item = null )
		{
			if( item == null )
			{
				item = new OGameObject( obj );

				// Add the children.
				//
				for( uint x = 0; x < obj.NumChildren(); ++x )
					AddObject( obj.GetChild( x ), item );

				Objects_TreeView.Items.Add( item );
			}
			else
			{
				OGameObject child = new OGameObject( obj );
				
				// Add the children.
				//
				for( uint x = 0; x < obj.NumChildren(); ++x )
					AddObject( obj.GetChild( x ), child );

				item.Items.Add( child );
			}
		}

		private void Objects_TreeView_Selected( Object sender, RoutedEventArgs e )
		{
			OGameObject item = Objects_TreeView.SelectedItem as OGameObject;
			
			mSelectedObject = item.Data;

			Inspector_TreeView.Items.Clear();
			Inspector_TreeView.Items.Add( new IGameObject( mSelectedObject ));
		}
		#endregion

		#region Objects_TreeView Drag and Drop
		///
		/// Objects_TreeView Drag and Drop
		///
		private Point		Objects_LastMouseDown;
		private OGameObject Objects_DraggedItem;
		private OGameObject Objects_Target;

		private void Objects_TreeView_MouseDown( Object sender, MouseButtonEventArgs e )
		{
			// Get the last position the mouse was left clicked.
			//
			if( e.ChangedButton == System.Windows.Input.MouseButton.Left )
				Objects_LastMouseDown = e.GetPosition( Objects_TreeView );
		}

		private void Objects_TreeView_MouseMove( Object sender, MouseEventArgs e )
		{
			try
			{
				// Move objects in Objects Tree using the 
				// left mouse button to drag and drop items.
				//
				if( e.LeftButton == MouseButtonState.Pressed )
				{
					Point currentPosition = e.GetPosition( Objects_TreeView );
                
					if( Math.Abs( currentPosition.X - Objects_LastMouseDown.X ) > SystemParameters.MinimumHorizontalDragDistance ||
						Math.Abs( currentPosition.Y - Objects_LastMouseDown.Y ) > SystemParameters.MinimumVerticalDragDistance )
					{
						Objects_DraggedItem = Objects_TreeView.SelectedItem as OGameObject;

						if( Objects_DraggedItem != null )
						{
							DragDropEffects finalDropEffect = DragDrop.DoDragDrop( Objects_TreeView, Objects_TreeView.SelectedValue, DragDropEffects.Move );

							if( finalDropEffect == DragDropEffects.Move )
							{
								if( Objects_DraggedItem != Objects_Target )
								{
									if( Objects_DraggedItem.Parent != Objects_TreeView )
									{
										(Objects_DraggedItem.Parent as OGameObject).Items.Remove( Objects_DraggedItem );

										if( Objects_DraggedItem.Data.Parent != null )
											Objects_DraggedItem.Data.Parent.RemoveChild( Objects_DraggedItem.Data );
									}
									else
										Objects_TreeView.Items.Remove( Objects_DraggedItem );
									
									if( Objects_Target == null )
										mGameWorld.AddGameObject( Objects_DraggedItem.Data );
									else
										Objects_Target.Data.AddChild( Objects_DraggedItem.Data );
									
									AddObject( Objects_DraggedItem.Data, Objects_Target );

									Objects_DraggedItem.Data.Startup();
									
									Objects_DraggedItem = null;
									Objects_Target      = null;
								}
							}
						}
					}
				}
			}
			catch( Exception )
			{
				Objects_DraggedItem = null;
				Objects_Target      = null;
			}
		}

		private void Objects_TreeView_DragOver( Object sender, DragEventArgs e )
		{
			try
			{
				// Determine if the object has a new parent after
				// dragging over an object or the tree.
				//
				Point currentPosition = e.GetPosition( Objects_TreeView );
      
				if( Math.Abs( currentPosition.X - Objects_LastMouseDown.X ) > SystemParameters.MinimumHorizontalDragDistance ||
					Math.Abs( currentPosition.Y - Objects_LastMouseDown.Y ) > SystemParameters.MinimumVerticalDragDistance )
				{
					e.Effects = DragDropEffects.None;

					OGameObject item = e.Source as OGameObject;
					if( item != null )
					{
						if( Objects_DraggedItem != item )
						{
							OGameObject parent = item.Parent as OGameObject;
							while( parent != null )
							{
								if( parent == Objects_DraggedItem )
								{
									e.Handled = true;
									return;
								}

								parent = parent.Parent as OGameObject;
							}

							e.Effects = DragDropEffects.Move;
						}
					}
				}

				e.Handled = true;
			}
			catch( Exception )
			{
				e.Effects = DragDropEffects.None;
				e.Handled = true;
			}
		}

		private void Objects_TreeView_Drop( Object sender, DragEventArgs e )
		{
			try
			{
				// Determines what object is the target of
				// the drag and drop.
				//
				e.Effects = DragDropEffects.None;
				e.Handled = true;
            
				OGameObject item = e.Source as OGameObject;
				if( item != null && Objects_DraggedItem != null )
				{
					Objects_Target = item;
					e.Effects = DragDropEffects.Move;
				}
				else
				{
					Objects_Target = null;
				}
			}
			catch( Exception )
			{
				Objects_Target = null;
			}
		}
		#endregion

		#region Assets Tree
		private void AssetsFolderSelected( Object sender, RoutedEventArgs e )
		{
			(sender as TreeViewItem).IsSelected = false;
			e.Handled = true;
		}

		private void CreateAssetTree( ref TreeViewItem item, String name )
		{
			item = new TreeViewItem();
			item.Header		= name;
			item.Focusable	= false;
			item.Selected	+= AssetsFolderSelected;
			Assets_TreeView.Items.Add( item );
		}

		private void AssetSelected( Object sender, RoutedEventArgs e )
		{
			mSelectedAsset = sender as EditorAsset;
		}

		private void AddAsset( String name, WTexture texture )
		{
			ATexture item = new ATexture( name, texture );
			item.Selected += AssetSelected;
			
			ASSET_TEXTURE.Items.Add( item );
		}

		private void AddAsset( String name, WShader shader )
		{
			AShader item = new AShader( name, shader );
			item.Selected += AssetSelected;
			
			ASSET_SHADER.Items.Add( item );
		}

		private void AddAsset( String name, WMesh mesh )
		{
			AMesh item = new AMesh( name, mesh );
			item.Selected += AssetSelected;
			
			ASSET_MESH.Items.Add( item );
		}

		private void AddAsset( String name, WModel model )
		{
			AModel item = new AModel( name, model );
			item.Selected += AssetSelected;

			ASSET_MODEL.Items.Add( item );
		}

		private void AddAsset( String name, WSound source )
		{
			AAudio item = new AAudio( name, source );
			item.Selected += AssetSelected;

			ASSET_AUDIO.Items.Add( item );
		}

		///
		/// Find an Asset by Type.
		///
		static public AMesh FindAsset( WMesh mesh )
		{
			foreach( AMesh item in ASSET_MESH.Items )
				if( item.Data == mesh )
					return item;
			
			return null;
		}

		///
		/// Context Menu for Assets
		///
		private void Assets_AddTexture( Object sender, RoutedEventArgs e )
		{
			System.Windows.Forms.OpenFileDialog dialog = new System.Windows.Forms.OpenFileDialog();

			dialog.Title  = "Add Texture to Project";
			dialog.Filter = "PNG|*.png|All Files|*.*";
			dialog.FilterIndex = 1;

			//dialog.Multiselect = true;

			if( dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK )
			{
				WTexture texture = mRenderer.CreateTextureFromFile( dialog.FileName );
				String name  = Path.GetFileName( dialog.FileName );
				mTextureManager.Add( name, texture );
				AddAsset( name, texture );
			}
		}

		private void Assets_AddShader( Object sender, RoutedEventArgs e )
		{
			/*
			System.Windows.Forms.OpenFileDialog dialog = new System.Windows.Forms.OpenFileDialog();

			dialog.Title  = "Add Texture to Project";
			dialog.Filter = "PNG|*.png|All Files|*.*";
			dialog.FilterIndex = 1;

			//dialog.Multiselect = true;

			if( dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK )
			{
				WTexture texture = mRenderer.CreateTextureFromFile( dialog.FileName );
				String name  = Path.GetFileName( dialog.FileName );
				mTextureManager.Add( name, texture );
				AddAsset( name, texture );
			}
			*/
		}

		private void Assets_AddMesh( Object sender, RoutedEventArgs e )
		{}

		private void Assets_AddModel( Object sender, RoutedEventArgs e )
		{
			System.Windows.Forms.OpenFileDialog dialog = new System.Windows.Forms.OpenFileDialog();

			dialog.Title  = "Add Shader to Project";
			dialog.Filter = "Sentinel Model|*.M3D|Wavefront|*.OBJ|All Files|*.*";
			dialog.FilterIndex = 1;

			if( dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK )
			{
				WModel model = WModel.Load( dialog.FileName, mRenderer, mShaderManager, mTextureManager );
				String name  = Path.GetFileName( dialog.FileName );
				mModelManager.Add( name, model );
				AddAsset( name, model );
			}
		}

		private void Assets_AddSound( Object sender, RoutedEventArgs e )
		{
			System.Windows.Forms.OpenFileDialog dialog = new System.Windows.Forms.OpenFileDialog();

			dialog.Title  = "Add Sound to Project";
			dialog.Filter = "WAV|*.wav|OGG|*.ogg|All Files|*.*";
			dialog.FilterIndex = 1;

			//dialog.Multiselect = true;

			if( dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK )
			{
				WSound source = mAudioSystem.CreateSound( dialog.FileName );
				String name = Path.GetFileName( dialog.FileName );
				mSoundManager.Add( name, source );
				AddAsset( name, source );
			}
		}

		private void Assets_Refresh( Object sender, RoutedEventArgs e )
		{
			// TODO: Refresh Assets TreeView
		}
		#endregion

		#region Object Preparation
		///
		/// Prepare Shaders.
		///
		private void PrepareShaders()
		{
			Directory.SetCurrentDirectory( "Assets\\Shaders" );

			mShaderManager = new WShaderManager();
			if( !WShaderManager.LoadConfig( "config.xml", mRenderer, mShaderManager ))
			{
				MessageBox.Show( "Failed to load 'Assets\\Shaders\\config.xml'", "Shader Load Failure" );
				System.Environment.Exit( -1 );
			}
			
			List< String >  names   = new List< String >();
			List< WShader > shaders = new List< WShader >();

			mShaderManager.GetAll( ref names, ref shaders );

			for( int x = 0; x < names.Count; ++x )
			{
				AddAsset( names[ x ], shaders[ x ] );
				shaders[ x ].Dispose();
			}
			
			Directory.SetCurrentDirectory( "..\\.." );
		}

		///
		/// Prepare Game Objects.
		///
		private void PrepareObjects()
		{
			WMeshBuilder			meshBuilder = new WMeshBuilder();
			WGameObject				obj;
			WTransformComponent		transform;
			WPhysicsComponent		physics;
			WRigidBody				body;
			WMesh					mesh;

			mTextureManager		= new WTextureManager();
			mMeshManager		= new WMeshManager();
			mModelManager		= new WModelManager();
			mSoundManager		= new WSoundManager();

			mGameWorld.XRenderer			= mRenderer;
			mGameWorld.XTiming				= mTiming;
			mGameWorld.XPhysicsSystem		= mPhysicsSystem;
			mGameWorld.XTextureManager		= mTextureManager;
			mGameWorld.XShaderManager		= mShaderManager;
			mGameWorld.XMeshManager			= mMeshManager;
			mGameWorld.XModelManager		= mModelManager;
			mGameWorld.XSoundManager		= mSoundManager;

			////////////////////////////////////

			WTexture texture = mTextureManager.Add( "default-alpha.png", mRenderer.CreateTextureFromFile( "Assets\\Textures\\default-alpha.png" ));
			AddAsset( "default-alpha.png", texture );
			
			// Camera.
			//
			obj = mGameWorld.AddGameObject( new WGameObject(), "Main_Camera" );

			transform = (WTransformComponent)obj.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position = new WVector3f( 0, 25, 25 );
			transform.Rotation = new WVector3f( -45, 0, 0 );

			obj.AttachComponent( new WPlayerControllerComponent(), "Controller" );
			
			physics = (WPhysicsComponent)obj.AttachComponent( new WPhysicsComponent(), "Physics" );
			physics.SetRigidBody( mPhysicsSystem.CreateSphere( transform.Position, new WQuatf( transform.Rotation ), 1.0f, 1.0f ));
			body = physics.GetRigidBody();
			body.ShapeType		= PhysicsShapeType.SPHERE;
			body.Flags			= (int)PhysicsFlag.DISABLE_GRAVITY;
			body.Restitution	= 1.0f;
			body.SetDamping( 0.9f, 0.9f );
			
			obj.AttachComponent( new WPerspectiveCameraComponent( mGameWindow.GetInfo().Width(), mGameWindow.GetInfo().Height() ), "Camera" );

			AddObject( obj );
			
			// Point Light.
			//
			obj = mGameWorld.AddGameObject( new WGameObject(), "Point_Light" );

			transform = (WTransformComponent)obj.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position = new WVector3f( 0, 10, 0 );

			WLightComponent light = (WLightComponent)obj.AttachComponent( new WLightComponent(), "Light" );
			light.Attenuation = new WVector4f( 1, 1, 1, 25 );

			AddObject( obj );
			
			// Ground object.
			//
			meshBuilder.CreateCube( 1.0f );
			meshBuilder.Shader    = mShaderManager.Get( "Color" );
			meshBuilder.Primitive = PrimitiveType.TRIANGLE_LIST;

			mesh = mMeshManager.Add( "Ground", meshBuilder.BuildMesh( mRenderer ));
			AddAsset( "Ground", mesh );

			obj = mGameWorld.AddGameObject( new WGameObject(), "Ground" );

			transform = (WTransformComponent)obj.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position	 = new WVector3f( 0, 0, 0 );
			transform.Scale		 = new WVector3f( 100, 1, 100 );

			physics = (WPhysicsComponent)obj.AttachComponent( new WPhysicsComponent(), "Physics" );
			physics.SetRigidBody( mPhysicsSystem.CreateBox( transform.Position, new WQuatf( transform.Rotation ), transform.Scale, 0.0f ));
			body = physics.GetRigidBody();
			body.ShapeType	= PhysicsShapeType.BOX;
			body.Flags		= (int)PhysicsFlag.DISABLE_GRAVITY;
			
			obj.AttachComponent( new WMeshComponent( mesh ), "Mesh" );
			
			AddObject( obj );
			
			// Test object.
			//
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

			///////////////////////////////
			// CREATE EDITOR OBJECTS

			CreateTranslateMesh();

			CreateRotateMesh();
			
			CreateScaleMesh();
		}
		
		private void CreateTranslateMesh()
		{
			mTranslateObject = new WGameObject();
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
		}

		private void CreateRotateMesh()
		{
			mRotateObject = new WGameObject();

			mRotateObject.SetWorld( mGameWorld );
			mRotateObject.Startup();
		}

		private void CreateScaleMesh()
		{
			mScaleObject = new WGameObject();

			mScaleObject.SetWorld( mGameWorld );
			mScaleObject.Startup();
		}
		#endregion

		#region Menu and Toolbar Clicks
		///
		/// File
		///
		private void New_Click( Object sender, RoutedEventArgs e )
		{
			MessageBox.Show( "You clicked 'New...'" );
		}

		private void Open_Click( Object sender, RoutedEventArgs e )
		{
			System.Windows.Forms.OpenFileDialog dialog = new System.Windows.Forms.OpenFileDialog();

			dialog.Title  = "Open Map";
			dialog.Filter = "Sentinel Level|*.MAP|All Files|*.*";
			dialog.FilterIndex = 1;

			if( dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK )
			{
				mMapName = dialog.FileName;
				Window_Main.Title = "Sentinel Editor - " + Path.GetFileName( mMapName );
				
				// Clear current world and asset manager references.
				//
				foreach( AAudio item in ASSET_AUDIO.Items )
					item.Data.Dispose();
				ASSET_AUDIO.Items.Clear();

				foreach( AModel item in ASSET_MODEL.Items )
					item.Data.Dispose();
				ASSET_MODEL.Items.Clear();

				foreach( AMesh item in ASSET_MESH.Items )
					item.Data.Dispose();
				ASSET_MESH.Items.Clear();

				foreach( AShader item in ASSET_SHADER.Items )
					item.Data.Dispose();
				ASSET_SHADER.Items.Clear();

				foreach( ATexture item in ASSET_TEXTURE.Items )
					item.Data.Dispose();
				ASSET_TEXTURE.Items.Clear();

				// Load each manager.
				//
				WArchive archive = new WArchive();
				archive.Open( mMapName, "rb" );

				List< String > names = new List< String >();

				mTextureManager.Load( archive, mRenderer );

				List< WTexture > texture = new List< WTexture >();
				mTextureManager.GetAll( ref names, ref texture );

				for( int x = 0; x < names.Count; ++x )
				{
					AddAsset( names[ x ], texture[ x ] );
				}

				mShaderManager.Load( archive, mRenderer );

				names.Clear();
				List< WShader > shaders = new List< WShader >();
				mShaderManager.GetAll( ref names, ref shaders );

				for( int x = 0; x < names.Count; ++x )
				{
					AddAsset( names[ x ], shaders[ x ] );
				}

				mMeshManager.Load( archive, mRenderer, mShaderManager, mTextureManager );

				names.Clear();
				List< WMesh > mesh = new List< WMesh >();
				mMeshManager.GetAll( ref names, ref mesh );

				for( int x = 0; x < names.Count; ++x )
				{
					AddAsset( names[ x ], mesh[ x ] );
				}

				mModelManager.Load( archive, mRenderer, mShaderManager, mTextureManager );

				names.Clear();
				List< WModel > model = new List< WModel >();
				mModelManager.GetAll( ref names, ref model );

				for( int x = 0; x < names.Count; ++x )
				{
					AddAsset( names[ x ], model[ x ] );
				}

				mSoundManager.Load( archive, mAudioSystem );

				names.Clear();
				List< WSound > source = new List< WSound >();
				mSoundManager.GetAll( ref names, ref source );

				for( int x = 0; x < names.Count; ++x )
				{
					AddAsset( names[ x ], source[ x ] );
				}

				Objects_TreeView.Items.Clear();

				// Load the world.
				//
				mGameWorld.Load( archive );
				mGameWorld.Startup();

				mGameWindow.SetCamera();

				uint count = mGameWorld.NumGameObjects();
				for( uint x = 0; x < count; ++x )
				{
					AddObject( mGameWorld.GetGameObject( x ));
				}

				archive.Close();
			}
		}

		private void Save_Click( Object sender, RoutedEventArgs e )
		{
			WArchive archive = new WArchive();
			archive.Open( mMapName, "wb+" );

			mTextureManager.Save( archive, mRenderer );
			mShaderManager.Save( archive );
			mMeshManager.Save( archive, mRenderer, mShaderManager, mTextureManager );
			mModelManager.Save( archive, mRenderer, mShaderManager, mTextureManager );
			mSoundManager.Save( archive );

			mGameWorld.Save( archive );

			archive.Close();
		}

		private void SaveAs_Click( Object sender, RoutedEventArgs e )
		{
			System.Windows.Forms.SaveFileDialog dialog = new System.Windows.Forms.SaveFileDialog();

			dialog.Title  = "Save Map";
			dialog.Filter = "Sentinel Level|*.MAP|All Files|*.*";
			dialog.FilterIndex = 1;

			if( dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK )
			{
				mMapName = dialog.FileName;
				Window_Main.Title = "Sentinel Editor - " + Path.GetFileName( mMapName );
				
				Save_Click( sender, e );
			}
		}

		private void Exit_Click( Object sender, RoutedEventArgs e )
		{
			Close();
		}

		///
		/// Edit
		///

		///
		/// Help
		///
		private void About_Click( Object sender, RoutedEventArgs e )
		{
			MessageBox.Show( "Sentinel Editor\nVersion 1.0\nCopyright © BEHOLDER Software 2013", "About..." );
		}

		///
		/// ToolBar
		///
		private void ToolBar_New_Click( Object sender, RoutedEventArgs e )
		{
			New_Click( sender, e );
		}

		private void ToolBar_Open_Click( Object sender, RoutedEventArgs e )
		{
			Open_Click( sender, e );
		}

		private void ToolBar_Save_Click( Object sender, RoutedEventArgs e )
		{
			Save_Click( sender, e );
		}

		private void ToolBar_Select_Click( Object sender, RoutedEventArgs e )
		{
			Select_Object.IsChecked		= true;
			Translate_Object.IsChecked	= false;
			Rotate_Object.IsChecked		= false;
			Scale_Object.IsChecked		= false;
		}

		private void ToolBar_Translate_Click( Object sender, RoutedEventArgs e )
		{
			Select_Object.IsChecked		= false;
			Translate_Object.IsChecked	= true;
			Rotate_Object.IsChecked		= false;
			Scale_Object.IsChecked		= false;
		}

		private void ToolBar_Rotate_Click( Object sender, RoutedEventArgs e )
		{
			Select_Object.IsChecked		= false;
			Translate_Object.IsChecked	= false;
			Rotate_Object.IsChecked		= true;
			Scale_Object.IsChecked		= false;
		}

		private void ToolBar_Scale_Click( Object sender, RoutedEventArgs e )
		{
			Select_Object.IsChecked		= false;
			Translate_Object.IsChecked	= false;
			Rotate_Object.IsChecked		= false;
			Scale_Object.IsChecked		= true;
		}
		#endregion
	}
}
