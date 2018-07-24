using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class SplitButtonPage : Page
    {
        public SplitButtonPage()
        {
            this.InitializeComponent();
        }

        private void ColorButton_Click(object sender, RoutedEventArgs e)
        {
            // Extract the color of the button that was clicked.
            Button clickedColor = (Button)sender;
            var rectangle = (Windows.UI.Xaml.Shapes.Rectangle)clickedColor.Content;
            var color = ((Windows.UI.Xaml.Media.SolidColorBrush)rectangle.Fill).Color;

            myRichEditBox.Document.Selection.CharacterFormat.ForegroundColor = color;
            CurrentColor.Fill = new SolidColorBrush(color);

            myColorButton.Flyout.Hide();
            myRichEditBox.Focus(Windows.UI.Xaml.FocusState.Keyboard);
        }

        private void myColorButton_Click(SplitButton sender, SplitButtonClickEventArgs args)
        {
            var rectangle = (Windows.UI.Xaml.Shapes.Rectangle)sender.Content;
            var color = ((Windows.UI.Xaml.Media.SolidColorBrush)rectangle.Fill).Color;

            myRichEditBox.Document.Selection.CharacterFormat.ForegroundColor = color;
        }
    }
}
