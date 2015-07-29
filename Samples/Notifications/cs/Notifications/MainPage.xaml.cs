using Notifications.Helpers;
using Notifications.Model;
using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace Notifications
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : BackButtonPage
    {
        public MainPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.Parameter is ScenarioGroup)
            {
                ScenarioGroup = e.Parameter as ScenarioGroup;

                if (!base.Frame.CanGoBack && ScenarioGroup != Scenarios.MainGroup)
                    base.Frame.BackStack.Add(new PageStackEntry(typeof(MainPage), Scenarios.MainGroup.FindParent(ScenarioGroup), null));
            }

            else
            {
                ScenarioGroup = Scenarios.MainGroup;
                base.Frame.BackStack.Clear(); //can't go back any further (in case they restored from tombstoning, we don't restore navigation history, so we start fresh)
            }

            base.OnNavigatedTo(e);
        }

        public static readonly DependencyProperty ScenarioGroupProperty = DependencyProperty.Register("ScenarioGroup", typeof(ScenarioGroup), typeof(MainPage), new PropertyMetadata(null));

        public ScenarioGroup ScenarioGroup
        {
            get { return GetValue(ScenarioGroupProperty) as ScenarioGroup; }
            set { SetValue(ScenarioGroupProperty, value); }
        }

        private void ListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ListView lv = sender as ListView;

            // Get the selected scenario, if something's selected
            IScenario scenario = lv.SelectedItem as IScenario;
            if (scenario == null)
                return;

            // Unselect the item so it can be clicked again
            lv.SelectedItem = null;

            OpenScenario(scenario);
        }

        private void OpenScenario(IScenario scenario)
        {
            if (scenario == null)
                return;

            // If it's a scenario group, open that group
            if (scenario is ScenarioGroup)
                base.Frame.Navigate(typeof(MainPage), scenario);

            // Otherwise if it's a atomic scenario, open the page for that
            else if (scenario is Scenario)
                base.Frame.Navigate(typeof(ScenarioPage), scenario);

            else
                throw new NotImplementedException("Unknown type of scenario");
        }

        private async void Footer_Click(object sender, RoutedEventArgs e)
        {
            await Windows.System.Launcher.LaunchUriAsync(new Uri(((HyperlinkButton)sender).Tag.ToString()));
        }
    }
}
