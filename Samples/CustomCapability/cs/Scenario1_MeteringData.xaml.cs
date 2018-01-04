using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace SDKTemplate
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MeteringData : Page
    {
        public ServiceViewModel ViewModel;

        public MeteringData()
        {
            InitializeComponent();
            ViewModel = ServiceViewModel.Current;
        }
        public async void button_Click_StartMetering(Object sender, RoutedEventArgs e)
        {
            await ViewModel.StartMetering((int)SamplePeriodSlider.Value);
        }

        public async void button_Click_StopMetering(Object sender, RoutedEventArgs e)
        {
            await ViewModel.StopMetering();
        }

        public void slider_ValueChanged(Object sender, RangeBaseValueChangedEventArgs e)
        {
            // Ignore slider value changes prior to ViewModel initialization.
            if (ViewModel != null)
            {
                // Update the sample period in response to slider change.
                ViewModel.SetSamplePeriod((int)SamplePeriodSlider.Value);
            }
        }
    }
}
