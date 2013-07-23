using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using System.Runtime.InteropServices;

//using Sentinel;
//using Sentinel.Math;
//using Sentinel.Assets;
//using Sentinel.Systems;
using Sentinel.Assets;
using Sentinel.Math;

namespace Sentinel_Editor
{
    /// <summary>
    /// Editor Window
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            Sentinel.Math.Vector3fw mTestX = new Sentinel.Math.Vector3fw();
        }

        ///
        /// File
        ///
        private void Click_Open(Object sender, RoutedEventArgs e)
        {
            MessageBox.Show("You clicked 'Open...'");
        }

        private void Click_Save(Object sender, RoutedEventArgs e)
        {
            MessageBox.Show("You clicked 'Save...'");
        }

        private void Click_SaveAs(Object sender, RoutedEventArgs e)
        {
            MessageBox.Show("You clicked 'Save As...'");
        }

        private void Click_Exit(Object sender, RoutedEventArgs e)
        {
            MessageBoxResult result = MessageBox.Show("Save before Exiting?",
                                                      "Exit Application",
                                                      MessageBoxButton.YesNoCancel,
                                                      MessageBoxImage.Question);

            switch(result)
            {
                case MessageBoxResult.Yes:
                    MessageBox.Show("Saving...");
                    break;

                case MessageBoxResult.No:
                    MessageBox.Show("Not Saving...");
                    break;

                case MessageBoxResult.Cancel:
                    return;
            }

            System.Environment.Exit(0);
        }

        ///
        /// Edit
        ///

        ///
        /// Help
        ///
        private void Click_About(Object sender, RoutedEventArgs e)
        {
            MessageBox.Show("Sentinel Editor\nVersion 1.0.0.0\nCopyright © BEHOLDER Software 2013", "About...");
        }
    }
}
