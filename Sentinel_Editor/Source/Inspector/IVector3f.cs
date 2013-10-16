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
		public WVector3f Data { get; set; }

		public IVector3f( WVector3f v, String name )
		{
			Header	= name;
			Data	= v;

			StackPanel panel  = new StackPanel();
			panel.Orientation = Orientation.Horizontal;
			panel.Focusable   = false;
			
			Label   label;
			TextBox box;
			
			//////////////////////////////////

			label = Inspector.CreateLabel( "X: " );
			label.Focusable = false;
			panel.Children.Add( label );

			box = Inspector.CreateEdit( v.x );
			box.TextChanged += TextChanged_X;
			panel.Children.Add( box );

			//////////////////////////////////

			label = Inspector.CreateLabel( "Y: " );
			label.Focusable = false;
			panel.Children.Add( label );

			box = Inspector.CreateEdit( v.y );
			box.TextChanged += TextChanged_Y;
			panel.Children.Add( box );

			//////////////////////////////////

			label = Inspector.CreateLabel( "Z: " );
			label.Focusable = false;
			panel.Children.Add( label );

			box = Inspector.CreateEdit( v.z );
			box.TextChanged += TextChanged_Z;
			panel.Children.Add( box );

			//////////////////////////////////

			Items.Add( new Inspector( panel ));
		}

		private void TextChanged_X( Object sender, TextChangedEventArgs e )
		{
			TextBox box = sender as TextBox;

			float result;

			if( float.TryParse( box.Text, out result ))
			{
				Data.x = result;

				if( ChangedValue != null )
					ChangedValue();
			}
		}

		private void TextChanged_Y( Object sender, TextChangedEventArgs e )
		{
			TextBox box = sender as TextBox;

			float result;

			if( float.TryParse( box.Text, out result ))
			{
				Data.y = result;
				
				if( ChangedValue != null )
					ChangedValue();
			}
		}

		private void TextChanged_Z( Object sender, TextChangedEventArgs e )
		{
			TextBox box = sender as TextBox;

			float result;

			if( float.TryParse( box.Text, out result ))
			{
				Data.z = result;
				
				if( ChangedValue != null )
					ChangedValue();
			}
		}
	}
}
