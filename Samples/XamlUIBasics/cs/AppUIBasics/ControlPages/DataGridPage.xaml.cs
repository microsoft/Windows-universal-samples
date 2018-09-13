using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace AppUIBasics.ControlPages
{ 
    public sealed partial class DataGridPage : Page
    {
        public DataGridPage()
        {
            this.InitializeComponent();
        }

        private async void LaunchToolkitButton_Click(object sender, RoutedEventArgs e)
        {
            // Set the recommended app
            var options = new Windows.System.LauncherOptions();
            options.PreferredApplicationPackageFamilyName = "Microsoft.UWPCommunityToolkitSampleApp_8wekyb3d8bbwe";
            options.PreferredApplicationDisplayName = "Windows Community Toolkit";

            await Windows.System.Launcher.LaunchUriAsync(new Uri("uwpct://controls?sample=datagrid"), options);
        }
    }
}
