using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

using Sentinel.Assets;

namespace Sentinel_Editor
{
	public enum EditorAssetType
	{
		TEXTURE,
		SHADER,
		MESH,
		MODEL,

		NUM_ASSETS
	};

	public class EditorAsset : TreeViewItem
	{
		///
		/// Must be initialized before creating a new object.
		/// Initializes all children to default Style.
		///
		public static Style			TreeStyle { get; set; }
		
		///
		/// Since each object possesses a different type of
		/// asset, this label ensures that it can be converted
		/// into the correct type quickly.
		/// 
		/// Additionally, it adds a fast determination of
		/// whether this asset can be set on GameComponents
		/// within the Inspector.
		///
		protected EditorAssetType	mType;
		public EditorAssetType		Type { get { return mType; }}

		new public String			Name { get; set; }

		protected EditorAsset()
		{
			Style		= TreeStyle;

			Focusable	= true;
		}
	}
}
