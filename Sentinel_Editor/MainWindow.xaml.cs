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
			SHADER_COLOR,
			//SHADER_TEXTURE,
			//SHADER_NORMAL_MAP,
			//SHADER_SPRITE,

			NUM_SHADERS
		};

		private enum ShapeTypes
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

		private static WColorRGBA   mClearColor = new WColorRGBA( 0.0f, 0.2f, 0.8f, 1.0f );

		private GameWindow			mWindowWorld;
		private WShader[]			mShader;
		private bool				mDoUpdate;

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

			mWindowWorld = new GameWindow();
			mWindowWorld.Startup( "World", "WorldClass", info );
			mWindowWorld.SetCameraPosition( new WVector3f( 0, 25, 25 ));
			mWindowWorld.SetCameraRotation( new WVector3f( -45, 0, 0 ));
			
			Window_World.Child = mWindowWorld;

			//Objects_TreeView.Items.Add( new ObservableCollection< GameObject_TreeViewItem >() );
			
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

				mWindowWorld.Update();

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

			mWindowWorld.Shutdown();
			WGameWorld.Shutdown();
		}

		///
		/// Game Objects Tree.
		/// 
		private void AddObjectToTree( WGameObject obj )
		{
			GameObject_TreeViewItem item = new GameObject_TreeViewItem( obj );
			
			
			
			Objects_TreeView.Items.Add( item );
		}

		private void Objects_TreeView_Selected( Object sender, RoutedEventArgs e )
		{
			GameObject_TreeViewItem item = (GameObject_TreeViewItem)Objects_TreeView.SelectedItem;
			
			//Objects_TreeView.Items.Refresh();
			//Objects_TreeView.UpdateLayout();
		}

		///
		/// Assets Tree.
		/// 
		private void AddAssetToTree( String name )
		{
			TreeViewItem item = new TreeViewItem();
			item.Foreground = Brushes.White;
			item.Header = name;
			
			//item.Items.Add( new TreeViewItem() { Header = "Dummy" } );

			Assets_TreeView.Items.Add( item );
		}

		///
		/// Prepare Shaders.
		///
		private void PrepareShaders()
		{
			mShader = new WShader[ (int)ShaderTypes.NUM_SHADERS ];

			mShader[ (int)ShaderTypes.SHADER_COLOR ] = WRenderer.CreateShader( "Shaders\\colnorm", "PN", "PpVML" );
			if( mShader[ (int)ShaderTypes.SHADER_COLOR ] == null )
				MessageBox.Show( "Failed to load shader: colnorm", "Shader Load Failure" );
		}

		///
		/// Prepare Game Objects.
		///
		private void PrepareObjects()
		{
			WMeshBuilder					meshBuilder = new WMeshBuilder();
			WMaterial						material    = new WMaterial();
			WGameObject						obj;
			WTransformComponent				transform;
			
			////////////////////////////////////

			// Camera.
			//
			obj = WGameWorld.AddGameObject( new WGameObject(), "Main_Camera" );

			transform = (WTransformComponent)obj.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position = new WVector3f( 0, 25, 25 );
			transform.Orientation = new WQuatf( -45, 0, 0 );

			obj.AttachComponent( new WPerspectiveCameraComponent( mWindowWorld.GetInfo().Width(), mWindowWorld.GetInfo().Height() ), "Camera" );

			AddObjectToTree( obj );
			
			// Point Light.
			//
			obj = WGameWorld.AddGameObject( new WGameObject(), "Point_Light" );

			transform = (WTransformComponent)obj.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position = new WVector3f( 0, 10, 0 );

			WLightComponent light = (WLightComponent)obj.AttachComponent( new WLightComponent(), "Light" );
			light.Attenuation  = new WVector4f( 1, 1, 1, 25 );

			AddObjectToTree( obj );

			// Test object.
			//
			meshBuilder.CreateCube( 1.0f );
			meshBuilder.Shader = mShader[ (int)ShaderTypes.SHADER_COLOR ];
			meshBuilder.Primitive = PrimitiveType.TRIANGLE_LIST;

			obj = WGameWorld.AddGameObject( new WGameObject(), "Ground" );

			transform = (WTransformComponent)obj.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position	= new WVector3f( 0, 0, 0 );
			transform.Scale		= new WVector3f( 100, 1, 100 );

			obj.AttachComponent( new WMeshComponent( meshBuilder.BuildMesh(), material ), "Mesh" );

			AddObjectToTree( obj );

			// Test object.
			//
			meshBuilder.CreateCube( 1.0f );
			meshBuilder.Shader = mShader[ (int)ShaderTypes.SHADER_COLOR ];

			obj = WGameWorld.AddGameObject( new WGameObject(), "Cube" );

			transform = (WTransformComponent)obj.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position	= new WVector3f( 0, 2, 0 );
			//transform.Scale	= new WVector3f( 1, 1, 1 );

			obj.AttachComponent( new WMeshComponent( meshBuilder.BuildMesh(), material ), "Mesh" );

			AddObjectToTree( obj );
			
			WGameWorld.Startup();
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

		private void ToolBar_Translate_Click( Object sender, RoutedEventArgs e )
		{ }

		private void ToolBar_Rotate_Click( Object sender, RoutedEventArgs e )
		{ }

		private void ToolBar_Scale_Click( Object sender, RoutedEventArgs e )
		{ }
	}
}
