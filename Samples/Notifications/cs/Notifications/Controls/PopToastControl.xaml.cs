using BackgroundTasks.Helpers;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

namespace Notifications.Controls
{
    public sealed partial class PopToastControl : UserControl
    {
        public PopToastControl()
        {
            this.InitializeComponent();
        }

        private void ButtonPopToast_Click(object sender, RoutedEventArgs e)
        {
            ToastHelper.PopCustomToast(TextBoxPayload.Text);
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

        private static readonly DependencyProperty PayloadProperty = DependencyProperty.Register("Payload", typeof(string), typeof(PopToastControl), new PropertyMetadata(""));

        public string Payload
        {
            get { return GetValue(PayloadProperty) as string; }
            set { SetValue(PayloadProperty, value); }
        }
    }
}
