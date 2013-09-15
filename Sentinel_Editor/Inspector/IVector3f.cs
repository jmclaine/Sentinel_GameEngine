using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Media;
using System.ComponentModel;

using Sentinel.Math;

namespace Sentinel_Editor
{
	class IVector3f : Inspector
	{
		public WVector3f mData;

		public IVector3f( WVector3f v, String name )
		{
			Header	= name;
			mData	= v;

			Grid	grid = new Grid();
			Label	label;
			TextBox	box;

			for( int x = 0; x < 6; ++x )
			{
				ColumnDefinition col = new ColumnDefinition();
				grid.ColumnDefinitions.Add( col );
			}

			grid.Focusable = false;
			
			//////////////////////////////////

			label = Inspector.CreateLabel( "X: " );
			label.SetValue( Grid.ColumnProperty, 0 );
			label.Focusable = false;
			grid.Children.Add( label );

			box = Inspector.CreateEdit( v.x );
			box.SetValue( Grid.ColumnProperty, 1 );
			box.TextChanged += TextChanged_X;
			grid.Children.Add( box );

			//////////////////////////////////

			label = Inspector.CreateLabel( "Y: " );
			label.SetValue( Grid.ColumnProperty, 2 );
			label.Focusable = false;
			grid.Children.Add( label );

			box = Inspector.CreateEdit( v.y );
			box.SetValue( Grid.ColumnProperty, 3 );
			box.TextChanged += TextChanged_Y;
			grid.Children.Add( box );

			//////////////////////////////////

			label = Inspector.CreateLabel( "Z: " );
			label.SetValue( Grid.ColumnProperty, 4 );
			label.Focusable = false;
			grid.Children.Add( label );

			box = Inspector.CreateEdit( v.z );
			box.SetValue( Grid.ColumnProperty, 5 );
			box.TextChanged += TextChanged_Z;
			grid.Children.Add( box );

			//////////////////////////////////

			Items.Add( grid );
		}

		private void TextChanged_X( Object sender, TextChangedEventArgs e )
		{
			TextBox box = sender as TextBox;

			float result;

			if( float.TryParse( box.Text, out result ))
			{
				mData.x = result;

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
				mData.y = result;
				
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
				mData.z = result;
				
				if( ChangedValue != null )
					ChangedValue();
			}
		}
	}
}
