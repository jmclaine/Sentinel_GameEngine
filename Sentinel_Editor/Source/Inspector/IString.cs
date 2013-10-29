using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	class IString : Inspector
	{
		public String Data;

		public IString( String name, String data, double width = 200, double height = 20, double padding = 0 )
		{
			Data = data;

			StackPanel panel = new StackPanel();
			panel.Orientation = Orientation.Horizontal;
			panel.Margin      = new Thickness( -5, 0, 0, 0 );
			panel.Focusable   = false;
			
			Label label = CreateLabel( name );
			label.Focusable = false;
			panel.Children.Add( label );
			
			TextBox box = new TextBox();
			box.SelectedText = data;
			box.MinWidth	 = width;
			box.MaxWidth	 = width;
			box.MinHeight	 = height;
			box.MaxHeight	 = height;
			box.Padding		 = new Thickness( padding );
			box.TextChanged += TextChanged;

			panel.Children.Add( box );

			Header = panel;
		}

		private void TextChanged( Object sender, TextChangedEventArgs e )
		{
			TextBox box = sender as TextBox;

			Data = box.Text;

			if( ChangedValue != null )
				ChangedValue();
		}
	}
}
