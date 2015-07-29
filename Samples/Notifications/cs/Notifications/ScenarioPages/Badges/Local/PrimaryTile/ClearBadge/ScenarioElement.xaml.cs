using Windows.UI.Notifications;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

namespace Notifications.ScenarioPages.Badges.Local.PrimaryTile.ClearBadge
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

        private void ButtonClearBadge_Click(object sender, RoutedEventArgs e)
        {
            BadgeUpdateManager.CreateBadgeUpdaterForApplication().Clear();
        }
    }
}
