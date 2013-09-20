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
	public class Inspector : TreeViewItem
	{
		public delegate void ChangedValueDelegate();
		public ChangedValueDelegate ChangedValue;

		private const double EDIT_WIDTH  = 70;
		private const double EDIT_HEIGHT = 20;

		///
		/// Must be initialized before creating a new object.
		///
		public static Style TreeStyle { get; set; }
		public static Style TextStyle { get; set; }

		public Inspector()
		{
			Style = TreeStyle;
		}

		public Inspector( Object obj )
		{
			Style  = TreeStyle;
			Header = obj;
		}

		static public Label CreateLabel( String name )
		{
			Label label = new Label();

			label.Content = name;
			label.Style   = TextStyle;

			return label;
		}

		static public TextBox CreateEdit( String name, double width = EDIT_WIDTH, double height = EDIT_HEIGHT,
										  double padding = 0 )
		{
			TextBox box = new TextBox();

			box.SelectedText = name;
			box.MinWidth	 = width;
			box.MaxWidth	 = width;
			box.MinHeight	 = height;
			box.MaxHeight	 = height;
			box.Padding		 = new Thickness( padding );

			return box;
		}

		static public TextBox CreateEdit( float value, double width = EDIT_WIDTH, double height = EDIT_HEIGHT,
										  double padding = 0 )
		{
			TextBox box = new TextBox();

			box.SelectedText = "" + value;
			box.MinWidth	 = width;
			box.MaxWidth	 = width;
			box.MinHeight	 = height;
			box.MaxHeight	 = height;
			box.Padding		 = new Thickness( padding );
			box.PreviewTextInput  += PreviewTextInput_Float;
			box.LostKeyboardFocus += LostKeyboardFocus_Float;
			DataObject.AddPastingHandler( box, PreviewTextInput_FloatPaste );

			return box;
		}

		static public Panel CreateEdit( String labelName, String editName, 
									   double width = EDIT_WIDTH, double height = EDIT_HEIGHT,
									   double padding = 0 )
		{
			StackPanel	panel = new StackPanel();
			panel.Orientation = Orientation.Horizontal;
			panel.Focusable   = false;
			
			Label label = CreateLabel( labelName );
			label.Focusable   = false;
			panel.Children.Add( label );
			
			TextBox box = CreateEdit( editName, EDIT_WIDTH, EDIT_HEIGHT, padding );
			panel.Children.Add( box );

			return panel;
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
	}
}
