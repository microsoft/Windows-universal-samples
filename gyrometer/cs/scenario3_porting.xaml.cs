using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;
using SDKTemplate;
using System;
using Windows.Devices.Sensors;
using Windows.Foundation;
using System.Threading.Tasks;
using Windows.UI.Core;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkID=390556

namespace GyrometerCS
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Scenario3_Porting : Page
    {
        // A pointer back to the main page.  This is needed if you want to call methods in MainPage such
        // as NotifyUser()
        MainPage rootPage = MainPage.Current;
        Gyrometer gyrometerWP;
        Gyrometer gyrometerWindows;

        public Scenario3_Porting()
        {
            this.InitializeComponent();

            // Get two instances of the gyrometer:
            // One that returns the raw gyrometer data
            gyrometerWindows = Gyrometer.GetDefault();
            // Other on which the 'ReadingTransform' is updated so that data returned aligns with the native WP orientation (portrait)
            gyrometerWP = Gyrometer.GetDefault();

            if (gyrometerWP == null || gyrometerWindows == null)
            {
                rootPage.NotifyUser("No gyrometer found", NotifyType.ErrorMessage);
            }
            else
            {
                // Assumption is that this app has been developed for Windows Phone 8.1 (or earlier)
                // and hence assumes that the sensor returns readings in Portrait Mode, which may 
                // not be true when the app or sensor logic is being ported over to a 
                // Landscape-First Windows device
                // While we encourage you to re-design your app as a universal app to gain access 
                // to many other advantages of developing a universal app, this scenario demonstrates
                // a simple approach to let the runtime honor your assumption on the  
                // "native orientation" of the sensor.
                gyrometerWP.ReadingTransform = Windows.Graphics.Display.DisplayOrientations.Portrait;
                // If you were to go the route of universal app, make no assumptions about the 
                // native orientation of the device. Instead rely on using a display orientation 
                // (absolute or current) to enforce the reference frame for the sensor readings. 
                // (which is done by updating 'ReadingTransform' property with the appropriate orientation)
            }
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame.
        /// </summary>
        /// <param name="e">Event data that describes how this page was reached.
        /// This parameter is typically used to configure the page.</param>
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (gyrometerWP ==null || gyrometerWindows == null)
            {
                GetSampleButton.IsEnabled = false;
            }
            else
            {
                GetSampleButton.IsEnabled = true;
            }
        }


        /// <summary>
        /// Invoked when 'Get Sample' button is pressed.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void GetGyrometerSample(object sender, RoutedEventArgs e)
        {
            GyrometerReading wpReading;
            GyrometerReading windowsReading;
            GetSampleButton.IsEnabled = false;

            // Establish the report interval
            gyrometerWP.ReportInterval = gyrometerWindows.MinimumReportInterval;
            gyrometerWindows.ReportInterval = gyrometerWindows.MinimumReportInterval;

            wpReading = gyrometerWP.GetCurrentReading();
            windowsReading = gyrometerWindows.GetCurrentReading();

            if (null != wpReading)
            {
                ScenarioOutput_X_WP.Text = wpReading.AngularVelocityX.ToString();
                ScenarioOutput_Y_WP.Text = wpReading.AngularVelocityY.ToString();
                ScenarioOutput_Z_WP.Text = wpReading.AngularVelocityZ.ToString();
            }

            if (null != windowsReading)
            {
                ScenarioOutput_X_Windows.Text = windowsReading.AngularVelocityX.ToString();
                ScenarioOutput_Y_Windows.Text = windowsReading.AngularVelocityY.ToString();
                ScenarioOutput_Z_Windows.Text = windowsReading.AngularVelocityZ.ToString();
            }

            gyrometerWP.ReportInterval = 0;
            gyrometerWindows.ReportInterval = 0;

            GetSampleButton.IsEnabled = true;
        }
    }
}
