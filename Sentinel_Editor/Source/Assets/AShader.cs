using System;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	public class AShader : EditorAsset
	{
		///
		/// Initialize this image to get an icon
		/// before the name in the TreeView.
		///
		public static BitmapImage DefaultImage;

		public WShader Data;

		public AShader( String name, WShader data )
		{
			mType	= EditorAssetType.SHADER;

			Data	= data;
			Name	= name;

			Header  = EditorAsset.CreatePanel( name, DefaultImage );
		}
	}
}
