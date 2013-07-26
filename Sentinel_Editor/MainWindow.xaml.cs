using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Interop;
using System.Windows.Threading;

using System.Runtime.InteropServices;

using Sentinel.Systems;
using Sentinel.Assets;
using Sentinel.Math;

namespace Sentinel_Editor
{
    /// <summary>
    /// Editor Window
    /// </summary>
    public partial class MainWindow : Window
    {
        private WRenderer           mRenderer;
        private static WColorRGBA   mClearColor = new WColorRGBA(0.0f, 0.2f, 0.8f, 1.0f);

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(Object sender, RoutedEventArgs e)
        {
            if (!WRenderer.Load("config.xml"))
            {
                MessageBox.Show("Failed to load config.xml", "Application Failure");
                System.Environment.Exit(0);
            }

            mRenderer = new WRenderer();
            RendererHWND.Child = mRenderer;

            DispatcherTimer timer = new DispatcherTimer();
            timer.Tick += new EventHandler(UpdateRenderer);
            timer.Interval = new TimeSpan(16);
            timer.Start();
        }

        private void UpdateRenderer(Object sender, EventArgs e)
        {
            mRenderer.Clear(mClearColor);

            // Draw stuff...

            mRenderer.Present();
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
