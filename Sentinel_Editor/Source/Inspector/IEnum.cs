using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	class IEnum : Inspector
	{
		public int Data;

		public IEnum( String name, String[] items, int startIndex, SelectionChangedEventHandler e, 
					  double width = 200, double height = 22 )
		{
			Data = startIndex;

			StackPanel panel  = new StackPanel();
			panel.Orientation = Orientation.Horizontal;
			panel.Margin      = new Thickness( -5, 0, 0, 0 );
			panel.Focusable   = false;

			ComboBox box = new ComboBox();

			foreach( String item in items )
				box.Items.Add( item );
			
			box.MinWidth	  = width;
			box.MaxWidth	  = width;
			box.MinHeight	  = height;
			box.MaxHeight	  = height;
			box.SelectedIndex = startIndex;
			box.SelectionChanged += e;

			panel.Children.Add( CreateLabel( name ));
			panel.Children.Add( box );

			Header = panel;
		}
	}
}
