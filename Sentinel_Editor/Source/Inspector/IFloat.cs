using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

using Sentinel.Wrapped;

namespace Sentinel_Editor
{
	class IFloat : Inspector
	{
		public float Data;

		public IFloat( String name, float data, double width = 70, double height = 20, double padding = 0 )
		{
			Data = data;

			StackPanel panel  = new StackPanel();
			panel.Orientation = Orientation.Horizontal;
			panel.Margin      = new Thickness( -5, 0, -5, 0 );
			panel.Focusable   = false;

			Label label = CreateLabel( name );
			label.Focusable = false;
			panel.Children.Add( label );

			TextBox box = new TextBox();
			box.SelectedText = "" + data;
			box.MinWidth	 = width;
			box.MaxWidth	 = width;
			box.MinHeight	 = height;
			box.MaxHeight	 = height;
			box.Padding		 = new Thickness( padding );
			box.PreviewTextInput  += PreviewTextInput;
			box.LostKeyboardFocus += LostKeyboardFocus;
			box.TextChanged	      += TextChanged;

			DataObject.AddPastingHandler( box, PreviewTextInputPaste );

			panel.Children.Add( box );

			Header = panel;
		}

		private void TextChanged( Object sender, TextChangedEventArgs e )
		{
			TextBox box = sender as TextBox;

			float result;

			if( float.TryParse( box.Text, out result ))
			{
				Data = result;
				
				if( ChangedValue != null )
					ChangedValue();
			}
		}

		private new void PreviewTextInput( Object sender, TextCompositionEventArgs e )
		{
			float value;

			TextBox box = sender as TextBox;

			int index = box.Text.IndexOf( box.SelectedText );

			String result = box.Text.Remove( index, box.SelectedText.Length );

			if( box.SelectionStart < result.Length )
				result = result.Insert( box.SelectionStart, e.Text );
			else
				result += e.Text;
			
			if( box.SelectionStart == 0 && e.Text == "-" )
			{
				if( box.SelectedText.Length == 0 && box.Text.Length > 0 )
					if( box.Text[ 0 ] == '-' )
						e.Handled = true;
			}
			else
			if( !float.TryParse( result, out value ))
				e.Handled = true;
		}

		private void PreviewTextInputPaste( Object sender, DataObjectPastingEventArgs e )
		{
			float value;

			TextBox box = sender as TextBox;

			String clipboard = e.DataObject.GetData( typeof( string )) as String;

			int index = box.Text.IndexOf( box.SelectedText );

			String result = box.Text.Remove( index, box.SelectedText.Length );

			if( box.SelectionStart < result.Length )
				result = result.Insert( box.SelectionStart, clipboard );
			else
				result += clipboard;
			
			if( float.TryParse( result, out value ))
				box.SelectedText = clipboard;

			e.CancelCommand();
			e.Handled = true;
		}

		private new void LostKeyboardFocus( Object sender, KeyboardFocusChangedEventArgs e )
		{
			float value;

			TextBox box = sender as TextBox;

			if( !float.TryParse( box.Text, out value ))
				box.Text = "0";
		}
	}
}
