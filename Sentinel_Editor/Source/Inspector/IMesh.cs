using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	class IMesh : Inspector
	{
		public AMesh Data;

		public IMesh( String name, AMesh data )
		{
			Data = data;

			StackPanel panel = new StackPanel();
			panel.Orientation = Orientation.Horizontal;
			panel.Margin      = new Thickness( -5, 0, 0, 0 );
			panel.Focusable   = false;
			
			Label label = CreateLabel( name );
			label.Focusable = false;
			panel.Children.Add( label );
			
			Panel mesh = Data.CreatePanel();
			mesh.Margin = new Thickness( 0, 7, 0, 0 );
			panel.Children.Add( mesh );

			Header = panel;
		}
	}
}
