using System.Windows.Controls;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	public class IMeshComponent : Inspector
	{
		public WMeshComponent Data { get; set; }

		public IMeshComponent( WMeshComponent component )
		{
			Header	= "Mesh";
			Data	= component;

			Items.Add( new Inspector( Inspector.CreateEdit( "Name: ", Data.Name )));

			WMesh mesh = Data.Mesh;
			AMesh asset = MainWindow.FindAsset( mesh );

			if( asset != null )
				Items.Add( Create() );
			else
				Items.Add( "null" );

			mesh.Dispose();
		}

		private Inspector Create()
		{
			StackPanel panel = new StackPanel();
			panel.Orientation = Orientation.Horizontal;

			panel.Children.Add( Inspector.CreateLabel( "Mesh: " ));

			return new Inspector( panel );
		}
	}
}
