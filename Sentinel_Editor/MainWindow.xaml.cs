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
using System.Windows.Shapes;
using System.Windows.Interop;
using System.Windows.Threading;
using System.ComponentModel;
using System.Collections.ObjectModel;

using System.Runtime.InteropServices;

using Sentinel.Systems;
using Sentinel.Assets;
using Sentinel.Math;
using Sentinel.Utilities;
using Sentinel.Components;

namespace Sentinel_Editor
{
	///
	/// Editor Window
	///
	public partial class MainWindow : Window
	{
		private enum ShaderTypes
		{
			COLOR_ONLY,
			COLOR,
			TEXTURE,
			//NORMAL_MAP,
			//SPRITE,

			NUM_SHADERS
		};

		private enum ShapeTypes
		{
			CUBE,
			CYLINDER,
			OCTAHEDRON,
			TETRAHEDRON,
			DODECAHEDRON,
			SPHERE,
			WIRE_SPHERE,

			NUM_SHAPES
		};

		private static WColorRGBA   mClearColor = new WColorRGBA( 0.0f, 0.2f, 0.8f, 1.0f );

		private GameWindow			mGameWindow;
		private WShader[]			mShader;
		private WTexture			mTexture;
		private bool				mDoUpdate;

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

		///
		/// Starting Point of Application.
		///
		public MainWindow()
		{
			InitializeComponent();
		}

		private void Window_Loaded( Object sender, RoutedEventArgs e )
		{
			///////////////////////////////////////
			// Initialize Renderer.
			//
			WWindowInfo info = WRenderer.Load( "config.xml" );
			if( info == null )
			{
				MessageBox.Show( "Failed to load config.xml", "Application Failure" );

				System.Environment.Exit( 0 );
			}

			WGameWorld.Load( "Default.MAP" );

			mGameWindow = new GameWindow();
			mGameWindow.Startup( "World", "WorldClass", info );
			mGameWindow.SetCameraPosition( new WVector3f( 0, 25, 25 ));
			mGameWindow.SetCameraRotation( new WVector3f( -45, 0, 0 ));
			
			Window_World.Child = mGameWindow;

			// Create editor materials.
			//
			mMaterial_X_Axis   = new WMaterial( new WColorRGBA( 1, 0, 0, TRANSFORM_OBJECT_ALPHA ), new WColorRGBA( 0, 0, 0, 0 ), new WColorRGBA( 0, 0, 0, 0 ), 0 );
			mMaterial_Y_Axis   = new WMaterial( new WColorRGBA( 0, 1, 0, TRANSFORM_OBJECT_ALPHA ), new WColorRGBA( 0, 0, 0, 0 ), new WColorRGBA( 0, 0, 0, 0 ), 0 );
			mMaterial_Z_Axis   = new WMaterial( new WColorRGBA( 0, 0, 1, TRANSFORM_OBJECT_ALPHA ), new WColorRGBA( 0, 0, 0, 0 ), new WColorRGBA( 0, 0, 0, 0 ), 0 );

			mMaterial_Selected = new WMaterial( new WColorRGBA( 1, 0.5f, 1, 1 ), new WColorRGBA( 0, 0, 0, 0 ), new WColorRGBA( 0, 0, 0, 0 ), 0 );

			PrepareShaders();
			PrepareObjects();

			//Status_Image.Source = new BitmapImage( new Uri( "pack://application:,,,/Images/Warning.png" ));

			///////////////////////////////////////
			// Setup game loop.
			//
			mDoUpdate = true;

			DispatcherTimer timer = new DispatcherTimer();
			timer.Tick += new EventHandler( Update );
			timer.Interval = new TimeSpan( 16 );
			timer.Start();
		}

		private void Update( Object sender, EventArgs e )
		{
			if( mDoUpdate )
			{
				WMouse.Update();

				WRenderer.SetDepthStencil( 0 );
				WRenderer.SetViewport( 0 );
				WRenderer.SetRenderTarget( 0 );

				WRenderer.Clear( mClearColor );

				WRenderer.SetCull( CullType.CCW );
				WRenderer.SetDepthStencilState( StencilType.DEFAULT );

				mGameWindow.Update();

				// Draw Object Transform Tool.
				//
				if( mSelectedObject != null )
				{
					WRenderer.SetBlend( BlendType.PARTICLE );
					WRenderer.SetDepthStencilState( StencilType.NO_ZBUFFER );

					WTransformComponent selectedObjectTransform = WTransformComponent.Cast( mSelectedObject.FindComponent( ComponentType.TRANSFORM ));
					WVector3f selectedObjectPosition = selectedObjectTransform.GetMatrixWorld().Transform( new WVector3f( 0, 0, 0 ));

					if( Select_Object.IsChecked == true )
					{
						// Draw yellow wireframe around the selected object.
						//
						WGameComponent component = mSelectedObject.FindComponent( ComponentType.DRAWABLE );
						
						if( component != null )
						{
							WRenderer.SetFill( FillType.WIREFRAME );

							WMeshComponent meshComp = WMeshComponent.Cast( component );
							
							if( meshComp != null )
							{
								WMaterial material = new WMaterial( meshComp.Material );
								meshComp.Material  = mMaterial_Selected;

								WMesh mesh = meshComp.Mesh;
								WShader shader = mesh.Shader;

								mesh.Shader = mShader[ (int)ShaderTypes.COLOR_ONLY ];

								mSelectedObject.UpdateTransform();
								mSelectedObject.UpdateDrawable( false );

								meshComp.Material = material;
								mesh.Shader = shader;

								// Not calling this here causes the garbage collector
								// to accumulate shared_ptr counters.
								//
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
									
									mSelectedObject.UpdateTransform();
									mSelectedObject.UpdateDrawable( false );

									modelComp.SetMaterial( material );

									// Not calling this here causes the garbage collector
									// to accumulate shared_ptr counters.
									//
									model.Dispose();
								}
							}

							WRenderer.SetFill( FillType.SOLID );
						}
					}
					else
					if( Translate_Object.IsChecked == true )
					{
						// Draw the translate object.
						//
						WTransformComponent transform = WTransformComponent.Cast( mTranslateObject.FindComponent( ComponentType.TRANSFORM ));
						transform.Position = selectedObjectPosition;

						WVector3f distance = mGameWindow.GetCamera().GetTransform().Position - transform.Position;
						distance.x = Math.Abs( distance.x );
						distance.y = Math.Abs( distance.y );
						distance.z = Math.Abs( distance.z );
						float d = distance.Length() * TRANSFORM_OBJECT_SCALE;
						transform.Scale = new WVector3f( d, d, d );

						WRenderer.SetCull( CullType.NONE );
						WRenderer.SetDepthStencilState( StencilType.NO_ZBUFFER );

						//(new WMeshComponent( mTranslateObject.GetChild( 0 ).FindComponent( ComponentType.DRAWABLE ))).Material = mMaterial_Selected;
						mTranslateObject.UpdateTransform();
						mTranslateObject.UpdateDrawable();
					}
				}

				WRenderer.Present();
			}
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
					MessageBox.Show( "Saving..." );
					break;

				case MessageBoxResult.No:
					MessageBox.Show( "Not Saving..." );
					break;

				case MessageBoxResult.Cancel:
					e.Cancel = true;
					return;
			}
		}

		private void Window_Closed( Object sender, EventArgs e )
		{
			mDoUpdate = false;

			for( int x = 0; x < mShader.Length; ++x )
				mShader[ x ].Dispose();

			mTranslateObject.Shutdown();
			mTranslateObject.Delete();

			mRotateObject.Shutdown();
			mRotateObject.Delete();

			mScaleObject.Shutdown();
			mScaleObject.Delete();

			mGameWindow.Shutdown();
			WGameWorld.Shutdown();
		}

		///
		/// Game Objects Tree.
		/// 
		private void AddObjectToTree( WGameObject obj, GameObject_TreeViewItem item = null )
		{
			if( item == null )
			{
				item = new GameObject_TreeViewItem( obj );

				// Add the children.
				//
				for( int x = 0; x < obj.NumChildren(); ++x )
					AddObjectToTree( obj.GetChild( x ), item );

				Objects_TreeView.Items.Add( item );
			}
			else
			{
				item.Items.Add( new GameObject_TreeViewItem( obj ));
			}
		}

		private void Objects_TreeView_Selected( Object sender, RoutedEventArgs e )
		{
			GameObject_TreeViewItem item = (GameObject_TreeViewItem)Objects_TreeView.SelectedItem;
			
			mSelectedObject = item.mObject;

			//Objects_TreeView.Items.Refresh();
			//Objects_TreeView.UpdateLayout();
		}

		///
		/// Assets Tree.
		/// 
		private void AddAssetToTree( String name )
		{
			// TODO: Create new TreeViewItem(s) to support various assets.

			TreeViewItem item = new TreeViewItem();
			item.Foreground = Brushes.White;
			item.Header     = name;
			
			Assets_TreeView.Items.Add( item );
		}

		///
		/// Prepare Shaders.
		///
		private void PrepareShaders()
		{
			mShader = new WShader[ (int)ShaderTypes.NUM_SHADERS ];

			mShader[ (int)ShaderTypes.COLOR_ONLY ] = WRenderer.CreateShader( "Shaders\\color", "P", "PM" );
			if( mShader[ (int)ShaderTypes.COLOR_ONLY ] == null )
			{
				MessageBox.Show( "Failed to load shader: color", "Shader Load Failure" );
				System.Environment.Exit( -1 );
			}

			mShader[ (int)ShaderTypes.COLOR ] = WRenderer.CreateShader( "Shaders\\colnorm", "PN", "PpVML" );
			if( mShader[ (int)ShaderTypes.COLOR ] == null )
			{
				MessageBox.Show( "Failed to load shader: colnorm", "Shader Load Failure" );
				System.Environment.Exit( -1 );
			}

			mShader[ (int)ShaderTypes.TEXTURE ] = WRenderer.CreateShader( "Shaders\\texture", "PXN", "PpXVML" );
			if( mShader[ (int)ShaderTypes.TEXTURE ] == null )
			{
				MessageBox.Show( "Failed to load 'texture' shader.", "Shader Load Failure" );
				System.Environment.Exit( -1 );
			}
		}

		///
		/// Prepare Game Objects.
		///
		private void PrepareObjects()
		{
			WMeshBuilder			meshBuilder = new WMeshBuilder();
			WGameObject				obj;
			WTransformComponent		transform;
			WMesh					mesh;
			
			////////////////////////////////////

			mTexture = WRenderer.CreateTextureFromFile( "Assets\\Images\\default-alpha.png" );

			// Camera.
			//
			obj = WGameWorld.AddGameObject( new WGameObject(), "Main_Camera" );

			transform = (WTransformComponent)obj.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position = new WVector3f( 0, 25, 25 );
			transform.Orientation = new WQuatf( -45, 0, 0 );

			obj.AttachComponent( new WPerspectiveCameraComponent( mGameWindow.GetInfo().Width(), mGameWindow.GetInfo().Height() ), "Camera" );

			AddObjectToTree( obj );
			
			// Point Light.
			//
			obj = WGameWorld.AddGameObject( new WGameObject(), "Point_Light" );

			transform = (WTransformComponent)obj.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position = new WVector3f( 0, 10, 0 );

			WLightComponent light = (WLightComponent)obj.AttachComponent( new WLightComponent(), "Light" );
			light.Attenuation = new WVector4f( 1, 1, 1, 25 );

			AddObjectToTree( obj );

			// Ground object.
			//
			meshBuilder.CreateCube( 1.0f );
			meshBuilder.Shader    = mShader[ (int)ShaderTypes.COLOR ];
			meshBuilder.Primitive = PrimitiveType.TRIANGLE_LIST;

			obj = WGameWorld.AddGameObject( new WGameObject(), "Ground" );

			transform = (WTransformComponent)obj.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position	 = new WVector3f( 0, 0, 0 );
			transform.Scale		 = new WVector3f( 100, 1, 100 );

			mesh = meshBuilder.BuildMesh();
			obj.AttachComponent( new WMeshComponent( mesh ), "Mesh" );

			AddObjectToTree( obj );

			// Test object.
			//
			meshBuilder.Shader = mShader[ (int)ShaderTypes.TEXTURE ];
			meshBuilder.Texture( (int)TextureType.DIFFUSE ).Set( mTexture );
			
			WGameObject obj2 = WGameWorld.AddGameObject( new WGameObject(), "Cube" );

			transform = (WTransformComponent)obj2.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position	  = new WVector3f( 0, 4, 0 );
			transform.Scale		  = new WVector3f( 0.5f, 0.5f, 0.5f );
			transform.Orientation = new WQuatf( 45, 45, 45 );

			mesh = meshBuilder.BuildMesh();
			obj2.AttachComponent( new WMeshComponent( mesh ), "Mesh" );

			// Parent test object.
			//
			obj = WGameWorld.AddGameObject( new WGameObject(), "Cube2" );

			transform = (WTransformComponent)obj.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position	  = new WVector3f( -10, 4, 0 );
			transform.Scale		  = new WVector3f( 2, 2, 2 );

			obj.AttachComponent( new WMeshComponent( mesh ), "Mesh" );

			obj.AddChild( obj2 );

			AddObjectToTree( obj );

			///////////////////////////////
			
			WGameWorld.Startup();

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

			WModel.SetShaderColor( mShader[ (int)ShaderTypes.COLOR_ONLY ] );
			WModel model = WModel.Load( "Editor\\Translate.M3D" );

			// Root Translate Object.
			//
			mTranslateObject.AttachComponent( new WTransformComponent(), "Transform" );
			
			///////////////////////////////

			WGameObject obj = new WGameObject();

			WTransformComponent transform = new WTransformComponent();
			transform.Position    = new WVector3f( tileSize, 0, 0 );
			transform.Orientation = new WQuatf( 0, 0, -90 );
			obj.AttachComponent( transform, "Tile_X" );

			WModelComponent modelComp = new WModelComponent( model );
			modelComp.SetMaterial( mMaterial_X_Axis );
			obj.AttachComponent( modelComp, "Model_X" );

			mTranslateObject.AddChild( obj );

			///////////////////////////////
			
			obj = new WGameObject();

			transform = new WTransformComponent();
			transform.Position    = new WVector3f( 0, tileSize, 0 );
			transform.Orientation = new WQuatf( 0, 0, 0 );
			obj.AttachComponent( transform, "Tile_Y" );

			modelComp = new WModelComponent( model );
			modelComp.SetMaterial( mMaterial_Y_Axis );
			obj.AttachComponent( modelComp, "Model_Y" );

			mTranslateObject.AddChild( obj );

			///////////////////////////////

			obj = new WGameObject();

			transform = new WTransformComponent();
			transform.Position    = new WVector3f( 0, 0, tileSize );
			transform.Orientation = new WQuatf( 90, 0, 0 );
			obj.AttachComponent( transform, "Tile_Z" );

			modelComp = new WModelComponent( model );
			modelComp.SetMaterial( mMaterial_Z_Axis );
			obj.AttachComponent( modelComp, "Model_Z" );

			mTranslateObject.AddChild( obj );

			///////////////////////////////

			mTranslateObject.Startup();
		}

		private void CreateRotateMesh()
		{
			mRotateObject = new WGameObject();
		}

		private void CreateScaleMesh()
		{
			mScaleObject = new WGameObject();
		}

		///
		/// File
		///
		private void New_Click( Object sender, RoutedEventArgs e )
		{
			MessageBox.Show( "You clicked 'New...'" );
		}

		private void Open_Click( Object sender, RoutedEventArgs e )
		{
			MessageBox.Show( "You clicked 'Open...'" );
		}

		private void Save_Click( Object sender, RoutedEventArgs e )
		{
			MessageBox.Show( "You clicked 'Save...'" );
		}

		private void SaveAs_Click( Object sender, RoutedEventArgs e )
		{
			MessageBox.Show( "You clicked 'Save As...'" );
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
			MessageBox.Show( "Sentinel Editor\nVersion 1.0.0.0\nCopyright © BEHOLDER Software 2013", "About..." );
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
	}
}
