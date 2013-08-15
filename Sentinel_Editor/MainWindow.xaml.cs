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

using System.Runtime.InteropServices;

using Sentinel.Systems;
using Sentinel.Assets;
using Sentinel.Math;
using Sentinel.Utilities;
using Sentinel.Components;

namespace Sentinel_Editor
{
	/// <summary>
	/// Editor Window
	/// </summary>
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

		private WGameWindow		mWindowWorld;
		private static float	mBlue;
		//private static WColorRGBA   mClearColor = new WColorRGBA(0.0f, 0.2f, 0.8f, 1.0f);

		private WShader[]		mShader;
		private WMesh			mShape;
		private WMaterial		mMaterial;

		private bool			mDoUpdate;

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

			mWindowWorld = new WGameWindow();
			mWindowWorld.Startup( "World", "WorldClass", info );

			Window_World.Child = mWindowWorld;

			///////////////////////////////////////
			// Prepare shaders.
			//
			mShader = new WShader[ (int)ShaderTypes.NUM_SHADERS ];

			mShader[ (int)ShaderTypes.SHADER_COLOR ] = WRenderer.CreateShader( "Shaders\\colnorm", "PN", "PpVML" );
			if( mShader[ (int)ShaderTypes.SHADER_COLOR ] == null )
				MessageBox.Show( "Failed to load shader: colnorm", "Shader Load Failure" );

			///////////////////////////////////////
			// Prepare Game Objects.
			//
			WGameObject obj = WGameWorld.AddGameObject( new WGameObject(), "Test_Object" );

			WGameWorld.Startup( "NoMap.MAP" );

			///////////////////////////////////////
			// Setup game loop.
			//
			mDoUpdate = true;

			DispatcherTimer timer = new DispatcherTimer();
			timer.Tick += new EventHandler( Update );
			timer.Interval = new TimeSpan( 16 );
			timer.Start();
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

			//mWindowWorld.Shutdown();
			//WGameWorld.Shutdown();
		}

		private void Update( Object sender, EventArgs e )
		{
			if( mDoUpdate )
			{
				mBlue += 0.001f;
				if( mBlue >= 1.0f )
					mBlue = 0.0f;
				WColorRGBA mClearColor = new WColorRGBA( 0.0f, 0.2f * mBlue, mBlue, 1.0f );

				//mWindowWorld.SetActive();

				WRenderer.SetDepthStencil( 0 );
				WRenderer.SetViewport( 0 );
				WRenderer.SetRenderTarget( 0 );

				WRenderer.Clear( mClearColor );

				//WGameWorld.UpdateDrawable();

				WRenderer.Present();
			}
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
