using AppUIBasics.Common;
using System;
using System.Linq;
using Windows.Storage;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

// The Settings Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace AppUIBasics
{
    /// <summary>
    /// A page that displays the app's settings.
    /// </summary>
    public sealed partial class SettingsPage : Page
    {
        public SettingsPage()
        {
            this.InitializeComponent();
            Loaded += SettingsPage_Loaded;
        }

        private void SettingsPage_Loaded(object sender, RoutedEventArgs e)
        {
            string selectedTheme = ApplicationData.Current.LocalSettings.Values["SelectedAppTheme"]?.ToString();
            ((RadioButton)ThemePanel.Children.FirstOrDefault(c => ((RadioButton)c)?.Tag?.ToString() == selectedTheme)).IsChecked = true;
            NavigationRootPage.Current.NavigationView.Header = "Settings";
        }

        private void RadioButton_Checked(object sender, RoutedEventArgs e)
        {
            ApplicationData.Current.LocalSettings.Values["SelectedAppTheme"] = ((RadioButton)sender)?.Tag?.ToString();
        }

        private async void Button_Click(object sender, RoutedEventArgs e)
        {
            await Launcher.LaunchUriAsync(new System.Uri("feedback-hub:"));
        }

        private void RadioButton_KeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == VirtualKey.Up)
            {
                NavigationRootPage.Current.PageHeader.Focus(FocusState.Programmatic);
            }
        }
    }
}
