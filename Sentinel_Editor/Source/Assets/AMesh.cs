using System;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	public class AMesh : EditorAsset
	{
		///
		/// Initialize this image to get an icon
		/// before the name in the TreeView.
		///
		public static BitmapImage DefaultImage;

		public WMesh Data;

		public AMesh( String name, WMesh mesh )
		{
			mType	= EditorAssetType.MESH;

			Data	= mesh;
			Name	= name;

			Header  = CreatePanel();
		}

		public Panel CreatePanel()
		{
			return EditorAsset.CreateDefaultAssetPanel( Name, DefaultImage );
		}
	}
}
