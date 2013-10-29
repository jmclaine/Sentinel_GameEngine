using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Media;
using System.ComponentModel;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	class IVector3f : Inspector
	{
		public WVector3f Data;

		private IFloat mX;
		private IFloat mY;
		private IFloat mZ;

		public IVector3f( String name, WVector3f data )
		{
			Header	= name;
			Data	= data;

			StackPanel panel  = new StackPanel();
			panel.Orientation = Orientation.Horizontal;
			panel.Margin      = new Thickness( -38, 0, 0, 0 );
			panel.Focusable   = false;
			
			mX = new IFloat( "X: ", Data.x );
			mX.ChangedValue = TextChanged_X;
			panel.Children.Add( mX );

			mY = new IFloat( "Y: ", Data.y );
			mY.ChangedValue = TextChanged_Y;
			panel.Children.Add( mY );

			mZ = new IFloat( "Z: ", Data.z );
			mZ.ChangedValue = TextChanged_Z;
			panel.Children.Add( mZ );

			Items.Add( panel );
		}

		private void TextChanged_X()
		{
			Data.x = mX.Data;
		}

		private void TextChanged_Y()
		{
			Data.y = mY.Data;
		}

		private void TextChanged_Z()
		{
			Data.z = mZ.Data;
		}
	}
}
