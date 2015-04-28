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

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ContentDialogPage : Page
    {
        public ContentDialogPage()
        {
            this.InitializeComponent();
        }

        private async void ShowDialog_Click(object sender, RoutedEventArgs e)
        {
            ContentDialogExample dialog = new ContentDialogExample();
            await dialog.ShowAsync();

            if(dialog.Result == SignInResult.SignInOK)
            {
                DialogResult.Text = "Dialog result successful.";
            }
            else if(dialog.Result == SignInResult.SignInCancel)
            {
                DialogResult.Text = "Dialog result canceled.";
            }
            else if(dialog.Result == SignInResult.Nothing)
            {
                DialogResult.Text = "Dialog dismissed.";
            }
        }
    }
}
