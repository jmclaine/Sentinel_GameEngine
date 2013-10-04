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
		private static WColorRGBA   mClearColor = new WColorRGBA( 0.0f, 0.2f, 0.8f, 1.0f );

		private GameWindow			mGameWindow;
		
		private static TreeViewItem	mAsset_Texture;
		private static TreeViewItem	mAsset_Shader;
		private static TreeViewItem	mAsset_Mesh;
		private static TreeViewItem	mAsset_Model;

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
		/// Starting Point of Application.
		///
		public MainWindow()
		{
			try
			{
				InitializeComponent();

				WShaderManager.Create();
				WTextureManager.Create();
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

			// Initialize Renderer.
			//
			WWindowInfo info = WRenderer.Create( "config.xml" );
			if( info == null )
			{
				MessageBox.Show( "Failed to load config.xml", "Application Failure" );

				System.Environment.Exit( 0 );
			}

			mGameWindow = new GameWindow();
			mGameWindow.Startup( "World", "WorldClass", info );
			mGameWindow.SetCameraPosition( new WVector3f( 0, 25, 25 ));
			mGameWindow.SetCameraRotation( new WVector3f( -45, 0, 0 ));
			
			Window_World.Child = mGameWindow;
			
			mMapName = "Default.MAP";
			//WGameWorld.Load( "Default.MAP" );
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
			CreateAssetTree( ref mAsset_Texture, "Texture" );
			CreateAssetTree( ref mAsset_Shader,	 "Shader" );
			CreateAssetTree( ref mAsset_Mesh,	 "Mesh" );
			CreateAssetTree( ref mAsset_Model,	 "Model" );

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
			mTranslateObject.Delete();

			mRotateObject.Shutdown();
			mRotateObject.Delete();

			mScaleObject.Shutdown();
			mScaleObject.Delete();

			foreach( AMesh item in mAsset_Mesh.Items )
				item.Data.Dispose();

			foreach( AModel item in mAsset_Model.Items )
				item.Data.Dispose();

			WGameWorld.Shutdown();
			mGameWindow.Shutdown();

			WRenderer.Shutdown();
		}
		#endregion

		#region Game World Rendering
		private void Update( Object sender, EventArgs e )
		{
			WMouse.Update();

			WRenderer.SetDepthStencil( 0 );
			WRenderer.SetViewport( 0, 0, mGameWindow.GetInfo().Width(), mGameWindow.GetInfo().Height() );
			WRenderer.SetRenderTarget( 0 );

			WRenderer.Clear( mClearColor );

			WRenderer.SetCull( CullType.CCW );
			WRenderer.SetDepthStencilState( StencilType.DEFAULT );

			mGameWindow.Update();

			DrawSelection();

			WRenderer.Present();
		}

		private void DrawSelection()
		{
			// Draw Object Transform Tool.
			//
			if( mSelectedObject != null )
			{
				WRenderer.SetBlend( BlendType.PARTICLE );
				WRenderer.SetDepthStencilState( StencilType.NO_ZBUFFER );

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
							WRenderer.SetFill( FillType.WIREFRAME );

							WMeshComponent meshComp = WMeshComponent.Cast( component );
							
							if( meshComp != null )
							{
								WMaterial material = new WMaterial( meshComp.Material );
								meshComp.Material  = mMaterial_Selected;

								WMesh mesh = meshComp.Mesh;
								WShader shader = mesh.Shader;

								mesh.Shader = WShaderManager.Get( "COLOR_ONLY" );

								mSelectedObject.UpdateDrawable( false );

								meshComp.Material = material;
								mesh.Shader = shader;

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
			}
		}
		#endregion

		#region Game Objects Tree
		///
		/// Game Objects Tree.
		/// 
		private void AddObjectToTree( WGameObject obj, OGameObject item = null )
		{
			if( item == null )
			{
				item = new OGameObject( obj );

				// Add the children.
				//
				for( uint x = 0; x < obj.NumChildren(); ++x )
					AddObjectToTree( obj.GetChild( x ), item );

				Objects_TreeView.Items.Add( item );
			}
			else
			{
				OGameObject child = new OGameObject( obj );
				
				// Add the children.
				//
				for( uint x = 0; x < obj.NumChildren(); ++x )
					AddObjectToTree( obj.GetChild( x ), child );

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
			if( e.ChangedButton == System.Windows.Input.MouseButton.Left )
				Objects_LastMouseDown = e.GetPosition( Objects_TreeView );
		}

		private void Objects_TreeView_MouseMove( Object sender, MouseEventArgs e )
		{
			try
			{
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
										WGameWorld.AddGameObject( Objects_DraggedItem.Data );
									else
										Objects_Target.Data.AddChild( Objects_DraggedItem.Data );
									
									AddObjectToTree( Objects_DraggedItem.Data, Objects_Target );

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
				e.Effects = DragDropEffects.None;
				e.Handled = true;
            
				OGameObject item = e.Source as OGameObject;
				if( item != null && Objects_DraggedItem != null )
				{
					Objects_Target = item;
					e.Effects = DragDropEffects.Move;
				}
				else
					Objects_Target = null;
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

		private void AddAsset( WTexture texture, String name )
		{
			ATexture item = new ATexture( texture, name );
			item.Selected += AssetSelected;
			
			mAsset_Texture.Items.Add( item );
		}

		private void AddAsset( WShader shader, String name )
		{
			AShader item = new AShader( shader, name );
			item.Selected += AssetSelected;
			
			mAsset_Shader.Items.Add( item );
		}

		private void AddAsset( WMesh mesh, String name )
		{
			AMesh item = new AMesh( mesh, name );
			item.Selected += AssetSelected;
			
			mAsset_Mesh.Items.Add( item );
		}

		private void AddAsset( WModel model, String name )
		{
			AModel item = new AModel( model, name );
			item.Selected += AssetSelected;

			mAsset_Model.Items.Add( item );
		}

		///
		/// Find an Asset by Type.
		///
		static public AMesh FindAsset( WMesh mesh )
		{
			foreach( AMesh item in mAsset_Mesh.Items )
				if( item.Data == mesh )
					return item;
			
			return null;
		}

		///
		/// Context Menu
		///
		private void Assets_AddTexture( Object sender, RoutedEventArgs e )
		{
			System.Windows.Forms.OpenFileDialog dialog = new System.Windows.Forms.OpenFileDialog();

			dialog.Title  = "Add Texture to Project";
			dialog.Filter = "PNG|*.png|All Files|*.*";
			dialog.FilterIndex = 1;

			//dialog.Multiselect = true;

			if( dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK )
				AddAsset( WRenderer.CreateTextureFromFile( dialog.FileName ), Path.GetFileName( dialog.FileName ));
		}

		private void Assets_AddModel( Object sender, RoutedEventArgs e )
		{
			System.Windows.Forms.OpenFileDialog dialog = new System.Windows.Forms.OpenFileDialog();

			dialog.Title  = "Add Shader to Project";
			dialog.Filter = "Sentinel Model|*.M3D|Wavefront|*.OBJ|All Files|*.*";
			dialog.FilterIndex = 1;

			if( dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK )
				AddAsset( WModel.Load( dialog.FileName ), Path.GetFileName( dialog.FileName ));
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

			if( !WShaderManager.LoadConfig( "config.xml" ))
			{
				MessageBox.Show( "Failed to load 'Assets\\Shaders\\config.xml'", "Shader Load Failure" );
				System.Environment.Exit( -1 );
			}

			List< String >  names   = new List< String >();
			List< WShader > shaders = new List< WShader >();

			WShaderManager.GetAll( ref names, ref shaders );

			for( int x = 0; x < names.Count; ++x )
				AddAsset( shaders[ x ], names[ x ] );

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
			WMesh					mesh;
			
			////////////////////////////////////

			WTexture texture = WTextureManager.Add( "default-alpha.png", WRenderer.CreateTextureFromFile( "Assets\\Textures\\default-alpha.png" ));
			AddAsset( texture, "default-alpha.png" );
			
			// Camera.
			//
			obj = WGameWorld.AddGameObject( new WGameObject(), "Main_Camera" );

			transform = (WTransformComponent)obj.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position = new WVector3f( 0, 25, 25 );
			transform.Rotation = new WVector3f( -45, 0, 0 );

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
			meshBuilder.Shader    = WShaderManager.Get( "COLOR" );
			meshBuilder.Primitive = PrimitiveType.TRIANGLE_LIST;

			mesh = meshBuilder.BuildMesh();
			AddAsset( mesh, "Ground" );

			obj = WGameWorld.AddGameObject( new WGameObject(), "Ground" );

			transform = (WTransformComponent)obj.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position	 = new WVector3f( 0, 0, 0 );
			transform.Scale		 = new WVector3f( 100, 1, 100 );

			obj.AttachComponent( new WMeshComponent( mesh ), "Mesh" );
			
			AddObjectToTree( obj );
			
			// Test object.
			//
			meshBuilder.ClearGeometry();
			meshBuilder.CreateDodecahedron( 1.0f );
			meshBuilder.Shader = WShaderManager.Get( "TEXTURE" );
			meshBuilder.Texture( (int)TextureType.DIFFUSE ).Set( WTextureManager.Get( "default-alpha.png" ));

			mesh = meshBuilder.BuildMesh();
			AddAsset( mesh, "Dodecahedron" );
			
			WGameObject obj2 = WGameWorld.AddGameObject( new WGameObject(), "Dodecahedron" );

			transform = (WTransformComponent)obj2.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position	= new WVector3f( 0, 4, 0 );
			transform.Scale		= new WVector3f( 0.5f, 0.5f, 0.5f );
			transform.Rotation	= new WVector3f( 90, 180, 270 );

			obj2.AttachComponent( new WMeshComponent( mesh ), "Mesh" );
			
			// Parent test object.
			//
			meshBuilder.ClearGeometry();
			meshBuilder.CreateSphere( 1.0f, 10, 10 );
			
			mesh = meshBuilder.BuildMesh();
			AddAsset( mesh, "Sphere" );

			obj = WGameWorld.AddGameObject( new WGameObject(), "Sphere" );

			transform = (WTransformComponent)obj.AttachComponent( new WTransformComponent(), "Transform" );
			transform.Position	= new WVector3f( -10, 4, 0 );
			transform.Scale		= new WVector3f( 2, 2, 2 );
			transform.Rotation	= new WVector3f( 45, 45, 45 );

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

			WModel.SetShaderColor( WShaderManager.Get( "COLOR_ONLY" ));
			WModel model = WModel.Load( "Assets\\Editor\\Translate.M3D" );

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

			mTranslateObject.Startup();
			model.Dispose();
		}

		private void CreateRotateMesh()
		{
			mRotateObject = new WGameObject();
		}

		private void CreateScaleMesh()
		{
			mScaleObject = new WGameObject();
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
				Window_Main.Title = "Sentinel Editor - " + dialog.FileName;
				WGameWorld.Load( mMapName );
			}
		}

		private void Save_Click( Object sender, RoutedEventArgs e )
		{
			WGameWorld.Save( mMapName );
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
				Window_Main.Title = "Sentinel Editor - " + dialog.FileName;
				WGameWorld.Save( mMapName );
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
