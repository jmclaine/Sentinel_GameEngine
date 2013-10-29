using System;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	public class AModel : EditorAsset
	{
		///
		/// Initialize this image to get an icon
		/// before the name in the TreeView.
		///
		public static BitmapImage DefaultImage;

		public WModel Data;

		public AModel( String name, WModel data )
		{
			mType	= EditorAssetType.MODEL;

			Data	= data;
			Name	= name;

			Header  = EditorAsset.CreateDefaultAssetPanel( name, DefaultImage );
		}
	}
}
