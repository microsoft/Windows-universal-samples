using Notifications.Helpers;
using Notifications.Model;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class ScenarioPage : BackButtonPage
    {
        public ScenarioPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            Scenario = e.Parameter as Scenario;

            if (!base.Frame.CanGoBack)
                base.Frame.BackStack.Add(new PageStackEntry(typeof(MainPage), Scenario, null));

            if (Scenario == null)
            {
                base.Frame.GoBack();
                return;
            }

            // Handle keyboard showing up
            ApplicationView.GetForCurrentView().VisibleBoundsChanged += ScenarioTestPage_VisibleBoundsChanged;

            base.OnNavigatedTo(e);
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            base.OnNavigatedFrom(e);

            ApplicationView.GetForCurrentView().VisibleBoundsChanged -= ScenarioTestPage_VisibleBoundsChanged;
        }

        private void ScenarioTestPage_VisibleBoundsChanged(ApplicationView sender, object args)
        {
            base.Width = sender.VisibleBounds.Width;
            base.Height = sender.VisibleBounds.Height;
        }

        public static readonly DependencyProperty ScenarioProperty = DependencyProperty.Register("Scenario", typeof(Scenario), typeof(ScenarioPage), new PropertyMetadata(null));

        public Scenario Scenario
        {
            get { return GetValue(ScenarioProperty) as Scenario; }
            set { SetValue(ScenarioProperty, value); }
        }
    }
}
