using System;
using Windows.Storage;
using Windows.UI.Notifications;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Toasts.AdaptiveTemplates.Image.Src.FromAppData
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ScenarioElement : UserControl
    {
        private static readonly string FILE_NAME = typeof(ScenarioElement).FullName + ".jpg";
        private static readonly string ABSOLUTE_FILE_NAME = "ms-appdata:///local/" + FILE_NAME;

        public ScenarioElement()
        {
            this.InitializeComponent();
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            Initialize();
        }

        private async void Initialize()
        {
            // Clear all existing notifications
            ToastNotificationManager.History.Clear();

            // Load the image from our package files
            StorageFile fromPackage = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///ScenarioPages/Toasts/AdaptiveTemplates/Image/Src/FromAppData/Image.jpg"));
            if (fromPackage == null)
            {
                Error("Failed to open image from application package. Normally, this would open the image from the app package, and then save it to local app data.");
                return;
            }

            try
            {
                await fromPackage.CopyAsync(ApplicationData.Current.LocalFolder, FILE_NAME, NameCollisionOption.ReplaceExisting);
            }
            
            catch
            {
                Error("Failed to copy the image from application package to local app data.");
                return;
            }

            image.Source = new BitmapImage(new Uri(ABSOLUTE_FILE_NAME));

            // Pop notifications
            popToastControl.Payload =
                $@"
                <toast scenario='reminder'>
                    <visual>
                        <binding template='ToastGeneric'>
                            <text>Image Src - From App Data</text>
                            <text>Make sure an image is displayed below this text.</text>
                            <image src='{ABSOLUTE_FILE_NAME}'/>
                        </binding>
                    </visual>
                </toast>";
        }

        private async void Error(string message)
        {
            await new MessageDialog(message, "ERROR").ShowAsync();
        }
    }
}
