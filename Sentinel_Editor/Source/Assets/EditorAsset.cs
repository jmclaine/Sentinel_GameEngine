﻿using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	public enum EditorAssetType
	{
		TEXTURE,
		SHADER,
		MESH,
		MODEL,
		AUDIO_SOURCE,

		NUM_ASSETS
	};

	public class EditorAsset : TreeViewItem
	{
		///
		/// Must be initialized before creating a new object.
		/// Initializes all children to default Style.
		///
		public static Style			TreeStyle;
		
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

		new public String			Name;

		protected EditorAsset()
		{
			Style		= TreeStyle;

			Focusable	= true;
		}

		protected static Panel CreatePanel( String name, BitmapImage bitmap )
		{
			StackPanel panel = new StackPanel();
			panel.Orientation = Orientation.Horizontal;
			
			Image image  = new Image();
			image.Source = bitmap;
			image.HorizontalAlignment = HorizontalAlignment.Left;
			image.VerticalAlignment   = VerticalAlignment.Center;
			image.Width  = 22;
			image.Height = 16;

			TextBlock text = new TextBlock();
			text.Text = name;

			panel.Children.Add( image );
			panel.Children.Add( text );

			return panel;
		}
	}
}
