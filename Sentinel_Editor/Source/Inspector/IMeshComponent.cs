using System.Windows.Controls;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	public class IMeshComponent : Inspector
	{
		public WMeshComponent Data;

		private IString mName;

		public IMeshComponent( WMeshComponent component )
		{
			Header	= "Mesh";
			Data	= component;

			mName = new IString( "Name: ", Data.Name );
			mName.ChangedValue = ChangedName;
			Items.Add( mName );

			WMesh mesh = Data.Mesh;
			AMesh asset = MainWindow.FindAsset( mesh );

			Items.Add( new IMesh( "Mesh: ", asset ));
			
			mesh.Dispose();
		}

		private void ChangedName()
		{
			Data.Name = mName.Data;
		}
	}
}
