using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Media.Core;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace CustomMediaTransportControls
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private int likes = 0;

        public MainPage()
        {
            this.InitializeComponent();

            //This following line is setting the source for the MediaPlayerElement
            this.MainMPE.Source = MediaSource.CreateFromUri(new Uri("ms-appx:///Assets/fishes.wmv"));            
        }

        private async void CustomMTC_Liked(object sender, EventArgs e)
        {
            var messageDialog = new MessageDialog("You liked this video " + (++likes) + " times.");
            await messageDialog.ShowAsync();
        }
    }
}
