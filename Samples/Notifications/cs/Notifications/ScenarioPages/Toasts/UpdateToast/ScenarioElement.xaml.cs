using BackgroundTasks.Helpers;
using System;
using Windows.UI.Notifications;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.UpdateToast
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ScenarioElement : UserControl
    {
        public ScenarioElement()
        {
            this.InitializeComponent();
        }
        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            Initialize();
        }

        private void Initialize()
        {
            // Clear all existing notifications
            ToastNotificationManager.History.Clear();

            string textbox = ToastUpdateText.Text;

            ToastHelper.PopToast("The Textbox says:", textbox, "Replace", "Toast1");
        }

        private void Replace_Click(object sender, RoutedEventArgs e)
        {
            if (String.IsNullOrEmpty(ToastUpdateText.Text))
            {
                return;
            }
            string textbox = ToastUpdateText.Text;
            ToastHelper.PopToast("The Textbox says:", textbox, "Replace", "Toast1");
        }
    }
}
