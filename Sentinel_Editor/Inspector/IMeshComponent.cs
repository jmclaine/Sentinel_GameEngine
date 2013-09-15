using System.Windows.Controls;

using Sentinel.Components;

namespace Sentinel_Editor
{
	public class IMeshComponent : Inspector
	{
		public WMeshComponent mData;

		public IMeshComponent( WMeshComponent component )
		{
			Header	= "Mesh";
			mData	= component;

			Items.Add( Inspector.CreateEdit( "Name: ", mData.Name ));

			AMesh mesh = MainWindow.FindAsset( mData.Mesh );

			if( mesh != null )
				Items.Add( Inspector.CreateLabel( "Mesh: " ));
			else
				Items.Add( "null" );
		}
	}
}
