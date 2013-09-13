using System;
using System.Globalization;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

using Sentinel.Math;
using Sentinel.Components;

namespace Sentinel_Editor
{
	class Inspector_TreeViewItem : TreeViewItem
	{
		public delegate void ChangedValueDelegate();
		public ChangedValueDelegate ChangedValue;

		private const double EDIT_WIDTH  = 70;
		private const double EDIT_HEIGHT = 20;

		///
		/// Must be initialized before creating a new object.
		///
		public static Style mTextStyle;
		public static Style mTreeStyle;

		public Inspector_TreeViewItem()
		{
			Selected += Selected_Inspector;

			Style = mTreeStyle;
		}

		static public void Selected_Inspector( Object sender, RoutedEventArgs e )
		{
			(sender as TreeViewItem).IsSelected = false;
		}

		static public Label CreateLabel( String name )
		{
			Label label = new Label();

			label.Content = name;
			label.Style   = mTextStyle;

			return label;
		}

		static public TextBox CreateEdit( String name )
		{
			TextBox box = new TextBox();

			box.SelectedText = name;
			box.MinWidth	 = EDIT_WIDTH;
			box.MaxWidth	 = EDIT_WIDTH;
			box.MinHeight	 = EDIT_HEIGHT;
			box.MaxHeight	 = EDIT_HEIGHT;

			return box;
		}

		static private void PreviewTextInput_Float( Object sender, TextCompositionEventArgs e )
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

		static private void PreviewTextInput_FloatPaste( Object sender, DataObjectPastingEventArgs e )
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

		static private void LostKeyboardFocus_Float( Object sender, KeyboardFocusChangedEventArgs e )
		{
			float value;

			TextBox box = sender as TextBox;

			if( !float.TryParse( box.Text, out value ))
				box.Text = "0";
		}

		static public TextBox CreateEdit( float v )
		{
			TextBox box = new TextBox();

			box.SelectedText = "" + v;
			box.MinWidth	 = EDIT_WIDTH;
			box.MaxWidth	 = EDIT_WIDTH;
			box.MinHeight	 = EDIT_HEIGHT;
			box.MaxHeight	 = EDIT_HEIGHT;
			box.Padding		 = new Thickness( 0 );
			box.PreviewTextInput  += PreviewTextInput_Float;
			box.LostKeyboardFocus += LostKeyboardFocus_Float;
			DataObject.AddPastingHandler( box, PreviewTextInput_FloatPaste );

			return box;
		}
	}
}
