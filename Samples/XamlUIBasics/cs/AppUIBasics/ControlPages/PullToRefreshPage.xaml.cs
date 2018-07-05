using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Foundation.Metadata;
using Windows.UI.Composition;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Hosting;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace AppUIBasics.ControlPages
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class PullToRefreshPage : Page
    {
        private ObservableCollection<string> items1 = new ObservableCollection<string>();
        private ObservableCollection<string> items2 = new ObservableCollection<string>();
        private DispatcherTimer timer1 = new DispatcherTimer();
        private DispatcherTimer timer2 = new DispatcherTimer();
        private Visual visualizerContentVisual;
        private static RefreshContainer rc2;
        private RefreshVisualizer rv2;

        private Deferral RefreshCompletionDeferral1
        {
            get;
            set;
        }
        private Deferral RefreshCompletionDeferral2
        {
            get;
            set;
        }

        public PullToRefreshPage()
        {
            this.InitializeComponent();

            if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 6))
            {
                rc2 = new RefreshContainer();
                rc2.RefreshRequested +=
                    new TypedEventHandler<RefreshContainer, RefreshRequestedEventArgs>(rc2_RefreshRequested);

                rv2 = new RefreshVisualizer();
                rv2.RefreshStateChanged +=
                    new TypedEventHandler<RefreshVisualizer, RefreshStateChangedEventArgs>(rv2_RefreshStateChanged);

                Image ptrImage = new Image();
                if (App.RootTheme == ElementTheme.Light || Application.Current.RequestedTheme == ApplicationTheme.Light)
                {
                    ptrImage.Source = new BitmapImage(new Uri("ms-appx:///Assets/sun-100-Black.png"));
                } else
                {
                    ptrImage.Source = new BitmapImage(new Uri("ms-appx:///Assets/Sun.32.scale-100-White.png"));
                }

                ptrImage.Width = 35;
                ptrImage.Height = 35;

                rv2.Content = ptrImage;
                rc2.Visualizer = rv2;

                ListView lv2 = new ListView();
                lv2.Width = 200;
                lv2.Height = 200;
                lv2.BorderThickness = new Thickness(1);
                lv2.HorizontalAlignment = HorizontalAlignment.Center;
                lv2.BorderBrush = (Brush)Application.Current.Resources["TextControlBorderBrush"];


                rc2.Content = lv2;

                Ex2Grid.Children.Add(rc2);
                Grid.SetRow( rc2, 1);
                Grid.SetRow(lv2, 1);

                timer1.Interval = new TimeSpan(0, 0, 0, 0, 500);
                timer1.Tick += Timer1_Tick;

                timer2.Interval = new TimeSpan(0, 0, 0, 0, 800);
                timer2.Tick += Timer2_Tick;

                foreach (var c in @"AcrylicBrush ColorPicker NavigationView ParallaxView PersonPicture PullToRefreshPage RatingsControl RevealBrush TreeView".Split(' '))
                    items1.Add(c);
                lv.ItemsSource = items1;

                foreach (var c in @"Mike Ben Barbra Claire Justin Shawn Drew Lili".Split(' '))
                    items2.Add(c);
                lv2.ItemsSource = items2;

                this.Loaded += PullToRefreshPage_Loaded;
            }
        }

        private void PullToRefreshPage_Loaded(object sender, RoutedEventArgs e)
        {
            visualizerContentVisual = ElementCompositionPreview.GetElementVisual(rv2.Content);

            this.Loaded -= PullToRefreshPage_Loaded;
        }

        async private void Timer1_Tick(object sender, object e)
        {
            CoreDispatcher disp = Windows.ApplicationModel.Core.CoreApplication.MainView.CoreWindow.Dispatcher;
            if (disp.HasThreadAccess)
            {
                Timer1_TickImpl();
            }
            else
            {
                await disp.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    Timer1_TickImpl();
                });
            }
        }

        async private void Timer2_Tick(object sender, object e)
        {
            CoreDispatcher disp = Windows.ApplicationModel.Core.CoreApplication.MainView.CoreWindow.Dispatcher;
            if (disp.HasThreadAccess)
            {
                Timer2_TickImpl();
            }
            else
            {
                await disp.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    Timer2_TickImpl();
                });
            }
        }

        private void Timer1_TickImpl()
        {
            items1.Insert(0, "NewControl");
            timer1.Stop();
            if (this.RefreshCompletionDeferral1 != null)
            {
                this.RefreshCompletionDeferral1.Complete();
                this.RefreshCompletionDeferral1.Dispose();
                this.RefreshCompletionDeferral1 = null;
            }
        }

        private void Timer2_TickImpl()
        {
            items2.Insert(0, "New Friend");
            timer2.Stop();
            if (this.RefreshCompletionDeferral2 != null)
            {
                this.RefreshCompletionDeferral2.Complete();
                this.RefreshCompletionDeferral2.Dispose();
                this.RefreshCompletionDeferral2 = null;
            }
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);
            timer1.Stop();
            timer2.Stop();
        }

        private void rc_RefreshRequested(RefreshContainer sender, RefreshRequestedEventArgs args)
        {
            this.RefreshCompletionDeferral1 = args.GetDeferral();
            //Do some work to show new content!
            timer1.Start();
        }
        private void rc2_RefreshRequested(RefreshContainer sender, RefreshRequestedEventArgs args)
        {
            this.RefreshCompletionDeferral2 = args.GetDeferral();
            //Do some work to show new content!
            timer2.Start();
        }

        private void rv2_RefreshStateChanged(RefreshVisualizer sender, RefreshStateChangedEventArgs args)
        {
            //visualizerContentVisual.StopAnimation("RotationAngle");
         }
    }
}
