using System.Windows.Controls;

using Sentinel.Components;

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

			AMesh mesh = MainWindow.FindAsset( Data.Mesh );

			if( mesh != null )
				Items.Add( Create() );
			else
				Items.Add( "null" );
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
