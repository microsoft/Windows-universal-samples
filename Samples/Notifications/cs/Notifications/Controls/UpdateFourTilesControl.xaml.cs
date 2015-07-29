using Notifications.Helpers;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace Notifications.Controls
{
    public sealed partial class UpdateFourTilesControl : UserControl
    {
        public UpdateFourTilesControl()
        {
            this.InitializeComponent();
        }

        private async void ButtonPin_Click(object sender, RoutedEventArgs e)
        {
            ButtonPin.IsEnabled = false;

            await TilesHelper.UpdateTiles(TextBoxPayload.Text);

            ButtonPin.IsEnabled = true;
        }

        public void SetTilePayload(string xml)
        {
            TextBoxPayload.Text = xml;
        }

        private void ButtonToggleShowPayload_Click(object sender, RoutedEventArgs e)
        {
            if (TextBoxPayload.Visibility == Visibility.Visible)
            {
                TextBoxPayload.Visibility = Visibility.Collapsed;
                ButtonToggleShowPayload.Content = "Show Payload";
            }

            else
            {
                TextBoxPayload.Visibility = Visibility.Visible;
                ButtonToggleShowPayload.Content = "Hide Payload";
            }
        }
    }
}
